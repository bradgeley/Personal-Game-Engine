// Bradley Christensen - 2022-2023
#pragma once
#include "EngineConstantBuffers.h"
#include <cstdint>



class Shader;
class Texture;



//----------------------------------------------------------------------------------------------------------------------
enum class FillMode : uint8_t
{
    Solid,
    Wireframe
};



//----------------------------------------------------------------------------------------------------------------------
enum class Winding : uint8_t
{
    Clockwise,
    CounterClockwise
};



//----------------------------------------------------------------------------------------------------------------------
enum class BlendMode : uint8_t
{
    Opaque,
    Alpha,
    Additive,

    Count
};



//------------------------------------------------------------------------------------------------------------
enum class CullMode : uint8_t
{
    None,
    Front,
    Back
};



//------------------------------------------------------------------------------------------------------------
enum class SamplerAddressMode : uint8_t
{
    Clamp,
    Wrap,
};



//------------------------------------------------------------------------------------------------------------
enum class SamplerFilter : uint8_t
{
    Point,
    Bilinear,
    Trilinear,
    Anisotropic_2,
    Anisotropic_4,
    Anisotropic_8,
    Anisotropic_16,

    Count,
};



//------------------------------------------------------------------------------------------------------------
enum class DepthTest : uint8_t
{
    ALWAYS,
    NEVER,
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
};



//------------------------------------------------------------------------------------------------------------
// Renderer settings
//
// All the rendering pipeline state that is configurable by the user
//
struct RendererSettings
{
    CameraConstants m_cameraConstants;
    ModelConstants  m_modelConstants;
    FontConstants   m_fontConstants;

    Texture* m_texture                      = nullptr;
    Shader* m_shader                        = nullptr;
    
    FillMode m_fillMode                     = FillMode::Solid;
    Winding m_winding                       = Winding::CounterClockwise;
    BlendMode m_blendMode                   = BlendMode::Alpha;
    CullMode m_cullMode                     = CullMode::None;
    SamplerFilter m_samplerFilter           = SamplerFilter::Point;
    SamplerAddressMode m_samplerAddressMode = SamplerAddressMode::Clamp;
    DepthTest m_depthTest                   = DepthTest::LESS_EQUAL;
    bool m_writeDepth                       = false;
};