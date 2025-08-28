// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "D3D11Swapchain.h"
#include "D3D11Internal.h"
#include "D3D11Renderer.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void D3D11Swapchain::ReleaseResources()
{
	DX_SAFE_RELEASE(m_swapChain)
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Swapchain::Present()
{
	RendererUserSettings perUserSettings = g_renderer->GetPerUserSettings();
	UINT vsync = perUserSettings.m_vsyncEnabled ? 1 : 0;
	m_swapChain->Present(vsync, 0);
}

#endif // RENDERER_D3D11