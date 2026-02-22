// Bradley Christensen - 2022-2025
#include "Clock.h"
#include "Engine/Core/ErrorUtils.h"
#include <chrono>
#include "Timer.h"



//----------------------------------------------------------------------------------------------------------------------
// Keeps chrono out of header (tradeoff: potential extra cache miss per clock update)
//
struct ClockInternalData
{
public:

	static void UpdateInternal(Clock& clock, std::chrono::time_point<std::chrono::high_resolution_clock> const& updatedTime);

public:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdatedTime;
};



//----------------------------------------------------------------------------------------------------------------------
void ClockInternalData::UpdateInternal(Clock& clock, std::chrono::time_point<std::chrono::high_resolution_clock> const& updatedTime)
{
	ASSERT_OR_DIE(clock.m_internalData != nullptr, "Clock has no internal data.");

	if (clock.m_parentClock)
	{
		// If we have a parent clock, don't calculate our own delta seconds, rather take our parent's deltaSeconds
		// and tack on our time dilation. Because this function is recursive, our parent's deltaSeconds will also
		// take into account all of its parents' time dilations up the chain.
		clock.m_deltaSeconds = clock.m_parentClock->m_deltaSeconds * clock.GetLocalTimeDilation();
	}
	else
	{
		// Parent-most clock update: Calculate the base deltaSeconds that will be passed down to all children
		std::chrono::duration<double> timePassedSinceLastUpdated = updatedTime - clock.m_internalData->m_lastUpdatedTime;
		clock.m_deltaSeconds = timePassedSinceLastUpdated.count() * clock.GetLocalTimeDilation();
		clock.m_internalData->m_lastUpdatedTime = updatedTime;
	}

	// if looking for perf improvement, could avoid cache miss on child clocks by only updating m_internalData when they are or become a parent
	// probably only relevant if using 1000's of clocks, which is unlikely
	clock.m_currentTimeSeconds += clock.m_deltaSeconds;

	for (Clock*& childClock : clock.m_childClocks)
	{
		ASSERT_OR_DIE(childClock != nullptr, "Child clock was somehow nullptr");
		UpdateInternal(*childClock, updatedTime);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Clock::Clock()
{
	m_internalData = new ClockInternalData();
	std::chrono::high_resolution_clock clock;
	m_internalData->m_lastUpdatedTime = clock.now();
}



//----------------------------------------------------------------------------------------------------------------------
Clock::Clock(Clock* parentClock) : Clock()
{
    ASSERT_OR_DIE(parentClock != nullptr, "Cannot initialize clock with null parent.");
    if (parentClock) 
    {
        parentClock->AttachChildClock(this);
    }
}



//----------------------------------------------------------------------------------------------------------------------
Clock::~Clock()
{
	if (m_parentClock)
	{
		while (!m_childClocks.empty())
		{
			// Don't orphan children clocks - let them still get updated by the parent. Rare case but could happen.
			m_childClocks[0]->Reparent(m_parentClock);
		}

		m_parentClock->DetachChildClock(this);
	}
	else
	{
		while (!m_childClocks.empty())
		{
			DetachChildClock(m_childClocks[0]);
		}
	}

	if (m_internalData)
	{
		delete m_internalData;
		m_internalData = nullptr;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::Update()
{
	ASSERT_OR_DIE(m_parentClock == nullptr, "Update called on a child clock");

	// Clocks update from the last time-stamp that they recorded during their update. This is to allow
	// re-parenting clocks and changing timeDilation willie nillie without affecting m_currentTimeSeconds.
	// Time dilation complicates things, as clocks can inherit their parent's time dilation as well,
	// so we cannot store the start time to compare against in order to not accumulate errors.
	// This causes an accumulation of tiny errors, but for most games that will not ever be a problem.

	std::chrono::high_resolution_clock clock;
	std::chrono::time_point<std::chrono::high_resolution_clock> now = clock.now();

	// This func passes 'now' down to all child clocks, so they all receive the same exact value
	ClockInternalData::UpdateInternal(*this, now);
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::Update(double deltaSeconds)
{
	ASSERT_OR_DIE(m_parentClock == nullptr, "Update called on a child clock");

	// This func overrides the automatic functionality of Update(), by manually setting deltaSeconds then updating
	// all children with that value. The children clocks don't use "now", and parent clocks wont use the last updated time
	// so long as the clock is updating using this function instead of Update().

	m_deltaSeconds = deltaSeconds * GetLocalTimeDilation();

	std::chrono::high_resolution_clock clock;
	std::chrono::time_point<std::chrono::high_resolution_clock> now = clock.now();

	for (Clock*& childClock : m_childClocks)
	{
		ASSERT_OR_DIE(childClock != nullptr, "Child clock was somehow nullptr");
		ClockInternalData::UpdateInternal(*childClock, now);
	}
}



//----------------------------------------------------------------------------------------------------------------------
double Clock::GetCurrentTimeSeconds() const
{
	return m_currentTimeSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
float Clock::GetCurrentTimeSecondsF() const
{
	return static_cast<float>(m_currentTimeSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
double Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
float Clock::GetDeltaSecondsF() const
{
	return static_cast<float>(m_deltaSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
Clock* Clock::GetParentClock() const
{
	return m_parentClock;
}



//----------------------------------------------------------------------------------------------------------------------
Clock const* Clock::GetParentmostClock() const
{
	Clock const* result = this;
	while (result->m_parentClock != nullptr)
	{
		result = result->m_parentClock;
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<Clock*>& Clock::GetChildrenClocks()
{
	return m_childClocks;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<Clock*> const& Clock::GetChildrenClocks() const
{
	return m_childClocks;
}



//----------------------------------------------------------------------------------------------------------------------
ClockInternalData* Clock::GetInternalData() const
{
	return m_internalData;
}



//----------------------------------------------------------------------------------------------------------------------
bool Clock::AttachChildClock(Clock* childClock)
{
	ASSERT_OR_DIE(childClock != nullptr, "Cannot add null child clock.");
	ASSERT_OR_DIE(childClock->m_parentClock == nullptr, "Cannot add a child clock that already has a parent.");

	if (childClock == nullptr || childClock->m_parentClock != nullptr)
	{
		return false;
	}

	// todo: recursive check to make sure clocks can only appear in the tree one time, aka cycle detection

	childClock->m_parentClock = this;
	m_childClocks.push_back(childClock);
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool Clock::Reparent(Clock* newParent)
{
	ASSERT_OR_DIE(newParent != nullptr, "Cannot reparent to null clock.");
	if (newParent == nullptr)
	{
		return false;
	}
	if (m_parentClock)
	{
		if (!m_parentClock->DetachChildClock(this))
		{
			return false;
		}
	}
	
	return newParent->AttachChildClock(this);
}



//----------------------------------------------------------------------------------------------------------------------
bool Clock::DetachChildClock(Clock* childClock)
{
	ASSERT_OR_DIE(childClock != nullptr, "Cannot remove null child clock.");
	if (childClock == nullptr)
	{
		return false;
	}

	ASSERT_OR_DIE(childClock->m_parentClock == this, "Tried to remove child clock who's parent was not me.");
	if (childClock->m_parentClock != this)
	{
		return false;
	}

	// Update the last updated time for the child, because after removal it will be a parent and will generate its own deltaSeconds based off of this.
	// We update based off of the parentmost clock because that is the only clock that constantly updates that value.
	childClock->m_internalData->m_lastUpdatedTime = GetParentmostClock()->m_internalData->m_lastUpdatedTime;
	childClock->m_parentClock = nullptr;

	for (int i = 0; i < m_childClocks.size(); ++i)
	{
		if (m_childClocks[i] == childClock)
		{
			m_childClocks.erase(m_childClocks.begin() + i);
			return true;
		}
	}

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Clock::AttachToParent(Clock* parent)
{
	if (parent)
	{
		return parent->AttachChildClock(this);
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Clock::DetachFromParent()
{
	if (m_parentClock)
	{
		bool succeeded = m_parentClock->DetachChildClock(this);
		ASSERT_OR_DIE(succeeded, "Clock parent desync? Failed to remove from parent.");
		return succeeded;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::SetLocalTimeDilation(double timeDilation)
{
	m_timeDilation = timeDilation;
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::GetAggregateTimeDilation(double& out_timeDilation) const
{
	if (m_isPaused)
	{
		out_timeDilation = 0.0;
		return;
	}

	out_timeDilation *= m_timeDilation;
	if (m_parentClock) 
	{
		m_parentClock->GetAggregateTimeDilation(out_timeDilation);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::SetPaused(bool isPaused)
{
	m_isPaused = isPaused;
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::TogglePaused()
{
	m_isPaused = !m_isPaused;
}



//----------------------------------------------------------------------------------------------------------------------
bool Clock::IsPaused() const
{
	double timeDilation = 1.0;
	GetAggregateTimeDilation(timeDilation);
	return m_isPaused || timeDilation == 0.0;
}



//----------------------------------------------------------------------------------------------------------------------
double Clock::GetLocalTimeDilation() const
{
	if (IsPaused())
	{
		return 0.0;
	}
	return m_timeDilation;
}



//----------------------------------------------------------------------------------------------------------------------
float Clock::GetLocalTimeDilationF() const
{
	if (IsPaused())
	{
		return 0.f;
	}
	return static_cast<float>(m_timeDilation);
}