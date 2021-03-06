#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Scene.h"
#include "Utils/AudioSource.h"
#include "Utils/AudioEngine.h"

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
	Gameplay::Material::Sptr        StartMaterial;

	AudioSource sound;

	AudioEngine* audioEngine = AudioEngine::instance();

	int backgroundvolume = 5;
	int backgroundCooldown = 0;
	
	// Inherited from IComponent
	bool active = false;
	bool uparrow = false;
	bool downarrow = false;
	bool menu = true;
	bool controls = false;
	bool credits = false;
	bool list = false;
	bool cutScene = false;
	int select = 0;
	int objectives = 0;
	virtual void OnEnteredTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	virtual void OnLeavingTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual void SetRenderer(RenderComponent::Sptr);
	AudioSource GetSound();
	void ChangeSound(std::string name);
	virtual nlohmann::json ToJson() const override;
	static MainMenu::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(MainMenu);

	bool curIndex = 1;
	bool isMoving = false;
protected:
	RenderComponent::Sptr _renderer;
	bool isSwitching = false;
	bool hasResumed = false;
	std::vector<glm::vec3> pointerPositions{ glm::vec3(4.48f, 6.22f, 10.0f), glm::vec3(4.78f, 6.62f, 7.7f), glm::vec3(5.15f, 7.02f, 5.72f), glm::vec3(4.670f, 7.47f, 3.51f), glm::vec3(0.f, 0.0f, -20.f) };
	int cooldown = 0;
};