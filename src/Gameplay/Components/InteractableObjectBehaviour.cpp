#include "Gameplay/Components/InteractableObjectBehaviour.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MainMenu.h"

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

void InteractableObjectBehaviour::AddFeedbackBehaviour(InteractionFeedback f) {
	feedback.push_back(f);
}


void InteractableObjectBehaviour::Awake() {
	_windowPointer = GetGameObject()->GetScene()->Window;
}

void InteractableObjectBehaviour::Update(float deltaTime) {
	if (_playerInTrigger) {
		//GetGameObject()->SetRotation(GetGameObject()->GetRotationEuler() + glm::vec3(0.f, 0.f, 90.f) * deltaTime);

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E) == GLFW_PRESS) {
			_hasBeenActivated = true;
			_playerInTrigger = false;
			GetGameObject()->SetRotation(glm::vec3(0.f, 0.f, 0.f));
			PerformFeedback();
			_body->GetGameObject()->Get<SkinManager>()->AddSkin(_rewardMaterial);
			_body = nullptr;
			prompt->SetPosition(glm::vec3(prompt->GetPosition().x, prompt->GetPosition().y, -prompt->GetPosition().z));
			objective->SetPosition(glm::vec3(objective->GetPosition().x, objective->GetPosition().y, -objective->GetPosition().z));
			screen->Get<RenderComponent>()->SetMaterial(image);
			screen->SetPosition(glm::vec3(screen->GetPosition().x, screen->GetPosition().y, -screen->GetPosition().z));
			screen->Get<MainMenu>()->curIndex = !(screen->Get<MainMenu>()->curIndex);
			if (isSecret)
			{
				secret->SetPosition(glm::vec3(secret->GetPosition().x, secret->GetPosition().y, -secret->GetPosition().z));
			}
			screen->Get<MainMenu>()->objectives += 1;
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
}

void InteractableObjectBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	
	if (!_hasBeenActivated) {
		LOG_INFO("Body has left our trigger volume: {}", body->GetGameObject()->Name);
		_playerInTrigger = false;
		_body = nullptr;
		prompt->SetPosition(glm::vec3(prompt->GetPosition().x, prompt->GetPosition().y, -prompt->GetPosition().z));
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
	return result;
}

InteractableObjectBehaviour::Sptr InteractableObjectBehaviour::FromJson(const nlohmann::json& blob) {
	InteractableObjectBehaviour::Sptr result = std::make_shared<InteractableObjectBehaviour>();
	result->_rewardMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["reward_skin"].get<std::string>()));
	result->_hasBeenActivated = blob["has_been_activated"];
	return result;
}

