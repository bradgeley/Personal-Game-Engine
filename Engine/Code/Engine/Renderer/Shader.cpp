﻿// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Shader.h"



//----------------------------------------------------------------------------------------------------------------------
Shader const* const Shader::NullShader = nullptr;



//----------------------------------------------------------------------------------------------------------------------
Shader::Shader(ShaderConfig const& config) : m_config(config)
{
	
}
