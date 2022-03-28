#include "Gameplay/Components/WarpBehaviour.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/ObjectLinking.h"
#include "Gameplay/Components/InterpolationBehaviour.h"

void WarpBehaviour::Awake() {
	InitList();
}

void WarpBehaviour::Update(float deltaTime) {

	if (_playerInTrigger)
	{
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_1))
		{
			currentFloor = 1;
			UpdateListForNewRoom();

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

			//use this to turn the shroomba on for a room
			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (!shroombaMoveba->_isRunning) {
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = false;
				shroombaMoveba->ToggleBehaviour(0, true);//replace 0 with the index of the behaviour for the shroomba
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
			}	


			//bounding boxes
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetPosition(glm::vec3(0, 0, 11));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetPosition(glm::vec3(0, 0, -0.49));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetPosition(glm::vec3(5.87, 0, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetPosition(glm::vec3(-9.98, 0, 6));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetRotation(glm::vec3(90, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetRotation(glm::vec3(90, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(0, 5.91, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(0, -7, 6));

		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_2))
		{
			currentFloor = 2;
			UpdateListForNewRoom();

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

			//use this to turn the shroomba off for a room
			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (shroombaMoveba->_isRunning) {
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = true;
			}
			
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_3))
		{
			currentFloor = 3;
			UpdateListForNewRoom();

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

			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (shroombaMoveba->_isRunning) {
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = true;
			}
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_4))
		{
			currentFloor = 4;
			UpdateListForNewRoom();

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

			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (shroombaMoveba->_isRunning) {
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = true;
			}
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_5))
		{
			currentFloor = 5;
			UpdateListForNewRoom();

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

			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (shroombaMoveba->_isRunning) {
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = true;
			}
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_6))
		{
			currentFloor = 6;
			UpdateListForNewRoom();

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


			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (shroombaMoveba->_isRunning) {
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = true;
			}
			

			
			//bounding boxes
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetRotation(glm::vec3(0.f, 0.f, -22.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetRotation(glm::vec3(0.f, 0.f, -22.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetPosition(glm::vec3(0.f, 0.f, 11.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetPosition(glm::vec3(0.f, 0.f, -0.49f));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetRotation(glm::vec3(23.f, 87.98f, 0.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetRotation(glm::vec3(23.f, 87.98f, 0.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetPosition(glm::vec3(10.17f, 0.f, 6.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetPosition(glm::vec3(-10.95f, 0.f, 6.f));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetRotation(glm::vec3(90.f, 0.f, -32.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetRotation(glm::vec3(90.f, 0.f, -26.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(2.97f, 6.01f, 6.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(-2.55f, -7.09f, 6.f));
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



void WarpBehaviour::InitList() {
	for (int i = 0; i < 6; i++) {
		std::vector<bool> array{ 0, 0, 0, 0, 0 };

		listChecks.push_back(array);
	}
}

void WarpBehaviour::UpdateListForNewRoom() {
	for (int i = 0; i < 5; i++) {
		if (lines[i]->GetPosition().z < 80.f) {
			glm::vec3 pos = glm::vec3(lines[i]->GetPosition());
			pos.z += 100;
			lines[i]->SetPosition(pos);
		}


		int x = currentFloor - 1;
		if (listChecks[x][i]) {
			CrossOffItem(i);
		}

	}

	int lNum = currentFloor - 1;
	GetGameObject()->GetScene()->FindObjectByName("List")->Get<RenderComponent>()->SetMaterial(lists[lNum]);
}

void WarpBehaviour::CrossOffItem(int index) {
	int x = currentFloor - 1;
	(listChecks[x])[index] = true;
	glm::vec3 pos = glm::vec3(lines[index]->GetPosition());
	std::cout << "retrieving " << index << ": " << lines[index]->Name;
	pos.z -= 100;
	lines[index]->SetPosition(pos);
}



void WarpBehaviour::PushLines(std::vector<Gameplay::GameObject::Sptr> lineVec) {
	for (int i = 0; i < 5; i++) {
		lines.push_back(lineVec[i]);
	}
}

void WarpBehaviour::PushLists(std::vector<Gameplay::Material::Sptr> listVec) {
	for (int i = 0; i < 6; i++) {
		lists.push_back(listVec[i]);
	}
}