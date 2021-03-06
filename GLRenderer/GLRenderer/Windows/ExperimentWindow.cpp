#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLEnable.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/ComponentBuilderFactory.h>
#include <GLRenderer/Components/SkeletalMesh.h>
#include <GLRenderer/Components/SkyBox.h>
#include <GLRenderer/Debug.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/ImGui/GLImGUILayer.h>
#include <GLRenderer/Lights/GLAreaLight.h>
#include <GLRenderer/Materials/MaterialBuffer.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/PersistentDebug.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/SunShadowMapTechnique.h>
#include <GLRenderer/Textures/TextureUnitManager.h>
#include <GLRenderer/Windows/ExperimentWindow.h>
#include <GLRenderer/Windows/RayTrace.h>

#include <Renderer/Cameras/FreelookCamera.h>
#include <Renderer/Cameras/OrbitalCamera.h>
#include <Renderer/Lights/SunLight.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/ConsoleWindow.h>
#include <GUI/FileDialogWindow.h>

#include <Physics/Primitives/Intersection.h>
#include <Physics/Primitives/Ray.h>

#include <Entity/BasicEntity.h>
#include <Entity/ComponentManager.h>
#include <Entity/EntitiesWindow.h>
#include <Entity/IComponent.h>

#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/EventDispatcher.h>

#include <pugixml.hpp>

#include <imgui.h>

namespace GLEngine::GLRenderer::Windows {

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
	, m_LayerStack(std::string("ExperimentalWindowLayerStack"))
	, m_Samples("Frame Times")
	, m_GammaSlider(2.2f, 1.f, 5.f, "Gamma")
	, m_ExposureSlider(1.f, .1f, 10.f, "Exposure")
	, m_VSync(false)
	, m_HDRFBO(nullptr)
	, m_World(std::make_shared<Entity::C_EntityManager>())
	, m_MainPass(nullptr)
	, m_ShadowPass(nullptr)
	, m_GUITexts({{GUI::C_FormatedText("Avg frame time {:.2f}"), GUI::C_FormatedText("Avg fps {:.2f}"), GUI::C_FormatedText("Min/max frametime {:.2f}/{:.2f}")}})
	, m_Windows(std::string("Windows"))
	, m_EditorLayer(*&C_DebugDraw::Instance(), GetInput(), {0,0, GetSize()}) //< viewport could be different from windowsize in the future
{
	glfwMakeContextCurrent(m_Window);

	m_FrameTimer.reset();

	m_ImGUI = new C_GLImGUILayer(m_ID);
	m_ImGUI->OnAttach(); // manual call for now.
	m_LayerStack.PushLayer(m_ImGUI);
	m_LayerStack.PushLayer(&m_EditorLayer);
	m_LayerStack.PushLayer(&m_CamManager);

	m_VSync.SetName("Lock FPS");

	Entity::C_ComponentManager::Instance();
}

//=================================================================================
C_ExplerimentWindow::~C_ExplerimentWindow()
{
	static_cast<C_OGLRenderer*>(m_renderer.get())->DestroyControls(m_ImGUI->GetGUIMgr());
	m_CamManager.DestroyControls(m_ImGUI->GetGUIMgr());
	Renderer::C_MaterialManager::Instance().DestroyControls(m_ImGUI->GetGUIMgr());
};

//=================================================================================
void C_ExplerimentWindow::Update()
{
	m_EditorLayer.SetCamera(m_CamManager.GetActiveCamera());
	m_ImGUI->FrameBegin();
	m_LayerStack.OnUpdate();

	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.Reset();
	Shaders::C_ShaderManager::Instance().Update();
	// MouseSelect();
	C_DebugDraw::Instance().DrawAxis(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0, 1.f, 0.0f), glm::vec3(0.f, 0.f, 1.f));

