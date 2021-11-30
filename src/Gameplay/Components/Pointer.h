#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MainMenu.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Scene.h"

/// <summary>
/// Provides an example behaviour that uses some of the trigger interface to change the material
/// of the game object the component is attached to when entering or leaving a trigger
/// </summary>
class Pointer : public Gameplay::IComponent {

public:
	typedef std::shared_ptr<Pointer> Sptr;
	Pointer();
	virtual ~Pointer();

	Gameplay::Material::Sptr        MenuMaterial;
	Gameplay::Material::Sptr        ListMaterial;

	MainMenu::Sptr mainmenu;

	// Inherited from IComponent
	bool onScreen = false;
	bool menu = true;
	bool list = false;
	virtual void OnEnteredTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	virtual void OnLeavingTrigger(const std::shared_ptr<Gameplay::Physics::TriggerVolume>& trigger) override;
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void RenderImGui() override;
	virtual void SetRenderer(RenderComponent::Sptr);
	virtual nlohmann::json ToJson() const override;
	static Pointer::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(Pointer);

protected:
	RenderComponent::Sptr _renderer;

};