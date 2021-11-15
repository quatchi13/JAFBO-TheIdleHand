#pragma once
#include "IComponent.h"

/// <summary>
/// Showcases a very simple behaviour that rotates the parent gameobject at a fixed rate over time
/// </summary>
class MoveBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<MoveBehaviour> Sptr;

	MoveBehaviour() = default;
	glm::vec3 MoveSpeed;

	virtual void Update(float deltaTime) override;

	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static MoveBehaviour::Sptr FromJson(const nlohmann::json& data);

	MAKE_TYPENAME(MoveBehaviour);
};




