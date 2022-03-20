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

			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(0.0f, 1.0f, 17.23f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
			GetGameObject()->GetScene()->Lights[0].Range = -8.5f;
			
			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(-6.76f, 0.29f, 5.74f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(0.10196f, 0.73725f, 0.8f);
			GetGameObject()->GetScene()->Lights[1].Range = 85.4f;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(1.73f, 0.14f, 9.95f);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 0.2f, 0.10196f);
			GetGameObject()->GetScene()->Lights[2].Range = 90.9f;

			GetGameObject()->GetScene()->SetupShaderAndLights();
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
			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(-17.830f, 0.020f, 1.500f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[0].Range = 113.680;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(11.380f, -11.860f, 8.748f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(0.9f, 0.85f, 0.65f);
			GetGameObject()->GetScene()->Lights[1].Range = 6.400;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(-8.054f, 2.130f, 6.310f);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);
			GetGameObject()->GetScene()->Lights[2].Range = 6.400f;


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

			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(0.410f, -0.310f, 13.650f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[0].Range = 20.980;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(1.950f, 4.000f, 13.958f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[1].Range = 34.200;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(-7.914f, 5.110f, 13.958f);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);
			GetGameObject()->GetScene()->Lights[2].Range = 6.400;


			//very important line that will actually have your changes take effect visually, MUST be AFTER the parameters change
			GetGameObject()->GetScene()->SetupShaderAndLights();
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

			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(8.760f, -1.820f, 11.180f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[0].Range = 27.280;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(10.110f, 3.890f, 9.578f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[1].Range = 34.200;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(-5.864f, 3.390f, 9.460f);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);
			GetGameObject()->GetScene()->Lights[2].Range = 19.200;


			//very important line that will actually have your changes take effect visually, MUST be AFTER the parameters change
			GetGameObject()->GetScene()->SetupShaderAndLights();
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

			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(-1.020f, -9.930f, 2.680f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 0.82f, 0.f);
			GetGameObject()->GetScene()->Lights[0].Range = 11.880;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(-15.687f, 5.100f, 3.674f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[1].Range = 34.200;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(-4.017f, -11.777f, 6.780f);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 0.0f, 0.0f);
			GetGameObject()->GetScene()->Lights[2].Range = 1.000;


			//very important line that will actually have your changes take effect visually, MUST be AFTER the parameters change
			GetGameObject()->GetScene()->SetupShaderAndLights();
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

			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(-1.120f, 0.230f, 7.610f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 0.82f, 0.f);
			GetGameObject()->GetScene()->Lights[0].Range = 21.580;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(-14.097f, 2.620f, 0.974f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[1].Range = -2.723;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(-4.213f, 4.343f, 2.290);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 0.0f, 0.0f);
			GetGameObject()->GetScene()->Lights[2].Range = 2.585;


			//very important line that will actually have your changes take effect visually, MUST be AFTER the parameters change
			GetGameObject()->GetScene()->SetupShaderAndLights();
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
