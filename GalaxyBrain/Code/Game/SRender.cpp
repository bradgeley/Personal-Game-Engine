// Bradley Christensen - 2023
#include "SRender.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/StringUtils.h"
#include "Game/CRender.h"
#include "Game/CCamera.h"
#include "Game/CTransform.h"
#include "Game/SCRenderer.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_Z_LAYERS = 2;



//----------------------------------------------------------------------------------------------------------------------
void SRender::Startup()
{
    AddWriteDependencies<CRender, Renderer>();
    AddReadDependencies<CTransform, CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRender::Run(SystemContext const& context)
{
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

    // Todo: move this part out to a separate system - Copy transform or something
    for (auto renderIt = g_ecs->Iterate<CRender, CTransform>(context); renderIt.IsValid(); ++renderIt)
    {
        CRender& render = *renderStorage.Get(renderIt.m_currentIndex);
        CTransform& trans = *transStorage.Get(renderIt.m_currentIndex);

        float rotation = trans.m_orientation;
        if (trans.m_attachedToEntity != ENTITY_ID_INVALID)
        {
            CTransform& attachedToTrans = *transStorage.Get(trans.m_attachedToEntity);
            rotation = attachedToTrans.m_orientation + trans.m_polarCoords.x + 90.f;
        }

        render.m_modelConstants.m_modelMatrix.Reset();
        render.m_modelConstants.m_modelMatrix.AppendZRotation(rotation);
        render.m_modelConstants.m_modelMatrix.AppendUniformScale2D(render.m_scale);
        render.m_modelConstants.m_modelMatrix.SetTranslation2D(trans.m_pos);
    }

    for (auto camIt = g_ecs->Iterate<CCamera>(context); camIt.IsValid(); ++camIt)
    {
        // For each camera
        CCamera& camera = *cameraStorage.Get(camIt.m_currentIndex);
        g_renderer->BeginCamera(camera.m_camera);

        // Render all things
        for (int i = 0; i < MAX_Z_LAYERS; ++i)
        {
            for (auto renderIt = g_ecs->Iterate<CRender>(context); renderIt.IsValid(); ++renderIt)
            {
                CRender& render = *renderStorage.Get(renderIt.m_currentIndex);

                if (render.m_layer == i)
                {
                    g_renderer->SetModelConstants(render.m_modelConstants);
                    g_renderer->BindShader(nullptr);
                    g_renderer->BindTexture(render.m_texture);
                    g_renderer->DrawVertexBuffer(render.m_vbo);
                }
            }
        }

    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRender::Shutdown()
{
    auto& scRender = g_ecs->GetSingleton<SCRenderer>();

    // VBO's
    for (auto& vbo : scRender.m_vbos)
    {
        vbo.second->ReleaseResources();
        delete vbo.second;
    }
    scRender.m_vbos.clear();

    // Textures
    for (auto& tex : scRender.m_textures)
    {
        tex.second->ReleaseResources();
        delete tex.second;
    }
    scRender.m_textures.clear();
}



//----------------------------------------------------------------------------------------------------------------------
VertexBuffer* SRender::CreateOrGetVbo(std::string const& name)
{
    if (name == "")
    {
        return nullptr;
    }

    auto& scRender = g_ecs->GetSingleton<SCRenderer>();
    auto strings = SplitStringOnDelimeter(name, '_');

    auto& vbo = scRender.m_vbos[name];
    if (vbo == nullptr)
    {
        vbo = new VertexBuffer();
        if (strings[0] == "sphere")
        {
            int numSides = StringToInt(strings[1]);
            AddVertsForDisc2D(vbo->GetMutableVerts(), Vec2(0.f, 0.f), 1.f, numSides);
        }
        else if (strings[0] == "sprite")
        {
            IntVec2 dims = StringToIntVec2(strings[1]);
            AddVertsForRect2D(vbo->GetMutableVerts(), -0.5f * Vec2(dims.x, dims.y), 0.5f * Vec2(dims.x, dims.y));
        }
    }

    return vbo;
}



//----------------------------------------------------------------------------------------------------------------------
Texture* SRender::CreateOrGetTexture(std::string const& name)
{
    if (name == "")
    {
        return nullptr;
    }

    auto& scRender = g_ecs->GetSingleton<SCRenderer>();

    auto& tex = scRender.m_textures[name];
    if (tex == nullptr)
    {
        tex = new Texture();
        tex->LoadFromImageFile(name.c_str());
    }
    return tex;
}
