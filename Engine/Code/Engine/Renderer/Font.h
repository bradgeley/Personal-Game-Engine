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
    friend class Renderer;

public:
    
	void SetRendererState() const;
    
    void AddVertsForText2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& textMins, float cellHeight,
        std::string const& text, Rgba8 const& tint = Rgba8::Black);
    
    GlyphData const& GetGlyphData(uint8_t glyph) const;
    float GetKerning(uint8_t lhs, uint8_t rhs) const;
    
    Texture* GetTexture() const;
    Shader* GetShader() const;

protected:

    Font() = default;
    ~Font();

    void LoadFNT(const char* fntFilepath);

protected:

    Texture* m_texture = nullptr;
    Shader* m_shader = nullptr;
    GlyphData m_glyphData[MAX_GLYPHS];
    std::vector<KerningPair> m_kerningData[MAX_GLYPHS];
};




