#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

class C_StaticMesh : public Renderer::I_RenderableComponent {
public:
	C_StaticMesh(std::string meshFile);
	virtual void PerformDraw() const override;
protected:
	std::string										m_meshFile;
	std::shared_ptr<Mesh::C_StaticMeshResource>		m_Mesh;
};

}}}