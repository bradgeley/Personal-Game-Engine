// Bradley Christensen - 2023
#pragma once
#include <unordered_map>
#include "EntityID.h"
#include "Engine/DataStructures/BitArray.h"



struct Component;



//----------------------------------------------------------------------------------------------------------------------
// Base Storage - Abstract base class for all storage types
//
class BaseStorage 
{
public:

	virtual ~BaseStorage() = default;

	virtual void Destroy(EntityID index)						= 0;
	virtual void Clear()										= 0;
	virtual void NewAdd(EntityID [[maybe_unused]] index, Component const* [[maybe_unused]] copy) {}
};



//----------------------------------------------------------------------------------------------------------------------
enum class ComponentStorageType
{
	ARRAY,
	MAP,
	SINGLETON,
	TAG,
};



//----------------------------------------------------------------------------------------------------------------------
template<typename CType>
class TypedBaseStorage : public BaseStorage
{
public:

	virtual ~TypedBaseStorage() override = default;

	virtual CType*			Get(EntityID eid)							= 0;
	virtual CType const*	Get(EntityID eid) const						= 0;
	virtual CType*			Add(EntityID eid)							= 0;
	virtual CType*			Add(EntityID eid, CType const& copy)		= 0;

	// Not virtual on purpose for performance reasons - this is the most called function in the ECS
	CType& operator [](EntityID id) { return *Get(id); } 
};



//----------------------------------------------------------------------------------------------------------------------
// Array Storage - useful for widely used components
//
template<typename CType>
class ArrayStorage : public TypedBaseStorage<CType>
{
public:

	virtual ~ArrayStorage() override = default;

	virtual CType*			Get(EntityID eid)						override
	{
		return &m_data[eid];
	}

	virtual CType const*	Get(EntityID eid)						const override
	{
		return &m_data[eid];
	}

	virtual CType*			Add(EntityID eid)						override
	{
		m_data[eid] = CType();
		return &m_data[eid];
	}

	virtual CType*			Add(EntityID eid, CType const& copy)	override
	{
		m_data[eid] = copy;
		return &m_data[eid];
	}

	
	virtual void			NewAdd(EntityID eid, Component const* copy)	override
	{
		m_data[eid] = *reinterpret_cast<CType const*>(copy);
	}

	virtual void			Destroy(EntityID)						override
	{
		// pretend it doesn't exist (from admin side)
	}

	virtual void			Clear()									override
	{
		// pretend it doesn't exist (from admin side)
	}

	// Not virtual on purpose for performance reasons - this is the most called function in the ECS
	CType& operator [](EntityID id) { return m_data[id]; }

public:

	CType m_data[MAX_ENTITIES];
};



//----------------------------------------------------------------------------------------------------------------------
// Map Storage - useful for sparsely used components
//
template<typename CType>
class MapStorage : public TypedBaseStorage<CType>
{
public:

	virtual ~MapStorage() override = default;

	virtual CType*			Get(EntityID eid)						override
	{
		auto result = m_data.find(eid);
		if (result != m_data.end())
		{
			return &(result->second);
		}
		return nullptr;
	}
	
	virtual CType const*	Get(EntityID eid)						const override
	{
		auto result = m_data.find(eid);
		if (result != m_data.end())
		{
			return &(result->second);
		}
		return nullptr;
	}
	
	virtual CType*			Add(EntityID eid)						override
	{
		m_data[eid] = CType();
		return &m_data[eid];
	}
	
	virtual CType*			Add(EntityID eid, CType const& copy)	override
	{
		m_data[eid] = copy;
		return &m_data[eid];
	}

	virtual void			NewAdd(EntityID eid, Component const* copy)	override
	{
		m_data[eid] = *reinterpret_cast<CType const*>(copy);
	}
	
	virtual void			Destroy(EntityID eid)					override
	{
		if (m_data.find(eid) != m_data.end())
		{
			m_data.erase(eid);
		}
	}
	
	virtual void			Clear()									override
	{
		m_data.clear();
	}

	// Not virtual on purpose for performance reasons - this is the most called function in the ECS
	CType& operator [](EntityID id) { return m_data[id]; }

public:
	
	std::unordered_map<EntityID, CType> m_data;
};



//----------------------------------------------------------------------------------------------------------------------
// Singleton Storage - useful for global data
//
template<typename CType>
class SingletonStorage : public TypedBaseStorage<CType>
{
public:

	SingletonStorage() = default;
	virtual ~SingletonStorage() override = default;

	virtual CType*			Get(EntityID)							override
	{
		return &m_data;
	}
	
	virtual CType const*	Get(EntityID)							const override
	{
		return &m_data;
	}
	
	virtual CType*			Add(EntityID)							override
	{
		// not relevant to singleton
		return nullptr;
	}
	
	virtual CType*			Add(EntityID, CType const&)				override
	{
		// not relevant to singleton
		return nullptr;
	}
	
	virtual void			Destroy(EntityID)						override
	{
		// not relevant to singleton
	}
	
	virtual void			Clear()									override
	{
		// not relevant to singleton
	}
	
	// Not virtual on purpose for performance reasons - this is the most called function in the ECS
	CType& operator [](EntityID [[maybe_unused]] id) { return m_data; }

public:

	CType m_data;
};



//----------------------------------------------------------------------------------------------------------------------
// Tag Storage - useful for boolean flags/tags
//
template<typename CType>
class TagStorage : public TypedBaseStorage<CType>
{
public:

	TagStorage() : m_tags(false) {}
	
	virtual ~TagStorage() override = default;

	virtual CType*			Get(EntityID eid)						override
	{
		return (CType*) m_tags.Get(eid);
	}
	
	virtual CType const*	Get(EntityID eid)						const override
	{
		return (CType const*) m_tags.Get(eid);
	}
	
	virtual CType*			Add(EntityID eid)						override
	{
		m_tags.Set(eid);
		return nullptr;
	}
	
	virtual CType*			Add(EntityID eid, [[maybe_unused]] CType const& copy) override
	{
		m_tags.Set(eid);
		return nullptr;
	}
	
	virtual void			Destroy(EntityID eid)					override
	{
		m_tags.Unset(eid);
	}
	
	virtual void			Clear()									override
	{
		m_tags.SetAll(false);
	}

	// Not virtual on purpose for performance reasons - this is the most called function in the ECS
	bool operator[](EntityID id) { return m_tags.Get(id); }

public:
	
	BitArray<MAX_ENTITIES> m_tags;
};