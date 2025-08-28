// Bradley Christensen - 2022-2025
#pragma once

#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>



#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")



#define DX_SAFE_RELEASE(dxResource) if (dxResource) { (dxResource)->Release(); (dxResource) = nullptr; }

#endif // RENDERER_D3D11