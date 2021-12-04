#include "Gameplay/Components/LocomotionBehaviour.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include "Gameplay/Scene.h"
#include "GLFW/glfw3.h"

float DistanceBetween(glm::vec3 thing1, glm::vec3 thing2) {
	return(sqrt((pow(thing1.x - thing2.x, 2.0))) + (pow(thing1.y - thing2.y, 2.0)) + (pow(thing1.z - thing2.z, 2.0)));
}

float DistanceBetween(float x, float y) {
	float z = x - y;
	if (z < 0) { z *= -1; }
	return z;
}

LocomotionBehaviour::LocomotionBehaviour():IComponent(){
	//nothing to see here lmao
}

LocomotionBehaviour::LocomotionBehaviour(Gameplay::GameObject::Sptr b) : IComponent() {
	//we will assume that the first behaviour pushed is the one to use
	_currentBehaviour = 0;
	_isRunning = true;
	_amountOfBehaviours = 0;

	boid = b;
}


LocomotionBehaviour::~LocomotionBehaviour() = default;


void LocomotionBehaviour::AddBehaviour(PatrolBehaviour bhv) {
	_Behaviours.push_back(bhv);
	_Behaviours.back().Init();
}

/// <summary>
/// Allows the user to specify which interpolation behaviour to use, and whether or not it will loop. 
/// By default, the system will set the current behaviour to the first behaviour that was pushed, and will loop.
/// </summary>
/// <param name="name: ">the name of the interpolation behaviour that has already been pushed</param>
/// <param name="loops: ">if we want the behaviour to loop or run through once and stop</param>
void LocomotionBehaviour::ToggleBehaviour(std::string name) {

	//_Behaviours[_currentBehaviour]->Reset();

	//searches for behaviour with specified name
	for (int i = 0; i < _Behaviours.size(); i++) {
		if (_Behaviours[i].behaviourName == name) {
			_currentBehaviour = i;
			break;
		}
	}

}

/// <summary>
/// This function will pause or resume the current interpolation behaviour, 
/// basically it just sets _isRunning to the opposite value
/// </summary>
void LocomotionBehaviour::PauseOrResumeCurrentBehaviour() {
	_isRunning = !_isRunning;
}

void LocomotionBehaviour::Update(float deltaTime) {
	if (_isRunning) {
		_Behaviours[_currentBehaviour].Update(deltaTime);
		boid->SetPosition(_Behaviours[_currentBehaviour].boidPos);
	}
}


void PatrolBehaviour::Init() {
	AdaptiveSamplePath();
}

int PatrolPath::GetCurrentSegment() {
	int sum = 0;
	for (int i = 0; i < nodesPerSeg.size(); i++) {
		sum += nodesPerSeg[i];
		if (currentNode <= sum) {
			return i;
		}
	}

	return nodesPerSeg.size() - 1;
}



void PatrolBehaviour::Update(float deltaTime) {
	float t;
	glm::vec3 p0, p1;

	//add delta time to our current time value
	timer += deltaTime;

	//if we are over the duration of the current frame, update the index
	if (timer > speed) {
		timer = 0;
		path.currentNode++;
	}

	if (path.currentNode > path.nodes.size()-1) {
		path.currentNode = 0;
	}

	int i0 = path.currentNode;
	int i1 = (i0 < path.nodes.size() - 1) ? i0 + 1 : 0;


	//divide time by current duration
	t = timer / speed;
	//get p0 and p1
	
	p0 = path.SampledTargets[i0];
	p1 = path.SampledTargets[i1];

	//interpolate result
	boidPos = glm::mix(p0, p1, t);
}




/// <summary>
/// performs linear interpolation
/// </summary>
/// <returns>interpolated result between p0 and p1, in respect to t</returns>
template <typename T> T PatrolBehaviour::LERP(T p0, T p1, float t) {
	return((1.f - t) * p0) + (t * p1);
}

glm::vec3 PatrolBehaviour::Catmull(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	float x, y, z;

	//stores t^3, t^2, t, and 1 so that the multiplication looks *slightly* neater
	glm::vec4 interpolationMatrix = glm::vec4((t * t * t), (t * t), t, 1);

	//multiplies p0, p1, p2, and p3 by the catmull constraint matrix (it was unhappy when we did direct matrix multiplication)
	glm::vec4 pointVectorX = glm::vec4((-0.5 * p0.x + 1.5 * p1.x + -1.5 * p2.x + 0.5 * p3.x), (1 * p0.x + -2.5 * p1.x + 2 * p2.x + -0.5 * p3.x), (-0.5 * p0.x + 0 + 0.5 * p2.x + 0), (0 + 1 * p1.x + 0 + 0));
	glm::vec4 pointVectorY = glm::vec4((-0.5 * p0.y + 1.5 * p1.y + -1.5 * p2.y + 0.5 * p3.y), (1 * p0.y + -2.5 * p1.y + 2 * p2.y + -0.5 * p3.y), (-0.5 * p0.y + 0 + 0.5 * p2.y + 0), (0 + 1 * p1.y + 0 + 0));
	glm::vec4 pointVectorZ = glm::vec4((-0.5 * p0.z + 1.5 * p1.z + -1.5 * p2.z + 0.5 * p3.z), (1 * p0.z + -2.5 * p1.z + 2 * p2.z + -0.5 * p3.z), (-0.5 * p0.z + 0 + 0.5 * p2.z + 0), (0 + 1 * p1.z + 0 + 0));

	//multiplies resulting vectors by the interpolation to get 3 floats
	x = ((pointVectorX.x * interpolationMatrix.x) + (pointVectorX.y * interpolationMatrix.y) + (pointVectorX.z * interpolationMatrix.z) + pointVectorX.w);
	y = ((pointVectorY.x * interpolationMatrix.x) + (pointVectorY.y * interpolationMatrix.y) + (pointVectorY.z * interpolationMatrix.z) + pointVectorY.w);
	z = ((pointVectorZ.x * interpolationMatrix.x) + (pointVectorZ.y * interpolationMatrix.y) + (pointVectorZ.z * interpolationMatrix.z) + pointVectorZ.w);

	//returns floats as xyz coords
	return glm::vec3(x, y, z);
}





