// Bradley Christensen - 2023
#pragma once
#include "EntityID.h"
#include "SystemContext.h"
#include <climits>



class AdminSystem;



//----------------------------------------------------------------------------------------------------------------------
// Group Iterator
//
// Iterates over entities that have all of the components specified
//
struct GroupIter
{
	friend class AdminSystem;

public:

	explicit GroupIter(SystemContext const& context);

	bool IsValid() const;
	void Next();
	
public:

	EntityID m_currentIndex		= UINT_MAX;

protected:

	SystemContext m_context;
	BitMask m_groupMask			= 0;
};