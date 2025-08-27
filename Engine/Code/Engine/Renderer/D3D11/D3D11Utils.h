// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/RendererSettings.h"
#include <dxgiformat.h>



#if defined(_DEBUG)
struct IDXGIDebug;
#endif

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;