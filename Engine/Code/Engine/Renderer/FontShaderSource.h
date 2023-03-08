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

    float4 localPosition = float4(input.position, 1.0);
    float4 worldPosition = mul(localToWorld, localPosition);
    float4 cameraPosition = mul(worldToCamera, worldPosition);
    float4 clipPosition = mul(cameraToClip, cameraPosition);

    v2p.position = clipPosition;
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
    
    float lowerThreshold = 0.55;
    float upperThreshold = 0.65;
    float thresholdWidth = upperThreshold - lowerThreshold;
    
    if (surfaceColor.a <= lowerThreshold)
    {
        discard;
    }
    else
    {
        float outputAlpha = (surfaceColor.a - lowerThreshold) / thresholdWidth;
        finalColor.a = outputAlpha;
    }
    
    return finalColor;
}
    
)";