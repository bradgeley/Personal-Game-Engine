// Bradley Christensen - 2022-2025
#include "RenderTarget.h"
#include "Engine/Core/ErrorUtils.h"
#include "Renderer.h"
#include "Swapchain.h"
#include "Texture.h"



//----------------------------------------------------------------------------------------------------------------------
void RenderTarget::ReleaseResources()
{
	g_renderer->ReleaseTexture(m_backbufferTexture);
	g_renderer->ReleaseTexture(m_depthBuffer);
	g_renderer->ReleaseSwapchain(m_swapchain);
}



//----------------------------------------------------------------------------------------------------------------------
void RenderTarget::Present()
{
	Texture* backbuffer = g_renderer->GetTexture(m_backbufferTexture);
	Swapchain* swapchain = g_renderer->GetSwapchain(m_swapchain);
	ASSERT_OR_DIE(backbuffer && swapchain, "Trying to present a RT with no backbuffer or swapchain");

	backbuffer->CopyTo(swapchain);
	swapchain->Present();
}
