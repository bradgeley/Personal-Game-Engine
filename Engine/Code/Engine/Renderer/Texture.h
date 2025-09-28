// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Core/Name.h"
#include <string>



class Image;
class Swapchain;
struct Rgba8;



//----------------------------------------------------------------------------------------------------------------------
class Texture
{
    friend class Renderer;
    friend class TextureAsset;

public:

    Texture() = default;
    virtual ~Texture() = default;

	virtual void ReleaseResources() = 0;

    virtual bool IsValid() const = 0;
    virtual IntVec2 GetDimensions() const;

    virtual bool CreateUniformTexture(IntVec2 const& dims, Rgba8 const& tint);
    virtual bool CreateFromImage(Image const& image, bool createMipMap = true, bool isRenderTarget = false) = 0;

    virtual void CopyTo(Swapchain* swapchain) = 0;

	void SetSourceName(Name sourceName);

protected:

	Name m_sourceName       = Name::Invalid;
    IntVec2 m_dimensions    = IntVec2::ZeroVector;
};
