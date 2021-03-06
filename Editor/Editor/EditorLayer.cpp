#include <EditorStdafx.h>

#include <Editor/CurveEditor.h>
#include <Editor/EditorLayer.h>

#include <Renderer/DebugDraw.h>

#include <Core/EventSystem/EventDispatcher.h>

namespace GLEngine::Editor {

//=================================================================================
C_EditorLayer::C_EditorLayer(Renderer::I_DebugDraw& dd, const Core::I_Input& directInput, const Renderer::C_Viewport& viewport)
	: Core::C_Layer("Editor Layer")
	, m_dd(dd)
	, m_Input(directInput)
	, m_CurveEditing(nullptr)
	, m_Viewport(viewport)
{
}

//=================================================================================
C_EditorLayer::~C_EditorLayer() = default;

//=================================================================================
void C_EditorLayer::OnEvent(Core::I_Event& event)
{
	if (m_CurveEditing)
		m_CurveEditing->OnEvent(event);
}

//=================================================================================
void C_EditorLayer::OnUpdate()
{
	if (m_CurveEditing)
	{
		m_CurveEditing->Draw(m_dd);
		if (auto camera = m_Camera.lock())
			m_CurveEditing->OnUpdate(*camera.get(), m_Viewport);
	}
}

//=================================================================================
void C_EditorLayer::EditCurve(Renderer::C_Curve& curve)
{
	m_CurveEditing = std::make_unique<C_CurveEditor>(curve, m_Input);
}

//=================================================================================
void C_EditorLayer::SetCamera(std::shared_ptr<Renderer::I_CameraComponent> camera)
{
	m_Camera = camera;
}

//=================================================================================
void C_EditorLayer::SetViewport(const Renderer::C_Viewport& viewport)
{
	m_Viewport = viewport;
}

}; // namespace GLEngine::Editor