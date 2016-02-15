#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "Hash.h"
#include "Type.h"

class BloomFilter
{
    public:
        BloomFilter(INT size) : SIZE(size) { vec.resize(size); };
//      VOID setBit(std::ifstream &f);
        VOID setBit(const CHAR *str, INT len);
        VOID setBit(UINT count);

//      BOOL checkBit(std::ifstream &f);
        BOOL checkBit(const CHAR *str, INT len);
        BOOL checkBit(UINT count);
    private:
        std::vector<CHAR> vec;
        const UINT SIZE;
};

#endif

