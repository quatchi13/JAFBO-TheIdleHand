#include "Gameplay/Components/ObjectLinking.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

ObjectLinking::ObjectLinking() :IComponent(), parent(nullptr) {
	isRoot = true;
	PositionOffset = glm::vec3(0);
}

ObjectLinking::ObjectLinking(Gameplay::GameObject::Sptr p) : IComponent(), parent(p) {
	isRoot = false;
}

void ObjectLinking::Awake() {
	if (!isRoot) {
		float xOS = (GetGameObject()->GetPosition().x) - (parent->GetPosition().x);
		float yOS = (GetGameObject()->GetPosition().y) - (parent->GetPosition().y);
		float zOS = (GetGameObject()->GetPosition().z) - (parent->GetPosition().z);

		PositionOffset = glm::vec3(xOS, yOS, zOS);
	}
}

void ObjectLinking::CalculateOffset() {
	
}

void ObjectLinking::LinkObject(Gameplay::GameObject::Sptr o) {
	linkedObjects.push_back(o);
}

void ObjectLinking::Update(float deltaTime) {
	if (isRoot) {
		UpdateChildren();
	}
	
}

void ObjectLinking::UpdateChildren() {
	for (int i = 0; i < linkedObjects.size(); i++) {
		linkedObjects[i]->SetPosition(glm::vec3(linkedObjects[i]->Get<ObjectLinking>()->parent->GetPosition() + linkedObjects[i]->Get<ObjectLinking>()->PositionOffset));
		linkedObjects[i]->Get<ObjectLinking>()->UpdateChildren();
	}
}

void ObjectLinking::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat3, "Offset", &PositionOffset.x);
}

nlohmann::json ObjectLinking::ToJson() const {
	return {
		{ "Position Offset",  GlmToJson(PositionOffset) }
	};
}

ObjectLinking::Sptr ObjectLinking::FromJson(const nlohmann::json& data) {
	ObjectLinking::Sptr result = std::make_shared<ObjectLinking>();
	result->PositionOffset = ParseJsonVec3(data["Position Offset"]);

	return result;
}