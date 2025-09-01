// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/DataStructures/ThreadSafeQueue.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>



//----------------------------------------------------------------------------------------------------------------------
// Thread Safe Queue Tests
//
namespace TestThreadSafeQueue
{

    //----------------------------------------------------------------------------------------------------------------------
    struct Dummy
    {
        int value;
        Dummy(int v) : value(v) {}
    };



    //----------------------------------------------------------------------------------------------------------------------
    void DeleteAll(std::vector<Dummy*>& vec)
    {
        for (Dummy* d : vec)
        {
            delete d;
        }
        vec.clear();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 1: Single-threaded basic push/pop
    //
    TEST(ThreadSafeQueueTests, SingleThreadedBasic)
    {
        ThreadSafeQueue<Dummy> queue;
        EXPECT_TRUE(queue.IsEmpty());
        EXPECT_EQ(queue.Count(), 0);

        Dummy* d1 = new Dummy(1);
        Dummy* d2 = new Dummy(2);
        queue.Push(d1);
        queue.Push(d2);

        EXPECT_FALSE(queue.IsEmpty());
        EXPECT_EQ(queue.Count(), 2);

        Dummy* out1 = queue.Pop(false);
        Dummy* out2 = queue.Pop(false);
        Dummy* out3 = queue.Pop(false);

        EXPECT_EQ(out1->value, 1);
        EXPECT_EQ(out2->value, 2);
        EXPECT_EQ(out3, nullptr);

        delete out1;
        delete out2;
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 2: Multi-threaded stress test
    //
    TEST(ThreadSafeQueueTests, MultiThreadedStress)
    {
        ThreadSafeQueue<Dummy> queue;
        constexpr int numThreads = 4;
        constexpr int iterationsPerThread = 5000;
        std::atomic<int> totalProduced{0};
        std::atomic<int> totalConsumed{0};
        std::atomic<bool> running{true};

        // Each thread will push and pop
        auto worker = [&](int threadIdx)
        {
            std::vector<Dummy*> localProduced;
            std::vector<Dummy*> localConsumed;
            for (int i = 0; i < iterationsPerThread; ++i)
            {
                // Push
                Dummy* d = new Dummy(threadIdx * iterationsPerThread + i);
                queue.Push(d);
                localProduced.push_back(d);
                totalProduced.fetch_add(1, std::memory_order_relaxed);

                // Pop (may block if empty, so use non-blocking)
                Dummy* popped = queue.Pop(false);
                if (popped)
                {
                    localConsumed.push_back(popped);
                    totalConsumed.fetch_add(1, std::memory_order_relaxed);
                }
            }
            // After loop, try to pop any remaining
            while (true)
            {
                Dummy* popped = queue.Pop(false);
                if (!popped) break;
                localConsumed.push_back(popped);
                totalConsumed.fetch_add(1, std::memory_order_relaxed);
            }
            // Clean up
            DeleteAll(localConsumed);
        };

        std::vector<std::thread> threads;
        for (int t = 0; t < numThreads; ++t)
        {
            threads.emplace_back(worker, t);
        }

        for (auto& th : threads)
        {
            th.join();
        }

        // After all threads, queue should be empty
        EXPECT_TRUE(queue.IsEmpty());
        EXPECT_EQ(queue.Count(), 0);

        // All produced items should have been consumed
        EXPECT_EQ(totalProduced.load(), numThreads * iterationsPerThread);
        EXPECT_EQ(totalConsumed.load(), totalProduced.load());
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 3: Blocking Pop with Quit
    //
    TEST(ThreadSafeQueueTests, BlockingPopWithQuit)
    {
        ThreadSafeQueue<Dummy> queue;
        std::atomic<bool> started{false};
        std::atomic<bool> finished{false};

        std::thread t([&]()
        {
            started = true;
            Dummy* d = queue.Pop(true); // Should block until Quit is called
            EXPECT_EQ(d, nullptr);
            finished = true;
        });

        // Wait for thread to start and block
        while (!started) std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        EXPECT_FALSE(finished);

        queue.Quit();

        t.join();
        EXPECT_TRUE(finished);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 4: Iterator Safety (manual lock/unlock)
    //
    TEST(ThreadSafeQueueTests, IteratorSafety)
    {
        ThreadSafeQueue<Dummy> queue;
        for (int i = 0; i < 10; ++i)
        {
            queue.Push(new Dummy(i));
        }

        queue.Lock();
        int sum = 0;
        for (auto it = queue.begin(); it != queue.end(); ++it)
        {
            sum += (*it)->value;
        }
        queue.Unlock();

        EXPECT_EQ(sum, 45); // 0+1+...+9

        // Clean up
        Dummy* d = nullptr;
        while ((d = queue.Pop(false)) != nullptr)
        {
            delete d;
        }
    }

}
