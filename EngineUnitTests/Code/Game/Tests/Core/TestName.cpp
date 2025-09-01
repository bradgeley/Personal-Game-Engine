// Bradley Christensen 2022-2025
#include "pch.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/NameTable.h"
#include <vector>
#include <string>
#include <random>
#include <gtest/gtest.h>



//----------------------------------------------------------------------------------------------------------------------
namespace TestName
{

    //----------------------------------------------------------------------------------------------------------------------
    struct NameTableScope
    {
        NameTableScope()
        {
            g_nameTable = new NameTable();
            g_nameTable->Startup();
        }
        ~NameTableScope()
        {
            g_nameTable->Shutdown();
            delete g_nameTable;
            g_nameTable = nullptr;
        }
    };



    //----------------------------------------------------------------------------------------------------------------------
    // Test 1: Simple Functionality
    //
    TEST(NameTests, SimpleFunctionality)
    {
        NameTableScope scope;

        Name n1("Alice");
        Name n2("Bob");
        Name n3("Alice");
        Name n4("Charlie");

        // Test equality
        EXPECT_EQ(n1, n3);
        EXPECT_NE(n1, n2);
        EXPECT_NE(n2, n4);

        // Test validity
        EXPECT_TRUE(n1.IsValid());
        EXPECT_TRUE(n2.IsValid());
        EXPECT_TRUE(n3.IsValid());
        EXPECT_TRUE(n4.IsValid());

        // Test ToString
        EXPECT_EQ(n1.ToString(), "Alice");
        EXPECT_EQ(n2.ToString(), "Bob");
        EXPECT_EQ(n4.ToString(), "Charlie");

    #if defined(_DEBUG)
        EXPECT_EQ(n1.ToString(), n1.GetDebugString());
        EXPECT_EQ(n2.ToString(), n2.GetDebugString());
        EXPECT_EQ(n3.ToString(), n3.GetDebugString());
        EXPECT_EQ(n4.ToString(), n4.GetDebugString());
    #endif
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 2: Perf Test - Creation of 1000 unique names
    //
    TEST(NameTests, PerfCreation)
    {
        NameTableScope scope;

        std::vector<Name> names;
        names.reserve(1000);

        for (int i = 0; i < 1000; ++i)
        {
            names.emplace_back("Name_" + std::to_string(i));
        }

        // All names should be valid and unique
        for (int i = 0; i < 1000; ++i)
        {
            EXPECT_TRUE(names[i].IsValid());
            EXPECT_EQ(names[i].ToString(), "Name_" + std::to_string(i));
        }
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 3: Perf Test - Equality of 100 random pairs from 100 names, 100000 times
    //
    TEST(NameTests, PerfEquality)
    {
        NameTableScope scope;

        std::vector<Name> names;
        names.reserve(100);
        for (int i = 0; i < 100; ++i)
        {
            names.emplace_back("Name_" + std::to_string(i));
        }

        std::mt19937 rng(12345);
        std::uniform_int_distribution<int> dist(0, 99);

        for (int i = 0; i < 100000; ++i)
        {
            int a = dist(rng);
            int b = dist(rng);
            if (a == b)
            {
                EXPECT_EQ(names[a], names[b]);
            }
            else
            {
                EXPECT_NE(names[a], names[b]);
            }
        }
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 4: Case Insensitivity
    //
    TEST(NameTests, CaseInsensitivity)
    {
        NameTableScope scope;

        Name n1("TestName");
        Name n2("testname");
        Name n3("TESTNAME");

        // All should be equal due to case-insensitive table
        EXPECT_EQ(n1, n2);
        EXPECT_EQ(n2, n3);
        EXPECT_EQ(n1, n3);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 5: Invalid Name
    //
    TEST(NameTests, InvalidName)
    {
        NameTableScope scope;

        Name invalid;
        EXPECT_FALSE(invalid.IsValid());
        EXPECT_EQ(invalid, Name::s_invalidName);
        EXPECT_EQ(invalid.ToString(), Name::s_invalidNameString);
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Test 6: ToCStr Consistency
    //
    TEST(NameTests, ToCStr)
    {
        NameTableScope scope;

        Name n1("Alpha");
        const char* cstr = n1.ToCStr();
        EXPECT_EQ(std::string(cstr), "Alpha");
    }

}