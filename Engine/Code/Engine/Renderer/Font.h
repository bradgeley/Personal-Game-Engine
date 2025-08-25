// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/Vec2.h"
#include "Rgba8.h"
#include <string>
#include <vector>



struct Vertex_PCU;
class Shader;
class Texture;



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
    friend class RendererInterface;

public:

    Font() = default;
    ~Font();

    Texture* GetTexture() const;
    Shader* GetShader() const;

    void LoadFNT(const char* fntFilepath);
    
	void SetRendererState() const;
    
    // Add Verts
    void AddVertsForText2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::Black);
    void AddVertsForAlignedText2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& pivot, Vec2 const& alignment, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::Black);
    //void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& textMins, Vec2 const& textMaxs, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::Black, Vec2 const& alignment = AlignCentered);

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

    Texture* m_texture = nullptr;
    Shader* m_shader = nullptr;
    GlyphData m_glyphData[MAX_GLYPHS];
    std::vector<KerningPair> m_kerningData[MAX_GLYPHS];
};




