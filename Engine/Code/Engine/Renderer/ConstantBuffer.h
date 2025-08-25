// Bradley Christensen - 2022-2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Constant Buffer
//
class ConstantBuffer
{
    friend class RendererInterface;

protected:

    ConstantBuffer() = default;
    
public:

    ConstantBuffer(ConstantBuffer const& copy) = delete;
    virtual ~ConstantBuffer() = default;

    virtual void ReleaseResources() = 0;
    
    virtual void Update(void const* data, size_t byteWidth) = 0;

protected:

    virtual void Initialize(size_t byteWidth) = 0;

protected:
    
    size_t m_gpuBufferByteWidth = 0;
};