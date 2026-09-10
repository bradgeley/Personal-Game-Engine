// Bradley Christensen - 2022-2026
#include "SFloatingText.h"
#include "SCFloatingText.h"
#include "SCRenderer.h"
#include "WorldSettings.h"
#include "Engine/Assets/Font.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/AABB2.h"
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

	float realTimeDeltaSeconds = context.GetRealTimeDeltaSeconds();

	for (int i = (int) floatingText.m_floatingTextInstances.size() - 1; i >= 0; --i)
	{
		FloatingTextInstance& instance = floatingText.m_floatingTextInstances[i];

		instance.m_pos += instance.m_velocity * realTimeDeltaSeconds;
		instance.m_elapsedSeconds += realTimeDeltaSeconds;

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

	static AABB2 visibleWorldBounds = AABB2(Vec2(StaticWorldSettings::s_visibleWorldMinsX, StaticWorldSettings::s_visibleWorldMinsY), Vec2(StaticWorldSettings::s_visibleWorldMaxsX, StaticWorldSettings::s_visibleWorldMaxsY));

	for (FloatingTextInstance& instance : floatingText.m_floatingTextInstances)
	{
		float lineWidth =defaultFont->GetLineWidth(instance.m_scale, instance.m_text);
		Vec2 renderPos = instance.m_pos;
		Vec2 renderDims = Vec2(lineWidth, instance.m_scale);
		AABB2 textBounds = AABB2(renderPos - (renderDims * 0.5f), renderPos + (renderDims * 0.5f));
		textBounds.ClampInside(visibleWorldBounds);
		defaultFont->AddVertsForAlignedText2D(immediateVbo, textBounds.GetCenter(), Vec2::ZeroVector, instance.m_scale, instance.m_text, instance.m_tint);
	}

	defaultFont->SetRendererState(renderer);
	renderer.DrawVertexBuffer(immediateVbo);
}
