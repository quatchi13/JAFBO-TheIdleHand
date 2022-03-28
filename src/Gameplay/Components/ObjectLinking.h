#pragma once
#include "IComponent.h"
#include "Gameplay/Scene.h"
#include "Gameplay/GameObject.h"

/// <summary>
/// Showcases a very simple behaviour that rotates the parent gameobject at a fixed rate over time
/// </summary>
class ObjectLinking : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<ObjectLinking> Sptr;
	glm::vec3 PositionOffset;
	ObjectLinking();
	ObjectLinking(Gameplay::GameObject::Sptr);
	
	void LinkObject(Gameplay::GameObject::Sptr);

	virtual void Awake() override;

	virtual void Update(float deltaTime) override;
	void CalculateOffset();
	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static ObjectLinking::Sptr FromJson(const nlohmann::json& data);

	bool currentlyLinked = true;

	MAKE_TYPENAME(ObjectLinking);
protected:
	std::vector<Gameplay::GameObject::Sptr> linkedObjects;
	bool isRoot = false;
	Gameplay::GameObject::Sptr parent;
	
	void UpdateChildren();
};




