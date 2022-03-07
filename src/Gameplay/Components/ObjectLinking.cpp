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
	float xOS = (GetGameObject()->GetPosition().x) - (parent->GetPosition().x);
	float yOS = (GetGameObject()->GetPosition().y) - (parent->GetPosition().y);
	float zOS = (GetGameObject()->GetPosition().z) - (parent->GetPosition().z);

	PositionOffset = glm::vec3(xOS, yOS, zOS);
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
		if (linkedObjects[i]->Get<ObjectLinking>()->currentlyLinked) {
			linkedObjects[i]->SetPosition(glm::vec3(linkedObjects[i]->Get<ObjectLinking>()->parent->GetPosition() + linkedObjects[i]->Get<ObjectLinking>()->PositionOffset));
			/*TODO: PUT CODE HERE TO MATCH THE RENDER TOGGLE STATE OF PARENT*/
			linkedObjects[i]->Get<ObjectLinking>()->UpdateChildren();
		}

	}
}


void ObjectLinking::RenderImGui() {

	if (ImGui::Button("Toggle Link")) {
		currentlyLinked = !currentlyLinked;
	}

	if (ImGui::Button("Lock To Position")) {
		CalculateOffset();
	}

}

nlohmann::json ObjectLinking::ToJson() const {
	nlohmann::json result;

	result["position_offset"] = GlmToJson(PositionOffset);
	result["is_parent"] = isRoot;
	result["parent"] = (parent != nullptr) ? parent->GUID.str() : "null";
	result["number_of_children"] = linkedObjects.size();

	for (int i = 0; i < linkedObjects.size(); i++) {
		result["child_" + std::to_string(i)] = linkedObjects[i]->GUID.str();
	}
	return result;
}

ObjectLinking::Sptr ObjectLinking::FromJson(const nlohmann::json& blob) {
	ObjectLinking::Sptr result = std::make_shared<ObjectLinking>();
	result->PositionOffset = ParseJsonVec3(blob["Position Offset"]);
	result->isRoot = blob["is_parent"];
	result->parent = result->GetGameObject()->GetScene()->FindObjectByGUID(Guid(blob["parent"]));
	int numberOfChildren = blob["number_of_children"];

	for (int i = 0; i < numberOfChildren; i++) {
		result->linkedObjects.push_back(result->GetGameObject()->GetScene()->FindObjectByGUID(Guid(blob["child_" + std::to_string(i)])));
	}

	return result;
}