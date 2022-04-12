#include "Gameplay/Components/WarpBehaviour.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/ObjectLinking.h"
#include "Utils/AudioSource.h"

#include "Gameplay/Components/InterpolationBehaviour.h"

void WarpBehaviour::Awake() {
	InitList();

	twin.setSound("twin");
	twin.setPosition(FMOD_VECTOR{ 0,0,0 });
	twin.play();
	twin.setLooping(true);
	twin.setVolume(0.2);

	master.setSound("master");
	master.setPosition(FMOD_VECTOR{ 0,0,0 });
	master.setLooping(true);
	master.setVolume(0);
	master.play();

	bath.setSound("bath");
	bath.setPosition(FMOD_VECTOR{ 0,0,0 });
	bath.setLooping(true);
	bath.setVolume(0);
	bath.play();

	kitchen.setSound("kitchen");
	kitchen.setPosition(FMOD_VECTOR{ 0,0,0 });
	kitchen.setLooping(true);
	kitchen.setVolume(0);
	kitchen.play();

	living.setSound("living");
	living.setPosition(FMOD_VECTOR{ 0,0,0 });
	living.setLooping(true);
	living.setVolume(0);
	living.play();
	
	basement.setSound("basement");
	basement.setPosition(FMOD_VECTOR{ 0,0,0 });
	basement.setLooping(true);
	basement.setVolume(0);
	basement.play();


	master.setVolume(0);
	bath.setVolume(0);
	kitchen.setVolume(0);
	living.setVolume(0);
	basement.setVolume(0);

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
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetPosition(glm::vec3(0, 0, 14));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetPosition(glm::vec3(0, 0, -0.49));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetPosition(glm::vec3(5.87, 0, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetPosition(glm::vec3(-7.61, 0, 6));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetRotation(glm::vec3(90, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetRotation(glm::vec3(90, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(0, 5.91, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(0, -7, 6));

			
			master.setVolume(0);
			bath.setVolume(0);
			kitchen.setVolume(0);
			living.setVolume(0);
			basement.setVolume(0);
			twin.setVolume(0.2);
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_2))
		{
			currentFloor = 2;
			UpdateListForNewRoom();

			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(-1.750, -1.540, 0));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(0, 150, -50));
			roomFive->SetPosition(glm::vec3(0, 200, -50));
			roomSix->SetPosition(glm::vec3(0, 250, -50));



			//set all your lighting parameters here for all your lights :D
			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(-17.830f, 0.020f, 1.500f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[0].Range = 113.680;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(8.7f, -10.740f, 8.748f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(0.9f, 0.85f, 0.65f);
			GetGameObject()->GetScene()->Lights[1].Range = 6.400;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(-2.184f, 4.950f, 6.310f);
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
			
			//bounding boxes
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetPosition(glm::vec3(0, 0, 11));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetPosition(glm::vec3(0, 0, -0.49));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetRotation(glm::vec3(-9, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetPosition(glm::vec3(6.83, 0, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetPosition(glm::vec3(-9.98, 0, 6));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetRotation(glm::vec3(90, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetRotation(glm::vec3(90, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(0, 7.05, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(0, -11.540, 6));

			twin.setVolume(0);
			bath.setVolume(0);
			kitchen.setVolume(0);
			living.setVolume(0);
			basement.setVolume(0);
			master.setVolume(0.2);
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

			//bounding boxes
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetPosition(glm::vec3(0, 0, 11));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetPosition(glm::vec3(0, 0, -0.49));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetRotation(glm::vec3(31, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetRotation(glm::vec3(27, 85, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetPosition(glm::vec3(5.57, -3.26, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetPosition(glm::vec3(-13.16, 4.44, 6));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetRotation(glm::vec3(90, 0, -33));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetRotation(glm::vec3(90, 0, -36));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(4.47, 5.01, 7.58));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(-6.49, -3.89, 5.29));

			master.setVolume(0);
			kitchen.setVolume(0);
			living.setVolume(0);
			basement.setVolume(0);
			twin.setVolume(0);
			bath.setVolume(0.2);
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_4))
		{
			currentFloor = 4;
			UpdateListForNewRoom();

			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(0, 50, -50));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(-4.17, 2.49, 0.21));
			roomFive->SetPosition(glm::vec3(0, 200, -50));
			roomSix->SetPosition(glm::vec3(0, 250, -50));

			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(4.190f, 1.07f, 14.180f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[0].Range = 27.280;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(-3.710f, 3.890f, 14.748f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[1].Range = 34.200;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(1.296f, 3.280f, 14.570f);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);
			GetGameObject()->GetScene()->Lights[2].Range = 19.200;


			//very important line that will actually have your changes take effect visually, MUST be AFTER the parameters change
			GetGameObject()->GetScene()->SetupShaderAndLights();

			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (shroombaMoveba->_isRunning) {
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = true;
			}

			//bounding boxes
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetPosition(glm::vec3(0, 0, 11));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetPosition(glm::vec3(0, 0, -0.49));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetPosition(glm::vec3(7.62, 0, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetPosition(glm::vec3(-13.51, 4.08, 5.9));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetRotation(glm::vec3(90, 0, -11.0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetRotation(glm::vec3(90, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(-2.380, 7.47, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(-3.46, -5.37, 6));

			master.setVolume(0);
			living.setVolume(0);
			basement.setVolume(0);
			twin.setVolume(0);
			bath.setVolume(0);
			kitchen.setVolume(0.2);
		}

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_5))
		{
			currentFloor = 5;
			UpdateListForNewRoom();

			roomOne->SetPosition(glm::vec3(0, 0, -50));
			roomTwo->SetPosition(glm::vec3(0, 50, -50));
			roomThree->SetPosition(glm::vec3(0, 100, -50));
			roomFour->SetPosition(glm::vec3(0, 150, -50));
			roomFive->SetPosition(glm::vec3(-1.62, -3.3, 0));
			roomSix->SetPosition(glm::vec3(0, 250, -50));

			GetGameObject()->GetScene()->Lights[0].Position = glm::vec3(-2.53f, -13.56f, 2.680f);
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f, 0.82f, 0.f);
			GetGameObject()->GetScene()->Lights[0].Range = 3.28;

			GetGameObject()->GetScene()->Lights[1].Position = glm::vec3(-6.07f, -0.43f, 5.894f);
			GetGameObject()->GetScene()->Lights[1].Color = glm::vec3(1.f, 1.f, 1.f);
			GetGameObject()->GetScene()->Lights[1].Range = 27.100;

			GetGameObject()->GetScene()->Lights[2].Position = glm::vec3(-5.537f, -11.777f, 6.780f);
			GetGameObject()->GetScene()->Lights[2].Color = glm::vec3(1.0f, 0.0f, 0.0f);
			GetGameObject()->GetScene()->Lights[2].Range = 1.000;

			//very important line that will actually have your changes take effect visually, MUST be AFTER the parameters change
			GetGameObject()->GetScene()->SetupShaderAndLights();

			auto& shroombaMoveba = GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<InterpolationBehaviour>();
			if (shroombaMoveba->_isRunning) {
				shroombaMoveba->PauseOrResumeCurrentBehaviour();
				GetGameObject()->GetScene()->FindObjectByName("Bedroom Shroomba")->Get<ObjectLinking>()->currentlyLinked = true;
			}

			//bounding boxes
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetRotation(glm::vec3(0, 0, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetRotation(glm::vec3(0, 0, -9));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosZ")->SetPosition(glm::vec3(0, 0, 11));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegZ")->SetPosition(glm::vec3(1.08, -3.3, -0.48));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetRotation(glm::vec3(5, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetRotation(glm::vec3(0, 90, 0));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosX")->SetPosition(glm::vec3(8.79, -7.1, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegX")->SetPosition(glm::vec3(-13.58, -1.37, 6));

			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetRotation(glm::vec3(90, -3, -15));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetRotation(glm::vec3(90, 4, -17));
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(-0.65, 6.97, 6));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(-1.83, -14.33, 4.4));

			master.setVolume(0);
			basement.setVolume(0);
			twin.setVolume(0);
			bath.setVolume(0);
			kitchen.setVolume(0);
			living.setVolume(0.2);
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
			GetGameObject()->GetScene()->FindObjectByName("boundingPosY")->SetPosition(glm::vec3(1.28f, 6.94f, 6.f));
			GetGameObject()->GetScene()->FindObjectByName("boundingNegY")->SetPosition(glm::vec3(-2.55f, -7.09f, 6.f));


			master.setVolume(0);
			twin.setVolume(0);
			bath.setVolume(0);
			kitchen.setVolume(0);
			living.setVolume(0);
			basement.setVolume(0.2);
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