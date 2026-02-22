// Bradley Christensen - 2022-2026
#include "Engine/Audio/AudioSystem.h"



#if defined(AUDIO_SYSTEM_ENABLED)



//----------------------------------------------------------------------------------------------------------------------
// THE Audio System
//
AudioSystem* g_audioSystem = nullptr;
SoundID AudioSystem::s_invalidSoundID = UINT32_MAX;



//----------------------------------------------------------------------------------------------------------------------
AudioSystem::AudioSystem(AudioSystemConfig const& config) : m_config(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	g_audioSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
SoundID AudioSystem::RequestSoundID()
{
	return m_nextSoundId++;
}



#endif // AUDIO_SYSTEM_ENABLED