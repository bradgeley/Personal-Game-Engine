// Bradley Christensen - 2022-2026



//----------------------------------------------------------------------------------------------------------------------
struct VSInput
{
	//------------------------------------------------------
	// Instance Data (0)
	//------------------------------------------------------
	float3	position	        : POSITION;	        // 12 bytes
    float	radius	            : RADIUS;           // 4 bytes
	//------------------------------------------------------
    float4  tint                : TINT;             // 16 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct VSOutput
{
    float4 position             : SV_Position;
    float4 tint                 : TINT;
    float2 uvs                  : UVS;
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 gameToRender;
    float4x4 worldToCamera;
    float4x4 cameraToClip;
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer DiscRenderConstants : register(b5)
{
	//------------------------------------------------------
	uint numSides                : NUMSIDES;	// 4 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
static const float2 UnitCircle64[64] =
{
    float2(1.000000f, 0.000000f),
    float2(0.995185f, 0.098017f),
    float2(0.980785f, 0.195090f),
    float2(0.956940f, 0.290285f),
    float2(0.923880f, 0.382683f),
    float2(0.881921f, 0.471397f),
    float2(0.831470f, 0.555570f),
    float2(0.773010f, 0.634393f),
    float2(0.707107f, 0.707107f),
    float2(0.634393f, 0.773010f),
    float2(0.555570f, 0.831470f),
    float2(0.471397f, 0.881921f),
    float2(0.382683f, 0.923880f),
    float2(0.290285f, 0.956940f),
    float2(0.195090f, 0.980785f),
    float2(0.098017f, 0.995185f),
    float2(0.000000f, 1.000000f),
    float2(-0.098017f, 0.995185f),
    float2(-0.195090f, 0.980785f),
    float2(-0.290285f, 0.956940f),
    float2(-0.382683f, 0.923880f),
    float2(-0.471397f, 0.881921f),
    float2(-0.555570f, 0.831470f),
    float2(-0.634393f, 0.773010f),
    float2(-0.707107f, 0.707107f),
    float2(-0.773010f, 0.634393f),
    float2(-0.831470f, 0.555570f),
    float2(-0.881921f, 0.471397f),
    float2(-0.923880f, 0.382683f),
    float2(-0.956940f, 0.290285f),
    float2(-0.980785f, 0.195090f),
    float2(-0.995185f, 0.098017f),
    float2(-1.000000f, 0.000000f),
    float2(-0.995185f, -0.098017f),
    float2(-0.980785f, -0.195090f),
    float2(-0.956940f, -0.290285f),
    float2(-0.923880f, -0.382683f),
    float2(-0.881921f, -0.471397f),
    float2(-0.831470f, -0.555570f),
    float2(-0.773010f, -0.634393f),
    float2(-0.707107f, -0.707107f),
    float2(-0.634393f, -0.773010f),
    float2(-0.555570f, -0.831470f),
    float2(-0.471397f, -0.881921f),
    float2(-0.382683f, -0.923880f),
    float2(-0.290285f, -0.956940f),
    float2(-0.195090f, -0.980785f),
    float2(-0.098017f, -0.995185f),
    float2(0.000000f, -1.000000f),
    float2(0.098017f, -0.995185f),
    float2(0.195090f, -0.980785f),
    float2(0.290285f, -0.956940f),
    float2(0.382683f, -0.923880f),
    float2(0.471397f, -0.881921f),
    float2(0.555570f, -0.831470f),
    float2(0.634393f, -0.773010f),
    float2(0.707107f, -0.707107f),
    float2(0.773010f, -0.634393f),
    float2(0.831470f, -0.555570f),
    float2(0.881921f, -0.471397f),
    float2(0.923880f, -0.382683f),
    float2(0.956940f, -0.290285f),
    float2(0.980785f, -0.195090f),
    float2(0.995185f, -0.098017f)
};



//----------------------------------------------------------------------------------------------------------------------
VSOutput VertexMain(VSInput vin, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    float4 discCenter = float4(vin.position, 1.0f);
    float discRadius = vin.radius;
    float4 discTint = vin.tint;
    
    // Supports power of 2 numSides up to 64
    
    uint firstVertIndex = (vertexID / 3) * (64 / numSides);
    uint thirdVertIndex = firstVertIndex + (64 / numSides);
    if (thirdVertIndex >= 64)
    {
        thirdVertIndex = 0;
    }
    
    uint localVertexID = vertexID % 3;
    
    float2 unscaledVertexOffset;
    if (localVertexID == 0)
    {
        unscaledVertexOffset = UnitCircle64[firstVertIndex];
    }
    else if (localVertexID == 1)
    {
        unscaledVertexOffset = float2(0.f, 0.f);
    }
    else // (localVertexID == 2)
    {
        unscaledVertexOffset = UnitCircle64[thirdVertIndex];
    }
    
    VSOutput output;
    
    output.position = float4(unscaledVertexOffset, 0.f, 0.f) * discRadius + discCenter;
    output.position = mul(gameToRender, output.position);
    output.position = mul(worldToCamera, output.position);
    output.position = mul(cameraToClip, output.position);
    
    output.uvs = unscaledVertexOffset + float2(0.5f, 0.5f);
    output.tint = discTint;
	
    return output;
}



//----------------------------------------------------------------------------------------------------------------------
float4 PixelMain(VSOutput input) : SV_Target0
{
    float4 finalColor = input.tint;
    
    if (finalColor.a <= 0.001f)
    {
        discard;
    }
    
	return finalColor;
}