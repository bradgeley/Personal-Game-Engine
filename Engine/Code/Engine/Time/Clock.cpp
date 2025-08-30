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
		clock.m_deltaSeconds = clock.m_parentClock->m_deltaSeconds * clock.m_timeDilation;
	}
	else
	{
		// Parent-most clock update: Calculate the base deltaSeconds that will be passed down to all children
		std::chrono::duration<double> timePassedSinceLastUpdated = updatedTime - clock.m_internalData->m_lastUpdatedTime;
		clock.m_deltaSeconds = timePassedSinceLastUpdated.count() * clock.m_timeDilation;
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
	out_timeDilation *= m_timeDilation;
	if (m_parentClock) 
	{
		m_parentClock->GetAggregateTimeDilation(out_timeDilation);
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



#if defined(_DEBUG)

#include <unordered_set>
#include <thread>
#include <functional>

//----------------------------------------------------------------------------------------------------------------------
void Clock::RunUnitTests()
{
	// Helper: Recursively check that all clocks in the tree are connected to the root
	auto IsConnectedToRoot = [](Clock* node, Clock* root) -> bool
	{
		Clock* cur = node;
		while (cur != nullptr)
		{
			if (cur == root) return true;
			cur = cur->GetParentClock();
		}
		return false;
	};

	// Helper: Recursively collect all clocks in the tree
	std::function<void(Clock*, std::unordered_set<Clock*>&)> CollectTree;
	CollectTree = [&](Clock* node, std::unordered_set<Clock*>& out)
	{
		if (!node || out.count(node)) return;
		out.insert(node);
		for (Clock* child : node->m_childClocks)
		{
			CollectTree(child, out);
		}
	};

	// Create root clock
	Clock* root = new Clock();

	// Create 19 more clocks, each with a parent (tree structure)
	std::vector<Clock*> clocks;
	clocks.push_back(root);

	// Build a tree: root has 3 children, each of those has 2 children, etc.
	// For simplicity, let's make a 3-level tree: 1 + 3 + 8 + 8 = 20 clocks
	// Level 1: root (already created)
	// Level 2: 3 children of root
	for (int i = 0; i < 3; ++i)
	{
		Clock* c = new Clock(root);
		clocks.push_back(c);
	}
	// Level 3: each of the 3 children gets 2-3 children (to total 8)
	int childIdx = 1;
	for (int i = 0; i < 3; ++i)
	{
		int numChildren = (i < 2) ? 3 : 2; // 3, 3, 2 = 8
		for (int j = 0; j < numChildren; ++j)
		{
			Clock* c = new Clock(clocks[childIdx]);
			clocks.push_back(c);
		}
		++childIdx;
	}
	// Level 4: each of the 8 children gets 1 child (8 more clocks)
	for (int i = 4; i < 12; ++i)
	{
		Clock* c = new Clock(clocks[i]);
		clocks.push_back(c);
	}

	// Now we have 20 clocks in a tree

	// Check: All clocks are connected to root
	for (Clock* c : clocks)
	{
		ASSERT_OR_DIE(IsConnectedToRoot(c, root), "Clock is not connected to root after initial construction.");
	}
	// Check: Tree contains all clocks
	{
		std::unordered_set<Clock*> treeClocks;
		CollectTree(root, treeClocks);
		ASSERT_OR_DIE(treeClocks.size() == clocks.size(), "Tree does not contain all clocks after initial construction.");
	}

	// Pick a clock in the middle (e.g., clocks[5]) and remove it
	Clock* toRemove = clocks[5];
	Clock* parent = toRemove->GetParentClock();

	// Save children before removal
	std::vector<Clock*> children = toRemove->m_childClocks;

	// Remove the clock (deletes it, children should be reparented to its parent)
	delete toRemove;

	// Check that all children of the removed clock are now children of the parent
	for (Clock* child : children)
	{
		// Should have new parent
		ASSERT_OR_DIE(child->GetParentClock() == parent, "Child was not reparented correctly.");
		// Should be in parent's child list
		bool found = false;
		for (Clock* pChild : parent->m_childClocks)
		{
			if (pChild == child)
			{
				found = true;
				break;
			}
		}
		ASSERT_OR_DIE(found, "Child not found in new parent's child list.");
	}

	// Check: All remaining clocks are still connected to root
	for (Clock* c : clocks)
	{
		if (c == toRemove) continue;
		ASSERT_OR_DIE(IsConnectedToRoot(c, root), "Clock is not connected to root after deletion.");
	}
	// Check: Tree contains all remaining clocks (except deleted one)
	{
		std::unordered_set<Clock*> treeClocks;
		CollectTree(root, treeClocks);
		size_t expected = clocks.size() - 1;
		ASSERT_OR_DIE(treeClocks.size() == expected, "Tree does not contain all clocks after deletion.");
	}

	// Test insertion: Add a new clock as a child of root
	Clock* newChild = new Clock(root);
	clocks.push_back(newChild);
	ASSERT_OR_DIE(newChild->GetParentClock() == root, "New child not attached to root.");
	ASSERT_OR_DIE(IsConnectedToRoot(newChild, root), "New child not connected to root after insertion.");
	{
		std::unordered_set<Clock*> treeClocks;
		CollectTree(root, treeClocks);
		ASSERT_OR_DIE(treeClocks.count(newChild) == 1, "New child not found in tree after insertion.");
	}

	// Additional check: Detach a child from its parent and ensure m_lastUpdatedTime is updated
	Clock* detachTestParent = clocks[1];
	if (!detachTestParent->m_childClocks.empty())
	{
		Clock* childToDetach = detachTestParent->m_childClocks[0];
		// Save parent's last updated time
		auto expectedTime = detachTestParent->GetParentmostClock()->m_internalData->m_lastUpdatedTime;
		bool detached = detachTestParent->DetachChildClock(childToDetach);
		ASSERT_OR_DIE(detached, "Failed to detach child clock.");
		ASSERT_OR_DIE(childToDetach->GetParentClock() == nullptr, "Detached child still has a parent.");
		ASSERT_OR_DIE(childToDetach->m_internalData->m_lastUpdatedTime == expectedTime, "Detached child's m_lastUpdatedTime was not updated correctly.");
	}

    // --- New check: Time dilation accumulation in a 3-level hierarchy ---
    {
		Clock* top = new Clock();
		Clock* mid = new Clock(top);
		Clock* bot = new Clock(mid);

		top->SetLocalTimeDilation(0.5);
		mid->SetLocalTimeDilation(0.5);
		bot->SetLocalTimeDilation(0.5);

		// Wait for 0.1 seconds using a busy-wait loop
		auto start = std::chrono::high_resolution_clock::now();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 100) {}

		top->Update();

		double topDelta = top->GetDeltaSeconds();
		double midDelta = mid->GetDeltaSeconds();
		double botDelta = bot->GetDeltaSeconds();

		// Acceptable error margin (5ms)
		const double epsilon = 0.005;

		ASSERT_OR_DIE(std::abs(topDelta - 0.05) < epsilon, "Top clock deltaSeconds not as expected for time dilation (expected ~0.05s)");
		ASSERT_OR_DIE(std::abs(midDelta - 0.025) < epsilon, "Mid clock deltaSeconds not as expected for time dilation (expected ~0.025s)");
		ASSERT_OR_DIE(std::abs(botDelta - 0.0125) < epsilon, "Bot clock deltaSeconds not as expected for time dilation (expected ~0.0125s)");

		// --- Additional check: Current time is accurate after update ---
		{
		double expectedTopTime = topDelta;
		double expectedMidTime = midDelta;
		double expectedBotTime = botDelta;
		ASSERT_OR_DIE(std::abs(top->GetCurrentTimeSeconds() - expectedTopTime) < epsilon, "Top clock current time not as expected after update.");
		ASSERT_OR_DIE(std::abs(mid->GetCurrentTimeSeconds() - expectedMidTime) < epsilon, "Mid clock current time not as expected after update.");
		ASSERT_OR_DIE(std::abs(bot->GetCurrentTimeSeconds() - expectedBotTime) < epsilon, "Bot clock current time not as expected after update.");
		}

		// --- Detach all clocks so they are independent, wait, and verify each delta is ~0.05s ---
		bool detachedBot = bot->DetachFromParent();
		bool detachedMid = mid->DetachFromParent();
		ASSERT_OR_DIE(detachedBot, "Failed to detach bot clock from mid.");
		ASSERT_OR_DIE(detachedMid, "Failed to detach mid clock from top.");
		ASSERT_OR_DIE(mid->GetParentClock() == nullptr, "Mid clock should have no parent after detach.");
		ASSERT_OR_DIE(bot->GetParentClock() == nullptr, "Bot clock should have no parent after detach.");
		ASSERT_OR_DIE(top->GetParentClock() == nullptr, "Top clock should have no parent.");

		// Wait for 0.1 seconds using a busy-wait loop
		start = std::chrono::high_resolution_clock::now();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 100) {}

		top->Update();
		mid->Update();
		bot->Update();
		double topDelta2 = top->GetDeltaSeconds();
		double midDelta2 = mid->GetDeltaSeconds();
		double botDelta2 = bot->GetDeltaSeconds();

		ASSERT_OR_DIE(std::abs(topDelta2 - 0.05) < epsilon, "Top clock deltaSeconds not as expected after detaching (expected ~0.05s)");
		ASSERT_OR_DIE(std::abs(midDelta2 - 0.05) < epsilon, "Mid clock deltaSeconds not as expected after detaching (expected ~0.05s)");
		ASSERT_OR_DIE(std::abs(botDelta2 - 0.05) < epsilon, "Bot clock deltaSeconds not as expected after detaching (expected ~0.05s)");

		// --- Additional check: Current time is accurate after update when independent ---
		{
		double expectedTopTime = topDelta + topDelta2;
		double expectedMidTime = midDelta + midDelta2;
		double expectedBotTime = botDelta + botDelta2;
		ASSERT_OR_DIE(std::abs(top->GetCurrentTimeSeconds() - expectedTopTime) < epsilon, "Top clock current time not as expected after detaching and updating.");
		ASSERT_OR_DIE(std::abs(mid->GetCurrentTimeSeconds() - expectedMidTime) < epsilon, "Mid clock current time not as expected after detaching and updating.");
		ASSERT_OR_DIE(std::abs(bot->GetCurrentTimeSeconds() - expectedBotTime) < epsilon, "Bot clock current time not as expected after detaching and updating.");
    }

    delete bot;
    delete mid;
    delete top;
    }
	// Clean up all clocks (avoid double delete)
	std::unordered_set<Clock*> deleted;
	for (Clock* c : clocks)
	{
		if (c != toRemove && deleted.find(c) == deleted.end())
		{
			delete c;
			deleted.insert(c);
		}
	}
	// Also delete the new child if not already deleted
	if (deleted.find(newChild) == deleted.end())
	{
		delete newChild;
	}
}


#endif // _DEBUG