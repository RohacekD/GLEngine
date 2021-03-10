#pragma once

#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/TextureID.h>

namespace GLEngine::Renderer
{
class C_Material
{
public:
	explicit C_Material(std::string name);
	C_Material Clone() const;

	C_Material(const C_Material& other);

	void Set(const MeshData::Material& material);
	bool IsDirty() const { return m_DirtyFlag; }
	void SetClean() { m_DirtyFlag = false; }

	const std::string& GetName() const { return m_Name; }
	void SetName(const std::string& val) { m_Name = val; m_DirtyFlag = true; }
	const glm::vec4& GetAmbient() const { return m_ambient; }
	void SetAmbient(const glm::vec4& val) { m_ambient = val; m_DirtyFlag = true; }
	const glm::vec4& GetDiffuse() const { return m_diffuse; }
	void SetDiffuse(const glm::vec4& val) { m_diffuse = val; m_DirtyFlag = true; }
	const glm::vec4& GetSpecular() const { return m_specular; }
	void SetSpecular(const glm::vec4& val) { m_specular = val; m_DirtyFlag = true; }
	const Textures::TextureID& SetAlbedoMap() const { return m_AlbedoMap; }
	void GetAlbedoMap(const Textures::TextureID& val) { m_AlbedoMap = val; m_DirtyFlag = true; }
	const Textures::TextureID& GetRougnessMap() const { return m_RougnessMap; }
	void SetRougnessMap(const Textures::TextureID& val) { m_RougnessMap = val; m_DirtyFlag = true; }
	const Textures::TextureID& GetNormalMap() const { return m_NormalMap; }
	void SetNormalMap(const Textures::TextureID& val) { m_NormalMap = val; m_DirtyFlag = true; }
	const float& GetShininess() const { return m_shininess; }
	void SetShininess(const float& val) { m_shininess = val; m_DirtyFlag = true; }
private:
	std::string			m_Name;
	glm::vec4			m_ambient;
	glm::vec4			m_diffuse;
	glm::vec4			m_specular;
	Textures::TextureID m_AlbedoMap;
	Textures::TextureID m_RougnessMap;
	Textures::TextureID m_NormalMap;
	float				m_shininess;	// Defines the shininess of a phong-shaded material. This is actually the exponent of the phong specular equation
						//http://assimp.sourceforge.net/lib_html/materials.html


	bool				m_DirtyFlag : 1;
};

}

