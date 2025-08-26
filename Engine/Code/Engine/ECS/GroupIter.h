// Bradley Christensen - 2022-2025
#pragma once
#include "EntityID.h"



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
	EntityID				m_currentIndex		= ENTITY_ID_INVALID;
	BitMask					m_groupMask			= 0;
};