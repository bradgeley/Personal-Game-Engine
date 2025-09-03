// Bradley Christensen - 2022-2025
#include "SAnimation.h"
#include "CAnimation.h"
#include "Engine/Assets/Sprites/GridSpriteSheet.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Startup()
{
    AddWriteDependencies<CAnimation, Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Run(SystemContext const& context)
{
    TextureID testTexture = g_renderer->MakeTexture();
    Texture* tex = g_renderer->GetTexture(testTexture);
    tex->LoadFromImageFile("Data/Images/Soldier.png");

    static GridSpriteSheet spriteSheet;
    spriteSheet.CreateFromTexture(testTexture, IntVec2(3, 4), IntVec2(0, 0), IntVec2(0, 0));

    for (auto it = g_ecs->Iterate<CAnimation>(context); it.IsValid(); ++it)
    {
        CAnimation& anim = *g_ecs->GetComponent<CAnimation>(it);

        // move to init step
        anim.m_spriteSheet = spriteSheet;
        anim.m_secondsPerFrame = 0.25f;
        anim.m_pingpong = true;

		anim.m_animation.Update(context.m_deltaSeconds);
	}
}
