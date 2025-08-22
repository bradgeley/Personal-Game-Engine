// Bradley Christensen - 2025
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"



#if defined(AUDIO_SYSTEM_ENABLED)



class AudioSystem;
struct AudioSystemConfig;



//----------------------------------------------------------------------------------------------------------------------
namespace AudioUtils
{
	//----------------------------------------------------------------------------------------------------------------------
	// Factory function for making an audio system of the correct type based on EngineBuildPreferences
	//
	AudioSystem* MakeAudioSystem(AudioSystemConfig const& config);
};



#endif // AUDIO_SYSTEM_ENABLED
