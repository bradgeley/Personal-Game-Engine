// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Time/Timer.h"
#include "Engine/Time/Clock.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>



//----------------------------------------------------------------------------------------------------------------------
// Timer Unit Tests
//
namespace TestTimer
{
    //-----------------------------------------------------------------------------------------------------------------------
    static void WaitMilliseconds(int ms)
    {
        auto start = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < ms) {}
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 1: Constructor and Set
    //
    TEST(TimerTests, ConstructorAndSet)
    {
        Clock clock;
        Timer t1(&clock);
        EXPECT_EQ(t1.m_parentClock, &clock);
        EXPECT_EQ(t1.m_durationSeconds, 0.0);
        EXPECT_EQ(t1.m_remainingSeconds, 0.0);
        EXPECT_FALSE(t1.m_looping);

        Timer t2(&clock, 2.5, true);
        EXPECT_EQ(t2.m_parentClock, &clock);
        EXPECT_EQ(t2.m_durationSeconds, 2.5);
        EXPECT_EQ(t2.m_remainingSeconds, 2.5);
        EXPECT_TRUE(t2.m_looping);

        bool setResult = t1.Set(&clock, 1.5, false);
        EXPECT_TRUE(setResult);
        EXPECT_EQ(t1.m_parentClock, &clock);
        EXPECT_EQ(t1.m_durationSeconds, 1.5);
        EXPECT_EQ(t1.m_remainingSeconds, 1.5);
        EXPECT_FALSE(t1.m_looping);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 2: Getters
    //
    TEST(TimerTests, Getters)
    {
        Clock clock;
        Timer timer(&clock, 3.0, false);

        EXPECT_EQ(timer.GetParentClock(), &clock);
        EXPECT_DOUBLE_EQ(timer.GetDurationSeconds(), 3.0);
        EXPECT_DOUBLE_EQ(timer.GetRemainingSeconds(), 3.0);
        EXPECT_DOUBLE_EQ(timer.GetRemainingFraction(), 1.0);
        EXPECT_DOUBLE_EQ(timer.GetElapsedFraction(), 0.0);
        EXPECT_FALSE(timer.IsComplete());
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 3: Timer with Nested Clocks and Time Dilation
    //
    TEST(TimerTests, UpdateAndCompletion)
    {
        Clock clock;
        Timer timer(&clock, 0.1, false);

        // Timer should not be complete initially
        EXPECT_FALSE(timer.IsComplete());

        // Simulate time passing
        bool completedOnce = false;
        for (int i = 0; i < 10; ++i)
        {
			WaitMilliseconds(15);

            clock.Update();
            bool complete = timer.Update();
            if (complete) 
            {
                EXPECT_FALSE(completedOnce);
                completedOnce = complete;
            }
        }

        EXPECT_TRUE(timer.IsComplete());
        EXPECT_TRUE(completedOnce);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 4: GetRemainingFraction and GetElapsedFraction
    //
    TEST(TimerTests, Fractions)
    {
        Clock clock;
        Timer timer(&clock, 0.1, false);

        clock.Update();
        timer.Update();
        double beforeRemaining = timer.GetRemainingFraction();
        double beforeElapsed = timer.GetElapsedFraction();

        WaitMilliseconds(75);

        clock.Update();
        timer.Update();
        double afterRemaining = timer.GetRemainingFraction();
        double afterElapsed = timer.GetElapsedFraction();

        EXPECT_LT(afterRemaining, beforeRemaining);
        EXPECT_GT(timer.GetElapsedFraction(), 0.0);
        EXPECT_LE(timer.GetElapsedFraction(), 1.0);
        EXPECT_NEAR(beforeRemaining, 1.0, 0.01);
        EXPECT_NEAR(afterRemaining, 0.25, 0.01);

        EXPECT_NEAR(beforeElapsed, 0.0, 0.01);
        EXPECT_NEAR(afterElapsed, 0.75, 0.01);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 5: Timer with Nested Clocks and Time Dilation
    //
    TEST(TimerTests, TimerWithNestedClocksAndTimeDilation)
    {
        // Create parent and child clocks
        Clock parentClock;
        Clock childClock(&parentClock);

        // Set time dilations < 1
        parentClock.SetLocalTimeDilation(0.5);   // Parent runs at half speed
        childClock.SetLocalTimeDilation(0.5);    // Child runs at half speed of parent (total 0.25)

        // Timer on child clock, 0.01s duration (real time should be 0.8s due to dilation)
        Timer timer(&childClock, 0.01, false);

        // Simulate time passing on parent clock
        double elapsedRealTime = 0.0;
        bool completed = false;
        while (!completed && elapsedRealTime < 2.0) // safety cap
        {
            WaitMilliseconds(1);
            elapsedRealTime += 0.001;

            parentClock.Update(); // Only update parent; child gets updated via parent
            completed = timer.Update();
        }

        // Timer should be complete, and real time should be close to 0.8s (0.2 / 0.25)
        EXPECT_TRUE(timer.IsComplete());
        EXPECT_NEAR(elapsedRealTime, 0.04, 0.01);
    }



}
