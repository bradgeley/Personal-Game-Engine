// Bradley Christensen - 2022-2025
#include "ShaderAsset.h"
#include "Engine/Assets/Image.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Core/FileUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Asset* ShaderAsset::Load(Name assetName)
{
	// Asset name here should be a .xml file path, e.g. "Data/Shaders/MyShader.xml"
	Strings pathAndExtension = StringUtils::SplitStringOnDelimiter(assetName.ToString(), '.');
	ASSERT_OR_DIE(pathAndExtension.size() == 2, StringUtils::StringF("Shader asset name must be a .xml file path, e.g. Data/Shaders/MyShader.xml. Got: %s", assetName.ToCStr()));

	std::string xmlFilePath = assetName.ToString();

    ShaderAsset* shaderAsset = new ShaderAsset();
    shaderAsset->m_name = assetName;

    // Make config from .xml file
    ShaderConfig config = ShaderConfig::MakeFromXML(xmlFilePath.c_str());

	// Load source code from .hlsl file
    std::string sourceCode;
    int bytesRead = FileUtils::FileReadToString(config.m_sourceFilePath, sourceCode);
	ASSERT_OR_DIE(bytesRead > 0, StringUtils::StringF("Failed to read shader source code from %s", config.m_sourceFilePath.c_str()));

    if (g_renderer)
    {
        shaderAsset->m_shaderID = g_renderer->MakeShader(config);
		Shader* shader = g_renderer->GetShader(shaderAsset->m_shaderID);
        shader->FullCompileFromSource(sourceCode);
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
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool ShaderAsset::CompleteSyncLoad()
{
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void ShaderAsset::ReleaseResources()
{
    if (g_renderer)
    {
        g_renderer->ReleaseShader(m_shaderID);
    }
}
