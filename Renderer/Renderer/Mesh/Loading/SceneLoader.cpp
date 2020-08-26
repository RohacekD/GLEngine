#include <RendererStdafx.h>

#include <Renderer/Mesh/Loading/SceneLoader.h>

#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>

#include <Renderer/Mesh/Scene.h>

namespace GLEngine::Renderer::Mesh {

//=================================================================================
bool SceneLoader::addModelFromFileToScene(const std::filesystem::path& filepath, const std::filesystem::path& filename,
											std::shared_ptr<MeshData::Scene> scene, const glm::mat4& transform)
{
	//Load geometry first
	ModelLoader ml;
	// dr
	ml.Reset();
	std::vector< std::string > texNames;

	const auto fullFilename = filepath / filename;

	bool retval = ml.addModelFromFileToScene(fullFilename, scene, texNames, transform);

	if (!retval)
		return false;

	for (const auto& mesh : scene->meshes)
		scene->bbox.Add(mesh.bbox.getTransformedAABB(mesh.modelMatrix));

	Textures::TextureLoader tl;

	const auto texureRoot = fullFilename.parent_path();

	for (const auto & texName : texNames)
	{
		Renderer::MeshData::Texture t;
		
		retval = tl.loadTexture(texureRoot / texName, t);

		if (!retval)
			return false;

		scene->textures.push_back(t);
	}

	return true;
}

}