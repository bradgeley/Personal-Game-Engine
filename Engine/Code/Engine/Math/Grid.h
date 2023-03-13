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

    bool IsValidIndex(int index) const;
    bool IsValidCoords(IntVec2 const& coords) const;
	int GetIndexForCoords(int x, int y) const;
    int GetIndexForCoords(IntVec2 const& coords) const;
    IntVec2 GetCoordsForIndex(int index) const;
    IntVec2 GetDimensions() const;
    int GetWidth() const;
    int GetHeight() const;
    void* GetRawData() const;

    T& operator[](int index);

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
bool Grid<T>::IsValidIndex(int index) const
{
    return index > 0 && index < m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool Grid<T>::IsValidCoords(IntVec2 const& coords) const
{
	return  coords.x < m_dimensions.x   && 
            coords.y < m_dimensions.y   && 
            coords.x >= 0               && 
            coords.y >= 0;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid<T>::GetIndexForCoords(IntVec2 const& coords) const
{
    return coords.y * m_dimensions.x + coords.x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid<T>::GetIndexForCoords(int x, int y) const
{
	return y * m_dimensions.x + x;
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



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Grid<T>::operator[](int index)
{
    return m_data[index];
}
