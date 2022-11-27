// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/Core/Engine.h"



// Trivial test, just so I have this file
TEST(Engine, CreateDestroy)
{
    Engine* engine = new Engine();
    EXPECT_TRUE(engine != nullptr);
    delete engine;
    EXPECT_TRUE(engine == nullptr);
}