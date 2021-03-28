#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>
#include <Audio/ListenerComponent.h>

#include <fmod.hpp>

namespace GLEngine::Audio {

//=================================================================================
C_AudioSystemManager& C_AudioSystemManager::Instance()
{
	static C_AudioSystemManager instance; // Guaranteed to be destroyed.
										  // Instantiated on first use.
	return instance;
}

//=================================================================================
C_AudioSystemManager::C_AudioSystemManager()
	: m_System(nullptr)
	, m_Initialized(false)
	, m_ActiveListener(nullptr)
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_System);
	if (IsError(result))
		return;

	void*	   extradriverdata = nullptr;
	const auto initResult	   = m_System->init(100, FMOD_INIT_NORMAL, extradriverdata);
	if (IsError(initResult))
		return;


	unsigned int version;
	const auto	 getVersionResult = m_System->getVersion(&version);
	if (IsError(getVersionResult))
		return;

	if (version < FMOD_VERSION)
	{
		CORE_LOG(E_Level::Error, E_Context::Audio, "FMOD lib version {} doesn't match header version {}", version, FMOD_VERSION);
		return;
	}

	/*
		Set the distance units. (meters/feet etc).
	*/
	const auto setUnits = m_System->set3DSettings(1.0, 1.f, 1.0f);
	if (IsError(getVersionResult))
		return;

	const auto setNumListeners = m_System->set3DNumListeners(1); // Right now I support only one listener per scene
	if (IsError(setNumListeners))
		return;

	m_Initialized = true;
}

//=================================================================================
C_AudioSystemManager::~C_AudioSystemManager()
{
	GLE_ASSERT(!m_System && !m_Initialized, "Shuting down the application without closing fmod.");
}

//=================================================================================
void C_AudioSystemManager::Done()
{
	if (!m_System || !m_Initialized)
		return;
	FMOD_RESULT result;


	result = m_System->close();
	if (IsError(result))
		return;
	result = m_System->release();
	if (IsError(result))
		return;

	m_Initialized = false;
	m_System	  = nullptr;
}

//=================================================================================
bool C_AudioSystemManager::IsError(const FMOD_RESULT result) const
{
	ReportError(result);
	return result != FMOD_OK;
}

//=================================================================================
void C_AudioSystemManager::ReportError(const FMOD_RESULT result) const
{
	if (result != FMOD_OK)
	{
		CORE_LOG(E_Level::Error, E_Context::Audio, "Unable to initialize audio system. '{}'", result);
	}
}

//=================================================================================
void C_AudioSystemManager::Update()
{
	if (m_ActiveListener)
	{
		const auto	pos			= m_ActiveListener->GetPosition();
		FMOD_VECTOR listenerPos = {pos.x, pos.y, pos.z};
	FMOD_VECTOR forward		= {0.0f, 0.0f, 1.0f};
	FMOD_VECTOR up			= {0.0f, 1.0f, 0.0f};
		FMOD_VECTOR vel			= {0.0f, 0.0f, 0.0f};

		const auto listenerResult = m_System->set3DListenerAttributes(0, &listenerPos, &vel, &forward, &up);
	ReportError(listenerResult);
	}


	const auto updateResult = m_System->update();
	ReportError(updateResult);
}

//=================================================================================
void C_AudioSystemManager::ActivateListener(std::shared_ptr<C_ListenerComponent> listener)
{
	m_ActiveListener = listener;
}

//=================================================================================
std::shared_ptr<C_ListenerComponent> C_AudioSystemManager::GetActiveListener() const
{
	return m_ActiveListener;
}

} // namespace GLEngine::Audio
