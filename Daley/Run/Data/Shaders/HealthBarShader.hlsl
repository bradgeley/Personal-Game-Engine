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
    uint    iconMask            : ICONMASK;         // 4 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
struct VSOutput
{
    float4  position            : SV_Position;
    float2  uvs                 : UVS;
    
    float   healthFraction      : HEALTHFRACTION;
    float   fireFraction        : FIREFRACTION;
    float   poisonFraction      : POISONFRACTION;
    
    bool    isIconVert          : ISICONVERT;
};



//----------------------------------------------------------------------------------------------------------------------
Texture2D<float4> IconTexture : register(t0);



//----------------------------------------------------------------------------------------------------------------------
SamplerState SurfaceSampler : register(s0);



//----------------------------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 gameToRender;
    float4x4 worldToCamera;
    float4x4 cameraToClip;
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer SpriteSheetConstants : register(b5)
{
	//------------------------------------------------------
    uint2 layout                : LAYOUT;       // 8 bytes
    uint2 edgePadding           : EDGEPADDING;  // 8 bytes
	//------------------------------------------------------
    uint2 innerPadding          : INNERPADDING; // 8 bytes
    uint2 textureDims           : TEXTUREDIMS;  // 8 bytes
	//------------------------------------------------------
};



//----------------------------------------------------------------------------------------------------------------------
cbuffer HealthBarRenderConstants : register(b6)
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
float4 ComputeUVs(uint spriteIndex)
{
    int spriteX = (int) (spriteIndex % layout.x);
    int spriteY = (int) (spriteIndex / layout.x);

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
VSOutput VertexMain(VSInput vin, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    int localVertexID = vertexID % 6;
    float2 pos = vin.position;
    float4 healthBarCenter = float4(pos, 0, 1);
    
    VSOutput output;
    output.healthFraction = vin.healthFraction;
    output.fireFraction = vin.fireFraction;
    output.poisonFraction = vin.poisonFraction;
    
    if (vertexID >= 0 && vertexID < 6)
    {
        // Health bar verts
        
        output.isIconVert = false;
        
        float2 vertOffset = QuadVerts[localVertexID] - float2(0.5, 0.5);
        float2 scaledVertOffset = vertOffset * vin.dimensions;
        float4 vertPos = healthBarCenter + float4(scaledVertOffset, 0, 0);
    
        output.position = vertPos;
    
        output.uvs = QuadVerts[localVertexID];
    }
    else 
    {
        // Icon verts
        
        output.isIconVert = true;
        
        int iconIndex = -1;
        int iconSlotIndex = (vertexID / 6) - 1;
        
        // Calculate Vertex position
        float2 iconsOrigin = healthBarCenter.xy + float2(-vin.dimensions.x * 0.5f, vin.dimensions.y * 0.5f);
        float2 iconDimentions = float2(vin.dimensions.x / 3.f, vin.dimensions.x / 3.f);
        float2 iconOffset = float2(float(iconSlotIndex) * iconDimentions.x, 0);
        
        float2 iconOrigin = iconsOrigin + iconOffset;
        float2 iconTopLeft = iconOrigin + float2(0, iconDimentions.y);
        float2 iconTopRight = iconOrigin + float2(iconDimentions.x, iconDimentions.y);
        float2 iconBotRight = iconOrigin + float2(iconDimentions.x, 0);
        
        int iconsCount = 0;
        for (int i = 0; i < 32; i++)
        {
            if ((vin.iconMask & (1 << i)) != 0)
            {
                iconsCount++;
                if (iconsCount > iconSlotIndex)
                {
                    iconIndex = i;
                    break;
                }
            }
        }
        
        if (iconIndex == -1)
        {
            output.position = float4(0, 0, 0, 0);
            output.uvs = float2(0, 0);
            return output;
        }
        
        float4 spriteUVs = ComputeUVs(iconIndex);
        
        if (localVertexID == 0 || localVertexID == 3)
        {
		    // Bot left corner
            output.uvs = spriteUVs.rg;
            output.position = float4(iconOrigin, 0, 1);
        }
        else if (localVertexID == 5)
        {
		    // Top left corner
            output.uvs = float2(spriteUVs.r, spriteUVs.a);
            output.position = float4(iconTopLeft, 0, 1);

        }
        else if (localVertexID == 2 || localVertexID == 4)
        {
		    // Top right corner
            output.uvs = spriteUVs.ba;
            output.position = float4(iconTopRight, 0, 1);
        }
        else // if (localVertexID == 1)
        {
		    // Bot right corner
            output.uvs = float2(spriteUVs.b, spriteUVs.g);
            output.position = float4(iconBotRight, 0, 1);
        }
    }
	
    output.position = mul(gameToRender, output.position);
    output.position = mul(worldToCamera, output.position);
    output.position = mul(cameraToClip, output.position);
    
    return output;
}



//----------------------------------------------------------------------------------------------------------------------
float4 PixelMain(VSOutput input) : SV_Target0
{
    float4 finalColor = float4(0, 0, 0, 0);
    
    if (input.isIconVert)
    {
        // Icon Verts
        
        finalColor = IconTexture.Sample(SurfaceSampler, input.uvs);
    }
    else
    {
        // Health bar verts
    
        finalColor = backgroundTint;
    
        if (input.uvs.x < input.healthFraction)
        {
            finalColor = healthTint;
        
            float fireThreshold = input.fireFraction * input.healthFraction;
            if (input.uvs.x < fireThreshold)
            {
                finalColor = fireTint;
            }
    
            float poisonThreshold = input.healthFraction - input.poisonFraction * input.healthFraction;
            if (input.uvs.x > poisonThreshold)
            {
                finalColor = poisonTint;
            }
        }
    }
    
    if (finalColor.a <= 0.001f)
    {
        discard;
    }
    
	return finalColor;
}