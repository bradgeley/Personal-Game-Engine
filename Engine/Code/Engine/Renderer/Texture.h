// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/IntVec2.h"
#include <string>



class Image;
struct Rgba8;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct IDXGISwapChain;



//----------------------------------------------------------------------------------------------------------------------
class Texture
{
    friend class Renderer;

public:

    Texture() = default;

    bool CreateUniformTexture(IntVec2 const& dims, Rgba8 const& tint);
    bool CreateFromImage(Image const& image, bool createMipMap = true);
    bool LoadFromImageFile(const char* imageSource, bool createMipMap = true);
    
    void ReleaseResources();
    
protected:

    // Renderer constructor for creating a texture from the swap chain backbuffer
    bool CreateFromSwapChain(IDXGISwapChain* swapChain);
    
    ID3D11DepthStencilView*		CreateOrGetDepthStencilView();
    ID3D11RenderTargetView*		CreateOrGetRenderTargetView();
    ID3D11ShaderResourceView*	CreateOrGetShaderResourceView();
    
    static Texture*	CreateDepthBuffer(IntVec2 const& texelSize);

protected:

    std::string m_sourceImagePath;
    IntVec2 m_dimensions;
    
    ID3D11Texture2D*			m_textureHandle = nullptr;
    ID3D11DepthStencilView*		m_depthStencilView = nullptr;
    ID3D11RenderTargetView*		m_renderTargetView = nullptr;
    ID3D11ShaderResourceView*	m_shaderResourceView = nullptr;		
};
