#ifndef HASH_H
#define HASH_H

#include "Type.h"

UINT RSHash(const CHAR* str, UINT len);
UINT JSHash(const CHAR* str, UINT len);
UINT PJWHash(const CHAR* str, UINT len);
UINT ELFHash(const CHAR* str, UINT len);
UINT BKDRHash(const CHAR* str, UINT len);
UINT SDBMHash(const CHAR* str, UINT len);
UINT DJBHash(const CHAR* str, UINT len);
UINT DEKHash(const CHAR* str, UINT len);
UINT BPHash(const CHAR* str, UINT len);
UINT FNVHash(const CHAR* str, UINT len);

#endif
