#include "Hash.h"

UINT RSHash(const CHAR* str, UINT len)
{
   UINT b = 378551;
   UINT a = 63689;
   UINT hash = 0;
   UINT i = 0;

   for(i=0; i<len; str++, i++)
   {
      hash = hash*a + (*str);
      a = a*b;
   }

   return hash;
}
/* End Of RS Hash Function */


UINT JSHash(const CHAR* str, UINT len)
{
   UINT hash = 1315423911;
   UINT i    = 0;

   for(i=0; i<len; str++, i++)
   {
      hash ^= ((hash<<5) + (*str) + (hash>>2));
   }

   return hash;
}
/* End Of JS Hash Function */


UINT PJWHash(const CHAR* str, UINT len)
{
   const UINT BitsInUnsignedInt = (UINT)(sizeof(UINT) * 8);
   const UINT ThreeQuarters = (UINT)((BitsInUnsignedInt  * 3) / 4);
   const UINT OneEighth = (UINT)(BitsInUnsignedInt / 8);
   const UINT HighBits = (UINT)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
   UINT hash = 0;
   UINT test = 0;
   UINT i = 0;

   for(i=0;i<len; str++, i++)
   {
      hash = (hash<<OneEighth) + (*str);

      if((test = hash & HighBits)  != 0)
      {
         hash = ((hash ^(test >> ThreeQuarters)) & (~HighBits));
      }
   }

   return hash;
}
/* End Of  P. J. Weinberger Hash Function */


UINT ELFHash(const CHAR* str, UINT len)
{
   UINT hash = 0;
   UINT x    = 0;
   UINT i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (hash << 4) + (*str);
      if((x = hash & 0xF0000000L) != 0)
      {
         hash ^= (x >> 24);
      }
      hash &= ~x;
   }

   return hash;
}
/* End Of ELF Hash Function */


UINT BKDRHash(const CHAR* str, UINT len)
{
   UINT seed = 131; /* 31 131 1313 13131 131313 etc.. */
   UINT hash = 0;
   UINT i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (hash * seed) + (*str);
   }

   return hash;
}
/* End Of BKDR Hash Function */


UINT SDBMHash(const CHAR* str, UINT len)
{
   UINT hash = 0;
   UINT i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (*str) + (hash << 6) + (hash << 16) - hash;
   }

   return hash;
}
/* End Of SDBM Hash Function */


UINT DJBHash(const CHAR* str, UINT len)
{
   UINT hash = 5381;
   UINT i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = ((hash << 5) + hash) + (*str);
   }

   return hash;
}
/* End Of DJB Hash Function */


UINT DEKHash(const CHAR* str, UINT len)
{
   UINT hash = len;
   UINT i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
   }
   return hash;
}
/* End Of DEK Hash Function */


UINT BPHash(const CHAR* str, UINT len)
{
   UINT hash = 0;
   UINT i    = 0;
   for(i = 0; i < len; str++, i++)
   {
      hash = hash << 7 ^ (*str);
   }

   return hash;
}
/* End Of BP Hash Function */


UINT FNVHash(const CHAR* str, UINT len)
{
   const UINT fnv_prime = 0x811C9DC5;
   UINT hash      = 0;
   UINT i         = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash *= fnv_prime;
      hash ^= (*str);
   }

   return hash;
}
/* End Of FNV Hash Function */
