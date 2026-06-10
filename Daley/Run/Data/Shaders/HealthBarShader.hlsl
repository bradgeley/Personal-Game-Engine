// Bradley Christensen - 2022-2026



//----------------------------------------------------------------------------------------------------------------------
struct VSInput
{
	//------------------------------------------------------
	// Instance Data (0)
	//------------------------------------------------------
	float2	position	        : POSITION;	        // 8 bytes
    float2	dimensions	        : DIMENSIONS;       // 8 bytes
	//------------------------------------------------------
    float   healthFraction      : HEALTHFRACTION;   // 4 bytes
    float   fireFraction        : FIREFRACTION;     // 4 bytes
    float   poisonFraction      : POISONFRACTION;   // 4 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct VSOutput
{
    float4 position             : SV_Position;
    float2 uvs                  : UVS;
    float healthFraction        : HEALTHFRACTION;
    float fireFraction          : FIREFRACTION;
    float poisonFraction        : POISONFRACTION;
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 gameToRender;
    float4x4 worldToCamera;
    float4x4 cameraToClip;
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer HealthBarRenderConstants : register(b5)
{
	//------------------------------------------------------
	float4	backgroundTint	    : BACKGROUNDTINT;	// 8 bytes
    float4  healthTint          : HEALTHTINT;       // 8 bytes
	//------------------------------------------------------
    float4  fireTint            : FIRETINT;         // 8 bytes
    float4  poisonTint          : POISONTINT;       // 8 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
static const float2 QuadVerts[6] =
{
    float2(0, 0),
    float2(1, 0),
    float2(1, 1),

    float2(0, 0),
    float2(1, 1),
    float2(0, 1)
};



//----------------------------------------------------------------------------------------------------------------------
VSOutput VertexMain(VSInput vin, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    float2 pos = vin.position;
    
    VSOutput output;
    
    float4 healthBarCenter = float4(pos, 0, 1);
    float2 vertOffset = QuadVerts[vertexID] - float2(0.5, 0.5);
    float2 scaledVertOffset = vertOffset * vin.dimensions;
    float4 vertPos = healthBarCenter + float4(scaledVertOffset, 0, 0);
    
    output.position = vertPos;
    output.position = mul(gameToRender, output.position);
    output.position = mul(worldToCamera, output.position);
    output.position = mul(cameraToClip, output.position);
    
    output.uvs = QuadVerts[vertexID];
    output.healthFraction = vin.healthFraction;
    output.fireFraction = vin.fireFraction;
    output.poisonFraction = vin.poisonFraction;
	
    return output;
}



//----------------------------------------------------------------------------------------------------------------------
float4 PixelMain(VSOutput input) : SV_Target0
{
    float4 finalColor = float4(1, 1, 1, 1);
    
	return finalColor;
}