	const auto avgMsPerFrame = m_Samples.Avg();
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFrametime)].UpdateText(m_Samples.Avg());
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFps)].UpdateText(1000.f / avgMsPerFrame);
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMaxFrametime)].UpdateText(*std::min_element(m_Samples.cbegin(), m_Samples.cend()),
																										*std::max_element(m_Samples.cbegin(), m_Samples.cend()));

	glfwSwapInterval(m_VSync ? 1 : 0);

	m_World->OnUpdate();

	glfwMakeContextCurrent(m_Window);

	// m_ShadowPass->Render();

	const auto camera = m_CamManager.GetActiveCamera();
	GLE_ASSERT(camera, "No active camera");


	m_HDRFBO->Bind<E_FramebufferTarget::Draw>();

	{
		// shadow pass
		m_SunShadow->Render(*m_World.get(), camera.get());
		::ImGui::Begin("shadowMap");
		::ImGui::Image((void*)(intptr_t)m_SunShadow->GetZBuffer()->GetTexture(), ImVec2(256, 256));
		::ImGui::End();
		m_MainPass->SetSunShadowMap(m_SunShadow->GetZBuffer()->GetHandle());
		m_MainPass->SetSunViewProjection(m_SunShadow->GetLastViewProjection());
	}
	m_HDRFBO->Bind<E_FramebufferTarget::Draw>();
	m_MainPass->Render(camera, GetWidth(), GetHeight());

	// ----- Frame init -------
	auto& shmgr = Shaders::C_ShaderManager::Instance();

	shmgr.DeactivateShader();
	// ----- Frame init -------

	{
		RenderDoc::C_DebugScope s("Persistent debug");
		C_PersistentDebug::Instance().DrawAll();
	}

	{
		RenderDoc::C_DebugScope s("Merged debug");
		C_DebugDraw::Instance().DrawMergedGeoms();
	}

	m_HDRFBO->Unbind<E_FramebufferTarget::Draw>();

	{
		using namespace Commands;
		m_renderer->AddCommand(std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth));
		m_renderer->AddCommand(std::make_unique<C_GLViewport>(Renderer::C_Viewport(0, 0, GetSize())));
	}
	m_HDRFBO->Bind<E_FramebufferTarget::Read>();

	auto HDRTexture = m_HDRFBO->GetAttachement(GL_COLOR_ATTACHMENT0);
	auto worldDepth = m_HDRFBO->GetAttachement(GL_DEPTH_ATTACHMENT);

	auto shader = shmgr.GetProgram("screenQuad");
	shmgr.ActivateShader(shader);

	tm.BindTextureToUnit(*(HDRTexture.get()), 0);
	tm.BindTextureToUnit(*(worldDepth.get()), 1);

	m_renderer->AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this, shader]() {
			shader->SetUniform("gamma", m_GammaSlider.GetValue());
			shader->SetUniform("exposure", m_ExposureSlider.GetValue());
			shader->SetUniform("hdrBuffer", 0);
			shader->SetUniform("depthBuffer", 1);
		},
		"Update HDR"));

	m_renderer->AddCommand(std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_ScreenQuad));

	shmgr.DeactivateShader();

	m_HDRFBO->Unbind<E_FramebufferTarget::Read>();
	{
		RenderDoc::C_DebugScope s("ImGUI");
		m_renderer->AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([this, shader]() { m_ImGUI->FrameEnd(); }, "m_ImGUI->FrameEnd"));
	}

	// commit of final commands - from commit few lines above
	m_renderer->SortCommands();
	m_renderer->Commit();
	m_renderer->ClearCommandBuffers();
	glfwSwapBuffers(m_Window);
	sampleTime(float(m_FrameTimer.getElapsedTimeFromLastQueryMilliseconds()));
}

//=================================================================================
void C_ExplerimentWindow::sampleTime(double new_sample)
{
	m_Samples.Sample(static_cast<float>(new_sample));
}

