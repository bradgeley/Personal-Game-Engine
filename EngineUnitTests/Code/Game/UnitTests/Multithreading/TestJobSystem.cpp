// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/Multithreading/JobSystem.h"



TEST(JobSystem, CreateDestroy)
{
    JobSystem* js = new JobSystem();
    EXPECT_TRUE(js != nullptr);
    delete js;
    EXPECT_TRUE(js == nullptr);
}