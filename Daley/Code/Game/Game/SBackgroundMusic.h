// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SBackgroundMusic : public System
{
public:

    SBackgroundMusic(Name name = "BackgroundMusic", Rgba8 const& debugTint = Rgba8::Violet) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;

protected:

    bool ToggleBGM(NamedProperties& args);
    bool SetVolume_BGM(NamedProperties& args);

private:

    void PlayBGM();
};
