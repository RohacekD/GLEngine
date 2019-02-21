#include <CoreStdafx.h>

#include <Core/Logging/Logging.h>

#include <Core/Logging/ILogger.h>

namespace Core {
namespace Logging {

//=================================================================================
std::ostream & operator<< (std::ostream &out, const E_Context &c)
{
	const char* text = nullptr;
	switch (c)
	{
	default:
		text = "UnknownContext";
		break;
	case E_Context::Core:
		text = "Core";
		break;
	case E_Context::Render:
		text = "Render";
		break;
	}
	out << text;
	return out;
}

//=================================================================================
std::ostream & operator<< (std::ostream &out, const E_Level &c)
{
	const char* text = nullptr;
	switch (c)
	{

	default:
		text = "UnknownLevel";
		break;
	case E_Level::Error:
		text = "Error";
		break;
	case E_Level::Warning:
		text = "Warning";
		break;
	case E_Level::Info:
		text = "Info";
		break;
	case E_Level::Debug:
		text = "Debug";
		break;
	}
	out << text;
	return out;
}

//=================================================================================
C_LoggingSystem& C_LoggingSystem::Instance()
{
	static C_LoggingSystem myInstance;
	return myInstance;
}

//=================================================================================
C_LoggingSystem::C_LoggingSystem()
	: m_Loggers(new std::remove_pointer<decltype(m_Loggers)>::type)
{
	m_Loggers->emplace_back(new C_CoutLogger());
}

//=================================================================================
C_LoggingSystem::~C_LoggingSystem()
{
	delete m_Loggers;
}

}
}