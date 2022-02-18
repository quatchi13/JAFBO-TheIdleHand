#include "Gameplay/Components/WarpBehaviour.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

void WarpBehaviour::Update(float deltaTime) {
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_SPACE))
	{
		currentFloor = 2;
		roomOne->SetPosition(glm::vec3(0, 0, -50));
		roomTwo->SetPosition(glm::vec3(0, 0, 0));
	}
	
	if ((glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_BACKSPACE)))
	{
		currentFloor = 1;
		roomTwo->SetPosition(glm::vec3(0, 0, -50));
		roomOne->SetPosition(glm::vec3(0, 0, 0));
	}
}

void WarpBehaviour::RenderImGui() {
	
}

nlohmann::json WarpBehaviour::ToJson() const {
	return {
		
	};
}

WarpBehaviour::Sptr WarpBehaviour::FromJson(const nlohmann::json& data) {
	WarpBehaviour::Sptr result = std::make_shared<WarpBehaviour>();
	
	return result;
}