//=================================================================================
void C_ExplerimentWindow::OnEvent(Core::I_Event& event)
{
	RenderDoc::C_DebugScope s("OnEvent-Fail");
	// base can make filtering
	T_Base::OnEvent(event);

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_ExplerimentWindow::OnKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_AppEvent>(std::bind(&C_ExplerimentWindow::OnAppEvent, this, std::placeholders::_1));
	d.Dispatch<Core::C_WindowResizedEvent>(std::bind(&C_ExplerimentWindow::OnWindowResized, this, std::placeholders::_1));

	m_LayerStack.OnEvent(event);
}

//=================================================================================
bool C_ExplerimentWindow::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	// if (event.GetWindowGUID() != GetGUID()) {
	// 	return false;
	// }


	return false;
}

//=================================================================================
void C_ExplerimentWindow::OnAppInit()
{
	m_MainPass = std::make_unique<C_MainPassTechnique>(m_World);
	{
		using namespace Commands;
		m_renderer->AddCommand(std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::DEPTH_TEST));
		m_renderer->AddCommand(std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::CULL_FACE));
		m_renderer->AddCommand(std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::BLEND));
		m_renderer->AddCommand(std::make_unique<HACK::C_LambdaCommand>([]() { glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }));
		m_renderer->AddCommand(std::make_unique<C_GLClearColor>(Colours::black));
		m_renderer->AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Front));
	}

	Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_ModelData>("modelData");
	{
		// billboard
		Renderer::MeshData::Mesh billboardMesh;
		billboardMesh.vertices.emplace_back(-1.f, 1.f, 0, 1);  // 1
		billboardMesh.vertices.emplace_back(-1.f, -1.f, 0, 1); // 2
		billboardMesh.vertices.emplace_back(1.0f, 1.0f, 0, 1); // 3
		billboardMesh.vertices.emplace_back(-1.f, -1.f, 0, 1); // 4 = 2
		billboardMesh.vertices.emplace_back(1.f, -1.f, 0, 1);  // 5
		billboardMesh.vertices.emplace_back(1.0f, 1.0f, 0, 1); // 6 = 3


		billboardMesh.texcoords.emplace_back(0, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 0);
		billboardMesh.texcoords.emplace_back(1, 1);

		m_ScreenQuad = std::make_shared<Mesh::C_StaticMeshResource>(billboardMesh);
	}
	SetupWorld("Levels/atmosphere.xml");

	m_HDRFBO		= std::make_unique<C_Framebuffer>("HDR");
	auto HDRTexture = std::make_shared<Textures::C_Texture>("hdrTexture");

	HDRTexture->bind();
	// HDRTexture setup
	HDRTexture->SetDimensions({GetWidth(), GetHeight()});
	HDRTexture->SetInternalFormat(Renderer::E_TextureFormat::RGBA16f, GL_RGBA);
	HDRTexture->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
	// ~HDRTexture setup
	m_HDRFBO->AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);
	HDRTexture->unbind();

	auto depthStencilTexture = std::make_shared<Textures::C_Texture>("hdrDepthTexture");

	depthStencilTexture->bind();
	// depthStencilTexture setup
	depthStencilTexture->SetDimensions({GetWidth(), GetHeight()});
	depthStencilTexture->SetInternalFormat(Renderer::E_TextureFormat::D16, GL_DEPTH_COMPONENT);
	depthStencilTexture->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
	// ~depthStencilTexture setup
	m_HDRFBO->AttachTexture(GL_DEPTH_ATTACHMENT, depthStencilTexture);
	depthStencilTexture->unbind();


	auto& guiMGR = m_ImGUI->GetGUIMgr();

	// Console window
	{
		m_ConsoleWindowGUID = NextGUID();

		auto consoleWindow = new GUI::C_ConsoleWindow(m_ConsoleWindowGUID);
		guiMGR.AddCustomWindow(consoleWindow);
		consoleWindow->SetVisible();
	}

	// RT window
	{
		m_RayTraceGUID = NextGUID();

		m_RayTraceWindow = new C_RayTraceWindow(m_RayTraceGUID, m_CamManager.GetActiveCamera());

		guiMGR.AddCustomWindow(m_RayTraceWindow);
		m_RayTraceWindow->SetVisible();
	}

	// Entity window
	{
		m_EntitiesWindowGUID = NextGUID();

		auto entitiesWindow = new Entity::C_EntitiesWindow(m_EntitiesWindowGUID, m_World);
		guiMGR.AddCustomWindow(entitiesWindow);
		entitiesWindow->SetVisible();
	}

	m_FrameStatsGUID = guiMGR.CreateGUIWindow("Frame stats");
	auto* frameStats = guiMGR.GetWindow(m_FrameStatsGUID);

	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFrametime)]);
	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFps)]);
	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMaxFrametime)]);
	frameStats->AddComponent(m_Samples);
	frameStats->AddComponent(m_VSync);
	frameStats->SetVisible(true);
	frameStats->AddMenu(m_Windows);

	m_HDRSettingsGUID = guiMGR.CreateGUIWindow("HDR Settings");
	auto* hdrSettings = guiMGR.GetWindow(m_HDRSettingsGUID);

	hdrSettings->AddComponent(m_GammaSlider);
	hdrSettings->AddComponent(m_ExposureSlider);


	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("HDR Settings", m_HDRSettingsGUID, guiMGR));

	const auto rendererWindow = static_cast<C_OGLRenderer*>(m_renderer.get())->SetupControls(guiMGR);
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Renderer", rendererWindow, guiMGR));

	const auto camManager = m_CamManager.SetupControls(guiMGR);
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Camera manager", camManager, guiMGR));

	const auto materialManager = Renderer::C_MaterialManager::Instance().SetupControls(guiMGR);
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Material manager", materialManager, guiMGR));

	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Open level", [&]() {
		const auto levelSelectorGUID = NextGUID();
		auto*	   levelSelectWindwo = new GUI::C_FileDialogWindow(
			 ".xml", "Select level",
			 [&, levelSelectorGUID](const std::filesystem::path& level) {
				 SetupWorld(level);
				 m_ImGUI->GetGUIMgr().DestroyWindow(levelSelectorGUID);
			 },
			 levelSelectorGUID, "./Levels");
		guiMGR.AddCustomWindow(levelSelectWindwo);
		levelSelectWindwo->SetVisible();
	}));
}

