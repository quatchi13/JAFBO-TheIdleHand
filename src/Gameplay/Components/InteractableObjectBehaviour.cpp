#include "Gameplay/Components/InteractableObjectBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/SimpleScreenBehaviour.h"

InteractionFeedback::InteractionFeedback(Gameplay::Material::Sptr mat, Gameplay::GameObject::Sptr o) {
	_SWAPMAT = mat;
	_TARGET = o;
	b = TEX;
}

InteractionFeedback::InteractionFeedback(Gameplay::MeshResource::Sptr mesh, Gameplay::GameObject::Sptr o) {
	_SWAPMESH = mesh;
	_TARGET = o;
	b = MESH;
}

InteractionFeedback::InteractionFeedback(std::vector<InteractionTForm> tforms, Gameplay::GameObject::Sptr o) {
	for (int i = 0; i < tforms.size(); i++) {
		_SWAPTRANSFORM.push_back(tforms[i]);
	}

	_TARGET = o;
	b = TRANSFORM;
}

InteractionFeedback::InteractionFeedback(int aIndex, Gameplay::GameObject::Sptr o) {
	_SWAPAINDEX = aIndex;
	_TARGET = o;
	b = ANIM;
}

void InteractionFeedback::SwapMat() {
	_TARGET->Get<RenderComponent>()->SetMaterial(_SWAPMAT);
	std::cout << "swapping material";
}

void InteractionFeedback::SwapMesh() {
	_TARGET->Get<RenderComponent>()->SetMesh(_SWAPMESH);
}

void InteractionFeedback::SwapTransforms() {
	for (int i = 0; i < _SWAPTRANSFORM.size(); i++) {
		switch (_SWAPTRANSFORM[i].trnsfrm) {
		case(InteractionTForm::pos):
			_TARGET->SetPosition(_SWAPTRANSFORM[i].tform);
			break;
		case(InteractionTForm::rot):
			_TARGET->SetRotation(_SWAPTRANSFORM[i].tform);
			break;
		default:
			_TARGET->SetScale(_SWAPTRANSFORM[i].tform);
		}
	}
}

void InteractionFeedback::SwapAnim() {
	_TARGET->Get<MorphAnimationManager>()->SetCurrentAnim(_SWAPAINDEX);
	_TARGET->Get<MorphAnimationManager>()->SetIdleIndex(_SWAPAINDEX);
}



InteractableObjectBehaviour::InteractableObjectBehaviour() :
	IComponent(), _windowPointer(nullptr)
{ 
	_hasBeenActivated = false;
}
InteractableObjectBehaviour::~InteractableObjectBehaviour() = default;

/// <summary>
/// Add a material that the player can use on the hand as a "reward" for interacting
/// </summary>
/// <param name="r">
/// : the material being used as a reward
/// </param>
void InteractableObjectBehaviour::AddRewardMaterial(Gameplay::Material::Sptr r) {
	_rewardMaterial = r;
}

void InteractableObjectBehaviour::AddSoundEffect(std::string soundName) {
	soundEffect = soundName;
}

void InteractableObjectBehaviour::AddFeedbackBehaviour(InteractionFeedback f) {
	feedback.push_back(f);
}


void InteractableObjectBehaviour::Awake() {
	_windowPointer = GetGameObject()->GetScene()->Window;
}

void InteractableObjectBehaviour::Update(float deltaTime) {
	if (_playerInTrigger) {

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E) == GLFW_PRESS) {
			_hasBeenActivated = true;
			_playerInTrigger = false;
			PerformFeedback();
			_body->GetGameObject()->Get<SkinManager>()->AddSkin(_rewardMaterial);
			_body = nullptr;
			prompt->SetPosition(glm::vec3(prompt->GetPosition().x, prompt->GetPosition().y, -prompt->GetPosition().z));
			
			screen->Get<SimpleScreenBehaviour>()->objectivesAchieved += 1;
			screen->Get<SimpleScreenBehaviour>()->active = true;
			audioEngine->playSoundByName(soundEffect);
		}

	}


		// For game sound assignment to show multi instruement
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_7))
		{

			srand(time(0));
			randomNumber = (rand() % 3);
			switch (randomNumber)
			{
			case 0:
				audioEngine->playSoundByName("Bang");

				break;

			case 1:
				audioEngine->playSoundByName("Bing");

				break;
			case 2:
				audioEngine->playSoundByName("Bong");
				break;
			}
		}
	
}


void InteractableObjectBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	if (!_hasBeenActivated) {
		LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
		_playerInTrigger = true;
		_body = body;
		prompt->SetPosition(glm::vec3(prompt->GetPosition().x, prompt->GetPosition().y, -prompt->GetPosition().z));
	}
	std::cout << "enter trigger";
	if (_hasBeenActivated == true) {
		makeBang = true;
		std::cout << std::endl << "trigger has been activated";
	}
}

void InteractableObjectBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	if (!_hasBeenActivated) {
		LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
		_playerInTrigger = false;
		_body = nullptr;
		prompt->SetPosition(glm::vec3(prompt->GetPosition().x, prompt->GetPosition().y, -prompt->GetPosition().z));
	}
	std::cout << "exit trigger";
	if (_hasBeenActivated == true) {
		makeBang = false;
		std::cout << std::endl << "trigger has been exited";
	}
		
}


void InteractableObjectBehaviour::PerformFeedback() {
	for (int i = 0; i < feedback.size(); i++) {
		std::cout << feedback[i].b;
		switch (feedback[i].b) {
		case(TEX):
			feedback[i].SwapMat();
			break;
		case(MESH):
			feedback[i].SwapMesh();
			break;
		case(TRANSFORM):
			feedback[i].SwapTransforms();
			break;
		default:
			feedback[i].SwapAnim();
		}
	}
}



void InteractableObjectBehaviour::RenderImGui() { }

nlohmann::json InteractableObjectBehaviour::ToJson() const {
	nlohmann::json result;
	result["reward_skin"] = (_rewardMaterial) ? _rewardMaterial->GetGUID().str() : "null";
	result["has_been_activated"] = _hasBeenActivated;
	result["number_of_feedbacks"] = feedback.size();
	for (int i = 0; i < feedback.size(); i++) {
		std::string fbackName = "feedback_" + std::to_string(i);
		result[fbackName + "_target"] = feedback[i]._TARGET->GUID.str();
		result[fbackName + "_type"] = (int)feedback[i].b;

		switch (feedback[i].b) {
		case(TEX):
			result[fbackName + "_mat"] = feedback[i]._SWAPMAT->GetGUID().str();
			break;
		case(MESH):
			result[fbackName + "_mesh"] = feedback[i]._SWAPMESH->GetGUID().str();
			break;
		case(TRANSFORM):
			result[fbackName + "_tformcount"] = feedback[i]._TRANSFORMCOUNT;
			for (int j = 0; j < feedback[i]._TRANSFORMCOUNT; j++) {
				std::string tformName = fbackName + "_tform_" + std::to_string(j);
				result[tformName + "_type"] = (int)feedback[i]._SWAPTRANSFORM[j].trnsfrm;
				result[tformName + "_vec"] = GlmToJson(feedback[i]._SWAPTRANSFORM[j].tform);
			}
			break;
		default:
			result[fbackName + "_animIndex"] = feedback[i]._SWAPAINDEX;
		}
	}
	return result;
}

InteractableObjectBehaviour::Sptr InteractableObjectBehaviour::FromJson(const nlohmann::json& blob) {
	InteractableObjectBehaviour::Sptr result = std::make_shared<InteractableObjectBehaviour>();
	result->_rewardMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["reward_skin"].get<std::string>()));
	result->_hasBeenActivated = blob["has_been_activated"];
	int y = blob["number_of_feedbacks"];
	int count = 0;
	for (int i = 0; i < y; i++) {
		std::string fbackName = "feedback_" + std::to_string(i);
		int x = blob[fbackName + "_type"];
		if (!x) {
			InteractionFeedback iF(ResourceManager::Get<Gameplay::Material>(Guid(blob[fbackName + "_mat"].get<std::string>())), result->GetGameObject()->GetScene()->FindObjectByGUID(Guid(blob[fbackName + "_target"])));
			result->AddFeedbackBehaviour(iF);
		}
		else if (x == 1) {
			InteractionFeedback iF(ResourceManager::Get<Gameplay::MeshResource>(Guid(blob[fbackName + "_mesh"].get<std::string>())), result->GetGameObject()->GetScene()->FindObjectByGUID(Guid(blob[fbackName + "_target"])));
			result->AddFeedbackBehaviour(iF);
		}
		else if (x == 2) {
			std::vector<InteractionTForm> tForms;
			int tFormSize = blob[fbackName + "_tformcount"];
			for (int j = 0; j < tFormSize; j++) {
				std::string tformName = fbackName + ("_tform_" + std::to_string(j));
				int tft = blob[tformName + "_type"];
				tForms.push_back(InteractionTForm(InteractionTForm::tformt(tft), ParseJsonVec3(blob[tformName + "_vec"])));
			}
			InteractionFeedback iF(tForms, result->GetGameObject()->GetScene()->FindObjectByGUID(Guid(blob[fbackName + "_target"])));
		}

		else {
			InteractionFeedback iF((int)blob[fbackName + "_animIndex"], result->GetGameObject()->GetScene()->FindObjectByGUID(Guid(blob[fbackName + "target"])));
			result->AddFeedbackBehaviour(iF);
		}
		count++;
	}

	return result;
}

