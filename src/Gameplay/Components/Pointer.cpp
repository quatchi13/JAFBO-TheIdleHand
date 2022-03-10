#include "Gameplay/Components/Pointer.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"


Pointer::Pointer() :
	IComponent(),
	_renderer(nullptr),
	MenuMaterial(nullptr),
	ListMaterial(nullptr)
{ }
Pointer::~Pointer() = default;

void Pointer::OnEnteredTrigger(const Gameplay::Physics::TriggerVolume::Sptr& trigger) {

		if (trigger->GetGameObject()->Has<RenderComponent>())
		{
			
			trigger->GetGameObject()->Get<RenderComponent>()->SetMaterial(MenuMaterial);
		}
		
		if (_renderer && MenuMaterial) {
			_renderer->SetMaterial(MenuMaterial);
		}
	
	
	LOG_INFO("Entered trigger: {}", trigger->GetGameObject()->Name);
}

void Pointer::OnLeavingTrigger(const Gameplay::Physics::TriggerVolume::Sptr& trigger) {
	if (_renderer && ListMaterial) {
		_renderer->SetMaterial(ListMaterial);
	}
	LOG_INFO("Left trigger: {}", trigger->GetGameObject()->Name);
}

void Pointer::Awake() {
	_renderer = GetComponent<RenderComponent>();

	
}

void Pointer::Update(float deltaTime)
{
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER) == GLFW_PRESS && !onScreen)
	{
		onScreen = true;
		//std::cout << "poosh";
		//GetGameObject()->SetPosition(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, -GetGameObject()->GetPosition().z));
		_renderer->SetMaterial(ListMaterial);
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER) == GLFW_RELEASE)
	{
		onScreen = false;
	}

	switch (mainmenu->select)
	{
	case 1:
		GetGameObject()->SetPosition(glm::vec3(2.29f, 6.39f, 15.63f));
		break;
	case 2:
		GetGameObject()->SetPosition(glm::vec3(3.05f, 6.75f, 13.77f));
		break;
	case 3:
		GetGameObject()->SetPosition(glm::vec3(3.5f, 7.6f, 11.96f));
		break;
	case 4:
		GetGameObject()->SetPosition(glm::vec3(4.05f, 8.57f, 10.26f));
		break;
	}

	
}



void Pointer::RenderImGui() { }

void Pointer::SetRenderer(RenderComponent::Sptr render)
{
	_renderer = render;
}

nlohmann::json Pointer::ToJson() const {
	nlohmann::json result;

	result["renderer"] = (_renderer != nullptr) ? _renderer->ToJson() : "null";
	result["is_onscreen"] = onScreen;
	result["menu_material"] = (MenuMaterial != nullptr) ? MenuMaterial->GetGUID().str() : "null";
	result["list_material"] = (ListMaterial != nullptr) ? ListMaterial->GetGUID().str() : "null";

	return result;
}

Pointer::Sptr Pointer::FromJson(const nlohmann::json& blob) {
	Pointer::Sptr result = std::make_shared<Pointer>();

	result->_renderer = RenderComponent::FromJson(blob["renderer"]);
	result->onScreen = blob["is_onscreen"];
	result->MenuMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["menu_material"].get<std::string>()));
	result->ListMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["list_material"].get<std::string>()));

	return result;
}