//=================================================================================
bool C_ExplerimentWindow::OnWindowResized(Core::C_WindowResizedEvent& event)
{
	auto HDRTexture = m_HDRFBO->GetAttachement(GL_COLOR_ATTACHMENT0);
	HDRTexture->bind();
	HDRTexture->SetDimensions({event.GetWidth(), event.GetHeight()});
	HDRTexture->SetInternalFormat(Renderer::E_TextureFormat::RGBA16f, GL_RGBA);
	HDRTexture->unbind();

	auto depthStencilTexture = m_HDRFBO->GetAttachement(GL_DEPTH_ATTACHMENT);

	depthStencilTexture->bind();
	depthStencilTexture->SetDimensions({event.GetWidth(), event.GetHeight()});
	depthStencilTexture->SetInternalFormat(Renderer::E_TextureFormat::D16, GL_DEPTH_COMPONENT);
	depthStencilTexture->unbind();

	m_EditorLayer.SetViewport({0, 0, GetSize()});

	return true;
}

//=================================================================================
void C_ExplerimentWindow::SetupWorld(const std::filesystem::path& level)
{
	if (!m_World->LoadLevel(level, std::make_unique<Components::C_ComponentBuilderFactory>()))
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "Level not loaded");
		return;
	}

	AddMandatoryWorldParts();
}

