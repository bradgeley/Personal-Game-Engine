// Bradley Christensen - 2022-2025
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "D3D11Shader.h"
#include "D3D11Internal.h"
#include "D3D11Renderer.h"
#include "Engine/Core/ErrorUtils.h"
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
	m_vertexByteCode.clear();
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
		return DXGI_FORMAT_UNKNOWN;
	}
}



//----------------------------------------------------------------------------------------------------------------------
ID3D11InputLayout* D3D11Shader::CreateOrGetD3D11InputLayout()
{
	if (m_inputLayout)
	{
		return m_inputLayout;
	}

	ID3D11Device* device = D3D11Renderer::Get()->GetDevice();
	ASSERT_OR_DIE(device, "No device found in D3D11Renderer");

	constexpr int maxElements = 20;
	D3D11_INPUT_ELEMENT_DESC vertexDesc[maxElements]; // 10 slots for now

	int numElements = (int) m_config.m_inputLayout.m_attributes.size();
	ASSERT_OR_DIE(numElements <= maxElements, "Too many input layout elements, raise the max.");

	for (int i = 0; i < numElements; ++i)
	{
		InputLayoutAttribute const& attribute = m_config.m_inputLayout.m_attributes[i];
		vertexDesc[i].SemanticName			= InputLayout::GetInputLayoutSemanticName(attribute.m_semantic);
		vertexDesc[i].SemanticIndex			= attribute.m_semanticIndex;
		vertexDesc[i].Format				= GetD3D11Format(attribute.m_format);
		vertexDesc[i].InputSlot				= attribute.m_inputSlot;
		vertexDesc[i].AlignedByteOffset		= attribute.m_alignedByteOffset;
		vertexDesc[i].InputSlotClass		= attribute.m_isPerInstance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		vertexDesc[i].InstanceDataStepRate	= attribute.m_isPerInstance ? 1 : 0;
	}

	HRESULT result = device->CreateInputLayout(
		vertexDesc,
		numElements,
		m_vertexByteCode.data(),
		m_vertexByteCode.size(),
		&m_inputLayout
	);

	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create input layout for vertex PCU");

	#if defined(_DEBUG)
	std::string inputLayoutString = "Input Layout (Vertex_PCU)";
	m_inputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, (int) inputLayoutString.size(), inputLayoutString.data());
	#endif

	return m_inputLayout;
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Shader::CreateFromSource(std::string const& sourceCode)
{
	ASSERT_OR_DIE(!sourceCode.empty(), "Tried to create shader from empty source code");
	bool compiledVertex = CompileAsVertexShader(sourceCode);
	bool compiledPixel = CompileAsPixelShader(sourceCode);
	return compiledVertex && compiledPixel;
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Shader::CompileAsVertexShader(std::string const& sourceCode)
{
	ID3D11Device* device = D3D11Renderer::Get()->GetDevice();

	ID3DBlob* byteCode;
	ID3DBlob* errorBuffer;

	DWORD compileFlags = GetCompileFlags();

	HRESULT result = D3DCompile(
		sourceCode.c_str(),
		sourceCode.size(),
		m_config.m_name.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_config.m_vertexEntryPoint.c_str(),
		"vs_5_0",
		compileFlags,
		0,
		&byteCode,
		&errorBuffer);

	if (SUCCEEDED(result) && byteCode)
	{
		HRESULT vertexShaderResult = device->CreateVertexShader(
			byteCode->GetBufferPointer(),
			byteCode->GetBufferSize(),
			nullptr,
			&m_vertexShader);

		ASSERT_OR_DIE(SUCCEEDED(vertexShaderResult), "Failed to compile vertex shader.");

		#if defined(_DEBUG)
			m_vertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT) strlen("VertexShader"), "VertexShader");
		#endif

		// Copy off the byte code for use in making the input layout
		m_vertexByteCode.resize(byteCode->GetBufferSize());
		memcpy(m_vertexByteCode.data(), byteCode->GetBufferPointer(), byteCode->GetBufferSize());
	}
	else
	{
		char const* errorString = (char const*) errorBuffer->GetBufferPointer();
		ERROR_AND_DIE(errorString);
	}

	DX_SAFE_RELEASE(byteCode);
	DX_SAFE_RELEASE(errorBuffer);
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool D3D11Shader::CompileAsPixelShader(std::string const& sourceCode)
{
	ID3D11Device* device = D3D11Renderer::Get()->GetDevice();

	ID3DBlob* byteCode;
	ID3DBlob* errorBuffer;

	DWORD compileFlags = GetCompileFlags();

	HRESULT result = D3DCompile(
		sourceCode.c_str(),
		sourceCode.size(),
		m_config.m_name.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_config.m_pixelEntryPoint.c_str(),
		"ps_5_0",
		compileFlags,
		0,
		&byteCode,
		&errorBuffer);

	if (SUCCEEDED(result) && byteCode)
	{
		HRESULT pixelShaderResult = device->CreatePixelShader(
			byteCode->GetBufferPointer(),
			byteCode->GetBufferSize(),
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

	DX_SAFE_RELEASE(byteCode);
	DX_SAFE_RELEASE(errorBuffer);
	return true;
}

#endif // RENDERER_D3D11