// Bradley Christensen - 2022-2023
#include "Engine/Audio/MiniAudioSystem.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
#define MINIAUDIO_IMPLEMENTATION
#include "ThirdParty/miniaudio/miniaudio.h"



//----------------------------------------------------------------------------------------------------------------------
// This gets rid of warnings about miniaudio unscoped enums
#pragma warning(push)
#pragma warning(disable : 26812)



//----------------------------------------------------------------------------------------------------------------------
MiniAudioSystem::MiniAudioSystem(AudioSystemConfig const& config) : AudioSystem(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::Startup()
{
	m_miniAudioEngine = new ma_engine();
	ma_result result = ma_engine_init(nullptr, m_miniAudioEngine);
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

}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::Shutdown()
{
	ma_engine_uninit(m_miniAudioEngine);

	delete m_miniAudioEngine;
	m_miniAudioEngine = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
ma_engine* MiniAudioSystem::GetEngine() const
{
	return m_miniAudioEngine;
}



//----------------------------------------------------------------------------------------------------------------------
void MiniAudioSystem::PlaySoundFromFile(const char* filepath)
{
	ma_engine_play_sound(m_miniAudioEngine, filepath, nullptr);
}


#pragma warning(pop)