// Bradley Christensen - 2022-2025
#pragma once
#include <unordered_map>
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

	virtual void Destroy(int index)											= 0;
	virtual void Clear()													= 0;
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

	virtual CType*			Get(GroupIter const& it)						= 0;
	virtual CType const*	Get(GroupIter const& it) const					= 0;
	virtual CType*			Get(int entityIndex)							= 0;
	virtual CType const*	Get(int entityIndex) const						= 0;
	virtual CType*			Add(int entityIndex)							= 0;
	virtual CType*			Add(int entityIndex, CType const& copy)			= 0;

	// Not virtual on purpose for performance reasons - this is the most called function in the ECS
	inline CType& operator [](int id) { return *Get(id); }
	inline CType& operator [](GroupIter const& it) { return *Get(it.m_currentIndex); }
};



//----------------------------------------------------------------------------------------------------------------------
// Array Storage - useful for widely used components
//
template<typename CType>
class ArrayStorage : public TypedBaseStorage<CType>
{
public:

	ArrayStorage() = default;
	ArrayStorage(const ArrayStorage&) = delete;
	ArrayStorage& operator=(const ArrayStorage&) = delete;
	virtual ~ArrayStorage() override = default;

	virtual CType*			Get(GroupIter const& it)						override
	{
		return &m_data[it.m_currentIndex];
	}

	virtual CType const*	Get(GroupIter const& it)						const override
	{
		return &m_data[it.m_currentIndex];
	}

	virtual CType*			Get(int entityIndex)							override
	{
		return &m_data[entityIndex];
	}

	virtual CType const*	Get(int entityIndex)							const override
	{
		return &m_data[entityIndex];
	}

	virtual CType*			Add(int entityIndex)							override
	{
		CType& slot = m_data[entityIndex];
		slot = CType();
		return &slot;
	}

	virtual CType*			Add(int entityIndex, CType const& copy)			override
	{
		CType& slot = m_data[entityIndex];
		slot = copy;
		return &slot;
	}

	virtual void			Destroy(int entityIndex)						override
	{
		m_data[entityIndex].~CType();
	}

	virtual void			Clear()											override
	{
		for (CType& component : m_data)
		{
			component.~CType();
		}
	}

	inline CType& operator [](int id) { return m_data[id]; }
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

	MapStorage() = default;
	MapStorage(const MapStorage&) = delete;
	MapStorage& operator=(const MapStorage&) = delete;
	virtual ~MapStorage() override = default;

	virtual CType*			Get(int entityIndex)							override
	{
		auto result = m_data.find(entityIndex);
		if (result != m_data.end())
		{
			return &(result->second);
		}
		return nullptr;
	}
	
	virtual CType const*	Get(int entityIndex)							const override
	{
		auto result = m_data.find(entityIndex);
		if (result != m_data.end())
		{
			return &(result->second);
		}
		return nullptr;
	}

	virtual CType*			Get(GroupIter const& it)						override
	{
		auto result = m_data.find(it.m_currentIndex);
		if (result != m_data.end())
		{
			return &(result->second);
		}
		return nullptr;
	}

	virtual CType const*	Get(GroupIter const& it)						const override
	{
		auto result = m_data.find(it.m_currentIndex);
		if (result != m_data.end())
		{
			return &(result->second);
		}
		return nullptr;
	}
	
	virtual CType*			Add(int entityIndex)							override
	{
		m_data.emplace(entityIndex, CType());
		return &m_data[entityIndex];
	}
	
	virtual CType*			Add(int entityIndex, CType const& copy)			override
	{
		m_data.emplace(entityIndex, copy);
		return &m_data[entityIndex];
	}
	
	virtual void			Destroy(int entityIndex)						override
	{
		auto result = m_data.find(entityIndex);
		if (result != m_data.end())
		{
			m_data.erase(result);
		}
	}
	
	virtual void			Clear()											override
	{
		m_data.clear();
	}

	inline CType& operator [](int id) { return m_data[id]; }
	inline CType& operator [](GroupIter const& it) { return m_data[it.m_currentIndex]; }

public:
	
	std::unordered_map<int, CType> m_data;
};



//----------------------------------------------------------------------------------------------------------------------
// Singleton Storage - useful for global data
//
template<typename CType>
class SingletonStorage : public TypedBaseStorage<CType>
{
public:

	SingletonStorage() = default;
	SingletonStorage(const SingletonStorage&) = delete;
	SingletonStorage& operator=(const SingletonStorage&) = delete;
	virtual ~SingletonStorage() override = default;

	virtual CType*			Get(int)								override
	{
		return &m_data;
	}
	
	virtual CType const*	Get(int)								const override
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
	
	virtual CType*			Add(int)								override
	{
		// not relevant to singleton
		return nullptr;
	}
	
	virtual CType*			Add(int, CType const&)					override
	{
		// not relevant to singleton
		return nullptr;
	}
	
	virtual void			Destroy(int)							override
	{
		// not relevant to singleton
	}
	
	virtual void			Clear()									override
	{
		// not relevant to singleton
	}
	
	inline CType& operator [](int) { return m_data; }
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
	TagStorage(const TagStorage&) = delete;
	TagStorage& operator=(const TagStorage&) = delete;
	virtual ~TagStorage() override = default;

	virtual CType*			Get(int entityIndex)					override
	{
		return (CType*) m_tags.Get(entityIndex);
	}
	
	virtual CType const*	Get(int entityIndex)					const override
	{
		return (CType const*) m_tags.Get(entityIndex);
	}

	virtual CType*			Get(GroupIter const& it)				override
	{
		return (CType*) m_tags.Get(it.m_currentIndex);
	}

	virtual CType const*	Get(GroupIter const& it)				const override
	{
		return (CType const*) m_tags.Get(it.m_currentIndex);
	}
	
	virtual CType*			Add(int entityIndex)					override
	{
		m_tags.Set(entityIndex);
		return nullptr;
	}
	
	virtual CType*			Add(int entityIndex, CType const&)		override
	{
		m_tags.Set(entityIndex);
		return nullptr;
	}
	
	virtual void			Destroy(int entityIndex)						override
	{
		m_tags.Unset(entityIndex);
	}
	
	virtual void			Clear()									override
	{
		m_tags.SetAll(false);
	}

	inline bool operator[](int id) { return m_tags.Get(id); }
	inline bool operator[](GroupIter const& it) { return m_tags.Get(it.m_currentIndex); }

public:
	
	BitArray<MAX_ENTITIES> m_tags;
};