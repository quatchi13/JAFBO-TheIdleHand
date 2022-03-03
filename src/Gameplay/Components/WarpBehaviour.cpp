#include "Gameplay/Components/WarpBehaviour.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

void WarpBehaviour::Update(float deltaTime) {

	if (_playerInTrigger)
	{
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_SPACE))
		{
			if (currentFloor < floorMax)
			{
				currentFloor = 2;
				roomOne->SetPosition(glm::vec3(0, 0, -50));
				roomTwo->SetPosition(glm::vec3(1.38, -5.100, 0));
			}

		}

		if ((glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_BACKSPACE)))
		{
			if (currentFloor > 1)
			{
				currentFloor = 1;
				roomTwo->SetPosition(glm::vec3(0, 0, -50));
				roomOne->SetPosition(glm::vec3(0, 0, 0));
			}

		}
	}

}

void WarpBehaviour::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	_playerInTrigger = true;
}

void WarpBehaviour::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	_playerInTrigger = false;
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
