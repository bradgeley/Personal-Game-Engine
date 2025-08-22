// Bradley Christensen - 2025
#pragma once
#include "Engine/Core/EngineSubsystem.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Audio System
//
extern class AudioSystem* g_audioSystem;



//----------------------------------------------------------------------------------------------------------------------
struct AudioSystemConfig
{

};



//----------------------------------------------------------------------------------------------------------------------
class AudioSystem : public EngineSubsystem
{
public:

	AudioSystem(AudioSystemConfig const& config);

	virtual void PlaySoundFromFile(const char* filepath) = 0;

public:

	AudioSystemConfig const m_config;
};

