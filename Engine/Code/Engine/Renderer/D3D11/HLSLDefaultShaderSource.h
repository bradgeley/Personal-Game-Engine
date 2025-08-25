// Bradley Christensen - 2022-2023
#pragma once



static char const* s_HLSLDefaultShaderSource = R"(



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
    if (surfaceColor.a == 0.0 || tint.a == 0.0) discard;
    return finalColor;
}

)";