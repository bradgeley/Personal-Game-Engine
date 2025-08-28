// Bradley Christensen - 2022-2025
#pragma once
#include <cstddef>



//----------------------------------------------------------------------------------------------------------------------
// Statistics Utils
//
// Operations for returning statistics about sets of data
//
namespace StatisticsUtils
{
    
    template<typename T>
    double GetAverage(T* valArray, size_t numVals);

    template<typename T>
    T* GetMin(T* valArray, size_t numVals);

    template<typename T>
    T* GetMax(T* valArray, size_t numVals);



    //----------------------------------------------------------------------------------------------------------------------
    template<typename T>
    double GetAverage(T* valArray, size_t numVals)
    {
        if (numVals == 0)
        {
            return 0.0;
        }
        T total = 0;
        for (int i = 0; i < numVals; ++i)
        {
            total = total + valArray[i];
        }
        return (double) total / (double) numVals;
    }



    //----------------------------------------------------------------------------------------------------------------------
    template<typename T>
    T* GetMin(T* valArray, size_t numVals)
    {
        if (numVals == 0)
        {
            return nullptr;
        }
        T* minValue = nullptr;
        for (int i = 0; i < numVals; ++i)
        {
            T* val = valArray + i;
            if (minValue == nullptr)
            {
                minValue = val;
            }
            else if (*val < *minValue)
            {
                minValue = val;
            }
        }
        return minValue;
    }



    //----------------------------------------------------------------------------------------------------------------------
    template<typename T>
    T* GetMax(T* valArray, size_t numVals)
    {
        if (numVals == 0)
        {
            return nullptr;
        }
        T* maxValue = nullptr;
        for (int i = 0; i < numVals; ++i)
        {
            T* val = valArray + i;
            if (maxValue == nullptr)
            {
                maxValue = val;
            }
            else if (*val > *maxValue)
            {
                maxValue = val;
            }
        }
        return maxValue;
    }
}

