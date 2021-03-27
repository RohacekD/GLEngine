#pragma once

#include <Audio/AudioApi.h>

// fwd declaration
namespace FMOD {
class System;
}

enum FMOD_RESULT;

namespace GLEngine::Audio {

class AUDIO_API_EXPORT C_AudioSystemManager {
public:
	// Singleton stuff
	C_AudioSystemManager(C_AudioSystemManager const&)  = delete;
	void									   operator=(C_AudioSystemManager const&) = delete;
	[[nodiscard]] static C_AudioSystemManager& Instance();


private:
	C_AudioSystemManager();
	~C_AudioSystemManager();

	[[nodiscard]] bool IsError(const FMOD_RESULT result) const;

	FMOD::System* m_System;
};

} // namespace GLEngine::Audio