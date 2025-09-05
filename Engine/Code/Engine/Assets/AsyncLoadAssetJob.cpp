// Bradley Christensen - 2022-2025
#include "AsyncLoadAssetJob.h"
#include "AssetManager.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void AsyncLoadAssetJob::Execute()
{
	ASSERT_OR_DIE(m_loaderFunc != nullptr, "AsyncLoadAssetJob::Execute - Loader function is null");
    ASSERT_OR_DIE(g_assetManager != nullptr, "AsyncLoadAssetJob::Execute - AssetManager is null");
    if (m_loaderFunc)
    {
        m_loadedAsset = m_loaderFunc(m_assetKey.m_name);
        if (!m_loadedAsset)
        {
            g_assetManager->LogError(m_assetKey.m_name, AssetManagerError::FailedToLoad);
            return;
        }
        m_loadedAsset->m_name = m_assetKey.m_name;
		g_assetManager->LogLoaded(m_assetKey);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AsyncLoadAssetJob::Complete()
{
	ASSERT_OR_DIE(g_assetManager != nullptr, "AsyncLoadAssetJob::Complete - AssetManager is null");
    g_assetManager->m_loadedAssets[m_assetID].m_asset = m_loadedAsset;
    g_assetManager->m_loadedAssets[m_assetID].m_asset->CompleteLoad();
}