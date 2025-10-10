﻿// Bradley Christensen - 2022-2025
#include "Engine/Renderer/Font.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/TextureAsset.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Vec2 Font::AlignCentered	= Vec2(0.f, 0.f);
Vec2 Font::AlignTop			= Vec2(0.f, 1.f);
Vec2 Font::AlignBottom		= Vec2(0.f, -1.f);
Vec2 Font::AlignRight		= Vec2(1.0f, 0.f);
Vec2 Font::AlignLeft		= Vec2(-1.f, 0.5f);
Vec2 Font::AlignTopRight	= Vec2(1.f, 1.f);
Vec2 Font::AlignTopLeft		= Vec2(-1.f, 1.f);
Vec2 Font::AlignBottomRight = Vec2(1.0f, -1.f);
Vec2 Font::AlignBottomLeft	= Vec2(-1.f, -1.f);



//----------------------------------------------------------------------------------------------------------------------
Font::~Font()
{

}



//----------------------------------------------------------------------------------------------------------------------
void Font::ReleaseResources()
{
	g_assetManager->Release(m_textureAsset);
	g_renderer->ReleaseShader(m_shader);
}



//----------------------------------------------------------------------------------------------------------------------
void Font::SetRendererState() const
{
	g_renderer->SetCullMode(CullMode::None);
	g_renderer->SetWindingOrder(Winding::CounterClockwise);
	g_renderer->SetFillMode(FillMode::Solid);
	g_renderer->SetBlendMode(BlendMode::Alpha);
	g_renderer->BindTexture(m_texture);
	g_renderer->BindShader(m_shader);
}



