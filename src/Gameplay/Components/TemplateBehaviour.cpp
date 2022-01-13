#include "Gameplay/Components/TemplateBehaviour.h"

#include "Gameplay/GameObject.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

void TemplateBehaviour::Update(float deltaTime) {
	
}

void TemplateBehaviour::RenderImGui() {
	
}

nlohmann::json TemplateBehaviour::ToJson() const {
	return {
		{  }
	};
}

TemplateBehaviour::Sptr TemplateBehaviour::FromJson(const nlohmann::json& data) {
	TemplateBehaviour::Sptr result = std::make_shared<TemplateBehaviour>();
	
	return result;
}
