#pragma once
#include "IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Scene.h"
#include "Utils/AudioEngine.h"
#include "Utils/AudioSource.h"
#include <ctime>
#include <cstdlib>
/// <summary>
/// Showcases a very simple behaviour that rotates the parent gameobject at a fixed rate over time
/// </summary>
class WarpBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<WarpBehaviour> Sptr;

	WarpBehaviour() = default;

	int currentFloor = 1;
	Gameplay::GameObject::Sptr roomOne;
	Gameplay::GameObject::Sptr roomTwo;
	Gameplay::GameObject::Sptr roomThree;
	Gameplay::GameObject::Sptr roomFour;
	Gameplay::GameObject::Sptr roomFive;
	Gameplay::GameObject::Sptr roomSix;

	Gameplay::GameObject::Sptr menu;

	AudioEngine* audioEngine = AudioEngine::instance();

	AudioSource twin;
	AudioSource master;
	AudioSource bath;
	AudioSource living;
	AudioSource kitchen;
	AudioSource basement;

	int randomNumber;
	

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;
	virtual void OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;

	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static WarpBehaviour::Sptr FromJson(const nlohmann::json& data);

	void CrossOffItem(int);

	void PushLines(std::vector<Gameplay::GameObject::Sptr>);
	void PushLists(std::vector <Gameplay::Material::Sptr>);

	MAKE_TYPENAME(WarpBehaviour);
	 
	AudioSource sound;

protected:
	bool _playerInTrigger = false;
	int floorMax = 6;
	int floorSwitchTimer = 0;

	std::vector<std::vector<bool>> listChecks;
	std::vector<Gameplay::Material::Sptr>lists;
	std::vector<Gameplay::GameObject::Sptr>lines;
	std::vector<Gameplay::GameObject::Sptr>linesInUse;

	void InitList();
	void UpdateListForNewRoom();
};