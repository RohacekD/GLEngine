#include <GUIStdafx.h>

#include <GUI/Input/Transformations.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace GLEngine::GUI::Input {


//=================================================================================
C_Transformations::C_Transformations(glm::mat4 transformation, Utils::C_BitField<E_Transorms> enableTransforms)
	: m_enabledTransforms(enableTransforms)
	, m_Translation("Translate", glm::vec3(0.f))
	, m_Rotation("Rotation", glm::vec3(0.f))
	, m_Scale("Scale", glm::vec3(1.f, 1.f, 1.f))
{
	SetMatrix(transformation);
}

//=================================================================================
void C_Transformations::Draw() const
{
	if (m_enabledTransforms.CheckFlag(E_Transorms::Translate))
	{
		m_Translation.Draw();
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Rotate))
	{
		m_Rotation.Draw();
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Scale))
	{
		m_Scale.Draw();
	}
}

//=================================================================================
glm::mat4 C_Transformations::GetMatrix() const
{
	glm::mat4 transform(1.f);
	if (m_enabledTransforms.CheckFlag(E_Transorms::Translate))
	{
		transform = glm::translate(transform, GetTranslation());
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Rotate))
	{
		const auto rotations = GetRotation();
		transform			 = glm::rotate(transform, rotations.x, glm::vec3(1, 0, 0));
		transform			 = glm::rotate(transform, rotations.y, glm::vec3(0, 1, 0));
		transform			 = glm::rotate(transform, rotations.z, glm::vec3(0, 0, 1));
	}
	if (m_enabledTransforms.CheckFlag(E_Transorms::Scale))
	{
		transform = glm::scale(transform, GetScale());
	}
	return transform;
}

//=================================================================================
glm::vec3 C_Transformations::GetTranslation() const
{
	return m_Translation.GetValue();
}

//=================================================================================
glm::vec3 C_Transformations::GetRotation() const
{
	return m_Rotation.GetValue();
}

//=================================================================================
glm::vec3 C_Transformations::GetScale() const
{
	return m_Scale.GetValue();
}

//=================================================================================
void C_Transformations::SetMatrix(const glm::mat4& mat)
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(mat, scale, rotation, translation, skew, perspective);
	m_Scale		  = scale;
	m_Translation = translation;
	m_Rotation	  = glm::eulerAngles(glm::conjugate(rotation));
}

//=================================================================================
void C_Transformations::SetEnabledTransforms(Utils::C_BitField<E_Transorms> enableTransforms)
{
	m_enabledTransforms = enableTransforms;
}

} // namespace GLEngine::GUI::Input
