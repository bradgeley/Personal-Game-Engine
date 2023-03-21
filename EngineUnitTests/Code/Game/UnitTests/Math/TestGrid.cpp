// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Game/UnitTests/TestUtils.h"
#include "Engine/Math/Grid2D.h"



TEST(Grid2D, Construct)
{
    Grid2D gridOfFloats(5, 5, 1.f);
    for (auto& value : gridOfFloats.m_data)
    {
        EXPECT_FLOAT_EQ(value, 1.f);
    }

    Grid2D gridOfInts(5, 5, 99);
    for (auto& value : gridOfInts.m_data)
    {
        EXPECT_EQ(value, 99);
    }
}

TEST(Grid2D, Dims)
{
    Grid2D gridOfFloats(4, 5, 1.f);
    IntVec2 dims = gridOfFloats.GetDimensions();
    EXPECT_INTVEC2_EQf(dims, 4, 5);
    EXPECT_EQ(gridOfFloats.GetWidth(), 4);
    EXPECT_EQ(gridOfFloats.GetHeight(), 5);
}

TEST(Grid2D, Coords)
{
    Grid2D grid(420, 69, -69420.f);
    IntVec2 coords = grid.GetCoordsForIndex(100);
    EXPECT_INTVEC2_EQf(coords, 100, 0);
    IntVec2 coords2 = grid.GetCoordsForIndex(500);
    EXPECT_INTVEC2_EQf(coords2, 80, 1);
    int index = grid.GetIndexForCoords(coords);
    EXPECT_EQ(index, 100);
    int index2 = grid.GetIndexForCoords(coords2);
    EXPECT_EQ(index2, 500);
}


