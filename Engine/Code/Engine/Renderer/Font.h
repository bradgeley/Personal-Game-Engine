// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Core/Name.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/RendererUtils.h"
#include "Rgba8.h"
#include <string>
#include <vector>



struct Vertex_PCU;
class Shader;
class Texture;
class VertexBuffer;



constexpr int MAX_GLYPHS = UINT8_MAX + 1;



//----------------------------------------------------------------------------------------------------------------------
struct KerningPair
{
    KerningPair(int glyph, float val) : glyph(glyph), value(val) {}
    
    int glyph;
    float value;
};



//----------------------------------------------------------------------------------------------------------------------
struct GlyphData
{
    float m_aspect	 = 1.f;
    Vec2 m_uvMins	 = Vec2::ZeroVector;
    Vec2 m_uvMaxs	 = Vec2::ZeroVector;
    float a			 = 0.f;
    float b			 = 0.f; 
    float c			 = 0.f;
    float m_xAdvance = 0.f;
    float m_yOffset	 = 0.f;
    float m_height	 = 1.f;
    float m_width	 = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
class Font
{
    friend class Renderer;

public:

    Font() = default;
    ~Font();

	void ReleaseResources();

    TextureID GetTexture() const;
    ShaderID GetShader() const;

    void LoadFNT(const char* fntFilepath);
    
	void SetRendererState() const;
    
    // Vert Helpers
    void AddVertsForText2D(VertexBuffer& out_verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::Black);
    void AddVertsForAlignedText2D(VertexBuffer& out_verts, Vec2 const& pivot, Vec2 const& alignment, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::Black);
    //void AddVertsForTextInBox2D(VertexBuffer& out_verts, Vec2 const& textMins, Vec2 const& textMaxs, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::Black, Vec2 const& alignment = AlignCentered);

    float GetTextWidth(float cellHeight, std::string const& text);
    GlyphData const& GetGlyphData(uint8_t glyph) const;
    float GetKerning(uint8_t lhs, uint8_t rhs) const;
    float GetOffsetXOfCharIndex(std::string const& line, int index, float cellHeight, float aspectMultiplier = 1.f) const;

public:

    static Vec2 AlignCentered;
    static Vec2 AlignTop;
    static Vec2 AlignBottom;
    static Vec2 AlignRight;
    static Vec2 AlignLeft;
    static Vec2 AlignTopRight;
    static Vec2 AlignTopLeft;
    static Vec2 AlignBottomRight;
    static Vec2 AlignBottomLeft;

public:

	Name m_textureName = Name::s_invalidName;   
	AssetID m_textureAsset = INVALID_ASSET_ID;
    TextureID m_texture = RendererUtils::InvalidID;
    ShaderID m_shader = RendererUtils::InvalidID;
    GlyphData m_glyphData[MAX_GLYPHS];
    std::vector<KerningPair> m_kerningData[MAX_GLYPHS];
};




