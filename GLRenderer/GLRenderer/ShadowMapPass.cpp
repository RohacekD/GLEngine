#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Lights/GLAreaLight.h>
#include <GLRenderer/ShadowMapPass.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/ILight.h>
#include <Renderer/IRenderableComponent.h>
#include <Renderer/IRenderer.h>

#include <Physics/Primitives/Frustum.h>

#include <Entity/EntityManager.h>
#include <Entity/IEntity.h>

#include <Core/Application.h>

#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine::GLRenderer {

// TODO: broken as hell
//=================================================================================
C_ShadowMapTechnique::C_ShadowMapTechnique(std::shared_ptr<Entity::C_EntityManager> world, std::shared_ptr<Renderer::I_Light>& light)
	: m_WorldToRender(world)
	, m_Light(light)
	, m_ShadowPassFBO("ShadowPassFBO")
{
	m_FrameConstUBO = std::dynamic_pointer_cast<Buffers::UBO::C_FrameConstantsBuffer>(Buffers::C_UniformBuffersManager::Instance().GetBufferByName("frameConst"));
	if (!m_FrameConstUBO)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "UBO with name 'frameConst' either dosn't exists, or doesn't match type");
	}


	auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();
	m_ShadowPassFBO.Bind();
	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			glGenRenderbuffers(1, &m_ColorRenderbuffer);
			if (m_ColorRenderbuffer)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, m_ColorRenderbuffer);

				glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, 512, 512);

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_ColorRenderbuffer);
			}
		},
		"setup renderbuffer for shadow maps"));
	m_ShadowPassFBO.Unbind();
}

//=================================================================================
void C_ShadowMapTechnique::Render()
{
	RenderDoc::C_DebugScope s("C_ShadowMapTechnique::Render");
	auto&					renderer = (Core::C_Application::Get()).GetActiveRenderer();
	renderer.SetCurrentPassType(Renderer::E_PassType::ShadowPass);

	const auto areaLigh = std::dynamic_pointer_cast<C_GLAreaLight>(m_Light);
	const auto frustum	= areaLigh->GetShadingFrustum();
	if (!areaLigh)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Wrong type of light");
		return;
	}


	const auto left	  = glm::normalize(glm::cross(frustum.GetForeward(), frustum.GetUpVector()));
	const auto pos	  = frustum.GetPosition();
	const auto up	  = frustum.GetUpVector();
	const auto width  = areaLigh->GetWidth() / 2.0f;
	const auto height = areaLigh->GetHeight() / 2.0f;

	m_FrameConstUBO->SetView(glm::lookAt(pos, pos + frustum.GetForeward(), up));
	m_FrameConstUBO->SetProjection(glm::ortho(-width, width, -height, height, frustum.GetNear(), frustum.GetFar()));
	m_FrameConstUBO->SetCameraPosition(glm::vec4(frustum.GetPosition(), 1.0f));

	m_ShadowPassFBO.AttachTexture(GL_DEPTH_ATTACHMENT, areaLigh->GetShadowMap());
	m_ShadowPassFBO.Bind<E_FramebufferTarget::Draw>();

	if (m_ShadowPassFBO.NeedCheck())
	{
		// terribly ineffective
		auto	   completeness = m_ShadowPassFBO.CheckCompleteness<E_FramebufferTarget::Draw>();
		const auto lambda		= [](std::future<bool>&& completeness) {
			  if (!completeness.get())
			  {
				  CORE_LOG(E_Level::Error, E_Context::Render, "Shadow map fbo is uncomplete");
			  }
		};

		m_ShadowPassFBO.SetChecked();

		std::thread completenessCheck(lambda, std::move(completeness));
		completenessCheck.detach();
	}


	{
		RenderDoc::C_DebugScope s("Shadow map prepare");
		using namespace Commands;
		renderer.AddCommand(std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth));
		Renderer::C_Viewport shadowMapViewPort(0, 0, 512, 512);
		renderer.AddCommand(std::make_unique<C_GLViewport>(shadowMapViewPort));
		renderer.AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Back));
	}

	{
		RenderDoc::C_DebugScope s("UBO Upload");
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
			[&]() {
				m_FrameConstUBO->UploadData();
				m_FrameConstUBO->Activate(true);
			},
			"ShadowPass UBO upload"));
	}

	const auto entitiesInView = m_WorldToRender->GetEntities(frustum);

	{
		RenderDoc::C_DebugScope s("Commit geometry");
		for (auto& entity : entitiesInView)
		{
			if (auto renderable = entity->GetComponent<Entity::E_ComponentType::Graphical>())
			{
				renderable->PerformDraw();
			}
		}
	}
	m_ShadowPassFBO.Unbind<E_FramebufferTarget::Draw>();

	{
		using namespace Commands;
		renderer.AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Front));
	}
}

} // namespace GLEngine::GLRenderer