#include <RendererStdAfx.h>

#include <Renderer/Mesh/Material.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Material::C_Material(std::string name)
	: m_Name(name)
	, m_DirtyFlag(true)
{

}

//=================================================================================
C_Material::C_Material(const C_Material& other)
	: m_Name(other.m_Name)
	, m_ambient(other.m_ambient)
	, m_diffuse(other.m_diffuse)
	, m_specular(other.m_specular)
	, m_AlbedoMap(other.m_AlbedoMap)
	, m_RougnessMap(other.m_RougnessMap)
	, m_NormalMap(other.m_NormalMap)
	, m_shininess(other.m_shininess)
	, m_DirtyFlag(true)
{

}

//=================================================================================
void C_Material::Set(const MeshData::Material& material)
{
	SetName(material.m_Name);
	SetAmbient(material.ambient);
	SetDiffuse(material.diffuse);
	SetSpecular(material.specular);
	SetShininess(material.shininess);
	m_DirtyFlag = true;
}

//=================================================================================
C_Material C_Material::Clone() const
{
	// todo
	C_Material newMat(*this);
	newMat.m_Name += "001";
	return newMat;
}

}
