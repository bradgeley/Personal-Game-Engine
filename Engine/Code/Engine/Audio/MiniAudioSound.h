// Bradley Christensen - 2025
#pragma once
#include "Engine/Audio/Sound.h"



struct MiniAudioSoundImpl;
class MiniAudioSystem;



//----------------------------------------------------------------------------------------------------------------------
class MiniAudioSound : public Sound
{
public:

	MiniAudioSound(const char* filepath);
	~MiniAudioSound();

	virtual void Play() override;
	virtual void Stop() override;
	virtual void SetVolume(float volume) override;
	virtual void SetLooping(bool looping) override;

public:

	MiniAudioSoundImpl* m_miniSoundImpl;
};