//----------------------------------------------------------------------------------------------------------------------
void Font::AddVertsForText2D(VertexBuffer& out_verts, Vec2 const& textMins, float cellHeight,
	std::string const& text, Rgba8 const& tint)
{
	float penPosition = 0.f;
	for (int glyphIndex = 0; glyphIndex < (int) text.size(); ++glyphIndex)
	{
		uint8_t const& currentChar = text[glyphIndex];
		GlyphData& charData = m_glyphData[currentChar];
		
		float a = charData.a * cellHeight;
		float c = charData.c * cellHeight;
		float yOffset = charData.m_yOffset * cellHeight;
		
		penPosition += a;
		
		float glyphWidth = cellHeight * m_glyphData[currentChar].m_width;
		float glyphHeight = cellHeight * m_glyphData[currentChar].m_height;

		Vec2 currentGlyphPosTopLeft = textMins + Vec2(penPosition, cellHeight - yOffset); // start glyphs at the top left for some dumb reason
		Vec2 currentGlyphPosBotRight = currentGlyphPosTopLeft + Vec2(glyphWidth, -glyphHeight);
		
		AABB2 bounds, UVs;
		bounds.mins = Vec2(currentGlyphPosTopLeft.x, currentGlyphPosBotRight.y);
		bounds.maxs = Vec2(currentGlyphPosBotRight.x, currentGlyphPosTopLeft.y);

		UVs.mins = m_glyphData[currentChar].m_uvMins;
		UVs.maxs = m_glyphData[currentChar].m_uvMaxs;

		VertexUtils::AddVertsForAABB2(out_verts, bounds, tint, UVs);
		
		penPosition += glyphWidth;
		penPosition += c; // todo: don't add c for final character? seems right... maybe

		if (glyphIndex < (int) text.size() - 1)
		{
			uint8_t nextChar = text[glyphIndex + 1];
			float kerning = GetKerning(currentChar, nextChar);
			float scaledKerning = kerning * cellHeight;
			penPosition += scaledKerning;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Font::AddVertsForAlignedText2D(VertexBuffer& out_verts, Vec2 const& pivot, Vec2 const& alignment,
	float cellHeight, std::string const& text, Rgba8 const& tint, float lineSpacing /*= 0.5f*/)
{
	Strings lines = StringUtils::SplitStringOnDelimiter(text, '\n');
	if (lines.empty())
	{
		return;
	}

	if (lines.back().empty())
	{
		// Trims the last trailing new line
		lines.pop_back();
	}

	float paragraphHeight = cellHeight * (float) lines.size() + (lineSpacing * cellHeight) * ((float) lines.size() - 1);
	float paragraphWidth = 0.f;

	for (int i = 0; i < (int) lines.size(); ++i)
	{
		std::string const& line = lines[i];
		float lineWidth = GetTextWidth(cellHeight, line);
		paragraphWidth = MathUtils::Max(paragraphWidth, lineWidth);
	}

	Vec2 topLeft;
	topLeft.x = pivot.x - (paragraphWidth * 0.5f);
	topLeft.x += alignment.x * paragraphWidth * 0.5f;
	topLeft.y = pivot.y + (paragraphHeight * 0.5f);
	topLeft.y += alignment.y * paragraphHeight * 0.5f;

	float lineSpacingHeight = lineSpacing * cellHeight;

	for (int i = 0; i < (int) lines.size(); ++i)
	{
		std::string const& line = lines[i];
		float yOffset = -1.f * ((float)(i + 1) * cellHeight + ((float)(i * lineSpacingHeight)));
		Vec2 textMins = topLeft + Vec2(0.f, yOffset);
		AddVertsForText2D(out_verts, textMins, cellHeight, line, tint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
//void Font::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& textMins, Vec2 const& textMaxs,
//	float cellHeight, std::string const& text, Rgba8 const& tint, Vec2 const& alignment)
//{
//	
//}



//----------------------------------------------------------------------------------------------------------------------
float Font::GetTextWidth(float cellHeight, std::string const& text)
{
	float penPosition = 0.f;
	for (int glyphIndex = 0; glyphIndex < (int) text.size(); ++glyphIndex)
	{
		uint8_t const& currentChar = text[glyphIndex];
		GlyphData& charData = m_glyphData[currentChar];

		float a = charData.a * cellHeight;
		float glyphWidth = cellHeight * m_glyphData[currentChar].m_width;
		float c = charData.c * cellHeight;

		penPosition += a;
		penPosition += glyphWidth;
		penPosition += c; // todo: don't add c for final character? seems right... maybe

		if (glyphIndex < (int) text.size() - 1)
		{

			uint8_t nextChar = text[glyphIndex + 1];
			float kerning = GetKerning(currentChar, nextChar);
			float scaledKerning = kerning * cellHeight;
			penPosition += scaledKerning;
		}
	}
	return penPosition;
}



//----------------------------------------------------------------------------------------------------------------------
GlyphData const& Font::GetGlyphData(uint8_t glyph) const
{
	return m_glyphData[glyph];
}



//----------------------------------------------------------------------------------------------------------------------
float Font::GetKerning(uint8_t lhs, uint8_t rhs) const
{
	// Kernings are stored in a vector of pair of glyph id and kerning value
	for (auto& kerningPair : m_kerningData[(size_t) lhs])
	{
		if (kerningPair.glyph == rhs)
		{
			return kerningPair.value;
		}
	}
	return 0.f;
}



//----------------------------------------------------------------------------------------------------------------------
float Font::GetOffsetXOfCharIndex(std::string const& line, int index, float cellHeight, float aspectMultiplier) const
{
	float cursorOffsetX = 0.f;
	for (int i = 0; i < index && i < (int) line.size(); ++i)
	{
		char c = line[i];
		cursorOffsetX += m_glyphData[(uint8_t)c].m_xAdvance * cellHeight * aspectMultiplier;
	}
	return cursorOffsetX;
}



//----------------------------------------------------------------------------------------------------------------------
TextureID Font::GetTexture() const
{
	return m_texture;
}



//----------------------------------------------------------------------------------------------------------------------
ShaderID Font::GetShader() const
{
	return m_shader;
}



//----------------------------------------------------------------------------------------------------------------------
void Font::LoadFNT(const char* fntFilepath)
{
    XmlDocument resourcesXMLDoc;
    XmlError error = resourcesXMLDoc.LoadFile(fntFilepath);
    if (error != XmlError::XML_SUCCESS)
    {
        ERROR_AND_DIE(StringUtils::StringF("Failed to open Xml doc: %s", fntFilepath))
    }
    
	XmlElement const* fontElem = resourcesXMLDoc.RootElement();
    if (fontElem)
	{
		auto commonElem = fontElem->FirstChildElement("common");
		int lineHeight = commonElem->FindAttribute("lineHeight")->IntValue();
		float oneOverLightHeight = 1.f / (float) lineHeight;
		int imagePixelWidth = commonElem->FindAttribute("scaleW")->IntValue();
		int imagePixelHeight = commonElem->FindAttribute("scaleH")->IntValue();
		int pages = commonElem->FindAttribute("pages")->IntValue();
		ASSERT_OR_DIE(pages == 1, StringUtils::StringF("Font %s: more than 1 page for font not supported.", fntFilepath))

		// get texture name
		auto pagesElem = fontElem->FirstChildElement("pages");
		auto page = pagesElem->FirstChildElement("page");
		auto filename = page->FindAttribute("file");
		std::string fullFilePath = "Data/Fonts/" + std::string(filename->Value());

		// Set up texture, but don't create gpu resources yet
		m_textureName = fullFilePath;
		m_textureAsset = g_assetManager->LoadSynchronous<TextureAsset>(fullFilePath);
		ASSERT_OR_DIE(m_textureAsset != AssetID::Invalid, StringUtils::StringF("Font %s: failed to load texture %s", fntFilepath, fullFilePath.c_str()));

		TextureAsset* asset = g_assetManager->Get<TextureAsset>(m_textureAsset);
		m_texture = asset->GetTextureID();

		auto charsElem = fontElem->FirstChildElement("chars");
		auto count = charsElem->FindAttribute("count");

		int charCount = count->IntValue();
		ASSERT_OR_DIE(charCount <= 256, StringUtils::StringF("Font %s: char count exceeds supported amount (256)", fntFilepath))

		auto charElem = charsElem->FirstChildElement("char");
		for (int i = 0; i < charCount; ++i)
		{
			int charID = charElem->FindAttribute("id")->IntValue();
			int width = charElem->FindAttribute("width")->IntValue();
			int height = charElem->FindAttribute("height")->IntValue();
			int x = charElem->FindAttribute("x")->IntValue();
			int y = charElem->FindAttribute("y")->IntValue();
			int xoffset = charElem->FindAttribute("xoffset")->IntValue();
			int xadvance = charElem->FindAttribute("xadvance")->IntValue();
			int yoffset = charElem->FindAttribute("yoffset")->IntValue();

			float uvMinX = float(x) / float(imagePixelWidth);
			float uvMaxX = float(x + width) / float(imagePixelWidth);
			float uvMinY = float(imagePixelHeight - y - height) / float(imagePixelHeight); // min y uvs has the added height because 0,0 is in top left
			float uvMaxY = float(imagePixelHeight - y) / float(imagePixelHeight);

			GlyphData& charData = m_glyphData[charID];
			charData.m_aspect = float(width) / float(height);
			charData.m_uvMins = Vec2(uvMinX, uvMinY);
			charData.m_uvMaxs = Vec2(uvMaxX, uvMaxY);
			charData.a = float(xoffset) * oneOverLightHeight;
			charData.b = float(width) * oneOverLightHeight;
			charData.c = float(xadvance - width - xoffset) * oneOverLightHeight;
			charData.m_xAdvance = float(xadvance) * oneOverLightHeight;
			charData.m_yOffset = float(yoffset) * oneOverLightHeight;
			charData.m_width = float(width) * oneOverLightHeight;
			charData.m_height = float(height) * oneOverLightHeight;
			charElem = charElem->NextSiblingElement("char");
		}

		auto kerningsElem = fontElem->FirstChildElement("kernings");
		int kerningsCount = kerningsElem->FindAttribute("count")->IntValue();
		auto kerningElem = kerningsElem->FirstChildElement("kerning");

		for (int i = 0; i < kerningsCount; ++i)
		{
			int first = kerningElem->FindAttribute("first")->IntValue();
			int second = kerningElem->FindAttribute("second")->IntValue();
			int amount = kerningElem->FindAttribute("amount")->IntValue();

			float scaledKerning = (float) amount * oneOverLightHeight;

			m_kerningData[first].emplace_back(second, scaledKerning);
			kerningElem = kerningElem->NextSiblingElement("kerning");
		}
	}
}
