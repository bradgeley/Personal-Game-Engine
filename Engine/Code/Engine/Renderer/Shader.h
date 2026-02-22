// Bradley Christensen - 2022-2026
#pragma once
#include "InputLayout.h"
#include "Engine/Core/Name.h"
#include <string>
#include <vector>


struct InputLayout;


//----------------------------------------------------------------------------------------------------------------------
struct ShaderConfig
{
	static ShaderConfig MakeFromXML(char const* xmlFilePath);

    Name	    m_name;
    std::string	m_vertexEntry  = "VertexMain";
    std::string	m_pixelEntry   = "PixelMain";
    std::string m_sourceFilePath = "";
    InputLayout m_inputLayout;
};



//----------------------------------------------------------------------------------------------------------------------
class Shader
{
    friend class Renderer;

protected:

    explicit Shader(ShaderConfig const& config);

public:

	Shader(Shader const& copy) = delete;
    virtual ~Shader() = default;

    virtual void ReleaseResources() = 0;

    virtual bool FullCompileFromSource(std::string const& sourceCode) = 0;

protected:

    ShaderConfig const m_config;
};