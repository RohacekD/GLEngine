#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

namespace GLEngine {
namespace Renderer::MeshData
{
struct Mesh;
}
namespace GLRenderer {
namespace Shaders
{
class C_ShaderProgram;
}

namespace Components {

class C_StaticMesh : public Renderer::I_RenderableComponent {
public:
	C_StaticMesh(std::string meshFile, std::string_view shader);
	C_StaticMesh(const Renderer::MeshData::Mesh& meshFile, std::string_view shader);
	virtual void PerformDraw() const override;
	
protected:
	std::string																			m_meshFile;
	std::shared_ptr<Mesh::C_StaticMeshResource>			m_Mesh;
	std::shared_ptr<Shaders::C_ShaderProgram>				m_Shader;
};

class C_StaticMeshBuilder : public Entity::I_ComponenetBuilder
{
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

}}}