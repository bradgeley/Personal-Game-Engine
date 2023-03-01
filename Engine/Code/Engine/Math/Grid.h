// Bradley Christensen - 2022-2023
#pragma once
#include <vector>
#include "IntVec2.h"



//----------------------------------------------------------------------------------------------------------------------
// Grid
//
// A 2D grid, row-major
//
template<typename T>
class Grid
{
public:

    Grid() = default;
    explicit Grid(IntVec2 const& dimensions, T const& initialValue);
    explicit Grid(int width, int height, T const& initialValue);

    int GetIndexForCoords(IntVec2 const& coords) const;
    IntVec2 GetCoordsForIndex(int index) const;
    IntVec2 GetDimensions() const;
    int GetWidth() const;
    int GetHeight() const;
    void* GetRawData() const;

public:

    IntVec2 m_dimensions;
    std::vector<T> m_data;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
Grid<T>::Grid(IntVec2 const& dimensions, T const& initialValue) : m_dimensions(dimensions)
{
    m_data.resize((size_t) dimensions.x * dimensions.y);
    
    for (int i=0; i<dimensions.x * dimensions.y; ++i)
    {
        m_data[i] = initialValue;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
Grid<T>::Grid(int width, int height, T const& initialValue) : Grid(IntVec2(width, height), initialValue)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid<T>::GetIndexForCoords(IntVec2 const& coords) const
{
    return coords.y * m_dimensions.x + coords.x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
IntVec2 Grid<T>::GetCoordsForIndex(int index) const
{
    return IntVec2(index % m_dimensions.x, index / m_dimensions.x);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
IntVec2 Grid<T>::GetDimensions() const
{
    return m_dimensions;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid<T>::GetWidth() const
{
    return m_dimensions.x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid<T>::GetHeight() const
{
    return m_dimensions.y;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void* Grid<T>::GetRawData() const
{
    return (void*) m_data.data();
}
