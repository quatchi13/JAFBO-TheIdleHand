#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/SkinManager.h"
#include "GLFW/glfw3.h"
#include "Gameplay/Scene.h"

/// <summary>
/// Gives an object interactivity properties 
/// (ie: it will visually indicate the hand is hoovering over it, and can give the hand a "reward")
/// </summary>
class InteractableObjectBehaviour : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<InteractableObjectBehaviour> Sptr;
	InteractableObjectBehaviour();
	virtual ~InteractableObjectBehaviour();
	//adds a material that can be sent to the hand after an interaction
	void AddRewardMaterial(Gameplay::Material::Sptr r);

	// Inherited from IComponent
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static InteractableObjectBehaviour::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(InteractableObjectBehaviour);

protected:
	bool _playerInTrigger = false;
	bool _hasBeenActivated = false;

	Gameplay::Physics::RigidBody::Sptr _body;

	GLFWwindow* _windowPointer;
	Gameplay::Material::Sptr _rewardMaterial;
};