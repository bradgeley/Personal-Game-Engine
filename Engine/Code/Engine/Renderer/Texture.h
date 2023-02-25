// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Math/IntVec2.h"


struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;



class Texture
{
    friend class Renderer;

    void WatchInternal(ID3D11Texture2D* handle);
    void ReleaseResources();

    static Texture*	CreateDepthBuffer(IntVec2 const& texelSize);
    
protected:
    
    ID3D11DepthStencilView*		CreateOrGetDepthStencilView();
    ID3D11RenderTargetView*		CreateOrGetRenderTargetView();
    ID3D11ShaderResourceView*	CreateOrGetShaderResourceView();

protected:

    IntVec2 m_dimensions;
    
    ID3D11Texture2D*			m_textureHandle = nullptr;
    ID3D11DepthStencilView*		m_depthStencilView = nullptr;
    ID3D11RenderTargetView*		m_renderTargetView = nullptr;
    ID3D11ShaderResourceView*	m_shaderResourceView = nullptr;		
};
