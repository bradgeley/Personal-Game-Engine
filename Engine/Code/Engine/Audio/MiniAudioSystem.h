// Bradley Christensen - 2025
#pragma once
#include "Engine/Audio/AudioSystem.h"



#if defined(AUDIO_SYSTEM_ENABLED) && defined(AUDIO_SYSTEM_USE_MINI_AUDIO)



struct MiniAudioImpl;



//----------------------------------------------------------------------------------------------------------------------
class MiniAudioSystem : public AudioSystem
{
public:

    MiniAudioSystem(AudioSystemConfig const& config);

    virtual void Startup() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void EndFrame() override;
    virtual void Shutdown() override;

    // Play sound
    virtual SoundID PlaySoundFromFile(const char* filepath, bool looping = false, float volume = 1.f) override;

    // Stop sound
    virtual void StopSound(SoundID id) override;

    // Configure playing sound
    virtual void SetSoundVolume(SoundID id, float volume) override;
    virtual void SetSoundLooping(SoundID id, bool looping) override;

public:

    MiniAudioImpl* m_miniAudioImpl = nullptr;
};



#endif // AUDIO_SYSTEM_ENABLED && AUDIO_SYSTEM_USE_MINI_AUDIO