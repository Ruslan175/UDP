#include <algorithm>
#include <iostream>
#include "CVariant.hpp"


bool compareDoubles(double x, double y) 
{ 
    return (x > y);
}

bool compareLongDoubles(long double x, long double y) 
{ 
    return (x > y); 
}

bool compareFloats(float x, float y) 
{ 
    return (x > y); 
}


CVariant::CVariant()
: BufferDouble()
, BufferLongDouble()
, BufferFloat()
, mType(eDataTypeSz)
{
} 

CVariant::CVariant(eDataType tp)
: BufferDouble()
, BufferLongDouble()
, BufferFloat()
, mType(tp)
{
} 

void CVariant::setVariant(eDataType tp)
{
    mType = tp;
}

void CVariant::clear()
{
    mType = eDataTypeSz;
    BufferLongDouble.clear();
    BufferFloat.clear();
    BufferDouble.clear();
}

uint32_t CVariant::getElementSize() const
{
    switch(mType)
    {
        case eLongDouble:   return sizeof(long double);
        case eFloat:        return sizeof(float);
        default:            return sizeof(double);
    }
}

void CVariant::resize(uint32_t raw_byte_sz)
{
    switch(mType)
    {
        case eLongDouble:   BufferLongDouble.resize(raw_byte_sz / sizeof(long double));     break;
        case eFloat:        BufferFloat.resize(raw_byte_sz / sizeof(float));                break;
        default:            BufferDouble.resize(raw_byte_sz / sizeof(double));              break;
    }
}

char* CVariant::getRawDataPtr()
{
    char *ret = NULL;
    switch(mType)
    {
        case eLongDouble:   ret = reinterpret_cast<char*>(BufferLongDouble.data());     break;
        case eFloat:        ret = reinterpret_cast<char*>(BufferFloat.data());          break;
        default:            ret = reinterpret_cast<char*>(BufferDouble.data());         break;
    }
    return ret;
}

void CVariant::sort()
{
    switch(mType)
    {
        case eLongDouble:   std::sort( BufferLongDouble.begin(), BufferLongDouble.end(), compareLongDoubles);     break;
        case eFloat:        std::sort( BufferFloat.begin(), BufferFloat.end(), compareFloats);                    break;
        default:            std::sort( BufferDouble.begin(), BufferDouble.end(), compareDoubles);                 break;
    }
}

void CVariant::show1stLast()
{
    switch(mType)
    {
        case eLongDouble: 
           std::cout << "1st: "  << BufferLongDouble.front() << std::endl;
           std::cout << "last: " << BufferLongDouble.back() << std::endl;
           break;
        case eFloat:  
           std::cout << "1st: "  << BufferFloat.front() << std::endl;
           std::cout << "last: " << BufferFloat.back() << std::endl;
           break;
        default:
           std::cout << "1st: "  << BufferDouble.front() << std::endl;
           std::cout << "last: " << BufferDouble.back() << std::endl;
           break;     
    }
}




