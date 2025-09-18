// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



class Renderer;
struct RendererConfig;



//----------------------------------------------------------------------------------------------------------------------
typedef uint32_t RenderTargetID;
typedef uint32_t ConstantBufferID;
typedef uint32_t VertexBufferID;
typedef uint32_t InstanceBufferID;
typedef uint32_t ShaderID;
typedef uint32_t TextureID;
typedef uint32_t SwapchainID;
typedef uint32_t FontID;



//----------------------------------------------------------------------------------------------------------------------
namespace RendererUtils
{
	Renderer* MakeRenderer(RendererConfig const& config);

	extern uint32_t InvalidID;
}
