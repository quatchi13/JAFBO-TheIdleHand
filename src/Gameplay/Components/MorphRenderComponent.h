#pragma once
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Material.h"
#include "Utils/MeshFactory.h"


/// <summary>
/// Allows for objects to be animated via morph targets
/// </summary>
class MorphRenderComponent : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<MorphRenderComponent> Sptr;
	MorphRenderComponent();
	MorphRenderComponent(const Gameplay::MeshResource::Sptr& baseMesh);

	void UpdateVBOs(const Gameplay::MeshResource::Sptr& mesh_0, const Gameplay::MeshResource::Sptr& mesh_1, float t);

	// Inherited from IComponent
	//virtual void Awake() override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static MorphRenderComponent::Sptr FromJson(const nlohmann::json& data);
	MAKE_TYPENAME(MorphRenderComponent);

	float						 _t;
protected:
	//The meshes we are currently interpolating between
	Gameplay::MeshResource::Sptr _mesh0;
	Gameplay::MeshResource::Sptr _mesh1;
	//the interpolation value
	
};