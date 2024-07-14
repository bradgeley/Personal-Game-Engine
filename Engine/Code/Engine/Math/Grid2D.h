// Bradley Christensen - 2022-2023
#pragma once
#include <vector>
#include "IntVec2.h"



//----------------------------------------------------------------------------------------------------------------------
// Grid2D
//
// A 2D grid, row-major
//
template<typename T>
class Grid2D
{
public:

    Grid2D() = default;
    explicit Grid2D(Grid2D<T> const& copy);
    explicit Grid2D(IntVec2 const& dimensions, T const& initialValue);
    explicit Grid2D(int width, int height, T const& initialValue);

    void Initialize(IntVec2 const& dimensions, T const& initialValue);

    void Set(int index, T const& value);
    void Set(IntVec2 const& coords, T const& value);
    void Set(int x, int y, T const& value);
    void SetAll(T const& value);
	T Get(int index) const;
	T Get(int x, int y) const;
    T Get(IntVec2 const& coords) const;

    int GetIndexEastOf(int index, bool wrap = false) const; // +x
    int GetIndexWestOf(int index, bool wrap = false) const; // -x
    int GetIndexNorthOf(int index, bool wrap = false) const; // +y
    int GetIndexSouthOf(int index, bool wrap = false) const; // -y

    bool IsValidIndex(int index) const;
    bool IsValidCoords(IntVec2 const& coords) const;
	int GetIndexForCoords(int x, int y) const;
    int GetIndexForCoords(IntVec2 const& coords) const;
    IntVec2 GetCoordsForIndex(int index) const;
    IntVec2 GetDimensions() const;
    int GetWidth() const;
    int GetHeight() const;
    int Size() const;
    T* GetRawData();
    T const* GetRawData() const;

    auto begin() { return m_data.begin(); }
    auto begin() const { return m_data.begin(); }
    auto end() { return m_data.end(); }
    auto end() const { return m_data.end(); }

public:

    IntVec2 m_dimensions;
    std::vector<T> m_data;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
Grid2D<T>::Grid2D(Grid2D<T> const& copy) : m_data(copy.m_data), m_dimensions(copy.m_dimensions)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
Grid2D<T>::Grid2D(IntVec2 const& dimensions, T const& initialValue) : m_dimensions(dimensions)
{
    m_data.resize((size_t) dimensions.x * dimensions.y);
    
    for (int i=0; i<dimensions.x * dimensions.y; ++i)
    {
        m_data[i] = initialValue;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
Grid2D<T>::Grid2D(int width, int height, T const& initialValue) : Grid2D(IntVec2(width, height), initialValue)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid2D<T>::Initialize(IntVec2 const& dimensions, T const& initialValue)
{
    m_dimensions = dimensions;
    m_data.resize((size_t) dimensions.x * dimensions.y);

    for (int i = 0; i < dimensions.x * dimensions.y; ++i)
    {
        m_data[i] = initialValue;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid2D<T>::Set(int index, T const& value)
{
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid2D<T>::Set(IntVec2 const& coords, T const& value)
{
    int index = GetIndexForCoords(coords);
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid2D<T>::Set(int x, int y, T const& value)
{
    int index = GetIndexForCoords(x, y);
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid2D<T>::SetAll(T const& value)
{
    for (int i = 0; i < m_data.size(); ++i)
    {
        m_data[i] = value;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T Grid2D<T>::Get(int index) const
{
    return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T Grid2D<T>::Get(int x, int y) const
{
    int index = GetIndexForCoords(x, y);
	return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T Grid2D<T>::Get(IntVec2 const& coords) const
{
    int index = GetIndexForCoords(coords);
    return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetIndexEastOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return -1;
    }
    int xCoord = index % m_dimensions.x;

    if ((xCoord + 1) < m_dimensions.x)
    {
        return ++index;
    }

    // At the edge, wrap or clamp
    if (wrap)
    {
        // go back to the start of the row
        return index - m_dimensions.x + 1; 
    }
    // We're at the edge
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetIndexWestOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return -1;
    }
    int xCoord = index % m_dimensions.x;

    if ((xCoord - 1) >= 0)
    {
        return --index;
    }

    // At the edge, wrap or clamp
    if (wrap)
    {
        // go to the end of the row
        return index + m_dimensions.x - 1; 
    }
    // We're at the edge
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetIndexNorthOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return -1;
    }
    int yCoord = index / m_dimensions.x;

    if ((yCoord + 1) < m_dimensions.y)
    {
        return index + m_dimensions.x;
    }

    // At the edge, wrap or clamp
    if (wrap)
    {
        // go back to the bottom of the column
        int xCoord = index % m_dimensions.x;
        return xCoord; 
    }
    // We're at the edge
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetIndexSouthOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return -1;
    }
    int yCoord = index / m_dimensions.x;

    if ((yCoord - 1) >= 0)
    {
        return index - m_dimensions.x;
    }

    // At the edge, wrap or clamp
    if (wrap)
    {
        // go to the end of the row
        int xCoord = index % m_dimensions.x;
        return Size() - (m_dimensions.x - xCoord); 
    }
    // We're at the edge
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool Grid2D<T>::IsValidIndex(int index) const
{
    return index >= 0 && index < m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool Grid2D<T>::IsValidCoords(IntVec2 const& coords) const
{
	return  coords.x < m_dimensions.x   && 
            coords.y < m_dimensions.y   && 
            coords.x >= 0               && 
            coords.y >= 0;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetIndexForCoords(IntVec2 const& coords) const
{
    return coords.y * m_dimensions.x + coords.x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetIndexForCoords(int x, int y) const
{
	return y * m_dimensions.x + x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
IntVec2 Grid2D<T>::GetCoordsForIndex(int index) const
{
    return IntVec2(index % m_dimensions.x, index / m_dimensions.x);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
IntVec2 Grid2D<T>::GetDimensions() const
{
    return m_dimensions;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetWidth() const
{
    return m_dimensions.x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::GetHeight() const
{
    return m_dimensions.y;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid2D<T>::Size() const
{
    return (int) m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T* Grid2D<T>::GetRawData()
{
    return m_data.data();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T const* Grid2D<T>::GetRawData() const
{
    return m_data.data();
}