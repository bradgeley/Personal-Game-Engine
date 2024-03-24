// Bradley Christensen - 2022-2023
#pragma once
#include <string>
#include <vector>



struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;



//----------------------------------------------------------------------------------------------------------------------
struct ShaderConfig
{
    std::string	m_name;
    std::string	m_vertexEntryPoint = "VertexMain";
    std::string	m_pixelEntryPoint = "PixelMain";
};



//----------------------------------------------------------------------------------------------------------------------
class Shader
{
	friend class Renderer;

public:

    explicit Shader(ShaderConfig const& config);
	Shader(Shader const& copy) = delete;
    ~Shader();

    ID3D11InputLayout* CreateOrGetInputLayout();
    bool CreateFromSource(std::string const& sourceCode);
	void ReleaseResources();

protected:
    
    bool CompileAsVertexShader(std::string const& sourceCode);
    bool CompileAsPixelShader(std::string const& sourceCode);
    
    ID3D11InputLayout* CreateInputLayoutFor_Vertex_PCU();

protected:

    ShaderConfig const m_config;
    
    ID3D11InputLayout* m_inputLayout  = nullptr;
    ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader  = nullptr;
    
    std::vector<uint8_t> m_vertexByteCode;

    static Shader const* const NullShader;
};