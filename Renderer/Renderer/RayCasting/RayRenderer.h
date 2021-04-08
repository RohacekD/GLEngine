#pragma once

#include <Renderer/RayCasting/RayTraceScene.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_TextureViewStorage;
class RENDERER_API_EXPORT C_RayRenderer {
public:
	C_RayRenderer(const C_RayTraceScene& scene);

	void Render(I_CameraComponent& camera, I_TextureViewStorage& storage) const;

private:
	const C_RayTraceScene& m_Scene;
};
} // namespace GLEngine::Renderer
