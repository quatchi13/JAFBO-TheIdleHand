#pragma once
#include "IComponent.h"

/// <summary>
/// Showcases a very simple behaviour that rotates the parent gameobject at a fixed rate over time
/// </summary>
class TemplateBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<TemplateBehaviour> Sptr;

	TemplateBehaviour() = default;
	

	virtual void Update(float deltaTime) override;

	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static TemplateBehaviour::Sptr FromJson(const nlohmann::json& data);

	MAKE_TYPENAME(TemplateBehaviour);
};

