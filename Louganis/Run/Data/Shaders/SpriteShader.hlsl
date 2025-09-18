// Bradley Christensen - 2022-2025



//----------------------------------------------------------------------------------------------------------------------
struct VSInput
{
	// Vertex Data (0)
	//------------------------------------------------------
    float3	position			: POSITION;
    float4	tint				: TINT;
    float2	uvs					: UVS;
	//------------------------------------------------------
	
	// Instance Data (1)
	//------------------------------------------------------
	float3	instancePosition	: INSTANCEPOSITION;	// 12 bytes
    float	instanceRotation	: INSTANCEROTATION; // 4 bytes
	//------------------------------------------------------
    float4	instanceTint		: INSTANCETINT;		// 16 bytes
	//------------------------------------------------------
	float	instanceScale		: INSTANCESCALE;	// 4 bytes
	uint	spriteIndex			: INDEX;			// 4 bytes
	//		padding									// 8 bytes
	//------------------------------------------------------
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
	float2	padding			: PADDING;			// 8 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct VSOutput
{
	float4 position			: SV_Position;
	float4 tint				: TINT;
	float2 uvs				: UVS;
};



//----------------------------------------------------------------------------------------------------------------------
VSOutput VertexMain(VSInput vin, uint instanceID : SV_InstanceID)
{
	float3 pos = vin.position;
    float3 outPos = pos + vin.instancePosition;
	
	VSOutput output;
    output.position = float4(outPos, 1.f);
    output.position = mul(gameToRender, output.position);
    output.position = mul(worldToCamera, output.position);
    output.position = mul(cameraToClip, output.position);
	
	output.tint = vin.tint;
	output.uvs = vin.uvs;
	return output;
}



//----------------------------------------------------------------------------------------------------------------------
float4 PixelMain(VSOutput input) : SV_Target0
{
	float2 texCoord = input.uvs;
	float4 surfaceColor = SurfaceColorTexture.Sample(SurfaceSampler, texCoord);
	
	float4 tint = input.tint;
	float4 finalColor = tint * surfaceColor;
	return finalColor;
}