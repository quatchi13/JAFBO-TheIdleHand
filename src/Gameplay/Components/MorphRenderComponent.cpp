#include "Gameplay/Components/MorphRenderComponent.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Utils/ResourceManager/ResourceManager.h"

MorphRenderComponent::MorphRenderComponent() : IComponent() {
	/*_mesh0 = GetGameObject()->Get<RenderComponent>()->GetMesh();
	_mesh1 = _mesh0;

	UpdateVBOs(_mesh0, _mesh1, 0.f);*/
}

MorphRenderComponent::MorphRenderComponent(const Gameplay::MeshResource::Sptr& baseMesh): IComponent()
{
	_mesh0 = baseMesh;
	_mesh1 = baseMesh;
	_t = 0;

	std::cout << "beep boop";

	//UpdateVBOs(_mesh0, _mesh1, 0.f);

}

void MorphRenderComponent::UpdateVBOs(const Gameplay::MeshResource::Sptr& mesh0, const Gameplay::MeshResource::Sptr& mesh1, float t) {
	_t = t;

	VertexArrayObject::Sptr vao = mesh0->Mesh;


	VertexBuffer::Sptr pBuff = mesh1->Mesh->GetBufferBinding(AttribUsage::Position)->Buffer;
	VertexBuffer::Sptr nBuff = mesh1->Mesh->GetBufferBinding(AttribUsage::Normal)->Buffer;

	
	const VertexArrayObject::VertexDeclaration& v_dec = (mesh1->Mesh->GetVDecl());
	

	BufferAttribute posBuff(4, 3, AttributeType::Float, v_dec.at(0).Stride, v_dec.at(0).Offset, AttribUsage::Position);
	BufferAttribute norBuff(5, 3, AttributeType::Float, v_dec.at(2).Stride, v_dec.at(2).Offset, AttribUsage::Normal);

	const std::vector<BufferAttribute> posAttributes{ posBuff};
	const std::vector<BufferAttribute> norAttributes{ norBuff };

	
	vao->AddVertexBuffer(pBuff, posAttributes);
	vao->AddVertexBuffer(nBuff, norAttributes);

	GetGameObject()->Get<RenderComponent>()->SetVAO(vao);
}

nlohmann::json MorphRenderComponent::ToJson() const {
	nlohmann::json result;
	result["_mesh0"] = _mesh0 ? _mesh0->GetGUID().str() : "null";
	result["_mesh1"] = _mesh1 ? _mesh1->GetGUID().str() : "null";
	result["_t"] = _t;
	return result;
}

MorphRenderComponent::Sptr MorphRenderComponent::FromJson(const nlohmann::json& data) {
	MorphRenderComponent::Sptr result = std::make_shared<MorphRenderComponent>();
	result->_mesh0 = ResourceManager::Get<Gameplay::MeshResource>(Guid(data["_mesh0"].get<std::string>()));
	result->_mesh1 = ResourceManager::Get<Gameplay::MeshResource>(Guid(data["_mesh1"].get<std::string>()));
	result->_t = data["_t"];

	return result;
}

void MorphRenderComponent::RenderImGui() {
	
}
