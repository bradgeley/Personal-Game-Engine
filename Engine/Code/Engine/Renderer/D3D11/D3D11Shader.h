// Bradley Christensen - 2022-2026
#pragma once
#include "Game/Framework/EngineBuildPreferences.h"

#if defined(RENDERER_D3D11)

#include "Engine/Renderer/Shader.h"



struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;



//----------------------------------------------------------------------------------------------------------------------
// D3D11 Shader
//
class D3D11Shader : public Shader
{
    friend class D3D11Renderer;

protected:

    D3D11Shader(ShaderConfig const& config);

public:

    virtual void ReleaseResources() override;

    virtual bool FullCompileFromSource(std::string const& sourceCode) override;

    ID3D11InputLayout* GetD3D11InputLayout();

protected:

	bool CreateInputLayout(void* byteCode, size_t byteCodeSize);

protected:

    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
};

#endif // RENDERER_D3D11