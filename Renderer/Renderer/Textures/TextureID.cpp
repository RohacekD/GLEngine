#include <RendererStdAfx.h>

#include <Renderer/Textures/TextureID.h>

namespace GLEngine::Renderer::Textures {
TextureID::TexID TextureID::nextID = 1;

//=================================================================================
TextureID::TextureID()
	: m_Id(0)
{

}

//=================================================================================
TextureID::TexID TextureID::GetId() const
{
	return m_Id;
}

//=================================================================================
TextureID TextureID::CreateNewID()
{
	TextureID id;
	id.m_Id = nextID++;
	return id;
}

}
