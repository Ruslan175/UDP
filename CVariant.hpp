#ifndef  _CVARIANT_HPP
#define  _CVARIANT_HPP

#include <vector>
#include "types.h"



class CVariant
{
    public:
        std::vector<double>      BufferDouble;
        std::vector<long double> BufferLongDouble;
        std::vector<float>       BufferFloat;
        
        CVariant();
        CVariant(eDataType tp);
        void resize(uint32_t raw_byte_sz);
        char* getRawDataPtr();
        void sort();
        void show1stLast();
        void setVariant(eDataType tp);
        void clear();
        uint32_t getElementSize() const;

    private:
        eDataType mType;
};


#endif
