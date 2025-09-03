// Bradley Christensen - 2022-2025
#include "AssetManager.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Assets/Sprites/GridSpriteSheet.h"
#include "Engine/Assets/Sprites/SpriteAnimation.h"



//----------------------------------------------------------------------------------------------------------------------
// THE Asset Manager
//
AssetManager* g_assetManager = nullptr;



//----------------------------------------------------------------------------------------------------------------------
void AssetManager::Release(AssetID id)
{
    auto it = m_loadedAssets.find(id);
    if (it == m_loadedAssets.end())
    {
        return;
    }

    it->second.m_refCount--;

    if (it->second.m_refCount <= 0)
    {
        delete it->second.m_asset;
		m_loadedAssets.erase(it);
    }
}
