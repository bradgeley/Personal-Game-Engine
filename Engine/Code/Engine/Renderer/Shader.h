// Bradley Christensen - 2022-2023
#pragma once
#include <string>
#include <vector>





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
    friend class RendererInterface;

protected:

    explicit Shader(ShaderConfig const& config);

public:

	Shader(Shader const& copy) = delete;
    virtual ~Shader() = default;

    virtual void ReleaseResources() = 0;

    virtual bool CreateFromSource(std::string const& sourceCode) = 0;

protected:

    ShaderConfig const m_config;

    static Shader const* const NullShader;
};