// Bradley Christensen - 2022-2023
#include "Engine/Audio/AudioUtils.h"
#include "Engine/Audio/MiniAudioSystem.h"
#include "Game/Framework/EngineBuildPreferences.h"



//----------------------------------------------------------------------------------------------------------------------
AudioSystem* AudioUtils::MakeAudioSystem(AudioSystemConfig const& config)
{
	#if defined(AUDIO_SYSTEM_USE_MINI_AUDIO)
		return new MiniAudioSystem(config);
	#endif

	return nullptr;
}
