#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

class C_GLMakeTextureHandleResident final : public Renderer::I_RenderCommand {
public:
	C_GLMakeTextureHandleResident(std::uint64_t handle, bool makeResident);

	//=================================================================================
	virtual void Commit() override;
	virtual E_Type GetType() const override { return E_Type::StateChange; }
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override
	{
		return nullptr;
	}

private:
	std::uint64_t	m_Handle;
	bool			m_MakeResident;
};

}