// Bradley Christensen - 2022
#pragma once



struct ID3D11Buffer;



//----------------------------------------------------------------------------------------------------------------------
// Constant Buffer
//
class ConstantBuffer
{
    friend class Renderer;
    
public:

    ConstantBuffer() = default;
    ConstantBuffer(ConstantBuffer const& copy) = delete;

    void ReleaseResources();
    
    void Update(void const* data, size_t byteWidth);

private:

    void Initialize(size_t byteWidth);

protected:
    
    ID3D11Buffer* m_handle = nullptr;
    size_t m_gpuBufferByteWidth = 0;
};