//=================================================================================
void C_ExplerimentWindow::AddMandatoryWorldParts()
{
	m_Player = m_World->GetOrCreateEntity("Player");

	auto player = m_Player.lock();
	if (player)
	{
		float zoom		   = 5.0f;
		auto  playerCamera = std::make_shared<Renderer::Cameras::C_OrbitalCamera>(player);
		playerCamera->setupCameraProjection(0.1f, 2 * zoom * 100, static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()), 90.0f);
		// playerCamera->positionCamera({ 0,1,0 }, { 0,0,1 });
		playerCamera->setupCameraView(zoom, glm::vec3(0.0f), 90, 0);
		// playerCamera->adjustOrientation(20.f, 20.f);
		playerCamera->Update();
		player->AddComponent(playerCamera);
		m_CamManager.ActivateCamera(playerCamera);

		auto debugCam = std::make_shared<Renderer::Cameras::C_OrbitalCamera>(player);
		debugCam->setupCameraProjection(0.1f, 2 * zoom * 100, static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()), 90.0f);
		// playerCamera->positionCamera({ 0,1,0 }, { 0,0,1 });
		debugCam->setupCameraView(zoom, glm::vec3(0.0f), 90, 0);
		debugCam->adjustOrientation(0.f, 0.f);
		debugCam->Update();
		player->AddComponent(debugCam);
		m_CamManager.SetDebugCamera(debugCam);

		// area light
		// auto arealight = std::make_shared<C_GLAreaLight>(player);
		// player->AddComponent(arealight);
		//
		// m_ShadowPass = std::make_shared<C_ShadowMapTechnique>(m_World, std::static_pointer_cast<Renderer::I_Light>( arealight));
	}

	{
		// create default atmosphere
		auto entity	 = m_World->GetOrCreateEntity("atmosphere");
		auto sunComp = std::make_shared<Renderer::C_SunLight>(entity);
		m_SunShadow	 = std::make_shared<C_SunShadowMapTechnique>(sunComp);
		entity->AddComponent(sunComp);
	}
}

//=================================================================================
void C_ExplerimentWindow::MouseSelect()
{
	if (m_ImGUI->CapturingMouse())
	{
		return;
	}

	auto screenCoord = GetInput().GetMousePosition();
	auto camera		 = m_CamManager.GetActiveCamera();

	const auto clipPosition = ToClipSpace({screenCoord.first, screenCoord.second});

	using namespace Physics::Primitives;
	const auto ray = camera->GetRay(clipPosition);

	C_PersistentDebug::Instance().DrawLine(ray.origin, ray.origin + ray.direction, Colours::green);
	const S_RayIntersection inter = m_World->Select(ray);
	if (inter.distance > 0)
	{
		auto entity = m_World->GetEntity(inter.entityId);
		if (entity)
		{
			Core::C_UserEvent event("selected");
			entity->OnEvent(event);
		}
	}
}

//=================================================================================
bool C_ExplerimentWindow::OnAppEvent(Core::C_AppEvent& event)
{
	if (event.GetEventType() == Core::C_AppEvent::E_Type::AppInit)
	{
		OnAppInit();
		return true;
	}
	if (event.GetEventType() == Core::C_AppEvent::E_Type::WindowCloseRequest)
	{
		m_World->ClearLevel();
		auto& guiMGR = m_ImGUI->GetGUIMgr();
		guiMGR.DestroyWindow(m_EntitiesWindowGUID);
		guiMGR.DestroyWindow(m_RayTraceGUID);
		guiMGR.DestroyWindow(m_ConsoleWindowGUID);
		guiMGR.DestroyWindow(m_FrameStatsGUID);
		guiMGR.DestroyWindow(m_HDRSettingsGUID);
		m_ImGUI->OnDetach();
	}

	return false;
}

//=================================================================================
bool C_ExplerimentWindow::CanClose() const
{
	return m_LayerStack.ReadyForDestroy();
}

} // namespace GLEngine::GLRenderer::Windows