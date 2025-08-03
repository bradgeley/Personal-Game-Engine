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
    explicit Grid(Grid<T> const& copy);
    explicit Grid(IntVec2 const& dimensions, T const& initialValue);
    explicit Grid(int width, int height, T const& initialValue);

    void Initialize(IntVec2 const& dimensions, T const& initialValue);
    void Clear();

    void Set(int index, T const& value);
    void Set(IntVec2 const& coords, T const& value);
    void Set(int x, int y, T const& value);
    void SetAll(T const& value);
    void SetEdges(T const& value); // All tiles along the 4 edges

	T Get(int index) const;
	T Get(int x, int y) const;
    T Get(IntVec2 const& coords) const;

    int GetIndexEastOf(int index, bool wrap = false) const; // +x
    int GetIndexWestOf(int index, bool wrap = false) const; // -x
    int GetIndexNorthOf(int index, bool wrap = false) const; // +y
    int GetIndexSouthOf(int index, bool wrap = false) const; // -y
    int GetLastIndex() const;

    bool IsValidIndex(int index) const;
    bool IsValidCoords(IntVec2 const& coords) const;
    bool IsOnEdge(IntVec2 const& coords) const;
	int GetIndexForCoords(int x, int y) const;
    int GetIndexForCoords(IntVec2 const& coords) const;
    IntVec2 GetCoordsForIndex(int index) const;
    IntVec2 GetDimensions() const;
    int GetWidth() const;
    int GetHeight() const;
    int Size() const;
    int Count() const;

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
Grid<T>::Grid(Grid<T> const& copy) : m_data(copy.m_data), m_dimensions(copy.m_dimensions)
{
    
}



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
void Grid<T>::Initialize(IntVec2 const& dimensions, T const& initialValue)
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
void Grid<T>::Clear()
{
    m_data.clear();
    m_dimensions = IntVec2::ZeroVector;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid<T>::Set(int index, T const& value)
{
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid<T>::Set(IntVec2 const& coords, T const& value)
{
    int index = GetIndexForCoords(coords);
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid<T>::Set(int x, int y, T const& value)
{
    int index = GetIndexForCoords(x, y);
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid<T>::SetAll(T const& value)
{
    for (int i = 0; i < m_data.size(); ++i)
    {
        m_data[i] = value;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void Grid<T>::SetEdges(T const& value)
{
    // First row
    for (int x = 0; x < m_dimensions.x; ++x)
    {
        m_data[x] = value;
    }

    // Last row
    int width = GetWidth();
    int lastRowFirstIndex = Size() - width;
    int lastIndex = GetLastIndex();
    for (int x = lastRowFirstIndex; x <= lastIndex; ++x)
    {
        m_data[x] = value;
    }

    // Left Column (excluding first and last item)
    int secondRowFirstIndex = width;
    int secondToLastRowFirstIndex = lastRowFirstIndex - width;
    for (int i = secondRowFirstIndex; i <= secondToLastRowFirstIndex; i += width)
    {
        m_data[i] = value;
    }

    // Right Column (excluding first and last item)
    int secondRowLastIndex = 2 * width - 1;
    int secondToLastRowLastIndex = lastIndex - width;
    for (int i = secondRowLastIndex; i <= secondToLastRowLastIndex; i += width)
    {
        m_data[i] = value;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T Grid<T>::Get(int index) const
{
    return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T Grid<T>::Get(int x, int y) const
{
    int index = GetIndexForCoords(x, y);
	return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T Grid<T>::Get(IntVec2 const& coords) const
{
    int index = GetIndexForCoords(coords);
    return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid<T>::GetIndexEastOf(int index, bool wrap) const
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
int Grid<T>::GetIndexWestOf(int index, bool wrap) const
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
int Grid<T>::GetIndexNorthOf(int index, bool wrap) const
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
int Grid<T>::GetIndexSouthOf(int index, bool wrap) const
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
int Grid<T>::GetLastIndex() const
{
    return (int) m_data.size() - 1;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool Grid<T>::IsValidIndex(int index) const
{
    return index >= 0 && index < m_data.size();
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
bool Grid<T>::IsOnEdge(IntVec2 const& coords) const
{
    return (coords.x == 0) || (coords.y == 0) || (coords.x == GetWidth() - 1) || (coords.y == GetHeight() - 1);
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
int Grid<T>::Size() const
{
    return (int) m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
int Grid<T>::Count() const
{
    return (int) m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T* Grid<T>::GetRawData()
{
    return m_data.data();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T const* Grid<T>::GetRawData() const
{
    return m_data.data();
}