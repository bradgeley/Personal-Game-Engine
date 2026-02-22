// Bradley Christensen - 2022-2026
#include "Engine/Audio/AudioUtils.h"



#if defined(AUDIO_SYSTEM_ENABLED)



#include "Engine/Audio/MiniAudioSystem.h"
#include "Game/Framework/EngineBuildPreferences.h"



//----------------------------------------------------------------------------------------------------------------------
AudioSystem* AudioUtils::MakeAudioSystem(AudioSystemConfig const& config)
{
	#if defined(AUDIO_SYSTEM_USE_MINI_AUDIO)
		return new MiniAudioSystem(config);
	#else
		return nullptr;
	#endif
}



#endif // AUDIO_SYSTEM_ENABLED