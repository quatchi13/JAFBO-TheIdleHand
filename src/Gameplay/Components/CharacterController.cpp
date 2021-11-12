#include "Gameplay/Components/CharacterController.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"

CharacterController::CharacterController() :IComponent(), _velMultiplier(2.5f) {

}

void CharacterController::Awake() {
	_body = GetGameObject()->Get<Gameplay::Physics::RigidBody>();
}

void CharacterController::Update(float deltaTime) {
	xpos = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_S);
	xneg = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_W);
	ypos = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_D);
	yneg = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_A);
	zpos = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_UP);
	zneg = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_DOWN);

	xVelocity = (xpos * _velMultiplier) - (xneg * _velMultiplier);
	yVelocity = (ypos * _velMultiplier) - (yneg * _velMultiplier);
	zVelocity = (zpos * _velMultiplier) - (zneg * _velMultiplier);

	_body->SetLinearVelocity(glm::vec3(xVelocity, yVelocity, zVelocity));
	_body->SetAngularVelocity(glm::vec3(0.f, 0.f, 0.f));
	GetGameObject()->SetRotation(glm::vec3(0.f, 0.f, 0.f));

}

void CharacterController::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat3, "Velocity", &_velMultiplier);
}

nlohmann::json CharacterController::ToJson() const {
	return {
		{ "velocity_multiplier", _velMultiplier }
	};
	
}

CharacterController::Sptr CharacterController::FromJson(const nlohmann::json& blob) {
	CharacterController::Sptr result = std::make_shared<CharacterController>();
	result->_velMultiplier = blob["velocity_multiplier"];
	
	return result;
}
