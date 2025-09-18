﻿// Bradley Christensen - 2022-2025
#pragma once
#include "InputLayout.h"
#include <string>
#include <vector>


struct InputLayout;


//----------------------------------------------------------------------------------------------------------------------
struct ShaderConfig
{
    std::string	m_name;
    std::string	m_vertexEntryPoint  = "VertexMain";
    std::string	m_pixelEntryPoint   = "PixelMain";
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

    virtual bool CreateFromSource(std::string const& sourceCode) = 0;

protected:

    ShaderConfig const m_config;
};