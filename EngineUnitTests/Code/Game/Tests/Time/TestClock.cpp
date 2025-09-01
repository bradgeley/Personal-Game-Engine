// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Time/Clock.h"
#include "Engine/Core/ErrorUtils.h"
#include <chrono>
#include <thread>
#include <cmath>
#include <functional>
#include <unordered_set>
#include <gtest/gtest.h>



//-----------------------------------------------------------------------------------------------------------------------
// Clock Tests
//
namespace TestClock 
{

    //-----------------------------------------------------------------------------------------------------------------------
    static void WaitMilliseconds(int ms)
    {
        auto start = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < ms) {}
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Clock Basic Update
    //
    TEST(ClockTests, BasicUpdate)
    {
        Clock clock;
        WaitMilliseconds(50);
        clock.Update();
        double delta = clock.GetDeltaSeconds();
        float deltaF = clock.GetDeltaSecondsF();
        double cur = clock.GetCurrentTimeSeconds();
        float curF = clock.GetCurrentTimeSecondsF();
        const double epsilon = 0.005;
        EXPECT_NEAR(delta, 0.05, epsilon);
        EXPECT_NEAR(deltaF, 0.05f, epsilon);
        EXPECT_NEAR(cur, 0.05, epsilon);
        EXPECT_NEAR(curF, 0.05f, epsilon);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Time Dilation
    //
    TEST(ClockTests, TimeDilation)
    {
        Clock clock;
        clock.SetLocalTimeDilation(0.5);
        WaitMilliseconds(100);
        clock.Update();
        double delta = clock.GetDeltaSeconds();
        const double epsilon = 0.005;
        EXPECT_NEAR(delta, 0.05, epsilon);
        double dilation = clock.GetLocalTimeDilation();
        float dilationF = clock.GetLocalTimeDilationF();
        EXPECT_NEAR(dilation, 0.5, 0.0001);
        EXPECT_NEAR(dilationF, 0.5f, 0.0001f);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Parent Child Dilation
    //
    TEST(ClockTests, ParentChildDilation)
    {
        Clock parent;
        Clock child(&parent);
        parent.SetLocalTimeDilation(0.5);
        child.SetLocalTimeDilation(0.5);
        WaitMilliseconds(100);
        parent.Update();
        double parentDelta = parent.GetDeltaSeconds();
        double childDelta = child.GetDeltaSeconds();
        const double epsilon = 0.005;
        EXPECT_NEAR(parentDelta, 0.05, epsilon);
        EXPECT_NEAR(childDelta, 0.025, epsilon);
        double agg = 1.0;
        child.GetAggregateTimeDilation(agg);
        EXPECT_NEAR(agg, 0.25, 0.0001);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Attach/Detach
    //
    TEST(ClockTests, AttachDetach)
    {
        Clock parent;
        Clock child;
        bool attached = child.AttachToParent(&parent);
        EXPECT_TRUE(attached);
        EXPECT_EQ(child.GetParentClock(), &parent);
        bool detached = child.DetachFromParent();
        EXPECT_TRUE(detached);
        EXPECT_EQ(child.GetParentClock(), nullptr);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Reparent
    //
    TEST(ClockTests, Reparent)
    {
        Clock parent1;
        Clock parent2;
        Clock child(&parent1);
        bool reparented = child.Reparent(&parent2);
        EXPECT_TRUE(reparented);
        EXPECT_EQ(child.GetParentClock(), &parent2);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Detach
    //
    TEST(ClockTests, DetachChild)
    {
        Clock parent;
        Clock child(&parent);
        bool detached = parent.DetachChildClock(&child);
        EXPECT_TRUE(detached);
        EXPECT_EQ(child.GetParentClock(), nullptr);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Tree
    //
    TEST(ClockTests, ClockTree)
    {
        // Build a tree: root -> child1, child2; child1 -> grandchild
        Clock root;
        Clock child1(&root);
        Clock child2(&root);
        Clock grandchild(&child1);
        EXPECT_EQ(child1.GetParentClock(), &root);
        EXPECT_EQ(child2.GetParentClock(), &root);
        EXPECT_EQ(grandchild.GetParentClock(), &child1);
        EXPECT_EQ(grandchild.GetParentmostClock(), &root);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Internal Data Creation
    //
    TEST(ClockTests, InternalData)
    {
        Clock clock;
        EXPECT_NE(clock.GetInternalData(), nullptr);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Multi-functional test
    //
    TEST(ClockTests, Functional) 
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
            for (Clock* child : node->GetChildrenClocks())
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
            EXPECT_TRUE(IsConnectedToRoot(c, root)) << "Clock is not connected to root after initial construction.";
        }
        // Check: Tree contains all clocks
        {
            std::unordered_set<Clock*> treeClocks;
            CollectTree(root, treeClocks);
            EXPECT_EQ(treeClocks.size(), clocks.size()) << "Tree does not contain all clocks after initial construction.";
        }

        // Pick a clock in the middle (e.g., clocks[5]) and remove it
        Clock* toRemove = clocks[5];
        Clock* parent = toRemove->GetParentClock();

        // Save children before removal
        std::vector<Clock*> children = toRemove->GetChildrenClocks();

        // Remove the clock (deletes it, children should be reparented to its parent)
        delete toRemove;

        // Check that all children of the removed clock are now children of the parent
        for (Clock* child : children)
        {
            // Should have new parent
            EXPECT_EQ(child->GetParentClock(), parent) << "Child was not reparented correctly.";
            // Should be in parent's child list
            bool found = false;
            for (Clock* pChild : parent->GetChildrenClocks())
            {
                if (pChild == child)
                {
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found) << "Child not found in new parent's child list.";
        }

        // Check: All remaining clocks are still connected to root
        for (Clock* c : clocks)
        {
            if (c == toRemove) continue;
            EXPECT_TRUE(IsConnectedToRoot(c, root)) << "Clock is not connected to root after deletion.";
        }
        // Check: Tree contains all remaining clocks (except deleted one)
        {
            std::unordered_set<Clock*> treeClocks;
            CollectTree(root, treeClocks);
            size_t expected = clocks.size() - 1;
            EXPECT_EQ(treeClocks.size(), expected) << "Tree does not contain all clocks after deletion.";
        }

        // Test insertion: Add a new clock as a child of root
        Clock* newChild = new Clock(root);
        clocks.push_back(newChild);
        EXPECT_EQ(newChild->GetParentClock(), root) << "New child not attached to root.";
        EXPECT_TRUE(IsConnectedToRoot(newChild, root)) << "New child not connected to root after insertion.";
        {
            std::unordered_set<Clock*> treeClocks;
            CollectTree(root, treeClocks);
            EXPECT_EQ(treeClocks.count(newChild), 1u) << "New child not found in tree after insertion.";
        }

        // --- New check: Time dilation accumulation in a 3-level hierarchy ---
        {
            Clock* top = new Clock();
            Clock* mid = new Clock(top);
            Clock* bot = new Clock(mid);

            top->SetLocalTimeDilation(0.5);
            mid->SetLocalTimeDilation(0.5);
            bot->SetLocalTimeDilation(0.5);

            WaitMilliseconds(100);

            top->Update();

            double topDelta = top->GetDeltaSeconds();
            double midDelta = mid->GetDeltaSeconds();
            double botDelta = bot->GetDeltaSeconds();

            // Acceptable error margin (5ms)
            const double epsilon = 0.005;

            EXPECT_NEAR(topDelta, 0.05, epsilon) << "Top clock deltaSeconds not as expected for time dilation (expected ~0.05s)";
            EXPECT_NEAR(midDelta, 0.025, epsilon) << "Mid clock deltaSeconds not as expected for time dilation (expected ~0.025s)";
            EXPECT_NEAR(botDelta, 0.0125, epsilon) << "Bot clock deltaSeconds not as expected for time dilation (expected ~0.0125s)";

            // --- Additional check: Current time is accurate after update ---
            {
                double expectedTopTime = topDelta;
                double expectedMidTime = midDelta;
                double expectedBotTime = botDelta;
                EXPECT_NEAR(top->GetCurrentTimeSeconds(), expectedTopTime, epsilon) << "Top clock current time not as expected after update.";
                EXPECT_NEAR(mid->GetCurrentTimeSeconds(), expectedMidTime, epsilon) << "Mid clock current time not as expected after update.";
                EXPECT_NEAR(bot->GetCurrentTimeSeconds(), expectedBotTime, epsilon) << "Bot clock current time not as expected after update.";
            }

            // --- Detach all clocks so they are independent, wait, and verify each delta is ~0.05s ---
            bool detachedBot = bot->DetachFromParent();
            bool detachedMid = mid->DetachFromParent();
            EXPECT_TRUE(detachedBot) << "Failed to detach bot clock from mid.";
            EXPECT_TRUE(detachedMid) << "Failed to detach mid clock from top.";
            EXPECT_EQ(mid->GetParentClock(), nullptr) << "Mid clock should have no parent after detach.";
            EXPECT_EQ(bot->GetParentClock(), nullptr) << "Bot clock should have no parent after detach.";
            EXPECT_EQ(top->GetParentClock(), nullptr) << "Top clock should have no parent.";

            WaitMilliseconds(100);

            top->Update();
            mid->Update();
            bot->Update();
            double topDelta2 = top->GetDeltaSeconds();
            double midDelta2 = mid->GetDeltaSeconds();
            double botDelta2 = bot->GetDeltaSeconds();

            EXPECT_NEAR(topDelta2, 0.05, epsilon) << "Top clock deltaSeconds not as expected after detaching (expected ~0.05s)";
            EXPECT_NEAR(midDelta2, 0.05, epsilon) << "Mid clock deltaSeconds not as expected after detaching (expected ~0.05s)";
            EXPECT_NEAR(botDelta2, 0.05, epsilon) << "Bot clock deltaSeconds not as expected after detaching (expected ~0.05s)";

            // --- Additional check: Current time is accurate after update when independent ---
            {
                double expectedTopTime = topDelta + topDelta2;
                double expectedMidTime = midDelta + midDelta2;
                double expectedBotTime = botDelta + botDelta2;
                EXPECT_NEAR(top->GetCurrentTimeSeconds(), expectedTopTime, epsilon) << "Top clock current time not as expected after detaching and updating.";
                EXPECT_NEAR(mid->GetCurrentTimeSeconds(), expectedMidTime, epsilon) << "Mid clock current time not as expected after detaching and updating.";
                EXPECT_NEAR(bot->GetCurrentTimeSeconds(), expectedBotTime, epsilon) << "Bot clock current time not as expected after detaching and updating.";
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

}