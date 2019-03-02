#pragma once

#include <Core/CoreMacros.h>

#include <string>
#include <vector>
#include <iostream>

namespace Core {
namespace Logging {

class I_Logger;
enum class E_Context;
enum class E_Level;

//=================================================================================
std::ostream & operator<< (std::ostream &out, const E_Context &c);
std::ostream & operator<< (std::ostream &out, const E_Level &c);

//=================================================================================
class API_EXPORT C_LoggingSystem final {
public:
	static C_LoggingSystem& Instance();

	// delete copy and move constructors and assign operators
	C_LoggingSystem(C_LoggingSystem const&) = delete;             // Copy construct
	C_LoggingSystem(C_LoggingSystem&&) = delete;                  // Move construct
	C_LoggingSystem& operator=(C_LoggingSystem const&) = delete;  // Copy assign
	C_LoggingSystem& operator=(C_LoggingSystem &&) = delete;      // Move assign

	template<class ...Args>
	void Log(Args&&... args);

	void AddLogger(I_Logger* logger);
	void RemoveLogger(I_Logger* logger);
private:
	C_LoggingSystem();
	~C_LoggingSystem();


	std::vector<I_Logger*>* m_Loggers;
};

}
}

#include <Core/Logging/Logging.inl>