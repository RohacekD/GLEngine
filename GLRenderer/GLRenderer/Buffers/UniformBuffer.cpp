#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffer.h>


namespace GLEngine {
namespace GLRenderer {
namespace Buffers {

//=================================================================================
C_UniformBuffer::C_UniformBuffer(const std::string& blockName, unsigned int index)
	: m_blockName(blockName)
	, m_index(static_cast<GLuint>(index))
	, m_active(false)
{
	C_GLBuffer<GL_UNIFORM_BUFFER>::bind();
	glObjectLabel(GL_BUFFER, m_id, static_cast<GLsizei>(blockName.size()), blockName.c_str());
	C_GLBuffer<GL_UNIFORM_BUFFER>::unbind();
}

//=================================================================================
void C_UniformBuffer::bind() const
{
	C_GLBuffer<GL_UNIFORM_BUFFER>::bind();
	glBindBufferBase(GL_UNIFORM_BUFFER, GetIndex(), m_id);
}

//=================================================================================
void C_UniformBuffer::Activate(bool activate)
{
	if (activate) {
		bind();
		m_active = true;
	}
	else {
		unbind();
		m_active = false;
	}
}

}}}