// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include <string>



class Image;
class Swapchain;
struct Rgba8;



//----------------------------------------------------------------------------------------------------------------------
class Texture
{
    friend class Renderer;

public:

    Texture() = default;
    virtual ~Texture() = default;

    virtual void ReleaseResources() = 0;

    virtual bool IsValid() const = 0;
    virtual IntVec2 GetDimensions() const;

    virtual bool CreateUniformTexture(IntVec2 const& dims, Rgba8 const& tint);
    virtual bool CreateFromImage(Image const& image, bool createMipMap = true) = 0;
    virtual bool LoadFromImageFile(const char* imageSource, bool createMipMap = true);

    virtual void CopyTo(Swapchain* swapchain) = 0;

protected:

    std::string m_sourceImagePath;
    IntVec2 m_dimensions = IntVec2::ZeroVector;
};
