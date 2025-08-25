// Bradley Christensen - 2025
#include "RenderTarget.h"
#include "Texture.h"
#include "Swapchain.h"



//----------------------------------------------------------------------------------------------------------------------
void RenderTarget::ReleaseResources()
{
	if (m_backbufferTexture)
	{
		m_backbufferTexture->ReleaseResources();
	}
	if (m_depthBuffer)
	{
		m_depthBuffer->ReleaseResources();
	}
	if (m_swapchain)
	{
		m_swapchain->ReleaseResources();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RenderTarget::Present()
{
	if (m_backbufferTexture && m_swapchain)
	{
		m_backbufferTexture->CopyTo(m_swapchain);
		m_swapchain->Present();
	}
}
