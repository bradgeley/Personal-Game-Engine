// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/DataStructures/NamedProperties.h"



TEST(NamedProperties, Usage)
{
    std::string name = "Brad";
    NamedProperties props;
    props.Set("name", name);
    std::string name2 = props.Get("name", std::string());
    EXPECT_EQ(name, name2);

    props.Set("uint8", (uint8_t) 69);
    uint8_t uint8_val = props.Get("uint8", (uint8_t) 0);
    EXPECT_EQ(69, uint8_val);
    
    props.Set("int", 73);
    int int_val = props.Get("int", 0);
    EXPECT_EQ(73, int_val);
    
    props.Set("flt", 53.f);
    float flt_val = props.Get("flt", 0.f);
    EXPECT_EQ(53.f, flt_val);
    
    props.Set("dbl", 42.0);
    double dbl_val = props.Get("dbl", 0.0);
    EXPECT_EQ(42.0, dbl_val);
}



TEST(NamedProperties, OverwriteValue)
{
    NamedProperties props;
    props.Set("val", 69);
    int int_val = props.Get("val", 0);
    EXPECT_EQ(int_val, 69);
    props.Set("val", 420);
    int_val = props.Get("val", 0);
    EXPECT_EQ(int_val, 420);
}



TEST(NamedProperties, OverwriteType)
{
    int originalVal = 69;
    float overwrittenVal = 420.f;
    NamedProperties props;
    props.Set("val", originalVal);
    props.Set("val", overwrittenVal);
    
    int int_val = props.Get("val", 0);
    float flt_val = props.Get("val", 0.f);

    EXPECT_EQ(int_val, 0);
    EXPECT_EQ(flt_val, 420.f);
}



TEST(NamedProperties, OtherFuncs)
{
    NamedProperties props;
    EXPECT_EQ(props.Size(), 0);
    props.Set("val", 69);
    EXPECT_TRUE(props.Contains("val"));
    EXPECT_FALSE(props.Contains("VAL"));
    EXPECT_EQ(props.Size(), 1);
    props.Set("otherVal", 420);
    EXPECT_TRUE(props.Contains("otherVal"));
    EXPECT_TRUE(props.Contains("val"));
    EXPECT_EQ(props.Size(), 2);

    props.Clear();
    EXPECT_FALSE(props.Contains("val"));
    EXPECT_FALSE(props.Contains("otherVal"));
    EXPECT_EQ(props.Size(), 0);
}