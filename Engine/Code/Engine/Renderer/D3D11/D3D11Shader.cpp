// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "D3D11Shader.h"
#include "D3D11Internal.h"
#include "D3D11Renderer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Renderer/InputLayout.h"
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
DWORD GetCompileFlags()
{
	DWORD compileFlags = 0;
	#if defined (_DEBUG)
	{
		compileFlags |= D3DCOMPILE_DEBUG;
		compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
		compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	}
	#else 
	{
		compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	}
	#endif
	return compileFlags;
}



//----------------------------------------------------------------------------------------------------------------------
D3D11Shader::D3D11Shader(ShaderConfig const& config) : Shader(config)
{
}



//----------------------------------------------------------------------------------------------------------------------
void D3D11Shader::ReleaseResources()
{
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_vertexShader);
	DX_SAFE_RELEASE(m_inputLayout);
}



//----------------------------------------------------------------------------------------------------------------------
DXGI_FORMAT GetD3D11Format(InputLayoutAttributeFormat format)
{
	switch (format)
	{
	case InputLayoutAttributeFormat::Float1:   return DXGI_FORMAT_R32_FLOAT;
	case InputLayoutAttributeFormat::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
	case InputLayoutAttributeFormat::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
	case InputLayoutAttributeFormat::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case InputLayoutAttributeFormat::Rgba8:    return DXGI_FORMAT_R8G8B8A8_UNORM;
	case InputLayoutAttributeFormat::Uint1:    return DXGI_FORMAT_R32_UINT;
	case InputLayoutAttributeFormat::Uint2:    return DXGI_FORMAT_R32G32_UINT;
	case InputLayoutAttributeFormat::Uint3:    return DXGI_FORMAT_R32G32B32_UINT;
	case InputLayoutAttributeFormat::Uint4:    return DXGI_FORMAT_R32G32B32A32_UINT;
	default:              
		ERROR_AND_DIE("Unsupported InputLayoutAttributeFormat");
		//return DXGI_FORMAT_UNKNOWN;
	}
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11InputLayout* D3D11Shader::GetD3D11InputLayout()
{
	ASSERT_OR_DIE(m_inputLayout, StringUtils::StringF("Shader %s has no valid input layout, has it been compiled?", m_config.m_name.ToCStr()));
	return m_inputLayout;
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Shader::FullCompileFromSource(std::string const& sourceCode)
{
	ID3D11Device* device = D3D11Renderer::Get()->GetDevice();

	DWORD compileFlags = GetCompileFlags();
	ID3DBlob* errorBuffer;

	ID3DBlob* pixelByteCode;
	HRESULT pixelResult = D3DCompile(
		sourceCode.c_str(),
		sourceCode.size(),
		m_config.m_name.ToCStr(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_config.m_pixelEntry.c_str(),
		"ps_5_0",
		compileFlags,
		0,
		&pixelByteCode,
		&errorBuffer);

	if (SUCCEEDED(pixelResult) && pixelByteCode)
	{
		// Create Pixel Shader
		HRESULT pixelShaderResult = device->CreatePixelShader(
			pixelByteCode->GetBufferPointer(),
			pixelByteCode->GetBufferSize(),
			nullptr,
			&m_pixelShader);

		ASSERT_OR_DIE(SUCCEEDED(pixelShaderResult), "Failed to compile pixel shader.");

		#if defined(_DEBUG)
			m_pixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) strlen("PixelShader"), "PixelShader");
		#endif
	}
	else
	{
		char const* errorString = (char const*) errorBuffer->GetBufferPointer();
		ERROR_AND_DIE(errorString)
	}

	ID3DBlob* vertexByteCode;
	HRESULT vertexResult = D3DCompile(
		sourceCode.c_str(),
		sourceCode.size(),
		m_config.m_name.ToCStr(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_config.m_vertexEntry.c_str(),
		"vs_5_0",
		compileFlags,
		0,
		&vertexByteCode,
		&errorBuffer);

	if (SUCCEEDED(vertexResult) && vertexByteCode)
	{
		// Create Vertex Shader
		HRESULT vertexShaderResult = device->CreateVertexShader(
			vertexByteCode->GetBufferPointer(),
			vertexByteCode->GetBufferSize(),
			nullptr,
			&m_vertexShader);

		ASSERT_OR_DIE(SUCCEEDED(vertexShaderResult), "Failed to compile vertex shader.");

		#if defined(_DEBUG)
			m_vertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) strlen("VertexShader"), "VertexShader");
		#endif

		// Create Input Layout
		CreateInputLayout(vertexByteCode->GetBufferPointer(), vertexByteCode->GetBufferSize());
	}
	else
	{
		char const* errorString = (char const*) errorBuffer->GetBufferPointer();
		ERROR_AND_DIE(errorString)
	}

	DX_SAFE_RELEASE(pixelByteCode);
	DX_SAFE_RELEASE(vertexByteCode);
	DX_SAFE_RELEASE(errorBuffer);
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Shader::CreateInputLayout(void* byteCode, size_t byteCodeSize)
{
	ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
	ASSERT_OR_DIE(device, "No device found in D3D11Renderer");

	constexpr int maxElements = 20;
	D3D11_INPUT_ELEMENT_DESC vertexDesc[maxElements]; // 10 slots for now
	std::string semanticNames[maxElements];

	int numElements = (int) m_config.m_inputLayout.m_attributes.size();
	ASSERT_OR_DIE(numElements <= maxElements, "Too many input layout elements, raise the max.");

	for (int i = 0; i < numElements; ++i)
	{
		InputLayoutAttribute const& attribute = m_config.m_inputLayout.m_attributes[i];
		semanticNames[i] = InputLayout::GetInputLayoutSemanticName(attribute.m_semantic);
		vertexDesc[i].SemanticName = semanticNames[i].c_str();
		vertexDesc[i].SemanticIndex = attribute.m_semanticIndex;
		vertexDesc[i].Format = GetD3D11Format(attribute.m_format);
		vertexDesc[i].InputSlot = attribute.m_inputSlot;
		vertexDesc[i].AlignedByteOffset = attribute.m_alignedByteOffset;
		vertexDesc[i].InputSlotClass = attribute.m_isPerInstance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		vertexDesc[i].InstanceDataStepRate = attribute.m_isPerInstance ? 1 : 0;
	}

	HRESULT result = device->CreateInputLayout(
		vertexDesc,
		numElements,
		byteCode,
		byteCodeSize,
		&m_inputLayout
	);

	ASSERT_OR_DIE(SUCCEEDED(result), StringUtils::StringF("Failed to create input layout for %s", m_config.m_name.ToCStr()));

	#if defined(_DEBUG)
		std::string inputLayoutString = StringUtils::StringF("Input Layout: %s", m_config.m_name.ToCStr());
		m_inputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, (int) inputLayoutString.size(), inputLayoutString.data());
	#endif

	return true;
}

#endif // RENDERER_D3D11