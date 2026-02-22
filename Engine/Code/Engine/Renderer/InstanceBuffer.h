// Bradley Christensen - 2022-2026
#pragma once
#include <vector>



class GPUBuffer;



//----------------------------------------------------------------------------------------------------------------------
// InstanceBuffer
//
// Storage for cpu instances and their connection to a gpu buffer
// When the buffer is drawn, it will first update the gpu buffer if dirty.
// Uses a template function to initialize and add instances of any type. After initialization, the instances must be the same size
//
class InstanceBuffer
{
    friend class Renderer;

protected:

    InstanceBuffer() = default;
    
public:

    InstanceBuffer(InstanceBuffer const& copy) = delete;
    virtual ~InstanceBuffer() = default;

	virtual void ReleaseResources();

    template<typename T>
    void Initialize(size_t initialInstanceCount = 0);
    
    template<typename T>
    void AddInstance(T const& instance);

	template<typename T>
    void AddInstances(std::vector<T> const& instance);

    template<typename T>
    T& GetInstance(size_t index);

    template<typename T>
    T const& GetInstance(size_t index) const;

    virtual void ReserveAdditional(int numExpectedAdditionalInstances);
    virtual void ClearInstances();

    virtual int GetStride() const;
	virtual int GetNumInstances() const;

	virtual bool IsDirty() const;
    virtual bool IsEmpty() const;
	virtual void UpdateGPUBuffer();

protected:

    virtual void InitializeInternal(size_t instanceSize, size_t initialInstanceCount = 0) = 0;
    virtual void AddInstanceInternal(void const* instance, size_t instanceSize);
    virtual void AddInstancesInternal(void const* instance, size_t instanceSize, size_t numInstances);
	virtual void* GetInstanceInternal(size_t instanceSize, size_t index);
	virtual void const* GetInstanceInternal(size_t instanceSize, size_t index) const;

protected:

	size_t m_instanceSize = 0;
    GPUBuffer* m_gpuBuffer = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void InstanceBuffer::Initialize(size_t initialInstanceCount /*= 0*/)
{
    InitializeInternal(sizeof(T), initialInstanceCount);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void InstanceBuffer::AddInstance(T const& instance)
{
	AddInstanceInternal(&instance, sizeof(T));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void InstanceBuffer::AddInstances(std::vector<T> const& instances)
{
	AddInstancesInternal(instances.data(), sizeof(T), instances.size());
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
T& InstanceBuffer::GetInstance(size_t index)
{
    return *reinterpret_cast<T*>(GetInstanceInternal(sizeof(T), index));
}



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline T const& InstanceBuffer::GetInstance(size_t index) const
{
    return *reinterpret_cast<T const*>(GetInstanceInternal(sizeof(T), index));
}
