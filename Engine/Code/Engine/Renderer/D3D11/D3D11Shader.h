// Bradley Christensen - 2022-2025
#pragma once
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

    virtual bool CreateFromSource(std::string const& sourceCode) override;

    ID3D11InputLayout* CreateOrGetInputLayout();

protected:

    bool CompileAsVertexShader(std::string const& sourceCode);
    bool CompileAsPixelShader(std::string const& sourceCode);

    ID3D11InputLayout* CreateInputLayoutFor_Vertex_PCU();

protected:

    std::vector<uint8_t> m_vertexByteCode;

    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
};

