// Bradley Christensen - 2022-2026
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"



#if defined(AUDIO_SYSTEM_ENABLED)



#include "Engine/Core/EngineSubsystem.h"



typedef uint32_t SoundID;



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
	virtual ~AudioSystem() override;

	// Sound queries
	virtual bool IsValidSoundID(SoundID id) const = 0;
	virtual bool IsSoundPlaying(SoundID id) const = 0;
	virtual bool IsSoundPaused(SoundID id) const = 0;

	// Play sound
	virtual SoundID PlaySoundFromFile(const char* filepath, bool looping = false, float volume = 1.f) = 0;
	virtual bool ResumeSound(SoundID id) = 0; // return value = whether the sound is playing or not
	virtual bool TogglePaused(SoundID id) = 0;

	// Stop sound
	virtual void PauseSound(SoundID id) = 0;
	virtual void StopSound(SoundID id) = 0;

	// Configure playing sound
	virtual void SetSoundVolume(SoundID id, float volume) = 0;
	virtual void SetSoundLooping(SoundID id, bool looping) = 0;

protected:

	SoundID RequestSoundID();

public:

	AudioSystemConfig const m_config;

public:

	SoundID m_nextSoundId = 0;
	static SoundID s_invalidSoundID;
};



#endif // AUDIO_SYSTEM_ENABLED