void LocomotionBehaviour::RenderImGui() {

}

nlohmann::json LocomotionBehaviour::ToJson() const {
	nlohmann::json result;
	result["is_running"] = _isRunning;
	result["current_behaviour"] = _currentBehaviour;
	result["num_of_behaviour"] = _amountOfBehaviours;
	return result;
}

LocomotionBehaviour::Sptr LocomotionBehaviour::FromJson(const nlohmann::json & data) {
	LocomotionBehaviour::Sptr result = std::make_shared<LocomotionBehaviour>();
	result->_isRunning = data["is_running"];
	result->_currentBehaviour = data["current_behaviour"];
	result->_amountOfBehaviours = data["num_of_behaviours"];
	return result;
}



void PatrolBehaviour::AdaptSampleCalc(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t0, float t1, float rad) {
	asNode node1(Catmull(p0, p1, p2, p3, t0), t0), node2(Catmull(p0, p1, p2, p3, ((t0 + t1) / 2)), ((t0 + t1) / 2)), node3(Catmull(p0, p1, p2, p3, t1), t1);
	float a(DistanceBetween(node1.loc, node2.loc)), b(DistanceBetween(node2.loc, node3.loc)), c(DistanceBetween(node1.loc, node3.loc));

	if (((a + b) - c) > (rad)) {
		AdaptSampleCalc(p0, p1, p2, p3, node1.dist, node2.dist, rad);
		AdaptSampleCalc(p0, p1, p2, p3, node2.dist, node3.dist, rad);
	}

	curSegNodes.push_back(node1);
	curSegNodes.push_back(node3);
}

void PatrolBehaviour::SortASNodes()
{
	float keyDist;
	glm::vec3 keyLoc;
	for (int i = 1; i < curSegNodes.size(); i++)
	{
		keyDist = curSegNodes[i].dist;
		keyLoc = glm::vec3(curSegNodes[i].loc);
		int j = i - 1;

		while (j >= 0 && curSegNodes[j].dist > keyDist)
		{
			curSegNodes[j + 1].dist = curSegNodes[j].dist;
			curSegNodes[j + 1].loc = glm::vec3(curSegNodes[j].loc);
			j = j - 1;
		}
		curSegNodes[j + 1].dist = keyDist;
		curSegNodes[j + 1].loc = glm::vec3(keyLoc);
	}


	//removing duplicate nodes
	std::vector<asNode> nonDups;

	for (int i = 1; i < curSegNodes.size(); i++) {
		if (curSegNodes[i - 1].dist != curSegNodes[i].dist) {
			nonDups.push_back(curSegNodes[i - 1]);
		}
	}
	

	curSegNodes.clear();
	for (int i = 0; i < nonDups.size(); i++) {
		curSegNodes.push_back(nonDups[i]);
	}
}


void PatrolBehaviour::AdaptiveSamplePath()
{
	
	for (int j = 0; j < path.nodes.size(); j++) {
		int p0_index, p1_index, p2_index, p3_index;
		glm::vec3 p0, p1, p2, p3;

		p1_index = j;
		p0_index = (p1_index == 0) ? path.nodes.size() - 1 : p1_index - 1;
		p2_index = (p1_index == path.nodes.size() - 1) ? 0 : p1_index + 1;
		p3_index = (p2_index == path.nodes.size() - 1) ? 0 : p2_index + 1;

		p0 = path.nodes[p0_index];
		p1 = path.nodes[p1_index];
		p2 = path.nodes[p2_index];
		p3 = path.nodes[p3_index];

		AdaptSampleCalc(p0, p1, p2, p3, 0, 1, path.radius);
			
		SortASNodes();

		for (int k = 0; k < curSegNodes.size(); k++) {
			path.SampledTargets.push_back(glm::vec3(curSegNodes[k].loc));
		}
		nodesPerSeg.push_back(curSegNodes.size());
		path.totalNodes += nodesPerSeg.back();
			
		curSegNodes.clear();
	}

}



