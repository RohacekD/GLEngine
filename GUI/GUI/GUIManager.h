#pragma once

#include <GUI/GUIApi.h>

namespace GLEngine::GUI {
namespace Menu {
	class C_MenuItem;
}
class C_Window;
}

namespace GLEngine::GUI {
class GUI_API_EXPORT C_GUIManager
{
public:
	C_GUIManager();
	~C_GUIManager();
	C_GUIManager(const C_GUIManager&) = delete;
	C_GUIManager(C_GUIManager&&) = delete;

	GUID CreateGUIWindow(const std::string& name);
	void AddCustomWindow(C_Window* window);

	// modal windows coming

	C_Window* GetWindow(GUID guid) const;

	void DestroyWindow(GUID guid);
	void OnUpdate();

	template<class T, class ... Args>
	std::reference_wrapper<GUI::Menu::C_MenuItem> CreateMenuItem(Args&&... args);
private:
#pragma warning(push)
#pragma warning( disable : 4251)
	std::unordered_map<GUID, GUI::C_Window*>						m_Windwos;
	std::vector<std::unique_ptr<GUI::Menu::C_MenuItem>>	m_MenuItems;
#pragma warning(pop)
};

}

#include <GUI/GUIManager.inl>
