// Bradley Christensen - 2022-2025
#include "AsyncLoadAssetJob.h"
#include "AssetManager.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void AsyncLoadAssetJob::Execute()
{
    if (!g_assetManager->IsEnabled())
    {
        return;
    }

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
		m_loadedAsset->m_assetID = m_assetID;
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool AsyncLoadAssetJob::Complete()
{
    if (!g_assetManager->IsEnabled())
    {
        // The asset manager is disabled, so we know the engine is shutting down. We should not keep trying to load
        // dependencies for this asset. Rather, just release it and return true;
        if (m_loadedAsset)
        {
            m_loadedAsset->ReleaseResources();
            delete m_loadedAsset;
            m_loadedAsset = nullptr;
        }

        g_assetManager->m_futureAssets.erase(m_assetID);
        return true;
    }

	ASSERT_OR_DIE(g_assetManager != nullptr, "AsyncLoadAssetJob::Complete - AssetManager is null");
	ASSERT_OR_DIE(m_loadedAsset != nullptr, "AsyncLoadAssetJob::Complete - Loaded asset is null");
    bool completed = m_loadedAsset->CompleteAsyncLoad();
    if (completed)
    {
		LoadedAsset& loadedAssetEntry = g_assetManager->m_loadedAssets[m_assetID];
		loadedAssetEntry.m_key = m_assetKey;
        loadedAssetEntry.m_asset = m_loadedAsset;
		loadedAssetEntry.m_asset->m_name = m_assetKey.m_name;
        loadedAssetEntry.m_asset->m_assetID = m_assetID;

		g_assetManager->m_futureAssets.erase(m_assetID);

		g_assetManager->LogAsyncLoadCompleted(m_assetKey);
    }
    return completed;
}