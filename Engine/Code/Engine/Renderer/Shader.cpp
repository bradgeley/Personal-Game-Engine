// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Shader.h"
#include "Renderer.h"
#include "Vertex_PCU.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/RendererInternal.h"



Shader::Shader(ShaderConfig const& config) : m_config(config)
{
	
}



Shader::~Shader()
{
	ReleaseResources();
}



ID3D11InputLayout* Shader::CreateOrGetInputLayout()
{
	if (m_inputLayout)
	{
		return m_inputLayout;
	}
	return CreateInputLayoutFor_Vertex_PCU();
}



bool Shader::CreateFromSource(std::string const& sourceCode)
{
	ASSERT_OR_DIE(!sourceCode.empty(), "Tried to create shader from empty source code")
    bool compiledVertex = CompileAsVertexShader(sourceCode);
    bool compiledPixel = CompileAsPixelShader(sourceCode);
    return compiledVertex && compiledPixel;
}



void Shader::ReleaseResources()
{
	DX_SAFE_RELEASE(m_pixelShader)
	DX_SAFE_RELEASE(m_vertexShader)
	DX_SAFE_RELEASE(m_inputLayout)
}



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



bool Shader::CompileAsVertexShader(std::string const& sourceCode)
{
	ID3D11Device* device = g_renderer->GetDevice();
	
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
		&errorBuffer );

	if (SUCCEEDED(result) && byteCode)
	{
		HRESULT vertexShaderResult = device->CreateVertexShader(
			byteCode->GetBufferPointer(),
			byteCode->GetBufferSize(),
			nullptr,
			&m_vertexShader );

		ASSERT_OR_DIE(SUCCEEDED(vertexShaderResult), "Failed to compile vertex shader.")
		
		// Copy off the byte code for use in making the input layout
		m_vertexByteCode.resize(byteCode->GetBufferSize());
		memcpy(m_vertexByteCode.data(), byteCode->GetBufferPointer(), byteCode->GetBufferSize());
	}
	else
	{
		char const* errorString = (char const*) errorBuffer->GetBufferPointer();
		ERROR_AND_DIE( errorString )
	}

	DX_SAFE_RELEASE(byteCode)
	DX_SAFE_RELEASE(errorBuffer)
	return true;
}



bool Shader::CompileAsPixelShader(std::string const& sourceCode)
{
	ID3D11Device* device = g_renderer->GetDevice();
	
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
		&errorBuffer );

	if (SUCCEEDED(result) && byteCode)
	{
		HRESULT pixelShaderResult = device->CreatePixelShader(
			byteCode->GetBufferPointer(),
			byteCode->GetBufferSize(),
			nullptr,
			&m_pixelShader);

		ASSERT_OR_DIE(SUCCEEDED(pixelShaderResult), "Failed to compile pixel shader.")
	}
	else
	{
		char const* errorString = (char const*) errorBuffer->GetBufferPointer();
		ERROR_AND_DIE( errorString )
	}

	DX_SAFE_RELEASE(byteCode)
	DX_SAFE_RELEASE(errorBuffer)
	return true;
}



ID3D11InputLayout* Shader::CreateInputLayoutFor_Vertex_PCU()
{
	ID3D11Device* device = g_renderer->GetDevice();

	constexpr int NUM_ELEMENTS = 3;
	D3D11_INPUT_ELEMENT_DESC vertexDesc[NUM_ELEMENTS];

	// pos
	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].SemanticIndex = 0;
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].InputSlot = 0;
	vertexDesc[0].AlignedByteOffset = offsetof(Vertex_PCU, pos);
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[0].InstanceDataStepRate = 0;

	// tint
	vertexDesc[1].SemanticName = "TINT";
	vertexDesc[1].SemanticIndex = 0;
	vertexDesc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vertexDesc[1].InputSlot = 0;
	vertexDesc[1].AlignedByteOffset = offsetof(Vertex_PCU, tint);
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[1].InstanceDataStepRate = 0;

	// uvs
	vertexDesc[2].SemanticName = "UVS";
	vertexDesc[2].SemanticIndex = 0;
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].InputSlot = 0;
	vertexDesc[2].AlignedByteOffset = offsetof(Vertex_PCU, uvs);
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[2].InstanceDataStepRate = 0;

	HRESULT result = device->CreateInputLayout(
		vertexDesc,
		NUM_ELEMENTS,
		m_vertexByteCode.data(),
		m_vertexByteCode.size(),
		&m_inputLayout
	);
	
	ASSERT_OR_DIE(SUCCEEDED(result), "Failed to create input layout for vertex PCU")

	return m_inputLayout;
}
