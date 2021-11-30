#include "Gameplay/Components/MoveBehaviour.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

void MoveBehaviour::Update(float deltaTime) {

	GetGameObject()->SetPosition(GetGameObject()->GetPosition() + MoveSpeed * deltaTime);
}

void MoveBehaviour::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat3, "Speed", &MoveSpeed.x);
}

nlohmann::json MoveBehaviour::ToJson() const {
	return {
		{ "moveSpeed",  GlmToJson(MoveSpeed) }
	};
}

MoveBehaviour::Sptr MoveBehaviour::FromJson(const nlohmann::json& data) {
	MoveBehaviour::Sptr result = std::make_shared<MoveBehaviour>();
	result->MoveSpeed = ParseJsonVec3(data["moveSpeed"]);

	return result;
}