// Bradley Christensen - 2025
#pragma once
#include "Engine/Audio/AudioSystem.h"



struct ma_engine;



//----------------------------------------------------------------------------------------------------------------------
class MiniAudioSystem : public AudioSystem
{
public:

    MiniAudioSystem(AudioSystemConfig const& config);

    void Startup() override;
    void BeginFrame() override;
    void Update(float deltaSeconds) override;
    void EndFrame() override;
    void Shutdown() override;

    ma_engine* GetEngine() const;

    virtual void PlaySoundFromFile(const char* filepath) override;

public:

    ma_engine* m_miniAudioEngine = nullptr;
};

