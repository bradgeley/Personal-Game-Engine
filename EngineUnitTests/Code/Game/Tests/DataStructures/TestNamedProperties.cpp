// Bradley Christensen - 2022-2025
#include "pch.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Core/NameTable.h"
#include <gtest/gtest.h>
#include <string>



//----------------------------------------------------------------------------------------------------------------------
// Named Properties Tests
//
namespace TestNamedProperties
{

    //----------------------------------------------------------------------------------------------------------------------
    struct Dummy
    {
        int x = 0;
        bool operator==(const Dummy& other) const { return x == other.x; }
    };



    //----------------------------------------------------------------------------------------------------------------------
    void InitNameTable()
    {
        g_nameTable = new NameTable();
		g_nameTable->Startup();
    }



    //----------------------------------------------------------------------------------------------------------------------
    void DestroyNameTable()
    {
        g_nameTable->Shutdown();
        delete g_nameTable;
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Set and Get Float
    //
    TEST(NamedPropertiesTests, SetAndGetFloat)
    {
        InitNameTable();
        NamedProperties np;
        Name name("floatValue");
        np.Set(name, 3.14f);
        EXPECT_FLOAT_EQ(np.Get(name, 0.0f), 3.14f);
        EXPECT_FLOAT_EQ(np.Get(Name("notSet"), 42.0f), 42.0f);
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Set and Get String
    //
    TEST(NamedPropertiesTests, SetAndGetString)
    {
        InitNameTable();
        NamedProperties np;
        Name name("strValue");
        np.Set(name, std::string("hello"));
        EXPECT_EQ(np.Get(name, std::string("default")), "hello");
        EXPECT_EQ(np.Get(Name("notSet"), std::string("default")), "default");
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Set and Get Name
    //
    TEST(NamedPropertiesTests, SetAndGetName)
    {
        InitNameTable();
        NamedProperties np;
        Name name("nameValue");
        Name value("TestName");
        np.Set(name, value);
        EXPECT_EQ(np.Get(name, Name("Default")), value);
        EXPECT_EQ(np.Get(Name("notSet"), Name("Default")), Name("Default"));
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Set and Get Pointer
    //
    TEST(NamedPropertiesTests, SetAndGetPointer)
    {
        InitNameTable();
        NamedProperties np;
        Name name("ptrValue");
        Dummy d1{ 42 };
        Dummy* ptr = &d1;
        np.Set(name, ptr);
        EXPECT_EQ(np.Get<Dummy*>(name, nullptr), ptr);
        EXPECT_EQ(np.Get(Name("notSet"), nullptr), nullptr);
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Set and Get Int
    //
    TEST(NamedPropertiesTests, SetAndGetInt)
    {
        InitNameTable();
        NamedProperties np;
        Name name("intValue");
        np.Set(name, 123);
        EXPECT_EQ(np.Get(name, 0), 123);
        EXPECT_EQ(np.Get(Name("notSet"), 456), 456);
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Overwrite With Different Type
    //
    TEST(NamedPropertiesTests, OverwriteWithDifferentType)
    {
        InitNameTable();
        NamedProperties np;
        Name name("overwriteTest");
        np.Set(name, 1.23f);
        EXPECT_FLOAT_EQ(np.Get(name, 0.0f), 1.23f);

        // Overwrite with string
        np.Set(name, std::string("newValue"));
        EXPECT_EQ(np.Get(name, std::string("default")), "newValue");
        // Old type should return default
        EXPECT_FLOAT_EQ(np.Get(name, 0.0f), 0.0f);

        // Overwrite with int
        np.Set(name, 99);
        EXPECT_EQ(np.Get(name, 0), 99);
        EXPECT_EQ(np.Get(name, std::string("default")), "default");
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Contains and Size
    //
    TEST(NamedPropertiesTests, ContainsAndSize)
    {
        InitNameTable();
        NamedProperties np;
        Name n1("a"), n2("b");
        np.Set(n1, 1);
        np.Set(n2, 2.0f);
        EXPECT_TRUE(np.Contains(n1));
        EXPECT_TRUE(np.Contains(n2));
        EXPECT_FALSE(np.Contains(Name("c")));
        EXPECT_EQ(np.Size(), 2);
        np.Clear();
        EXPECT_EQ(np.Size(), 0);
        EXPECT_FALSE(np.Contains(n1));
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Set and Get Custom Struct
    //
    TEST(NamedPropertiesTests, SetAndGetCustomStruct)
    {
        InitNameTable();
        NamedProperties np;
        Name name("dummy");
        Dummy d{ 7 };
        np.Set(name, d);
        EXPECT_EQ(np.Get(name, Dummy{ 0 }), d);
        DestroyNameTable();
    }



    //----------------------------------------------------------------------------------------------------------------------
    // Null Pointer Handling
    //
    TEST(NamedPropertiesTests, PointerNullHandling)
    {
        InitNameTable();
        NamedProperties np;
        Name name("ptrNull");
        Dummy* ptr = nullptr;
        np.Set(name, ptr);
        EXPECT_EQ(np.Get(name, reinterpret_cast<Dummy*>((uintptr_t) 0xDEADBEEF)), nullptr);
        DestroyNameTable();
    }

}
