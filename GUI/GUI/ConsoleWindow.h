#pragma once

#include <GUI/GUIWindow.h>

#include <Utils/Logging/Data.h>
#include <Utils/Logging/ILogger.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_ConsoleWindow : public C_Window, public Utils::Logging::I_Logger {
public:
	explicit C_ConsoleWindow(GUID guid);
	virtual ~C_ConsoleWindow();

	virtual void Log(Utils::Logging::E_Level level, Utils::Logging::E_Context context, int line, const char* file, const std::string& text) override;
	virtual void Draw() const override;

private:
	//@todo replace with stack vector once we have 'em
	std::array<Utils::Logging::S_Data, 1000> m_Logs;
	std::size_t								 m_LastLogIndex;

	static std::array<ImVec4, 4> s_LevelColors;
};

} // namespace GLEngine::GUI