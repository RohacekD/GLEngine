#include <RendererStdafx.h>

#include <Renderer/Lights/AreaLight.h>

#include <Utils/Parsing/MatrixParse.h>
#include <Utils/Parsing/ColorParsing.h>

namespace GLEngine::Renderer {

//=================================================================================
C_AreaLight::C_AreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
	, m_Normal(glm::normalize(glm::vec3(0, 0, 1.0)))
	, m_UpVector(glm::normalize(glm::vec3(0, 1.0, 0)))
	, m_WidthSlider(10.f, 0.1f, 10.f, "Width")
	, m_HeightSlider(10.f, 0.1f, 10.f, "Height")
	, m_DiffuseColor("Diffuse colour", glm::vec3(1.f))
	, m_SpecularColor("Spec colour", glm::vec3(1.f)) {}

//=================================================================================
C_AreaLight::~C_AreaLight() = default;

//=================================================================================
Physics::Primitives::C_Frustum C_AreaLight::GetShadingFrustum() const
{
	auto transformMatrix = m_ComponentMatrix;
	if (const auto entity = GetOwner())
	{
		transformMatrix = entity->GetModelMatrix() * transformMatrix;
	}
	Physics::Primitives::C_Frustum ret(transformMatrix[3], m_UpVector, m_Normal, 0.1f, 50.f, 1.0f, 0.f);
	return ret;
}

//=================================================================================
Physics::Primitives::S_AABB C_AreaLight::GetAABB() const
{
	Physics::Primitives::S_AABB aabb;

	const auto dirX = glm::cross(m_Normal, m_UpVector);
	const auto dirY = m_UpVector;

	const auto width = std::sqrt(GetWidth() / 2.0f);
	const auto height = std::sqrt(GetHeight() / 2.0f);

	aabb.Add(+ dirY * height + dirX * width);
	aabb.Add(+ dirY * height - dirX * width);
	aabb.Add(- dirY * height + dirX * width);
	aabb.Add(- dirY * height - dirX * width);

	return aabb;
}

//=================================================================================
void C_AreaLight::DebugDrawGUI()
{
	m_WidthSlider.Draw();
	m_HeightSlider.Draw();
	m_DiffuseColor.Draw();
	m_SpecularColor.Draw();
}

//=================================================================================
std::string_view C_AreaLight::GetDebugComponentName() const
{
	return "Area light";
}

//=================================================================================
bool C_AreaLight::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
glm::vec3 C_AreaLight::DiffuseColour() const
{
	return m_DiffuseColor.GetValue();
}

//=================================================================================
glm::vec3 C_AreaLight::SpecularColour() const
{
	return m_SpecularColor.GetValue();
}

//=================================================================================
std::shared_ptr<GLEngine::Entity::I_Component> C_AreaLightCompBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	auto areaLight = std::make_shared<Renderer::C_AreaLight>(owner);
	const auto translation	= Utils::Parsing::C_MatrixParser::ParseTransformation(node);
	const auto rotation		= Utils::Parsing::C_MatrixParser::ParseRotations(node);
	areaLight->SetComponentMatrix(translation);
	const auto normal	= rotation * glm::vec4(0, 0, 1.0, 1.0);
	const auto up		= rotation * glm::vec4(0, 1.0, 0, 1.0);
	areaLight->m_Normal = glm::normalize(glm::vec3(normal.x, normal.y, normal.z) / normal.w);
	areaLight->m_UpVector = glm::normalize(glm::vec3(up.x, up.y, up.z) / up.w);

	if (const auto widthAttr = node.attribute("width"))
	{
		areaLight->m_WidthSlider = widthAttr.as_float();
	}

	if (const auto heightAttr = node.attribute("height"))
	{
		areaLight->m_HeightSlider = heightAttr.as_float();
	}

	areaLight->m_DiffuseColor = Utils::Parsing::C_ColorParser::ParseColorRGB(node, "diffuseColor");
	areaLight->m_SpecularColor = Utils::Parsing::C_ColorParser::ParseColorRGB(node, "specularColor");

	return areaLight;
}

}