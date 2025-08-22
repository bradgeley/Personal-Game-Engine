// Bradley Christensen - 2025
#include "SBackgroundMusic.h"
#include "Engine/Audio/AudioSystem.h"
#include "Engine/Events/EventSystem.h"
#include "SCAudio.h"



//----------------------------------------------------------------------------------------------------------------------
const char* BGM_FILEPATH = "Data/Sounds/Music/alex-productions-racing-sport-gaming-racing(chosic.com).mp3";



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Startup()
{
	AddWriteDependencies<AudioSystem, SCAudio>();

	PlayBGM();

	g_eventSystem->SubscribeMethod("ToggleBGM", this, &SBackgroundMusic::ToggleBGM);
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Run(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Shutdown()
{
	g_eventSystem->UnsubscribeMethod("ToggleBGM", this, &SBackgroundMusic::ToggleBGM);

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
void SBackgroundMusic::PlayBGM()
{
	SCAudio& scAudio = g_ecs->GetSingleton<SCAudio>();
	if (!g_audioSystem->IsSoundPlaying(scAudio.m_bgmSoundID))
	{
		scAudio.m_bgmSoundID = g_audioSystem->PlaySoundFromFile(BGM_FILEPATH, true, scAudio.m_bgmVolume);
	}
}
