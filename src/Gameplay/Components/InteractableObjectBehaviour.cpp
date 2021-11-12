#include "Gameplay/Components/InteractableObjectBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/RenderComponent.h"

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


void InteractableObjectBehaviour::Awake() {
	_windowPointer = GetGameObject()->GetScene()->Window;
}

void InteractableObjectBehaviour::Update(float deltaTime) {
	if (_playerInTrigger) {
		GetGameObject()->SetRotation(GetGameObject()->GetRotationEuler() + glm::vec3(0.f, 0.f, 90.f) * deltaTime);

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E) == GLFW_PRESS) {
			_hasBeenActivated = true;
			_playerInTrigger = false;
			GetGameObject()->SetRotation(glm::vec3(0.f, 0.f, 0.f));
			_body->GetGameObject()->Get<SkinManager>()->AddSkin(_rewardMaterial);
			_body = nullptr;
		}

	}
}


void InteractableObjectBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	if (!_hasBeenActivated) {
		LOG_INFO("Body has entered our trigger volume: {}", body->GetGameObject()->Name);
		_playerInTrigger = true;
		_body = body;
	}
}

void InteractableObjectBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	if (!_hasBeenActivated) {
		LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
		_playerInTrigger = false;
		_body = nullptr;
	}
		
}

void InteractableObjectBehaviour::RenderImGui() { }

nlohmann::json InteractableObjectBehaviour::ToJson() const {
	nlohmann::json result;
	result["reward_skin"] = (_rewardMaterial) ? _rewardMaterial->GetGUID().str() : "null";
	result["has_been_activated"] = _hasBeenActivated;
	return result;
}

InteractableObjectBehaviour::Sptr InteractableObjectBehaviour::FromJson(const nlohmann::json& blob) {
	InteractableObjectBehaviour::Sptr result = std::make_shared<InteractableObjectBehaviour>();
	result->_rewardMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["reward_skin"].get<std::string>()));
	result->_hasBeenActivated = blob["has_been_activated"];
	return result;
}

