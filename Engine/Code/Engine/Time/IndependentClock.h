// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
// A clock but simpler, smaller, and with no hierarchy or internal data ptr
//
class IndependentClock
{
	friend struct ClockInternalData;

public:

	IndependentClock();

	// Only call on parent-most clocks
	void Update();
	void Update(double deltaSeconds);

	double GetDeltaSeconds() const;
	float GetDeltaSecondsF() const;

	double GetCurrentTimeSeconds() const;
	float GetCurrentTimeSecondsF() const;

	double GetLocalTimeDilation() const;
	float GetLocalTimeDilationF() const;

	void SetLocalTimeDilation(double timeDilation);

protected:

	double m_deltaSeconds = 0.0;
	double m_currentTimeSeconds = 0.0;
	double m_timeDilation = 1.0;

private:

	uint64_t m_lastUpdatedTimeNano = 0;
};

