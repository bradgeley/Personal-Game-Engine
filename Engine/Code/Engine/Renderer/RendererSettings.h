// Bradley Christensen - 2022-2026
#pragma once
#include "EngineConstantBuffers.h"
#include "RendererUtils.h"
#include <cstdint>



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
    Always,
    Never,
    Equal,
    NotEqual,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
};



//------------------------------------------------------------------------------------------------------------
// Renderer settings
//
// All the rendering pipeline state that is configurable by the user
//
struct RendererSettings
{
    CameraConstants m_cameraConstants;
    ModelConstants m_modelConstants;
    FontConstants m_fontConstants;

    TextureID m_boundTextures[16]           = { RendererUtils::InvalidID };
    ShaderID m_boundShader                  = RendererUtils::InvalidID;
    
    FillMode m_fillMode                     = FillMode::Solid;
    Winding m_winding                       = Winding::CounterClockwise;
    BlendMode m_blendMode                   = BlendMode::Alpha;
    CullMode m_cullMode                     = CullMode::None;
    SamplerFilter m_samplerFilter           = SamplerFilter::Point;
    SamplerAddressMode m_samplerAddressMode = SamplerAddressMode::Clamp;
    DepthTest m_depthTest                   = DepthTest::LessOrEqual;
    bool m_writeDepth                       = false;
};



//------------------------------------------------------------------------------------------------------------
// Renderer User Settings
//
// Settings that the user might set, such as Vsync enabled, filtering mode, etc.
//
struct RendererUserSettings
{
    bool m_vsyncEnabled = false;
    bool m_msaaEnabled = true; // We just render at the highest msaa sample count possible for the machine
};



//------------------------------------------------------------------------------------------------------------
struct MSAASettings
{
    uint32_t m_sampleCount = 1;
    uint32_t m_qualityLevel = 0;
};