// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Events/EventUtils.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Core/NameTable.h"
#include <gtest/gtest.h>



//-----------------------------------------------------------------------------------------------------------------------
void StartupTest()
{
    g_nameTable = new NameTable();
    g_nameTable->Startup();
    g_eventSystem = new EventSystem(EventSystemConfig{});
    g_eventSystem->Startup();
}



//-----------------------------------------------------------------------------------------------------------------------
void ShutdownTest()
{
    if (g_eventSystem)
    {
        g_eventSystem->Shutdown();
        delete g_eventSystem;
        g_eventSystem = nullptr;
    }
    if (g_nameTable) 
    {
        g_nameTable->Shutdown();
        delete g_nameTable;
		g_nameTable = nullptr;
    }
}



//-----------------------------------------------------------------------------------------------------------------------
bool TestFunc1(NamedProperties& args)
{
    int* counter = nullptr;
    counter = args.Get<int*>("counter", counter);
    if (counter)
    {
        (*counter)++;
    }
    return false; // Only return true if you want to consume the event
}



//-----------------------------------------------------------------------------------------------------------------------
bool TestFunc2(NamedProperties& args)
{
    int* counter = nullptr;
    counter = args.Get<int*>("counter", counter);
    if (counter)
    {
        (*counter)++;
    }
    return true; // Consumes the event, stops further propagation
}



//-----------------------------------------------------------------------------------------------------------------------
struct TestObj
{
    int callCount = 0;
    bool shouldReturn = false; // Default to not consuming

    bool OnEvent(NamedProperties&)
    {
        ++callCount;
        return shouldReturn;
    }
};



//-----------------------------------------------------------------------------------------------------------------------
// Event System Tests
//
namespace EventSystemTests
{

    //-----------------------------------------------------------------------------------------------------------------------
    // Test that global events work with multiple subscribers
    //
    TEST(EventSystemTests, MultipleSubscribers)
    {
        StartupTest();

        int counter = 0;
        NamedProperties args;
        args.Set("counter", &counter);

        EventUtils::SubscribeEventCallbackFunction("TestEvent", TestFunc1);
        EventUtils::SubscribeEventCallbackFunction("TestEvent", TestFunc1);

        int numCalled = EventUtils::FireEvent("TestEvent", args);

        EXPECT_EQ(counter, 2);
        EXPECT_EQ(numCalled, 2);

        ShutdownTest();
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Test that returning true from an event stops the event from proceeding to later subscribers
    //
    TEST(EventSystemTests, StopOnTrueReturn)
    {
        StartupTest();

        int counter = 0;
        NamedProperties args;
        args.Set("counter", &counter);

        EventUtils::SubscribeEventCallbackFunction("TestEvent", TestFunc2); // returns true, consumes event
        EventUtils::SubscribeEventCallbackFunction("TestEvent", TestFunc1);

        int numCalled = EventUtils::FireEvent("TestEvent", args);

        // Only the first should be called
        EXPECT_EQ(counter, 1);
        EXPECT_EQ(numCalled, 1); // Both are registered, but only first executes

        ShutdownTest();
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Test that both the function and method versions of events work
    //
    TEST(EventSystemTests, FunctionAndMethodSubscribers)
    {
        StartupTest();

        int counter = 0;
        NamedProperties args;
        args.Set("counter", &counter);

        TestObj obj;
        obj.shouldReturn = true;

        g_eventSystem->SubscribeFunction("TestEvent", TestFunc1);
        g_eventSystem->SubscribeMethod<TestObj, bool (TestObj::*)(NamedProperties&)>("TestEvent", &obj, &TestObj::OnEvent);

        int numCalled = g_eventSystem->FireEvent("TestEvent", args);

        EXPECT_EQ(counter, 1);
        EXPECT_EQ(obj.callCount, 1);
        EXPECT_EQ(numCalled, 2);

        ShutdownTest();
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Test that EventDelegate supports function and method subscriptions and fires all correctly
    //
    TEST(EventSystemTests, EventDelegate_SubscribeAndFire)
    {
        StartupTest();

        // Setup
        int counter = 0;
        NamedProperties args;
        args.Set("counter", &counter);

        struct LocalObj
        {
            int callCount = 0;
            bool OnEvent(NamedProperties&) { ++callCount; return false; }
        } obj;

        EventDelegate delegateObj;
        delegateObj.SubscribeFunction(TestFunc1);
        delegateObj.SubscribeMethod<LocalObj, bool (LocalObj::*)(NamedProperties&)>(&obj, &LocalObj::OnEvent);

        // Fire the delegate multiple times to ensure both function and method are called each time
        int numBroadcasts = 3;
        for (int i = 0; i < numBroadcasts; ++i)
        {
            delegateObj.Broadcast(args);
        }

        // Validate
        EXPECT_EQ(counter, numBroadcasts); // TestFunc1 increments counter
        EXPECT_EQ(obj.callCount, numBroadcasts); // Method increments callCount

        // Cleanup
        ShutdownTest();
    }



    //-----------------------------------------------------------------------------------------------------------------------
    // Test that EventDelegate stops broadcasting to further subscribers when a subscriber returns true (consumes event)
    //
    TEST(EventSystemTests, EventDelegate_StopOnTrueReturn)
    {
        StartupTest();

        int counter = 0;
        NamedProperties args;
        args.Set("counter", &counter);

        // First function will consume the event (returns true), second should not be called
        EventDelegate delegateObj;
        delegateObj.SubscribeFunction(TestFunc2); // returns true, consumes event
        delegateObj.SubscribeFunction(TestFunc1); // should not be called

        int numExecuted = delegateObj.Broadcast(args);

        EXPECT_EQ(counter, 1); // Only TestFunc2 should increment counter
        EXPECT_EQ(numExecuted, 1); // Only TestFunc2 should increment counter

        ShutdownTest();
    }

}