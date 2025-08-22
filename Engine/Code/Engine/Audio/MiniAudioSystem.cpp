// Bradley Christensen - 2022-2023
#include "Engine/Audio/MiniAudioSystem.h"



#if defined(AUDIO_SYSTEM_ENABLED) && defined(AUDIO_SYSTEM_USE_MINI_AUDIO)



#include "Engine/Core/ErrorUtils.h"
#include <unordered_map>



//----------------------------------------------------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#include "ThirdParty/miniaudio/miniaudio.h"



//----------------------------------------------------------------------------------------------------------------------
// This gets rid of warnings about miniaudio unscoped enums
#pragma warning(push)
#pragma warning(disable : 26812)



//----------------------------------------------------------------------------------------------------------------------
// Mini Audio Sound
//
struct MiniAudioSoundImpl
{
	ma_sound m_sound;
};



//----------------------------------------------------------------------------------------------------------------------
// Mini Audio Engine
//
struct MiniAudioImpl
{
	ma_engine m_engine;
	std::unordered_map<SoundID, MiniAudioSoundImpl> m_sounds;
};



//----------------------------------------------------------------------------------------------------------------------
MiniAudioSystem::MiniAudioSystem(AudioSystemConfig const& config) : AudioSystem(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::Startup()
{
	m_miniAudioImpl = new MiniAudioImpl();
	ma_result result = ma_engine_init(nullptr, &m_miniAudioImpl->m_engine);
	ASSERT_OR_DIE(result == MA_SUCCESS, "Mini audio engine failed to init.");
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::BeginFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::Update(float)
{

}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::EndFrame()
{
	// Clean up finished sounds
	for (auto it = m_miniAudioImpl->m_sounds.begin(); it != m_miniAudioImpl->m_sounds.end();)
	{
		bool isAtEnd = ma_sound_at_end(&it->second.m_sound);
		if (isAtEnd)
		{
			ma_sound_uninit(&it->second.m_sound);
			it = m_miniAudioImpl->m_sounds.erase(it);
		}
		else
		{
			it++;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::Shutdown()
{
	// Clean up all sounds
	for (auto& it : m_miniAudioImpl->m_sounds)
	{
		ma_sound_uninit(&it.second.m_sound);
	}
	m_miniAudioImpl->m_sounds.clear();

	ma_engine_uninit(&m_miniAudioImpl->m_engine);

	delete m_miniAudioImpl;
	m_miniAudioImpl = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
SoundID MiniAudioSystem::PlaySoundFromFile(const char* filepath, bool looping /*= false*/, float volume /*= 1.f*/)
{
	SoundID newID = RequestSoundID();
	MiniAudioSoundImpl& newSound = m_miniAudioImpl->m_sounds[newID];
	ma_result result = ma_sound_init_from_file(&m_miniAudioImpl->m_engine, filepath, 0, nullptr, nullptr, &newSound.m_sound);
	if (result == MA_SUCCESS)
	{
		ma_sound_set_volume(&newSound.m_sound, volume);
		ma_sound_set_looping(&newSound.m_sound, looping);
		ma_sound_start(&newSound.m_sound);
		return newID;
	}
	return s_invalidSoundID;
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::StopSound(SoundID id)
{
	auto sound = m_miniAudioImpl->m_sounds.find(id);
	if (sound == m_miniAudioImpl->m_sounds.end())
	{
		return;
	}
	ma_sound_uninit(&sound->second.m_sound);
	m_miniAudioImpl->m_sounds.erase(sound);
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::SetSoundVolume(SoundID id, float volume)
{
	auto sound = m_miniAudioImpl->m_sounds.find(id);
	if (sound == m_miniAudioImpl->m_sounds.end())
	{
		return;
	}
	ma_sound_set_volume(&sound->second.m_sound, volume);
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::SetSoundLooping(SoundID id, bool looping)
{
	auto sound = m_miniAudioImpl->m_sounds.find(id);
	if (sound == m_miniAudioImpl->m_sounds.end())
	{
		return;
	}
	ma_sound_set_looping(&sound->second.m_sound, looping);
}



#pragma warning(pop)



#endif // AUDIO_SYSTEM_ENABLED && AUDIO_SYSTEM_USE_MINI_AUDIO