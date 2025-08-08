#pragma once
// Bradley Christensen - 2025
#include <vector>
#include "IntVec2.h"
#include "MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
// FastGrid
//
// A 2D grid, row-major, using power of 2 optimization
//
template<typename T, uint8_t Pow2>
class FastGrid
{
public:

    FastGrid() = default;
    explicit FastGrid(FastGrid<T, Pow2> const& copy);
    explicit FastGrid(T const& initialValue);

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

    int INVALID_INDEX = -1;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
FastGrid<T, Pow2>::FastGrid(FastGrid<T,  Pow2> const& copy) : m_data(copy.m_data), m_dimensions(copy.m_dimensions)
{

}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
FastGrid<T, Pow2>::FastGrid(T const& initialValue) : m_dimensions(Pow(2, Pow2), Pow(2, Pow2))
{
    m_data.resize((size_t) m_dimensions.x * m_dimensions.y);

    for (int i = 0; i < m_dimensions.x * m_dimensions.y; ++i)
    {
        m_data[i] = initialValue;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
void FastGrid<T, Pow2>::Initialize(IntVec2 const& dimensions, T const& initialValue)
{
    m_dimensions = dimensions;
    m_data.resize((size_t) dimensions.x * dimensions.y);

    for (int i = 0; i < dimensions.x * dimensions.y; ++i)
    {
        m_data[i] = initialValue;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
void FastGrid<T, Pow2>::Clear()
{
    m_data.clear();
    m_dimensions = IntVec2::ZeroVector;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
void FastGrid<T, Pow2>::Set(int index, T const& value)
{
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
void FastGrid<T, Pow2>::Set(IntVec2 const& coords, T const& value)
{
    int index = GetIndexForCoords(coords);
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
void FastGrid<T, Pow2>::Set(int x, int y, T const& value)
{
    int index = GetIndexForCoords(x, y);
    m_data[index] = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
void FastGrid<T, Pow2>::SetAll(T const& value)
{
    for (int i = 0; i < (int) m_data.size(); ++i)
    {
        m_data[i] = value;
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
void FastGrid<T, Pow2>::SetEdges(T const& value)
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
template <typename T, uint8_t Pow2>
T FastGrid<T, Pow2>::Get(int index) const
{
    return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
T FastGrid<T, Pow2>::Get(int x, int y) const
{
    int index = GetIndexForCoords(x, y);
    return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
T FastGrid<T, Pow2>::Get(IntVec2 const& coords) const
{
    int index = GetIndexForCoords(coords);
    return m_data[index];
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetIndexEastOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return INVALID_INDEX;
    }
    IntVec2 coords = GetCoordsForIndex(index);

    if ((coords.x + 1) < m_dimensions.x)
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
    return INVALID_INDEX;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetIndexWestOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return INVALID_INDEX;
    }
    IntVec2 coords = GetCoordsForIndex(index);

    if ((coords.x - 1) >= 0)
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
    return INVALID_INDEX;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetIndexNorthOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return INVALID_INDEX;
    }
    IntVec2 coords = GetCoordsForIndex(index);

    if ((coords.y + 1) < m_dimensions.y)
    {
        return index + m_dimensions.x;
    }

    // At the edge, wrap or clamp
    if (wrap)
    {
        // go back to the bottom of the column, where the index is the same as the x coord
        return coords.x;
    }
    // We're at the edge
    return INVALID_INDEX;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetIndexSouthOf(int index, bool wrap) const
{
    if (!IsValidIndex(index))
    {
        return INVALID_INDEX;
    }
    IntVec2 coords = GetCoordsForIndex(index);

    if ((coords.y - 1) >= 0)
    {
        return index - m_dimensions.x;
    }

    // At the edge, wrap or clamp
    if (wrap)
    {
        // go to the end of the row
        return Size() - (m_dimensions.x - coords.x);
    }
    // We're at the edge
    return INVALID_INDEX;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetLastIndex() const
{
    return (int) m_data.size() - 1;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
bool FastGrid<T, Pow2>::IsValidIndex(int index) const
{
    return index >= 0 && index < m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
bool FastGrid<T, Pow2>::IsValidCoords(IntVec2 const& coords) const
{
    return  coords.x < m_dimensions.x&&
        coords.y < m_dimensions.y&&
        coords.x >= 0 &&
        coords.y >= 0;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
bool FastGrid<T, Pow2>::IsOnEdge(IntVec2 const& coords) const
{
    return (coords.x == 0) || (coords.y == 0) || (coords.x == GetWidth() - 1) || (coords.y == GetHeight() - 1);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetIndexForCoords(IntVec2 const& coords) const
{
    return (coords.y << Pow2) + coords.x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetIndexForCoords(int x, int y) const
{
    return (y << Pow2) + x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
IntVec2 FastGrid<T, Pow2>::GetCoordsForIndex(int index) const
{
    constexpr uint32_t numBitsInInt = sizeof(int) * 8;
    static uint32_t xBitMask = 0xFFFF'FFFF >> (numBitsInInt - Pow2);
    int x = index & xBitMask;
    int y = index >> Pow2;
    return IntVec2(x, y);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
IntVec2 FastGrid<T, Pow2>::GetDimensions() const
{
    return m_dimensions;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetWidth() const
{
    return m_dimensions.x;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::GetHeight() const
{
    return m_dimensions.y;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::Size() const
{
    return (int) m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
int FastGrid<T, Pow2>::Count() const
{
    return (int) m_data.size();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
T* FastGrid<T, Pow2>::GetRawData()
{
    return m_data.data();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T, uint8_t Pow2>
T const* FastGrid<T, Pow2>::GetRawData() const
{
    return m_data.data();
}