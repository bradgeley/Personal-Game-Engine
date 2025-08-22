// Bradley Christensen - 2022-2023
#include "Engine/Audio/MiniAudioSound.h"
#include "Engine/Audio/MiniAudioSystem.h"
#include "ThirdParty/miniaudio/miniaudio.h"



//----------------------------------------------------------------------------------------------------------------------
// This gets rid of warnings about miniaudio unscoped enums
#pragma warning(push)
#pragma warning(disable : 26812)



struct MiniAudioSoundImpl
{
    ma_sound m_sound;
};



//----------------------------------------------------------------------------------------------------------------------
MiniAudioSound::MiniAudioSound(const char* filepath)
{
    MiniAudioSystem* audioSystem = reinterpret_cast<MiniAudioSystem*>(g_audioSystem);
    ma_sound_init_from_file(audioSystem->GetEngine(), filepath, MA_SOUND_FLAG_DECODE, nullptr, nullptr, &m_miniSoundImpl->m_sound);
}



//----------------------------------------------------------------------------------------------------------------------
MiniAudioSound::~MiniAudioSound()
{
    ma_sound_uninit(&m_miniSoundImpl->m_sound);
    delete m_miniSoundImpl;
    m_miniSoundImpl = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSound::Play()
{
    ma_sound_start(&m_miniSoundImpl->m_sound);
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSound::Stop()
{
    ma_sound_stop(&m_miniSoundImpl->m_sound);
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSound::SetVolume(float volume)
{
    ma_sound_set_volume(&m_miniSoundImpl->m_sound, volume);
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSound::SetLooping(bool looping)
{
    ma_sound_set_looping(&m_miniSoundImpl->m_sound, looping);
}



#pragma warning(pop)