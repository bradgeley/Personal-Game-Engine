// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Assets/Asset.h"
#include "Engine/Core/Name.h"
#include "Engine/Math/Grid.h"
#include "Engine/Renderer/Rgba8.h"



struct IntVec2;



//----------------------------------------------------------------------------------------------------------------------
// Image
//
// A grid of pixels
//
class Image : public Asset
{
public:
    
    Image() = default;
    explicit Image(IntVec2 const& dimensions, Rgba8 const& color);

    Grid<Rgba8> const& GetPixels() const;
    Grid<Rgba8>& GetPixelsRef();

    // Loading functions
    static Asset* Load(Name assetName);
    virtual bool CompleteAsyncLoad() override;
    virtual bool CompleteSyncLoad() override;
    virtual void ReleaseResources() override;

private:

    Grid<Rgba8> m_pixels;
};
