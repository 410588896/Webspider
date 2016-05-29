/*
 *  dechunk.h
 *  http_chunk_demo
 *
 *  Created by I MAC on 11-9-7.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DECHUNK_H
#define DECHUNK_H

#include <stdio.h>

#include "Type.h"

#define DCE_OK              0
#define DCE_ISRUNNING       1
#define DCE_FORMAT          2
#define DCE_ARGUMENT        3
#define DCE_MEM             4
#define DCE_LOCK            5


INT dechunk_init();
INT dechunk(VOID *input, INT inlen);
INT dechunk_getbuff(VOID **buff, INT *buf_size);
INT dechunk_free();


VOID *memstr(VOID *src, INT src_len, CHAR *sub);

#endif // _DECHUNK_H_
