// Bradley Christensen - 2022-2026
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
	EntityID GetEntityID() const;

	void operator++();
	
public:

	int						m_currentIndex		= 0;
	int						m_endIndex			= MAX_ENTITIES - 1;
	BitMask					m_groupMask			= 0;
};