// Bradley Christensen - 2025
#pragma once



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

