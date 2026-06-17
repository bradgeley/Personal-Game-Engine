// Bradley Christensen - 2022-2025
#pragma once



typedef uint32_t SoundID;



class AudioSystem;



//----------------------------------------------------------------------------------------------------------------------
class SCAudioSystem
{
public:

	AudioSystem const* GetAudioSystem() const		{ return m_audioSystem; }
	AudioSystem* GetAudioSystem()					{ return m_audioSystem; }
	void SetAudioSystem(AudioSystem* audioSystem)	{ m_audioSystem = audioSystem; }

public:

	// Background Music
	float m_bgmVolume = 0.02f;
	SoundID m_bgmSoundID;

private:

	AudioSystem* m_audioSystem = nullptr;
};