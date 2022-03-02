#pragma once
#include "IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Scene.h"

/// <summary>
/// Showcases a very simple behaviour that rotates the parent gameobject at a fixed rate over time
/// </summary>
class WarpBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<WarpBehaviour> Sptr;

	WarpBehaviour() = default;

	int currentFloor = 1;
	Gameplay::GameObject::Sptr roomOne;
	Gameplay::GameObject::Sptr roomTwo;
	//Gameplay::GameObject::Sptr roomThree;
	//Gameplay::GameObject::Sptr roomFour;
	//Gameplay::GameObject::Sptr roomFive;
	virtual void Update(float deltaTime) override;

	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;

	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static WarpBehaviour::Sptr FromJson(const nlohmann::json& data);

	MAKE_TYPENAME(WarpBehaviour);

protected:
	bool _playerInTrigger = false;
	int floorMax = 2;
};