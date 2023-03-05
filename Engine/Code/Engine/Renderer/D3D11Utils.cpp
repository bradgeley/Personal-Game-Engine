// Bradley Christensen - 2022-2023
#include "D3D11Utils.h"
#include "RendererInternal.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
D3D11_FILTER GetD3D11SamplerFilter(SamplerFilter filter, float& out_maxAnisotropy)
{
    out_maxAnisotropy = 0.f;
    switch (filter)
    {
        case SamplerFilter::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
        
        case SamplerFilter::Bilinear: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        
        case SamplerFilter::Trilinear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        
        case SamplerFilter::Anisotropic_2: 
            out_maxAnisotropy = 2.f;
            return D3D11_FILTER_ANISOTROPIC;
        
        case SamplerFilter::Anisotropic_4: 
            out_maxAnisotropy = 4.f;
            return D3D11_FILTER_ANISOTROPIC;
        
        case SamplerFilter::Anisotropic_8: 
            out_maxAnisotropy = 8.f;
            return D3D11_FILTER_ANISOTROPIC;
        
        case SamplerFilter::Anisotropic_16:
            out_maxAnisotropy = 16.f;
            return D3D11_FILTER_ANISOTROPIC;
        
        default:
            break;
    }
    ERROR_AND_DIE("SamplerFilter not supported.")
}



//----------------------------------------------------------------------------------------------------------------------
D3D11_TEXTURE_ADDRESS_MODE GetD3D11SamplerAddressMode(SamplerAddressMode addressMode)
{
    switch (addressMode)
    {
        case SamplerAddressMode::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
        default: return D3D11_TEXTURE_ADDRESS_CLAMP;
    }
}



//----------------------------------------------------------------------------------------------------------------------
D3D11_CULL_MODE GetD3D11CullMode(CullMode cullMode)
{
    switch (cullMode)
    {
        case CullMode::Front: return D3D11_CULL_FRONT;
        case CullMode::Back: return D3D11_CULL_BACK;
        default: return D3D11_CULL_NONE;
    }
}



//----------------------------------------------------------------------------------------------------------------------
D3D11_FILL_MODE GetD3D11FillMode(FillMode fillMode)
{
    switch (fillMode)
    {
        case FillMode::Wireframe: return D3D11_FILL_WIREFRAME;
        default: return D3D11_FILL_SOLID;
    }
}
