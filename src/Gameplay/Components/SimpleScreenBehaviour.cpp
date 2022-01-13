#include "Gameplay/Components/SimpleScreenBehaviour.h"
#include "Gameplay/Components/InterpolationBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"

#include "Gameplay/GameObject.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

void SimpleScreenBehaviour::Update(float deltaTime) {
	if (active && !isMoving) {
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER)) {
			if (objectivesAchieved < targetObjectives) {
				GetGameObject()->Get<InterpolationBehaviour>()->ToggleBehaviour("Lowering", false);
				GetGameObject()->Get<InterpolationBehaviour>()->PauseOrResumeCurrentBehaviour();
				isMoving = true;
			}
			else {
				GetGameObject()->Get<RenderComponent>()->SetMaterial(WinScreen);
				active = false;
			}
		}
	}
	else if (isMoving) {
		if (!GetGameObject()->Get<InterpolationBehaviour>()->_isRunning) {
			isMoving = false;
			active = false;
		}
	}
}

void SimpleScreenBehaviour::RenderImGui() {
	
}

nlohmann::json SimpleScreenBehaviour::ToJson() const {
	return {
	};
}

SimpleScreenBehaviour::Sptr SimpleScreenBehaviour::FromJson(const nlohmann::json& data) {
	SimpleScreenBehaviour::Sptr result = std::make_shared<SimpleScreenBehaviour>();
	return result;
}
