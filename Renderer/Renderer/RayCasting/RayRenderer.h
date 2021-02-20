#pragma once

#include <Renderer/RayCasting/RayTraceScene.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_TextureViewStorage;
class C_STDSampler;
class RENDERER_API_EXPORT C_RayRenderer {
public:
	C_RayRenderer(const C_RayTraceScene& scene);
	~C_RayRenderer();
	void SetDirectionalDebug(C_OctahedralTextureView view) { m_DirectionsView.emplace(view); }

	void Render(I_CameraComponent& camera, I_TextureViewStorage& storage);

private:
	void								   AddSample(const glm::ivec2 coord, C_TextureView view, const glm::vec3 sample);
	glm::vec3							   PathTrace(const Physics::Primitives::S_Ray& ray, C_STDSampler& rnd);
	const C_RayTraceScene&				   m_Scene;
	std::optional<C_OctahedralTextureView> m_DirectionsView;
	I_TextureViewStorage*				   m_Texture;
};
} // namespace GLEngine::Renderer
