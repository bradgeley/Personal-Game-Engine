// Bradley Christensen - 2022-2025
#pragma once



class AssetManager;



//----------------------------------------------------------------------------------------------------------------------
class SCAssetManager
{
public:

	AssetManager const* GetAssetManager() const			{ return m_assetManager; }
	AssetManager* GetAssetManager()						{ return m_assetManager; }
	void SetAssetManager(AssetManager* assetManager)	{ m_assetManager = assetManager; }

private:

	AssetManager* m_assetManager = nullptr;
};