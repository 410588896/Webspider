#ifndef COMPRESS_H
#define COMPRESS_H

#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Type.h"

INT gzcompress(BYTE *data, LONG ndata, BYTE *zdata, LONG *nzdata);

INT gzdecompress(BYTE *zdata, LONG nzdata, BYTE *data, LONG *ndata);

INT deflatedecompress(BYTE *zdata, LONG nzdata, BYTE *data, LONG *ndata);

INT DeChunk(BYTE *pChunkData, INT nChunkDataSize, BYTE *pNewData);

#endif

