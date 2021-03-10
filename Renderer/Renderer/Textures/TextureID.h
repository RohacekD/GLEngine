#pragma once

#include <Renderer/ResourceHandle.h>

namespace GLEngine::Renderer::Textures {
struct TextureID : public C_ResourceHandle<TextureID>
{
private:
	using TexID = unsigned int;
public:
	TextureID();
	TexID GetId() const;

	static TextureID CreateNewID();

	void reset()
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Releasing texture");
	}
private:
	TexID m_Id;
	static TexID nextID;
};
}
