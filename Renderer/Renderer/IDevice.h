#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/Textures/TextureID.h>

namespace GLEngine::Renderer {
class I_Device {
public:
	I_Device() = default;
	virtual ~I_Device() = default;

	virtual Textures::TextureID CreateTexture(TextureDescriptor& descriptor) = 0;
};
}

