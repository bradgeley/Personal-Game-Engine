#pragma once



static char const* s_defaultFontShaderSource = R"(

struct vs_input_t
{
    float3 position : POSITION;
    float4 tint     : TINT;
    float2 uvs      : UVS;
};

struct v2p_t
{
    float4 position : SV_Position;
    float4 tint     : TINT;
    float2 uvs      : UVS;
};

cbuffer CameraConstants : register(b2)
{
    float4x4 gameToRender;
    float4x4 worldToCamera;
    float4x4 cameraToClip;
};

cbuffer ModelConstants : register(b3)
{
    float4x4 localToWorld;
    float4 modelTint;
};

cbuffer FontConstants : register(b4)
{
    float4 outlineTint;
    float boldness;
    float antiAliasAmount;
    float outlineThickness;
    float pad;
};

v2p_t VertexMain(vs_input_t input)
{
    v2p_t v2p;

    float4 position = float4(input.position, 1.0);
    position = mul(gameToRender,  position);
    position = mul(localToWorld,  position);
    position = mul(worldToCamera, position);
    position = mul(cameraToClip,  position);

    v2p.position = position;
    v2p.tint = input.tint * modelTint;
    v2p.uvs = input.uvs;

    return v2p;
}


Texture2D<float4> SurfaceColorTexture : register(t0);
SamplerState SurfaceSampler : register(s0);


float4 PixelMain(v2p_t input) : SV_Target0
{
    float2 texCoord = input.uvs;
    float4 surfaceColor = SurfaceColorTexture.Sample(SurfaceSampler, texCoord);
	
    float4 tint = input.tint;
    float4 finalColor = tint * surfaceColor;

    float discardThreshold = 1 - (boldness + outlineThickness);
    float outlineThreshold = discardThreshold + outlineThickness;

    if (surfaceColor.a <= discardThreshold)
    {
        discard;
    }

    if (surfaceColor.a <= discardThreshold + antiAliasAmount)
    {
        float outputAlpha = (surfaceColor.a - discardThreshold) / antiAliasAmount;
        finalColor.a = outputAlpha;
    }
    else
    {
        finalColor.a = 1;
    }

    float innerBlendLowerThreshold = outlineThreshold - 0.05;
    float innerBlendUpperThreshold = outlineThreshold + 0.05;
    if (surfaceColor.a >= innerBlendLowerThreshold && surfaceColor.a <= innerBlendUpperThreshold)
    {
        // On the edge between the inner color and the outline, blend a bit
        float t = (surfaceColor.a - innerBlendLowerThreshold) / 0.1f;
        return lerp(outlineTint, finalColor, t);
    }
    if (surfaceColor.a <= outlineThreshold)
    {
        float a = finalColor.a;
        finalColor = outlineTint;
        finalColor.a = a;
        return finalColor;
    }

    finalColor.a *= input.tint.a;
    return finalColor;
}
    
)";