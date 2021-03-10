#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer {
struct S_Material : public Shaders::I_GLSLStruct<S_Material>
{
	S_Material();

	glm::vec4			m_ambient;
	glm::vec4			m_diffuse;
	glm::vec4			m_specular;
	std::uint64_t		m_AlbedoMap;
	std::uint64_t		m_RougnessMap;
	std::uint64_t		m_NormalMap;
	float				m_shininess;
	float				m_Roughness;

	Shaders::C_StructDescriptor GetDescriptionImpl() const;
	std::string GetNameImpl() const;
};
}