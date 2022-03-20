#include "Gameplay/Components/WarpBehaviour.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

void WarpBehaviour::Update(float deltaTime) {

	if (_playerInTrigger)
	{
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_1))
		{
			currentFloor = 1;
			roomOne->SetPosition(glm::vec3(0, 0, 0));
			roomTwo->SetPosition(glm::vec3(0, 50, -50));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(0, 150, -50));
			roomFive->SetPosition(glm::vec3(0, 200, -50));
			roomSix->SetPosition(glm::vec3(0, 250, -50));

			
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_2))
		{
			currentFloor = 2;
			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(1.38, -5.100, 0));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(0, 150, -50));
			roomFive->SetPosition(glm::vec3(0, 200, -50));
			roomSix->SetPosition(glm::vec3(0, 250, -50));



			//set all your lighting parameters here for all your lights :D
			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(0, 0, 0);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1, 0, 1);
			GetGameObject()->GetScene()->Lights[0].Range = 100;


			//very important line that will actually have your changes take effect visually, MUST be AFTER the parameters change
			GetGameObject()->GetScene()->SetupShaderAndLights();
			
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_3))
		{
			currentFloor = 3;
			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(0, 50, -50));
			roomThree->SetPosition(glm::vec3(-2.33, 1.98, 0));
			roomFour->SetPosition(glm::vec3(0, 150, -50));
			roomFive->SetPosition(glm::vec3(0, 200, -50));
			roomSix->SetPosition(glm::vec3(0, 250, -50));
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_4))
		{
			currentFloor = 4;
			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(0, 50, -50));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(0, 0, 0));
			roomFive->SetPosition(glm::vec3(0, 200, -50));
			roomSix->SetPosition(glm::vec3(0, 250, -50));
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_5))
		{
			currentFloor = 5;
			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(0, 50, -50));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(0, 150, -50));
			roomFive->SetPosition(glm::vec3(0, 0, 0));
			roomSix->SetPosition(glm::vec3(0, 250, -50));
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_6))
		{
			currentFloor = 6;
			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(0, 50, -50));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(0, 150, -50));
			roomFive->SetPosition(glm::vec3(0, 200, -150));
			roomSix->SetPosition(glm::vec3(0, 0, 0));
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
