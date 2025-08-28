// Bradley Christensen - 2022-2025
#pragma once
#include <unordered_map>
#include "EntityID.h"
#include "GroupIter.h"
#include "Engine/DataStructures/BitArray.h"



struct Component;



//----------------------------------------------------------------------------------------------------------------------
// Base Storage - Abstract base class for all storage types
//
class BaseStorage 
{
public:

	virtual ~BaseStorage() = default;

	virtual void Destroy(EntityID index)							= 0;
	virtual void Clear()											= 0;
};



//----------------------------------------------------------------------------------------------------------------------
enum class ComponentStorageType
{
	Array,
	Map,
	Singleton,
	Tag,
};



//----------------------------------------------------------------------------------------------------------------------
template<typename CType>
class TypedBaseStorage : public BaseStorage
{
public:

	virtual ~TypedBaseStorage() override = default;

	virtual CType*			Get(GroupIter const& it)				= 0;
	virtual CType const*	Get(GroupIter const& it) const			= 0;
	virtual CType*			Get(EntityID eid)						= 0;
	virtual CType const*	Get(EntityID eid) const					= 0;
	virtual CType*			Add(EntityID eid)						= 0;
	virtual CType*			Add(EntityID eid, CType const& copy)	= 0;

	// Not virtual on purpose for performance reasons - this is the most called function in the ECS
	inline CType& operator [](EntityID id) { return *Get(id); }
	inline CType& operator [](GroupIter const& it) { return *Get(it.m_currentIndex); }
};



//----------------------------------------------------------------------------------------------------------------------
// Array Storage - useful for widely used components
//
template<typename CType>
class ArrayStorage : public TypedBaseStorage<CType>
{
public:

	virtual ~ArrayStorage() override = default;

	virtual CType*			Get(GroupIter const& it)				override
	{
		return &m_data[it.m_currentIndex];
	}

	virtual CType const*	Get(GroupIter const& it)				const override
	{
		return &m_data[it.m_currentIndex];
	}

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

	virtual void			Destroy(EntityID eid)						override
	{
		m_data[eid].~CType();
	}

	virtual void			Clear()									override
	{
		for (CType& component : m_data)
		{
			component.~CType();
		}
	}

	inline CType& operator [](EntityID id) { return m_data[id]; }
	inline CType& operator [](GroupIter const& it) { return m_data[it.m_currentIndex]; }

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

	virtual CType*			Get(GroupIter const& it)				override
	{
		auto result = m_data.find(it.m_currentIndex);
		if (result != m_data.end())
		{
			return &(result->second);
		}
		return nullptr;
	}

	virtual CType const*	Get(GroupIter const& it)				const override
	{
		auto result = m_data.find(it.m_currentIndex);
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
	
	virtual void			Destroy(EntityID eid)					override
	{
		auto result = m_data.find(eid);
		if (result != m_data.end())
		{
			m_data.erase(result);
		}
	}
	
	virtual void			Clear()									override
	{
		m_data.clear();
	}

	inline CType& operator [](EntityID id) { return m_data[id]; }
	inline CType& operator [](GroupIter const& it) { return m_data[it.m_currentIndex]; }

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

	virtual CType*			Get(GroupIter const&)					override
	{
		return &m_data;
	}

	virtual CType const*	Get(GroupIter const&)					const override
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
	
	inline CType& operator [](EntityID) { return m_data; }
	inline CType& operator [](GroupIter const&) { return m_data; }

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

	virtual CType*			Get(GroupIter const& it)				override
	{
		return (CType*) m_tags.Get(it.m_currentIndex);
	}

	virtual CType const*	Get(GroupIter const& it)				const override
	{
		return (CType const*) m_tags.Get(it.m_currentIndex);
	}
	
	virtual CType*			Add(EntityID eid)						override
	{
		m_tags.Set(eid);
		return nullptr;
	}
	
	virtual CType*			Add(EntityID eid, CType const&)			override
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

	inline bool operator[](EntityID id) { return m_tags.Get(id); }
	inline bool operator[](GroupIter const& it) { return m_tags.Get(it.m_currentIndex); }

public:
	
	BitArray<MAX_ENTITIES> m_tags;
};