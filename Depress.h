#ifndef DEPRESS_H
#define DEPRESS_H

#include "Queue.h"
#include "Bloomfilter.h"
#include "Regex.h"
#include "Type.h"
#include "Compress.h"

VOID DeChunk(INT sockfd, BloomFilter *Bf, Queue *Urlqueue, Regex *regex, CHAR *responsehead, INT reslen, INT flag);

#endif
