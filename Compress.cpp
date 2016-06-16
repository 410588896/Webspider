#include "Compress.h"

/* Compress gzip data */
/* data 原数据 ndata 原数据长度 zdata 压缩后数据 nzdata 压缩后长度 */
INT gzcompress(BYTE *data, LONG ndata, BYTE *zdata, LONG *nzdata)
{
	z_stream c_stream;
	INT err = 0;

	if(data && ndata > 0) {
		c_stream.zalloc = NULL;
		c_stream.zfree = NULL;
		c_stream.opaque = NULL;
		//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
		if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
					MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in  = data;
		c_stream.avail_in  = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out  = *nzdata;
		while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
			if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if(c_stream.avail_in != 0) return c_stream.avail_in;
		for(;;) {
			if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if(err != Z_OK) return -1;
		}
		if(deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
INT gzdecompress(BYTE *zdata, LONG nzdata, BYTE *data, LONG *ndata)
{
	INT err = 0;
	z_stream d_stream = {0}; /* decompression stream */
	static CHAR dummy_head[2] = {
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in  = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if(err != Z_OK) {
			if(err == Z_DATA_ERROR) {
				d_stream.next_in = (Bytef*) dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					return -1;
				}
			} else return -1;
		}
	}
	if(inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}

/* Uncompress deflate data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
INT deflatedecompress(BYTE *zdata, LONG nzdata, BYTE *data, LONG *ndata)
{
	INT err = 0;
	z_stream d_stream = {0}; /* decompression stream */
	static CHAR dummy_head[2] = {
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in  = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if(inflateInit(&d_stream) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if(err != Z_OK) {
			if(err == Z_DATA_ERROR) {
				d_stream.next_in = (Bytef*) dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					return -1;
				}
			} else return -1;
		}
	}
	if(inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
} 

VOID DeChunk(BYTE *pChunkData, INT nChunkDataSize, BYTE *pNewData)		//组包
{
	CHAR *pStart = (CHAR *)pChunkData;		//chunked data
	CHAR *pTemp;			
	INT chunkedINTLength;				//chunked 数据包的大小 - 十进制
	CHAR chunkedStrLength[10];			//chunked 数据包的大小 - 十六进制字符型
	while (true)
	{
		chunkedINTLength = 0;
		memset(chunkedStrLength, 0, sizeof(chunkedStrLength));
		pTemp = strstr(pStart, "\r\n");
		INT chunkedByteLength = pTemp - pStart;	//chunked 大小 存放的字节长度
		for (INT i = 0; i < chunkedByteLength; i++)
			chunkedStrLength[i] = pStart[i];
		pStart = pTemp + 2;									//指针移位，跳过\r\n
		sscanf(chunkedStrLength, "%x", &chunkedINTLength);
		if (0 == chunkedINTLength)
			return;
		else
		{
			memcpy(pNewData, pStart, chunkedINTLength);
			pStart = pStart + chunkedINTLength + 2;
			pNewData = pNewData + chunkedINTLength;
		}
	}
}

