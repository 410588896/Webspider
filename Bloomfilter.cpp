#include "Bloomfilter.h"

#define DEBUG

/*
VOID BloomFilter::setBit(std::ifstream &f)
{
    std::string line;
    while(f >> line)
    {
        setBit(line);
    }
}

BOOL BloomFilter::checkBit(std::ifstream &f)
{
    string line;
    while(f >> line)
    {
        if(!checkBit(line))
            std::cout << line << std::endl;
    }
}
*/

VOID BloomFilter::setBit(const CHAR *str, INT len)
{
#ifdef DEBUG
	printf("Add %s:%d to BloomFilter\n", str, len);
#endif
	UINT bitpos = 0;
	
	bitpos = RSHash(str, len);
	setBit(bitpos);
	bitpos = JSHash(str, len);
	setBit(bitpos);
	bitpos = PJWHash(str, len);
	setBit(bitpos);
	bitpos = ELFHash(str, len);
	setBit(bitpos);
	bitpos = BKDRHash(str, len);
	setBit(bitpos);
	bitpos = SDBMHash(str, len);
	setBit(bitpos);
	bitpos = DJBHash(str, len);
	setBit(bitpos);
	bitpos = DEKHash(str, len);
	setBit(bitpos);
	bitpos = BPHash(str, len);
	setBit(bitpos);
	bitpos = FNVHash(str, len);
	setBit(bitpos);
}

BOOL BloomFilter::checkBit(const CHAR *str, INT len)
{
	UINT bitpos = 0;
	BOOL rev = true;

	bitpos = RSHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = JSHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = PJWHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = ELFHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = BKDRHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = SDBMHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = DJBHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = DEKHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = BPHash(str, len);
	rev &= checkBit(bitpos);
	bitpos = FNVHash(str, len);
	rev &= checkBit(bitpos);
	return rev;
}

VOID BloomFilter::setBit(UINT count)
{
	count = count % (SIZE * 8);
	vec[count / 8] |= (1 << (count % 8));
}

BOOL BloomFilter::checkBit(UINT count)
{
	count = count % (SIZE * 8);
	return vec[count / 8] &= (1 << (count % 8));
}
