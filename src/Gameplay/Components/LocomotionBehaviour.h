#pragma once
#include "IComponent.h"
#include "Gameplay/GameObject.h"



/// <summary>
/// behaviours for the boid
/// (patrol or wander)
/// </summary>
enum AI_Mode {
	PATROL,
	WANDER
};

/// <summary>
/// Stuff that the boid will avoid
/// </summary>
struct Obstacles {
public:
	glm::vec3 location;
	float radius;

	Obstacles(glm::vec3 l, float r) {
		location = glm::vec3(l);
		radius = r;
	}
};

/// <summary>
/// A path for the boid to follow
/// </summary>
class PatrolPath {
public:
	std::vector<glm::vec3>nodes;
	std::vector<Obstacles>pathObstacles;
	std::vector<glm::vec3>SampledTargets;
	float radius;
	int segment = 0;
	int currentNode = 0;
	int totalNodes = 0;
	std::vector<int>nodesPerSeg;
	bool isLastNode = false;
	
	PatrolPath() {};

	PatrolPath(std::vector<glm::vec3> n, float r) {
		for (int i = 0; i < n.size(); i++) {
			nodes.push_back(n[i]);
		}
		radius = r;
	}

	void AddPathObstacle(Obstacles o) {
		pathObstacles.push_back(o);
	}

	int GetCurrentSegment();
};

/// <summary>
/// Parent class for AI behaviour
/// </summary>
class AI_Behaviour {
public:
	virtual void Update(float) {};
	virtual void Init() {};
	AI_Mode mode;
	std::string behaviourName;
	glm::vec3 boidPos;

	template <typename T> T LERP(T p0, T p1, float t);
	glm::vec3 Catmull(glm::vec3, glm::vec3, glm::vec3, glm::vec3, float); 
	

};

/// <summary>
/// The Patrol child class for AI behaviour
/// </summary>
class PatrolBehaviour {
protected:
	struct asNode{
		glm::vec3 loc;
		float dist;
		asNode(glm::vec3 l, float d) {
			loc = glm::vec3(l);
			dist = d;
			std::cout << "added";
		}
	};

	std::vector<asNode> curSegNodes;
	std::vector<int>nodesPerSeg;
	
	bool isLastNode = false;
	bool isAvoiding = false;
public:
	AI_Mode mode;
	std::string behaviourName;
	glm::vec3 boidPos;

	template <typename T> T LERP(T p0, T p1, float t);
	glm::vec3 Catmull(glm::vec3, glm::vec3, glm::vec3, glm::vec3, float);

	PatrolPath path;
	float timer = 0.f;
	float circuitTime;
	float speed;
	float circuitLength;

	PatrolBehaviour(AI_Mode m, std::string name, PatrolPath p, float s) {
		mode = m;
		behaviourName = name;
		path = p;
		speed = s;
	}
	
	void Update(float);
	void Init();
	
	void AdaptiveSamplePath();
	void GetCircuitSpeed();
	void SetCurrentSpeed(int i0, int i1);
	void AdaptSampleCalc(glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, float, float);
	void SortASNodes();

	glm::vec3 PrevPos(float);
};




/// <summary>
/// A simple linear interpolation system that allows an object to follow a set path
///  or a set sequence of transformations. 
/// 
/// Multiple paths/sequences can be stored, but only one will be accessed at a time. 
/// Toggling is performed via the "ToggleBehaviour" function
/// </summary>
class LocomotionBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<LocomotionBehaviour> Sptr;

	LocomotionBehaviour();
	LocomotionBehaviour(Gameplay::GameObject::Sptr);
	~LocomotionBehaviour();

	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static LocomotionBehaviour::Sptr FromJson(const nlohmann::json& data);

	void ToggleBehaviour(std::string newBehaviour);
	void PauseOrResumeCurrentBehaviour();

	

	

	void AddBehaviour(PatrolBehaviour);

	MAKE_TYPENAME(LocomotionBehaviour);

protected:
	Gameplay::GameObject::Sptr boid;

	std::vector<PatrolBehaviour> _Behaviours;
	int _currentBehaviour;
	int _amountOfBehaviours;
	bool _isRunning;

private:
	
};
