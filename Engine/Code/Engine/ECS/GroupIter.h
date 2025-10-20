// Bradley Christensen - 2022-2025
#pragma once
#include "EntityID.h"



struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
// Group Iterator
//
// Iterates over entities that have all of the components specified
//
struct GroupIter
{
protected:

	friend class AdminSystem;

	GroupIter() = default;
	explicit GroupIter(SystemContext const& context);

public:

	bool IsValid() const;
	void Next();

	void operator++();
	
public:

	EntityID				m_currentIndex		= 0;
	EntityID				m_endIndex			= MAX_ENTITIES - 1;
	BitMask					m_groupMask			= 0;
};