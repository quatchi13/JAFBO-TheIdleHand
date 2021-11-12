#include "Gameplay/Components/HandSkinManager.h"
#include "Gameplay/GameObject.h"

HandSkinManager::HandSkinManager()
{
	framesOfCooldown = 0;
}

HandSkinManager::~HandSkinManager() = default;

void HandSkinManager::AddNewSkin(Gameplay::Material::Sptr newSkin) {
	_handSkins.push_back(newSkin);
}

void HandSkinManager::AddWindowPointer(GLFWwindow* w) {
	windowPointer = w;
}


void HandSkinManager::Update(float deltaTime) {
	if (!framesOfCooldown) {
		if (_handSkins.size()) {
			if (glfwGetKey(windowPointer, GLFW_KEY_COMMA) == GLFW_PRESS) {
				currentSkinIndex = (currentSkinIndex == 0) ? _handSkins.size() - 1 : currentSkinIndex--;
				framesOfCooldown = 10;
				GetGameObject()->Get<RenderComponent>()->SetMaterial(_handSkins[currentSkinIndex]);
			}
			else if(glfwGetKey(windowPointer, GLFW_KEY_PERIOD) == GLFW_PRESS){
				currentSkinIndex = (currentSkinIndex == _handSkins.size() - 1) ? 0 : currentSkinIndex++;
				framesOfCooldown = 10;
				GetGameObject()->Get<RenderComponent>()->SetMaterial(_handSkins[currentSkinIndex]);
			}
		}
		

	}
	else {
		framesOfCooldown--;
	}
}
