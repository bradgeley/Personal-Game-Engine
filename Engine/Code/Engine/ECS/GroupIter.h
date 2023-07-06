// Bradley Christensen - 2023
#pragma once
#include "EntityID.h"
#include <climits>



class AdminSystem;
struct SystemContext;



//----------------------------------------------------------------------------------------------------------------------
// Group Iterator
//
// Iterates over entities that have all of the components specified
//
struct GroupIter
{
public:

	explicit GroupIter(SystemContext const& context);

	bool IsValid() const;
	void Next();

	void operator++();
	
public:

	SystemContext const*	m_context			= nullptr;
	EntityID				m_currentIndex		= UINT_MAX;
	BitMask					m_groupMask			= 0;
};