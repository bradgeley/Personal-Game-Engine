// Bradley Christensen - 2022-2023
#include "Engine/Audio/AudioSystem.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Audio System
//
AudioSystem* g_audioSystem = nullptr;



//----------------------------------------------------------------------------------------------------------------------
AudioSystem::AudioSystem(AudioSystemConfig const& config) : m_config(config)
{

}

