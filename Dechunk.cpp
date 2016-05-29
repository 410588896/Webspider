/*
 *  dechunk.c
 *  http_chunk_demo
 *
 *  Created by I MAC on 11-9-7.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Dechunk.h"
#include <string.h>
#include <stdlib.h>

#define DC_MIN(x, y) (x) > (y) ? (y) : (x)

static CHAR     *g_buff_out     = NULL;
static CHAR     *g_buff_pt      = NULL;

static INT   g_buff_outlen   = 0;
static INT   g_chunk_len     = 0;
static INT   g_chunk_read    = 0;

static INT      g_is_running    = 0;
static INT      g_is_first      = 1;
static INT      g_is_chunkbegin = 0;

VOID *memstr(VOID *src, INT src_len, CHAR *sub)
{
	if (NULL == src || NULL == sub || src_len < strlen(sub))
	{
		return NULL;
	}

	CHAR *p     = (CHAR*)src;
	CHAR *q     = sub;
	INT indx    = src_len;
	INT sub_len = strlen(sub);

	while (indx > 0)
	{
		INT i = 0;

		while (i < sub_len)
		{
			CHAR cp = *(p + i);
			CHAR cq = *(q + i);

			// case ignore
			if (cq >= 'A' && cq <= 'Z')
			{
				cq |= 0x20;
			}

			if (cp >= 'A' && cp <= 'Z')
			{
				cp |= 0x20;
			}

			if (cq != cp)
			{
				break;
			}

			i++;
		}

		if (i == sub_len)
		{
			return p;
		}

		p++;
		indx--;
	}

	return NULL;
}

INT dechunk_init()
{
	if (g_is_running)
	{
		return DCE_ISRUNNING;
	}

	g_buff_out      = NULL;
	g_buff_pt       = NULL;

	g_buff_outlen   = 0;
	g_chunk_len     = 0;
	g_chunk_read    = 0;

	g_is_running    = 1;
	g_is_first      = 1;
	g_is_chunkbegin = 0;

	return DCE_OK;
}

INT dechunk(VOID *input, INT inlen)
{
	if (!g_is_running)
	{
		return DCE_LOCK;
	}

	if (NULL == input || inlen <= 0)
	{
		return DCE_ARGUMENT;
	}

	VOID *data_start = input;
	INT data_len = inlen;

	if (g_is_first)
	{
		data_start = memstr((VOID*)data_start, data_len, (CHAR*)"\r\n\r\n");
		if (NULL == data_start)
		{
			return DCE_FORMAT;
		}

		data_start = (VOID*)((CHAR*)data_start + 4);
		data_len -= ((CHAR*)data_start - (CHAR*)input);

		g_is_first = 0;
	}

	if (!g_is_chunkbegin)
	{
		CHAR *stmp = (CHAR*)data_start;
		INT itmp = 0;

		sscanf(stmp, "%x", &itmp);
		itmp = (itmp > 0 ? itmp - 2 : itmp);          // exclude the terminate "\r\n"

		data_start = memstr(stmp, data_len, (CHAR*)"\r\n");
		data_start = (VOID*)((CHAR*)data_start + 2);    // strlen("\r\n")

		data_len        -=  ((CHAR*)data_start - stmp);
		g_chunk_len     =   itmp;
		g_buff_outlen   +=  g_chunk_len;
		g_is_chunkbegin =   1;
		g_chunk_read    =   0;

		if (g_chunk_len > 0 && 0 != g_buff_outlen)
		{
			if (NULL == g_buff_out)
			{
				g_buff_out = (CHAR *)malloc(g_buff_outlen);
				g_buff_pt = g_buff_out;
			}
			else
			{
				g_buff_out = (CHAR*)realloc((VOID*)g_buff_out, g_buff_outlen);
			}

			if (NULL == g_buff_out)
			{
				return DCE_MEM;
			}
		}
	}

#define CHUNK_INIT() \
	do \
	{ \
		g_is_chunkbegin = 0; \
		g_chunk_len = 0; \
		g_chunk_read = 0; \
	} while (0)

	if (g_chunk_read < g_chunk_len)
	{
		INT cpsize = DC_MIN(g_chunk_len - g_chunk_read, data_len);
		memcpy(g_buff_pt, data_start, cpsize);

		g_buff_pt       += cpsize;
		g_chunk_read    += cpsize;
		data_len        -= (cpsize + 2);
		data_start      = (VOID*)((CHAR*)data_start + (cpsize + 2));

		if (g_chunk_read >= g_chunk_len)
		{
			CHUNK_INIT();

			if (data_len > 0)
			{
				return dechunk(data_start, data_len);
			}
		}
	}
	else
	{
		CHUNK_INIT();
	}

#undef CHUNK_INIT

	return DCE_OK;
}

INT dechunk_free()
{
	free(g_buff_out);

	g_buff_out      = NULL;
	g_buff_pt       = NULL;

	g_buff_outlen   = 0;
	g_chunk_len     = 0;
	g_chunk_read    = 0;

	g_is_running    = 0;
	g_is_first      = 1;
	g_is_chunkbegin = 0;

	return DCE_OK;
}

INT dechunk_getbuff(VOID **buff, INT *buf_size)
{
	*buff = g_buff_out;
	*buf_size = g_buff_outlen;

	return DCE_OK;
}	
