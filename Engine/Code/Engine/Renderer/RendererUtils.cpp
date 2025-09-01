// Bradley Christensen - 2022-2025
#include "RendererUtils.h"
#include "Engine/Core/EngineCommon.h"

#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)
#include "D3D11/D3D11Renderer.h"
#endif // RENDERER_D3D11



//----------------------------------------------------------------------------------------------------------------------
uint32_t RendererUtils::InvalidID = UINT32_MAX;



//----------------------------------------------------------------------------------------------------------------------
Renderer* RendererUtils::MakeRenderer(RendererConfig const& config)
{
	#if defined(RENDERER_D3D11)
		return new D3D11Renderer(config);
	#else
		UNUSED(config);
		return nullptr;
	#endif // RENDERER_D3D11
}