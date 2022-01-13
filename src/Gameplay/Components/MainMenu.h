#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Scene.h"

/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
class MainMenu : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<MainMenu> Sptr;
	MainMenu();
	virtual ~MainMenu();

	Gameplay::GameObject::Sptr      pointer;
	Gameplay::GameObject::Sptr      fbScreen;

	Gameplay::Material::Sptr        MenuMaterial;
	Gameplay::Material::Sptr        PauseMaterial;
	Gameplay::Material::Sptr        WinMaterial;
	
	// Inherited from IComponent
	bool active = false;
	bool uparrow = false;
	bool downarrow = false;
	bool menu = true;
	bool list = false;
	int select = 0;
	int objectives = 0;
	virtual void OnEnteredTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	virtual void OnLeavingTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual void SetRenderer(RenderComponent::Sptr);
	virtual nlohmann::json ToJson() const override;
	static MainMenu::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(MainMenu);

	bool curIndex = 1;
	bool isMoving = false;
protected:
	RenderComponent::Sptr _renderer;
	bool isSwitching = false;
	bool hasResumed = false;
	std::vector<glm::vec3> pointerPositions{ glm::vec3(2.29f, 6.39f, 15.63f), glm::vec3(3.05f, 6.75f, 13.77f), glm::vec3(3.5f, 7.6f, 11.96f), glm::vec3(4.05f, 8.57f, 10.26f), glm::vec3(0.f, 0.f, -20.f) };
	int cooldown = 0;
};