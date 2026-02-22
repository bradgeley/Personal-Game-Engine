// Bradley Christensen - 2022-2026
#pragma once
#include <cstddef>
#include <vector>



struct InputLayout;



//----------------------------------------------------------------------------------------------------------------------
// Determines the usage flags of the buffer
//
enum class BufferType
{
    VertexBuffer,
    IndexBuffer,
    InstanceBuffer,
    ConstantBuffer,
    Unknown
};



//----------------------------------------------------------------------------------------------------------------------
struct GpuBufferConfig
{
	BufferType m_bufferType     = BufferType::Unknown;
};



//----------------------------------------------------------------------------------------------------------------------
// GPU Buffer
//
// Generic GPU Buffer, to be used for vertex, index, instance buffers, etc.
// Stores a CPU-side copy of the data that is only pushed to the GPU when needed.
//
class GPUBuffer
{
    friend class Renderer;

protected:

    GPUBuffer(GpuBufferConfig const& config);
    
public:

    GPUBuffer(GPUBuffer const& copy) = delete;
    virtual ~GPUBuffer() = default;

    virtual void Initialize(size_t byteWidth) = 0;
     
    virtual void UpdateCPUBuffer(void const* data, size_t size);
	virtual void AddToCPUBuffer(void const* data, size_t size);
    
    virtual void ReleaseResources() = 0;
    virtual void UpdateGPUBuffer() = 0;

    void SetDirty();
    bool IsDirty() const;
	void Reserve(size_t byteWidth);
	void Resize(size_t byteWidth);
	void ClearCPUBuffer();
    size_t GetCPUBufferSize() const;
    uint8_t const* GetCPUBufferData() const;
    uint8_t* GetCPUBufferData();

protected:
    
    GpuBufferConfig const m_config;

	bool m_isDirty = true;
	size_t m_gpuBufferSize = 0;
    std::vector<uint8_t> m_cpuBuffer;
};