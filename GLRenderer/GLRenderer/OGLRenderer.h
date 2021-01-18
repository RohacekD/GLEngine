#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderer.h>

#include <GUI/PlotLine.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Text.h>
#include <GUI/Menu/Menu.h>
#include <GUI/Input/Button.h>

namespace GLEngine {
namespace GUI
{
class C_MenuItem;
}

namespace GUI::Menu
{
class C_MenuItem;
}

namespace GLRenderer {

class C_OGLRenderer : public Renderer::I_Renderer {
public:
	C_OGLRenderer();
	virtual ~C_OGLRenderer() override;

	//=================================================================================
	// Renderer::I_Renderer
	//=================================================================================
	virtual void AddCommand(Renderer::I_Renderer::T_CommandPtr) override;
	virtual void AddTransferCommand(T_CommandPtr) override;
	virtual void AddBatch(Renderer::I_Renderer::T_BatchPtr) override;

	virtual void SortCommands() override;
	virtual void ExtractData() override;
	virtual void TransformData() override;
	virtual void Commit() const override;
	virtual void ClearCommandBuffers() override;


	//=================================================================================
	virtual void Lock(bool lock = true) override;

	GUID SetupControls(GUI::C_GUIManager& guiMan);
	void DestroyControls(GUI::C_GUIManager& guiMan);

	//=================================================================================
	virtual Renderer::E_PassType GetCurrentPassType() const override;
	virtual void SetCurrentPassType(Renderer::E_PassType type) override;

	bool WantWireframe() const { return m_Wireframe.GetValue(); }
private:
	void CaputreCommands() const;

	bool m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>*	m_CommandQueue;
	std::vector<Renderer::I_Renderer::T_CommandPtr>		m_TransferQueue;
	GUI::C_PlotLine<500>								m_DrawCommands;
	GUI::Input::C_CheckBoxValue					m_CatchErrors;
	GUI::Input::C_CheckBoxValue					m_Wireframe;
	GUI::Input::C_Button						m_ScreenCaptureList;
	bool										m_OutputCommandList = false;
	bool																m_PreviousCatchErrorsVal;
	Renderer::E_PassType								m_CurrentPass;

	enum class E_GUITexts {
		AvgDrawCommands,
		MinMax,
		DrawCalls,
		Last,
	};
	std::array<GUI::C_FormatedText, static_cast<int>(E_GUITexts::Last)>				m_GUITexts;
	GUID																																			m_Window;
	GUI::Menu::C_Menu																													m_Windows;
};

}
}