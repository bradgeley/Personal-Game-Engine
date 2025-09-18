// Bradley Christensen - 2022-2025
#include "ShaderAsset.h"
#include "Engine/Assets/Image.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"



//----------------------------------------------------------------------------------------------------------------------
Asset* ShaderAsset::Load(Name assetName)
{
	// This is the async part of the Shader load, which is basically just creating the Shader asset, but not any gpu resources yet.
    ShaderAsset* shaderAsset = new ShaderAsset();
    shaderAsset->m_name = assetName;

    if (g_renderer)
    {
        shaderAsset->m_shaderID = g_renderer->MakeShader();
    }

    return shaderAsset;
}



//----------------------------------------------------------------------------------------------------------------------
AssetID ShaderAsset::GetImageAssetID() const
{
	return m_assetID;
}



//----------------------------------------------------------------------------------------------------------------------
ShaderID ShaderAsset::GetShaderID() const
{
	return m_shaderID;
}



//----------------------------------------------------------------------------------------------------------------------
bool ShaderAsset::CompleteAsyncLoad()
{

}



//----------------------------------------------------------------------------------------------------------------------
bool ShaderAsset::CompleteSyncLoad()
{

}



//----------------------------------------------------------------------------------------------------------------------
void ShaderAsset::ReleaseResources()
{

}
