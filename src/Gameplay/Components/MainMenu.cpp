#include "Gameplay/Components/MainMenu.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"


MainMenu::MainMenu() :
	IComponent(),
	_renderer(nullptr),
	EnterMaterial(nullptr),
	ExitMaterial(nullptr)
{ }
MainMenu::~MainMenu() = default;

void MainMenu::OnEnteredTrigger(const Gameplay::Physics::TriggerVolume::Sptr& trigger) {

		if (trigger->GetGameObject()->Has<RenderComponent>())
		{
			
			trigger->GetGameObject()->Get<RenderComponent>()->SetMaterial(EnterMaterial);
		}
		
		if (_renderer && EnterMaterial) {
			_renderer->SetMaterial(EnterMaterial);
		}
	
	
	LOG_INFO("Entered trigger: {}", trigger->GetGameObject()->Name);
}

void MainMenu::OnLeavingTrigger(const Gameplay::Physics::TriggerVolume::Sptr& trigger) {
	if (_renderer && ExitMaterial) {
		_renderer->SetMaterial(ExitMaterial);
	}
	LOG_INFO("Left trigger: {}", trigger->GetGameObject()->Name);
}

void MainMenu::Awake() {
	_renderer = GetComponent<RenderComponent>();
}

void MainMenu::Update(float deltaTime)
{
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		std::cout << "poosh";
		GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, -GetGameObject()->GetPosition().z));
	}

	
}



void MainMenu::RenderImGui() { }

void MainMenu::SetRenderer(RenderComponent::Sptr render)
{
	_renderer = render;
}

nlohmann::json MainMenu::ToJson() const {
	return {
		{ "enter_material", EnterMaterial != nullptr ? EnterMaterial->GetGUID().str() : "null" },
		{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	};
}

MainMenu::Sptr MainMenu::FromJson(const nlohmann::json& blob) {
	MainMenu::Sptr result = std::make_shared<MainMenu>();
	result->EnterMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["enter_material"]));
	result->ExitMaterial  = ResourceManager::Get<Gameplay::Material>(Guid(blob["exit_material"]));
	return result;
}
