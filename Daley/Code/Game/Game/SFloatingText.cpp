// Bradley Christensen - 2022-2026
#include "SFloatingText.h"
#include "SCFloatingText.h"
#include "SCRenderer.h"
#include "Engine/Assets/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SFloatingText::Startup()
{
	AddWriteDependencies<SCFloatingText, SCRenderer>();


}



//----------------------------------------------------------------------------------------------------------------------
void SFloatingText::Run(SystemContext const& context) const
{
    // Read Dependencies

	// Write Dependencies
	SCFloatingText& floatingText = context.GetSingleton<SCFloatingText>();
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	// Update

	for (int i = (int) floatingText.m_floatingTextInstances.size() - 1; i >= 0; --i)
	{
		FloatingTextInstance& instance = floatingText.m_floatingTextInstances[i];

		instance.m_pos += instance.m_velocity * context.m_deltaSeconds;
		instance.m_elapsedSeconds += context.m_deltaSeconds;

		if (instance.m_elapsedSeconds >= instance.m_lifetimeSeconds)
		{
			floatingText.m_floatingTextInstances.erase(floatingText.m_floatingTextInstances.begin() + i);
		}
	}

	// Render 

	Font const* defaultFont = renderer.GetDefaultFont();
	if (!defaultFont)
	{
		return;
	}

	VertexBuffer& immediateVbo = *renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
	immediateVbo.ClearVerts();

	for (auto const& instance : floatingText.m_floatingTextInstances)
	{
		defaultFont->AddVertsForAlignedText2D(immediateVbo, instance.m_pos, Vec2::ZeroVector, instance.m_scale, instance.m_text, instance.m_tint);
	}

	defaultFont->SetRendererState(renderer);
	renderer.DrawVertexBuffer(immediateVbo);
}
