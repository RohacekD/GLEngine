#include <GLRendererStdafx.h>

#include <GLRenderer/Components/ComponentBuilderFactory.h>

#include <GLRenderer/Components/SkyBox.h>
#include <GLRenderer/Components/StaticMesh.h>
#include <GLRenderer/Components/SkeletalMesh.h>
#include <GLRenderer/Components/GLGeomComponent.h>
#include <GLRenderer/Textures/TextureManager.h>
#include <Entity/Components/EntityDebugComponent.h>

#include <Renderer/Mesh/Loading/SceneLoader.h>

#include <Renderer/Lights/PointLight.h>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
std::unique_ptr<Entity::I_ComponenetBuilder> C_ComponentBuilderFactory::GetFactory(const std::string& name)
{
	if (name == "SkyBox")
	{
		return std::make_unique<C_SkyBoxCompBuilder>();
	}
	if (name == "staticMesh")
	{
		return std::make_unique<C_StaticMeshBuilder>();
	}
	if (name == "debug")
	{
		return std::make_unique<Entity::C_GUIDebugBuilder>();
	}
	if (name == "SkeletalMesh")
	{
		return std::make_unique<C_SkeletalMeshBuilder>();
	}
	if (name == "PointLight")
	{
		return std::make_unique<Renderer::C_PointLightCompBuilder>();
	}
	if (name == "Geometry")
	{
		return std::make_unique<C_GLGeomComponentBuilder>();
	}
	return nullptr;
}

//=================================================================================
void C_ComponentBuilderFactory::ConstructFromFile(std::shared_ptr<Entity::I_Entity> entity, const std::filesystem::path& file)
{
	auto& tmgr = Textures::C_TextureManager::Instance();
	auto sl = std::make_unique<Renderer::Mesh::SceneLoader>();

	auto scene = std::make_shared<Renderer::MeshData::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (!sl->addModelFromFileToScene("Models", file, scene, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}", file);
		return;
	}
	
	for (const auto& mesh : scene->meshes)
	{
		const auto material = scene->materials[mesh.materialIndex];
		const auto meshComp = std::make_shared<C_StaticMesh>(mesh, "basic", entity, &material);
		if (material.textureIndex >= 0)
		{
			auto colorMapTexture = tmgr.CreateTexture(scene->textures[material.textureIndex]);
			if (colorMapTexture)
			{
				colorMapTexture->StartGroupOp();
				colorMapTexture->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
				colorMapTexture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
				colorMapTexture->GenerateMipMaps();

				colorMapTexture->EndGroupOp();

				meshComp->SetColorMap(colorMapTexture);
			}
		}
		if (material.noramlTextureIndex >= 0)
		{
			auto normalMapTexture = tmgr.CreateTexture(scene->textures[material.noramlTextureIndex]);
			if (normalMapTexture)
			{
				normalMapTexture->StartGroupOp();
				normalMapTexture->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
				normalMapTexture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
				normalMapTexture->GenerateMipMaps();

				normalMapTexture->EndGroupOp();

				meshComp->SetNormalMap(normalMapTexture);
			}
		}

		entity->AddComponent(meshComp);
	}

	for (const auto& light : scene->lights)
	{
		const auto lightComp = std::make_shared<Renderer::C_PointLight>(entity, light);
		entity->AddComponent(lightComp);
	}
}

}