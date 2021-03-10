#include <GLRendererStdAfx.h>

#include <GLRenderer/Material/MaterialStruct.h>

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>

namespace GLEngine::GLRenderer {

//=================================================================================
const auto matReg = []() {
	Shaders::C_ShaderTypesReflection::Instance().Register<S_Material>();
	return true;
}();

//=================================================================================
S_Material::S_Material()
	: m_ambient()
	, m_diffuse()
	, m_specular()
	, m_AlbedoMap()
	, m_RougnessMap()
	, m_NormalMap()
	, m_shininess()
	, m_Roughness()
{

}

//=================================================================================
Shaders::C_StructDescriptor S_Material::GetDescriptionImpl() const
{
	Shaders::C_StructDescriptor ret("Material");
	ret.Push("ambient", &S_Material::m_ambient);
	ret.Push("diffuse", &S_Material::m_diffuse);
	ret.Push("specular", &S_Material::m_specular);
	ret.Push("AlbedoMap", &S_Material::m_AlbedoMap);
	ret.Push("RougnessMap", &S_Material::m_RougnessMap);
	ret.Push("NormalMap", &S_Material::m_NormalMap);
	ret.Push("shininess", &S_Material::m_shininess);
	ret.Push("roughness", &S_Material::m_Roughness);
	return ret;
}

//=================================================================================
std::string S_Material::GetNameImpl() const
{
	return "Material";
}

}
