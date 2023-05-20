// Bradley Christensen - 2022-2023
#pragma once
#include <float.h>
#include <type_traits>



//----------------------------------------------------------------------------------------------------------------------
// Statistics Utils
//
// Operations for returning statistics about sets of data
//
namespace StatisticsUtils
{
    
    //------------------------------------------------------------------------------------------------------------------
    template<typename T>
    double GetAverage(T* valArray, size_t numVals)
    {
        T total = 0;
        for (int i = 0; i < numVals; ++i)
        {
            total = total + valArray[i];
        }
        return (double) total / (double) numVals;
    }


    //------------------------------------------------------------------------------------------------------------------
    // Returns the address of the min value in the array
    template<typename T>
    T* GetMin(T* valArray, size_t numVals)
    {
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


    //------------------------------------------------------------------------------------------------------------------
    // Returns the address of the max value in the array
    template<typename T>
    T* GetMax(T* valArray, size_t numVals)
    {
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

