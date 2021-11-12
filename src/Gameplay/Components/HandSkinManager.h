#pragma once
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Material.h"
#include "GLFW/glfw3.h"

class HandSkinManager : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<HandSkinManager> Sptr;
	HandSkinManager();
	virtual ~HandSkinManager();

	void AddNewSkin(Gameplay::Material::Sptr);
	void AddWindowPointer(GLFWwindow*);
	void Update(float deltaTime) override;

protected:
	std::vector<Gameplay::Material::Sptr> _handSkins;
	int currentSkinIndex;
	int framesOfCooldown;

	GLFWwindow* windowPointer;
};