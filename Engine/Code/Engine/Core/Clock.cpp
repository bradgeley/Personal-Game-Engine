// Bradley Christensen - 2022-2025
#include "Clock.h"
#include "Engine/Core/ErrorUtils.h"
#include <chrono>



//----------------------------------------------------------------------------------------------------------------------
// Keeps chrono out of header
//
struct ClockInternalData
{
	std::chrono::high_resolution_clock m_clock;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdatedTime;
};



//----------------------------------------------------------------------------------------------------------------------
Clock::Clock()
{
	m_internalData = new ClockInternalData();
	m_internalData->m_lastUpdatedTime = m_internalData->m_clock.now();
}



//----------------------------------------------------------------------------------------------------------------------
Clock::Clock(Clock* parentClock) : Clock()
{
    m_parentClock = parentClock;
    ASSERT_OR_DIE(parentClock != nullptr, "Cannot initialize clock with null parent.");
    if (parentClock) 
    {
        parentClock->AddChildClock(this);
    }
}



//----------------------------------------------------------------------------------------------------------------------
Clock::~Clock()
{
	if (m_internalData)
	{
		delete m_internalData;
		m_internalData = nullptr;
	}
	if (m_parentClock) 
	{
		m_parentClock->RemoveChildClock(this);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::Update()
{
	ASSERT_OR_DIE(m_parentClock == nullptr, "Update called on a child clock");

	UpdateInternal();
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
void Clock::AddChildClock(Clock* childClock)
{
	m_childClocks.push_back(childClock);
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::RemoveChildClock(Clock* childClock)
{
	for (int i = 0; i < m_childClocks.size(); ++i)
	{
		if (m_childClocks[i] == childClock)
		{
			m_childClocks.erase(m_childClocks.begin() + i);
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::UpdateInternal()
{
	ASSERT_OR_DIE(m_internalData, "Clock has no internal data.");

	// Clocks update from the last time-stamp that they recorded during their update. This is to allow
	// re-parenting clocks and changing timeDilation willie nillie without affecting m_currentTimeSeconds.
	// Time dilation complicates things, as clocks can inherit their parent's time dilation as well,
	// so we cannot store the start time to compare against in order to not accumulate errors.
	// This causes an accumulation of tiny errors, but for most games that will not ever be a problem.

	// Record the time as of this update for all clocks, this is so if this is a child clock and detaches
	// from its parent, it can become a parent clock and will know the last time it was updated.
	std::chrono::time_point<std::chrono::high_resolution_clock> now = m_internalData->m_clock.now();

	if (m_parentClock)
	{
		// If we have a parent clock, don't calculate our own delta seconds, rather take our parent's deltaSeconds
		// and tack on our time dilation. Because this function is recursive, our parent's deltaSeconds will also
		// take into account all of its parents' time dilations up the chain.
		m_deltaSeconds = m_parentClock->m_deltaSeconds * m_timeDilation;
	}
	else
	{
		// Parent-most clock update: Calculate the base deltaSeconds that will be passed down to all children
		std::chrono::duration<double> timePassedSinceLastUpdated = now - m_internalData->m_lastUpdatedTime;
		m_deltaSeconds = timePassedSinceLastUpdated.count() * m_timeDilation;
	}

	m_internalData->m_lastUpdatedTime = now;
	m_currentTimeSeconds += m_deltaSeconds;

	for (Clock*& childClock : m_childClocks)
	{
		childClock->UpdateInternal();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::SetTimeDilation(double timeDilation)
{
	m_timeDilation = timeDilation;
}



//----------------------------------------------------------------------------------------------------------------------
void Clock::GetTimeDilation(double& out_timeDilation) const
{
	out_timeDilation *= m_timeDilation;
	if (m_parentClock) 
	{
		m_parentClock->GetTimeDilation(out_timeDilation);
	}
}



//----------------------------------------------------------------------------------------------------------------------
double Clock::GetLocalTimeDilation() const
{
	return m_timeDilation;
}



//----------------------------------------------------------------------------------------------------------------------
float Clock::GetLocalTimeDilationF() const
{
	return static_cast<float>(m_timeDilation);
}
