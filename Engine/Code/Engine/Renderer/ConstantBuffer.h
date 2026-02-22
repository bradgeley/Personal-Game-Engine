// Bradley Christensen - 2022-2026
#pragma once
#include <cstddef>



class GPUBuffer;



//----------------------------------------------------------------------------------------------------------------------
// Constant Buffer
//
class ConstantBuffer
{
    friend class Renderer;

protected:

    ConstantBuffer() = default;
    
public:

    ConstantBuffer(ConstantBuffer const& copy) = delete;
    virtual ~ConstantBuffer() = default;

    virtual void Initialize(size_t byteWidth) = 0;

    virtual void ReleaseResources();
    
    virtual void Update(void const* data, size_t byteWidth, bool updateGPU = true);

	template<typename T>
    void Update(T const& data, bool updateGPU = true);

protected:

    GPUBuffer* m_gpuBuffer = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline void ConstantBuffer::Update(T const& data, bool updateGPU /*= true*/)
{
    Update(&data, sizeof(T), updateGPU);
}