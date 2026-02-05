// Bradley Christensen - 2022-2025
#include "SBackgroundMusic.h"
#include "SCAudio.h"
#include "Engine/Audio/AudioSystem.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/DataStructures/NamedProperties.h"



//----------------------------------------------------------------------------------------------------------------------
const char* BGM_FILEPATH = "Data/Sounds/Music/alex-productions-racing-sport-gaming-racing(chosic.com).mp3";



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Startup()
{
	// AddWriteDependencies<AudioSystem, SCAudio>(); todo: reenable if/when Run does more stuff

	PlayBGM();

	g_eventSystem->SubscribeMethod("ToggleBGM", this, &SBackgroundMusic::ToggleBGM);

	g_eventSystem->SubscribeMethod("SetVolume_BGM", this, &SBackgroundMusic::SetVolume_BGM);
	g_devConsole->AddDevConsoleCommandInfo("SetVolume_BGM", "volume", DevConsoleArgType::Float);
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Run(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Shutdown()
{
	g_eventSystem->UnsubscribeMethod("ToggleBGM", this, &SBackgroundMusic::ToggleBGM);
	g_devConsole->RemoveDevConsoleCommandInfo("SetVolume_BGM");

	SCAudio& scAudio = g_ecs->GetSingleton<SCAudio>();
	g_audioSystem->StopSound(scAudio.m_bgmSoundID);
}



//----------------------------------------------------------------------------------------------------------------------
bool SBackgroundMusic::ToggleBGM(NamedProperties&)
{
	SCAudio& scAudio = g_ecs->GetSingleton<SCAudio>();
	if (!g_audioSystem->IsValidSoundID(scAudio.m_bgmSoundID))
	{
		PlayBGM();
	}
	else
	{
		g_audioSystem->TogglePaused(scAudio.m_bgmSoundID);
	}

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SBackgroundMusic::SetVolume_BGM(NamedProperties& args)
{
	SCAudio& scAudio = g_ecs->GetSingleton<SCAudio>();
	scAudio.m_bgmVolume = args.Get("volume", scAudio.m_bgmVolume);
	g_audioSystem->SetSoundVolume(scAudio.m_bgmSoundID, scAudio.m_bgmVolume);
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::PlayBGM()
{
	SCAudio& scAudio = g_ecs->GetSingleton<SCAudio>();
	if (!g_audioSystem->IsSoundPlaying(scAudio.m_bgmSoundID))
	{
		scAudio.m_bgmSoundID = g_audioSystem->PlaySoundFromFile(BGM_FILEPATH, true, scAudio.m_bgmVolume);
	}
}
