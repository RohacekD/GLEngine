#pragma once

#include <Physics/Primitives/Shapes.h>
#include <Physics/Primitives/AABB.h>

namespace GLEngine::Renderer::MeshData {

//=================================================================================
struct Material
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;

	std::string m_Name;

	//Index to a texture array
	//If negative - material has no texture
	int textureIndex;
};

//=================================================================================
struct Mesh
{
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;

	Physics::Primitives::S_AABB     bbox;
	glm::mat4						modelMatrix;
	unsigned int					materialIndex;
	std::string						m_name; // DR
};

//=================================================================================
struct AnimationData
{
	std::vector<glm::ivec3> jointIndices;
	std::vector<glm::vec3>	weights;
};

//=================================================================================
struct Light
{
	std::string m_name;
	glm::vec3	m_Color;

};

//Texture
//Always R8G8B8A8 format
//=================================================================================
struct Texture
{
	Texture()
		: width(0)
		, height(0)
		, data(nullptr)
	{}

	unsigned int width;
	unsigned int height;

	std::string m_name;

	std::shared_ptr<unsigned char> data;
};

//=================================================================================
struct Scene
{
	//This is the data that interests You
	std::vector<Mesh>				meshes;
	std::vector<Light>				lights;

	std::vector<Material>			materials;
	std::vector<Texture>			textures;

	Physics::Primitives::S_AABB     bbox;
};
}