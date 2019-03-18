#pragma once

#include <Utils/Logging/Logging.h>

#include <string>

namespace Utils {
namespace Logging {

// intentionally not exported
struct S_Data {
	std::string m_Text;
	std::string m_File;
	int			m_Line;
	E_Level		m_Level;
	E_Context	m_Context;
};
}
}
