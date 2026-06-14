// Bradley Christensen - 2022-2026



//----------------------------------------------------------------------------------------------------------------------
struct VSInput
{
	//------------------------------------------------------
	// Instance Data (0)
	//------------------------------------------------------
	float3	instancePosition	: INSTANCEPOSITION;	// 12 bytes
    float   instanceRotation    : INSTANCEROTATION; // 4 bytes
	//------------------------------------------------------
    float4	instanceTint		: INSTANCETINT;		// 16 bytes
	//------------------------------------------------------
	float2	instanceDims		: INSTANCEDIMS;	    // 8 bytes
    uint    spriteIndex         : INDEX;            // 4 bytes
    float   indoorLight         : INDOORLIGHT;      // 4 bytes
	//------------------------------------------------------
    float   outdoorLight        : OUTDOORLIGHT;     // 4 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct VSOutput
{
    float4 position : SV_Position;
    float4 tint : TINT;
    float2 uvs : UVS;
    float indoorLight : INDOORLIGHT;
    float outdoorLight : OUTDOORLIGHT;
};



//----------------------------------------------------------------------------------------------------------------------
static const float3 QuadVerts[6] =
{
    float3(-0.5f, -0.5f, 0.f),  // Bot Left
    float3(0.5f, -0.5f, 0.f),   // Bot Right
    float3(0.5f, 0.5f, 0.f),    // Top Right

    float3(-0.5f, -0.5f, 0.f),  // Bot Left
    float3(0.5f, 0.5f, 0.f),    // Top Right
    float3(-0.5f, 0.5f, 0.f)    // Top Left
};



//----------------------------------------------------------------------------------------------------------------------
Texture2D<float4> SurfaceColorTexture : register(t0);



//----------------------------------------------------------------------------------------------------------------------
SamplerState SurfaceSampler : register(s0);



//----------------------------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4 gameToRender;
	float4x4 worldToCamera;
	float4x4 cameraToClip;
};



// Model constants are b3 (unused here)



// Font constants are b4 (unused here)



//----------------------------------------------------------------------------------------------------------------------
cbuffer SpriteSheetConstants : register(b5)
{
	//------------------------------------------------------
	uint2	layout			: LAYOUT;			// 8 bytes
	uint2	edgePadding		: EDGEPADDING;		// 8 bytes
	//------------------------------------------------------
	uint2	innerPadding	: INNERPADDING;		// 8 bytes
	uint2	textureDims		: TEXTUREDIMS;		// 8 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer StaticWorldConstants : register(b6)
{
	//------------------------------------------------------
    float chunkWidth; // 4 bytes
    float tileWidth; // 4 bytes
    int numTilesInRow; // 4 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer LightingConstants : register(b7)
{
	//------------------------------------------------------
    float4 outdoorLightTint;
	//------------------------------------------------------
    float4 indoorLightTint;
	//------------------------------------------------------
    float4 ambientLightTint;
	//------------------------------------------------------
    float ambientLightIntensity;
	//------------------------------------------------------
    bool isLightingEnabled;
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
float4 ComputeUVs(VSInput vin)
{
    int spriteX = (int) (vin.spriteIndex % layout.x);
    int spriteY = (int) (vin.spriteIndex / layout.x);

    uint2 spriteDims = (textureDims - (edgePadding * 2) - (innerPadding * (layout - int2(1, 1)))) / layout;

    float4 result = float4(0, 0, 0, 0);
    result.r = edgePadding.x + (spriteX * spriteDims.x);
    result.g = edgePadding.y + (spriteY * spriteDims.y);
    result.ba = result.rg + spriteDims;
    
    result.rg /= float2(textureDims);
    result.ba /= float2(textureDims);
    
    float2 center = (result.rg + result.ba) * 0.5f;
    float2 vecToBotLeft = result.rg - center;
    float2 vecToTopRight = result.ba - center;
    vecToBotLeft *= 0.99f;
    vecToTopRight *= 0.99f;
    result.rg = center + vecToBotLeft;
    result.ba = center + vecToTopRight;

    return result;
}


//----------------------------------------------------------------------------------------------------------------------
float3 GetRotatedBy(float3 vec, float degrees)
{
    float vectorLength = length(vec.xy);
    float angleRad = atan2(vec.y, vec.x);
    float angleDegrees = angleRad * (180.f / 3.14159265358979323846f);
    angleDegrees += degrees;
    angleRad = angleDegrees * (3.14159265358979323846f / 180.f);
    float3 result;
    result.x = cos(angleRad) * vectorLength;
    result.y = sin(angleRad) * vectorLength;
    result.z = vec.z;
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
VSOutput VertexMain(VSInput vin, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    float3 pos = QuadVerts[vertexID];               // Local Position
    pos *= float3(vin.instanceDims, 1.f);           // Scale
    pos = GetRotatedBy(pos, vin.instanceRotation);  // Rotate around Z axis
    pos += float3(vin.instancePosition);            // Translate
	
	VSOutput output;
    output.position = float4(pos, 1.f);
    output.position = mul(gameToRender, output.position);
    output.position = mul(worldToCamera, output.position);
    output.position = mul(cameraToClip, output.position);
	
    output.tint = vin.instanceTint;
    output.indoorLight = vin.indoorLight;
    output.outdoorLight = vin.outdoorLight;
	
    float4 spriteUVs = ComputeUVs(vin);
    if (vertexID == 0 || vertexID == 3)
    {
		// Bot left corner
        output.uvs = spriteUVs.rg;
    }
    else if (vertexID == 5)
    {
		// Top left corner
        output.uvs = float2(spriteUVs.r, spriteUVs.a);
    }
    else if (vertexID == 2 || vertexID == 4)
    {
		// Top right corner
        output.uvs = spriteUVs.ba;
    }
    else // if (vertexID == 1)
    {
		// Bot right corner
        output.uvs = float2(spriteUVs.b, spriteUVs.g);
    }
	
    return output;
}



//----------------------------------------------------------------------------------------------------------------------
float4 PixelMain(VSOutput input) : SV_Target0
{
    float2 texCoord = input.uvs;
    float4 surfaceColor = SurfaceColorTexture.Sample(SurfaceSampler, texCoord);
    
    float4 finalColor = (input.tint * surfaceColor);
    
    if (isLightingEnabled)
    {
        float3 indoorLightContribution = indoorLightTint.rgb * input.indoorLight;
        float3 outdoorLightContribution = outdoorLightTint.rgb * input.outdoorLight;
        //float3 ambientLightContribution = ambientLightTint.rgb * ambientLightIntensity;
	
        float3 totalLightContribution = outdoorLightContribution + indoorLightContribution;
        totalLightContribution = saturate(totalLightContribution);
        
        finalColor *= float4(totalLightContribution, 1);
    }
    
    if (finalColor.a <= 0.001f)
    {
        discard;
    }
    
	return finalColor;
}