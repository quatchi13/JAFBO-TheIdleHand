#include "Gameplay/Components/MainMenu.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/InterpolationBehaviour.h"
#include "Gameplay/Components/SimpleScreenBehaviour.h"

MainMenu::MainMenu() :
	IComponent(),
	_renderer(nullptr),
	MenuMaterial(nullptr),
	PauseMaterial(nullptr),
	WinMaterial(nullptr)
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
	if (_renderer && PauseMaterial) {
		_renderer->SetMaterial(PauseMaterial);
	}
	LOG_INFO("Left trigger: {}", trigger->GetGameObject()->Name);
}

void MainMenu::Awake() {
	_renderer = GetComponent<RenderComponent>();
}

void MainMenu::Update(float deltaTime)
{

	if (backgroundCooldown) { backgroundCooldown--; }



	if (cooldown) { cooldown--; }



	if (!active) 
	{//this manages the main menu
		

		int c = select;
		bool dPress = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_DOWN);
		bool uPress = glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_UP);
		if (controls == 1 || credits == 1){
			dPress = 0;
			uPress = 0;
		}

		if (!cooldown) {
			select += ((uPress * -1) + dPress);
			select = (select < 0) ? 3 : ((select > 3) ? 0 : select);
			cooldown = (select != c) ? 30 : 0;
		}

		pointer->SetPosition(pointerPositions[select]); 
		GetGameObject()->Get<InterpolationBehaviour>()->AddBehaviourScript("interp_scripts/menu_behaviour.txt");

		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER) && !cooldown) {
			std::cout << "Enter";
			if (select == 0 && controls == 0 && credits == 0) {
				active = true;
				pointer->SetPosition(pointerPositions[4]);
				GetGameObject()->Get<InterpolationBehaviour>()->PauseOrResumeCurrentBehaviour();
				curIndex = 0;
				isMoving = true;
				cooldown = 30;
				sound.setSound("open");
				sound.setPosition(FMOD_VECTOR{ 0,0,0 });
				sound.setVolume(1.5);
				sound.play();

				
				
			}
			if (select == 1 && controls == 0 && credits == 0) {
				//GetGameObject()->SetPosition(glm::vec3(20.03f, -5.14f, 6.9f));
				controls = true;
				cooldown = 30;
				select = 4;
				GetGameObject()->Get<InterpolationBehaviour>()->ToggleBehaviour("MenuToControl", false);
				GetGameObject()->Get<InterpolationBehaviour>()->PauseOrResumeCurrentBehaviour();
			}
			if (select == 2 && controls == 0 && credits == 0) {
				//GetGameObject()->SetPosition(glm::vec3(-7.93f, 16.55f, 6.9f));
				credits = true;
				cooldown = 30;
				select = 4;
				GetGameObject()->Get<InterpolationBehaviour>()->ToggleBehaviour("MenuToCredits", false);
				GetGameObject()->Get<InterpolationBehaviour>()->PauseOrResumeCurrentBehaviour();
			}
			if (select == 3 && controls == 0 && credits == 0) {
				glfwSetWindowShouldClose(GetGameObject()->GetScene()->Window, true);
				cooldown = 30;
			}
			if (controls == true && !cooldown) {
				//GetGameObject()->SetPosition(glm::vec3(5.87f, 5.79f, 6.9f));
				cooldown = 30;
				controls = false;
				select = 4;	
				GetGameObject()->Get<InterpolationBehaviour>()->ToggleBehaviour("ControlToMenu", false);
				GetGameObject()->Get<InterpolationBehaviour>()->PauseOrResumeCurrentBehaviour();
			}
			if (credits == true && !cooldown) {
				//GetGameObject()->SetPosition(glm::vec3(5.87f, 5.79f, 6.9f));
				cooldown = 30;
				credits = false;
				select = 4;
				GetGameObject()->Get<InterpolationBehaviour>()->ToggleBehaviour("CreditsToMenu", false);
				GetGameObject()->Get<InterpolationBehaviour>()->PauseOrResumeCurrentBehaviour();
			}
		}

	}

	else if (!fbScreen->Get<SimpleScreenBehaviour>()->active) {//this will manage the pause menu
		if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER) && !cooldown) {
			GetGameObject()->Get<RenderComponent>()->SetMaterial(PauseMaterial);
			GetGameObject()->Get<InterpolationBehaviour>()->ToggleBehaviour(curIndex, false);
			GetGameObject()->Get<InterpolationBehaviour>()->PauseOrResumeCurrentBehaviour();
			curIndex = !curIndex;
			cooldown = 30;
			isMoving = true;

		}
	}
	else if (isMoving) {
		if (!GetGameObject()->Get<InterpolationBehaviour>()->_isRunning) {
			GetGameObject()->Get<RenderComponent>()->SetMaterial(PauseMaterial);
			isMoving = false;
		}
	}
	
	
}



void MainMenu::RenderImGui() { }

void MainMenu::SetRenderer(RenderComponent::Sptr render)
{
	_renderer = render;
}

AudioSource MainMenu::GetSound()
{
	return sound;
}

void MainMenu::ChangeSound(std::string name)
{
	sound.setSound(name);
}

nlohmann::json MainMenu::ToJson() const {
	return {
		{ "menu_material", MenuMaterial != nullptr ? MenuMaterial->GetGUID().str() : "null" },
		{ "pause_material", PauseMaterial != nullptr ? PauseMaterial->GetGUID().str() : "null" }
	};
}

MainMenu::Sptr MainMenu::FromJson(const nlohmann::json& blob) {
	MainMenu::Sptr result = std::make_shared<MainMenu>();
	result->MenuMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["menu_material"]));
	result->PauseMaterial  = ResourceManager::Get<Gameplay::Material>(Guid(blob["pause_material"]));
	return result;
}
