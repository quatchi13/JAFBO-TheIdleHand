#include "Gameplay/Components/MainMenu.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"


MainMenu::MainMenu() :
	IComponent(),
	_renderer(nullptr),
	MenuMaterial(nullptr),
	ListMaterial(nullptr)
{ }
MainMenu::~MainMenu() = default;

void MainMenu::OnEnteredTrigger(const Gameplay::Physics::TriggerVolume::Sptr& trigger) {

		if (trigger->GetGameObject()->Has<RenderComponent>())
		{
			
			trigger->GetGameObject()->Get<RenderComponent>()->SetMaterial(MenuMaterial);
		}
		
		if (_renderer && MenuMaterial) {
			_renderer->SetMaterial(MenuMaterial);
		}
	
	
	LOG_INFO("Entered trigger: {}", trigger->GetGameObject()->Name);
}

void MainMenu::OnLeavingTrigger(const Gameplay::Physics::TriggerVolume::Sptr& trigger) {
	if (_renderer && ListMaterial) {
		_renderer->SetMaterial(ListMaterial);
	}
	LOG_INFO("Left trigger: {}", trigger->GetGameObject()->Name);
}

void MainMenu::Awake() {
	_renderer = GetComponent<RenderComponent>();
}

void MainMenu::Update(float deltaTime)
{
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER) == GLFW_PRESS && !onScreen)
	{
		onScreen = true;
		std::cout << "poosh";
		GetGameObject()->SetPosition(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, -GetGameObject()->GetPosition().z));
		_renderer->SetMaterial(ListMaterial);
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER) == GLFW_RELEASE)
	{
		onScreen = false;
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_DOWN) == GLFW_PRESS && !onScreen && !downarrow)
	{
		downarrow = true;
		if (select < 4)
		{
			select++;
		}
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		downarrow = false;
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_UP) == GLFW_PRESS && !onScreen && !uparrow)
	{
		uparrow = true;
		if (select > 1)
		{
			select--;
		}
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		uparrow = false;
	}
	
}



void MainMenu::RenderImGui() { }

void MainMenu::SetRenderer(RenderComponent::Sptr render)
{
	_renderer = render;
}

nlohmann::json MainMenu::ToJson() const {
	return {
		{ "menu_material", MenuMaterial != nullptr ? MenuMaterial->GetGUID().str() : "null" },
		{ "list_material", ListMaterial != nullptr ? ListMaterial->GetGUID().str() : "null" }
	};
}

MainMenu::Sptr MainMenu::FromJson(const nlohmann::json& blob) {
	MainMenu::Sptr result = std::make_shared<MainMenu>();
	result->MenuMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["menu_material"]));
	result->ListMaterial  = ResourceManager::Get<Gameplay::Material>(Guid(blob["list_material"]));
	return result;
}
