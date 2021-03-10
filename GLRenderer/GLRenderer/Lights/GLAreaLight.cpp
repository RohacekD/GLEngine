#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/GLAreaLight.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_GLAreaLight::C_GLAreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::C_AreaLight(owner)
	//, m_ShadowMap(Textures::C_TextureManager::Instance().CreateEmptyTexture(owner->GetName() + "_ShadowMap"))
	, m_ShadowMap(Textures::C_TextureManager::Instance().GetIdentityTexture())
{
	// m_ShadowMap->bind();
	// m_ShadowMap->SetDimensions({512, 512});
	// m_ShadowMap->SetInternalFormat(Renderer::E_TextureFormat::D16, GL_DEPTH_COMPONENT);
	// m_ShadowMap->SetFilter(E_OpenGLFilter::Linear, E_OpenGLFilter::Linear);
	// m_ShadowMap->SetWrap(Renderer::E_WrapFunction::ClampToBorder, Renderer::E_WrapFunction::ClampToBorder);
	// m_ShadowMap->unbind();
}

//=================================================================================
const std::shared_ptr<Textures::C_Texture>& C_GLAreaLight::GetShadowMap() const
{
	return m_ShadowMap;
}

//=================================================================================
void C_GLAreaLight::DebugDraw() const
{
	const auto dirX	  = glm::cross(m_Normal, m_UpVector);
	const auto width  = std::sqrt(GetWidth() / 2.0f);
	const auto height = std::sqrt(GetHeight() / 2.0f);

	auto transformMatrix = GetComponentModelMatrix();

	const auto Pos = glm::vec3(transformMatrix[3]);

	C_DebugDraw::Instance().DrawLine(Pos, Pos + m_Normal, glm::vec3(1.f, 1.f, 0.f));

	C_DebugDraw::Instance().DrawLine(Pos + m_UpVector * height + dirX * width, Pos + m_UpVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));
	C_DebugDraw::Instance().DrawLine(Pos - m_UpVector * height + dirX * width, Pos - m_UpVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));

	C_DebugDraw::Instance().DrawPoint(Pos + m_UpVector * height + dirX * width, glm::vec3(0, 1, 0));

	C_DebugDraw::Instance().DrawLine(Pos - m_UpVector * height + dirX * width, Pos + m_UpVector * height + dirX * width, glm::vec3(1.f, 1.f, 0.f));
	C_DebugDraw::Instance().DrawLine(Pos - m_UpVector * height - dirX * width, Pos + m_UpVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));
}

} // namespace GLEngine::GLRenderer