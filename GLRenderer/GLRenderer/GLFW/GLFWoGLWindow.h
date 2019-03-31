#pragma once

#include <GLRenderer/GLFW/GLFWWindow.h>

#include <Renderer/IRenderer.h>

#include <glm/glm.hpp>

#include <memory>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
}

namespace GLRenderer {
namespace GLFW {

class C_GLFWoGLWindow : public GLFW::C_GLFWWindow {
public:
	C_GLFWoGLWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_GLFWoGLWindow() = default;
	virtual void Update() override;

	virtual const std::unique_ptr<Renderer::I_Renderer>& GetRenderer() const override;
	virtual void OnEvent(Core::I_Event& event) override;

	bool OnKeyPressed(Core::C_KeyPressedEvent& event);
protected:
	virtual void Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};

}
}
}

