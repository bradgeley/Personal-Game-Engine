// Bradley Christensen - 2022-2023
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

enum   D3D11_BLEND					: int;
enum   D3D11_BLEND_OP				: int;
enum   D3D11_FILTER					: int;
enum   D3D11_TEXTURE_ADDRESS_MODE	: int;
enum   D3D11_CULL_MODE				: int;
enum   D3D11_FILL_MODE				: int;



//----------------------------------------------------------------------------------------------------------------------
D3D11_FILTER GetD3D11SamplerFilter(SamplerFilter filter, float& out_maxAnisotropy);
D3D11_TEXTURE_ADDRESS_MODE GetD3D11SamplerAddressMode(SamplerAddressMode addressMode);
D3D11_CULL_MODE GetD3D11CullMode(CullMode cullMode);
D3D11_FILL_MODE GetD3D11FillMode(FillMode fillMode);