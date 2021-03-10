#include <GLRendererStdAfx.h>

#include <GLRenderer/Material/MaterialUBO.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_MaterialBuffer::C_MaterialBuffer(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
{
	constexpr auto materialSize = sizeof(m_Material);

	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, materialSize, &m_Material, GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_MaterialBuffer::UploadData() const
{
	bind();
	constexpr auto materialSize = sizeof(m_Material);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, materialSize, &m_Material);
	unbind();
}

}
