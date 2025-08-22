// Bradley Christensen - 2025
#include "SBackgroundMusic.h"
#include "Engine/Audio/AudioSystem.h"
#include "SCAudio.h"



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Startup()
{
	AddWriteDependencies<AudioSystem, SCAudio>();

	SCAudio& scAudio = g_ecs->GetSingleton<SCAudio>();
	scAudio.m_bgmSoundID = g_audioSystem->PlaySoundFromFile("Data/Sounds/Music/alex-productions-racing-sport-gaming-racing(chosic.com).mp3", true, scAudio.m_bgmVolume);
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Run(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundMusic::Shutdown()
{
	SCAudio& scAudio = g_ecs->GetSingleton<SCAudio>();
	g_audioSystem->StopSound(scAudio.m_bgmSoundID);
}
