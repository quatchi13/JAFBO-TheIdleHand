#include "Gameplay/Components/LevelManagerBehaviour.h"

#include "Gameplay/GameObject.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

void LevelManagerBehaviour::Update(float deltaTime) {
	
}

void LevelManagerBehaviour::RenderImGui() {
	
}

nlohmann::json LevelManagerBehaviour::ToJson() const {
	return {
		{  }
	};
}

LevelManagerBehaviour::Sptr LevelManagerBehaviour::FromJson(const nlohmann::json& data) {
	LevelManagerBehaviour::Sptr result = std::make_shared<LevelManagerBehaviour>();
	
	return result;
}
