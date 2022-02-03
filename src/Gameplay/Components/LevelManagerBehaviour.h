#pragma once
#include "IComponent.h"

/// <summary>
/// Showcases a very simple behaviour that rotates the parent gameobject at a fixed rate over time
/// </summary>
class LevelManagerBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<LevelManagerBehaviour> Sptr;

	LevelManagerBehaviour() = default;
	Gameplay::GameObject::Sptr levelPreviews[6];

	virtual void Update(float deltaTime) override;

	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static LevelManagerBehaviour::Sptr FromJson(const nlohmann::json& data);

	MAKE_TYPENAME(LevelManagerBehaviour);
};

