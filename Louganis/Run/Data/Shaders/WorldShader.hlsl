// Bradley Christensen - 2022-2025



//----------------------------------------------------------------------------------------------------------------------
struct VSInput
{
	//------------------------------------------------------
	// Vertex Data (0)
	//------------------------------------------------------
    float3	position			: POSITION;
    float4	tint				: TINT;
    float2	uvs					: UVS;
    float2	lightmapUVs			: LIGHTMAPUVS;
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct VSOutput
{
	float4 position			: SV_Position;
	float4 tint				: TINT;
	float2 uvs				: UVS;
	float2 lightmapUVs		: LIGHTMAPUVS;
};



//----------------------------------------------------------------------------------------------------------------------
Texture2D<float4> SurfaceColorTexture : register(t0);
Texture2D<float4> ChunkLightmapTexture : register(t1);



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



// Sprite sheet constants are b5 (unused here)



//----------------------------------------------------------------------------------------------------------------------
cbuffer StaticWorldConstants : register(b6)
{
	//------------------------------------------------------
	float	chunkWidth;			                // 4 bytes
	float	tileWidth;		                    // 4 bytes
    int     numTilesInRow;                      // 4 bytes
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
VSOutput VertexMain(VSInput vin)
{
	VSOutput output;
    output.position = float4(vin.position, 1.f);
    output.position = mul(gameToRender, output.position);
    output.position = mul(worldToCamera, output.position);
    output.position = mul(cameraToClip, output.position);
	
    output.tint = vin.tint;
    output.uvs = vin.uvs;
	
    output.lightmapUVs = vin.lightmapUVs; // todo
	
    return output;
}



//----------------------------------------------------------------------------------------------------------------------
float4 PixelMain(VSOutput input) : SV_Target0
{
	float2 texCoord = input.uvs;
	float4 surfaceColor = SurfaceColorTexture.Sample(SurfaceSampler, texCoord);
    float4 lightmapValue = ChunkLightmapTexture.Sample(SurfaceSampler, input.lightmapUVs);
	
	float4 tint = input.tint;
    float4 finalColor = (tint * surfaceColor);
	
    if (isLightingEnabled)
    {
        float indoorLightingBrightness = lightmapValue.x;
        float3 indoorLightContribution = indoorLightTint.rgb * indoorLightingBrightness;
        //float3 ambientLightContribution = ambientLightTint.rgb * ambientLightIntensity;
	
        float outdoorLightingBrightness = lightmapValue.g;
        float3 outdoorLightContribution = outdoorLightTint.rgb * outdoorLightingBrightness;
	
        float3 totalLightContribution = outdoorLightContribution + indoorLightContribution;
        totalLightContribution = saturate(totalLightContribution);
        finalColor *= float4(totalLightContribution, 1.f);
    }
    
    if (finalColor.a <= 0.001f)
    {
        discard;
    }
    
    return finalColor;
}