// Bradley Christensen - 2022-2026
#include "SBackgroundMusic.h"
#include "SCAudioSystem.h"
#include "Engine/Audio/AudioSystem.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Core/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
const char* BGM_FILEPATH = "Data/Sounds/Music/alex-productions-racing-sport-gaming-racing(chosic.com).mp3";



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Startup()
{
	// AddWriteDependencies<SCAudioSystem, SCAudio>(); todo: reenable if/when Run does more stuff

	PlayBGM();

	g_eventSystem->SubscribeMethod("ToggleBGM", this, &SBackgroundMusic::ToggleBGM);

	g_eventSystem->SubscribeMethod("SetVolume_BGM", this, &SBackgroundMusic::SetVolume_BGM);
	g_devConsole->AddDevConsoleCommandInfo("SetVolume_BGM", "volume", DevConsoleArgType::Float);

	m_ignoreRun = true;
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Shutdown() const
{
	g_eventSystem->UnsubscribeMethod("ToggleBGM", this, &SBackgroundMusic::ToggleBGM);
	g_devConsole->RemoveDevConsoleCommandInfo("SetVolume_BGM");

	SCAudioSystem& audio = g_ecs->GetSingleton<SCAudioSystem>();
	AudioSystem& audioSystem = *audio.GetAudioSystem();

	audioSystem.StopSound(audio.m_bgmSoundID);
}



//----------------------------------------------------------------------------------------------------------------------
bool SBackgroundMusic::ToggleBGM(NamedProperties&)
{
	SCAudioSystem& audio = g_ecs->GetSingleton<SCAudioSystem>();
	AudioSystem& audioSystem = *audio.GetAudioSystem();

	if (!audioSystem.IsValidSoundID(audio.m_bgmSoundID))
	{
		PlayBGM();
	}
	else
	{
		audioSystem.TogglePaused(audio.m_bgmSoundID);
	}

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SBackgroundMusic::SetVolume_BGM(NamedProperties& args)
{
	SCAudioSystem& audio = g_ecs->GetSingleton<SCAudioSystem>();
	AudioSystem& audioSystem = *audio.GetAudioSystem();

	audio.m_bgmVolume = args.Get("volume", audio.m_bgmVolume);
	audioSystem.SetSoundVolume(audio.m_bgmSoundID, audio.m_bgmVolume);
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::PlayBGM()
{
	SCAudioSystem& audio = g_ecs->GetSingleton<SCAudioSystem>();
	AudioSystem& audioSystem = *audio.GetAudioSystem();

	if (!audioSystem.IsSoundPlaying(audio.m_bgmSoundID))
	{
		audio.m_bgmSoundID = audioSystem.PlaySoundFromFile(BGM_FILEPATH, true, audio.m_bgmVolume);
	}
}
