#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/Texture.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Textures {

//=================================================================================
C_Texture::C_Texture(GLenum target)
	: m_bGroupOperations(false)
	, m_target(target)
{
	glGenTextures(1, &m_texture);
}

//=================================================================================
C_Texture::C_Texture(const std::string & name, GLenum target)
	:C_Texture(target)
{
	bind();
	glObjectLabel(GL_TEXTURE, m_texture, static_cast<GLsizei>(name.length()), name.c_str());
	unbind();
}

//=================================================================================
C_Texture::~C_Texture()
{
	glDeleteTextures(1, &m_texture);
}

//=================================================================================
void C_Texture::bind() const
{
	if (!m_bGroupOperations)
		glBindTexture(m_target, m_texture);
}

//=================================================================================
void C_Texture::unbind() const
{
	if (!m_bGroupOperations)
		glBindTexture(m_target, 0);
}

//=================================================================================
void C_Texture::SetWrap(GLint wrapS, GLint wrapT)
{
	bind();
	SetTexParameter(GL_TEXTURE_WRAP_S, wrapS);
	SetTexParameter(GL_TEXTURE_WRAP_T, wrapT);
	unbind();
}

//=================================================================================
void C_Texture::SetFilter(GLint min, GLint mag)
{
	bind();
	SetTexParameter(GL_TEXTURE_MIN_FILTER, min);
	SetTexParameter(GL_TEXTURE_MAG_FILTER, mag);
}

//=================================================================================
void C_Texture::SetTexParameter(GLenum pname, const glm::vec4& value)
{
	bind();
	glTexParameterfv(m_target, pname, glm::value_ptr(value));
	unbind();
}

//=================================================================================
void C_Texture::SetTexParameter(GLenum pname, GLint value)
{
	bind();
	glTexParameteri(m_target, pname, value);
	unbind();
}

//=================================================================================
void C_Texture::GenerateMipMaps()
{
	bind();
	glGenerateMipmap(m_target);
	unbind();
}

}}}