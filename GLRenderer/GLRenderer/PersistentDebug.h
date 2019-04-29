#pragma once

#include <chrono>
#include <GLRenderer/Debug.h>


namespace GLEngine {
namespace GLRenderer {

class C_PersistentDebug {
public:
	C_PersistentDebug(C_PersistentDebug const&) = delete;
	void operator=(C_PersistentDebug const&) = delete;
	static C_PersistentDebug& Instance();
	~C_PersistentDebug() = default;

	//template<class ... Args>
	//void DrawLine(Args&&... args) {
	//	m_Calls.push_back({ std::chrono::system_clock::now(), [=]() {
	//		C_DebugDraw::Instance().DrawLine(std::forward<Args>(args));
	//	} });
	//}

	void DrawLine(glm::vec4& A, glm::vec4& B, glm::vec3& color) {
		m_Calls.push_back({ std::chrono::system_clock::now(), [=]() {
			C_DebugDraw::Instance().DrawLine(A,B,color);
		} });
	}

	void DrawAll() {
		m_Calls.erase(
			std::remove_if(m_Calls.begin(), m_Calls.end(), [](std::pair<T_TimeStamp, std::function<void()>>& data) {
				if (data.first + std::chrono::seconds(5) < std::chrono::system_clock::now()) {
					return true;
				}
				return false;
			})
			,m_Calls.end());

		std::for_each(m_Calls.begin(), m_Calls.end(), [](std::pair<T_TimeStamp, std::function<void()>>& data) {
			data.second();
		});
	}

private:
	C_PersistentDebug() = default;

	using T_TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
	std::vector<std::pair<T_TimeStamp, std::function<void()>>> m_Calls;
};

}}