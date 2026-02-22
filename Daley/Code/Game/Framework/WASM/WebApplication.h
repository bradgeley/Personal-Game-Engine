// Bradley Christensen - 2022-2026
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(__EMSCRIPTEN__)
#include "Game/Framework/Application.h"



//----------------------------------------------------------------------------------------------------------------------
// WebApplication
//
// WASM app
//
class WebApplication : public Application
{
public:

    virtual void Shutdown() override;
};


#endif // __EMSCRIPTEN
