// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Core/Name.h"
#include "Engine/Math/Grid.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>



struct IntVec2;



//----------------------------------------------------------------------------------------------------------------------
// Image
//
// A grid of pixels
//
class Image : public IAsset
{
public:
    
    Image() = default;
    explicit Image(IntVec2 const& dimensions, Rgba8 const& color);

    static IAsset* Load(Name assetName);

    Name GetSourceImagePath() const;
    Grid<Rgba8> const& GetPixels() const;

protected:

	virtual void ReleaseResources() override;

private:

    Grid<Rgba8> m_pixels;
    Name m_sourceImagePath = Name::s_invalidName;
};
