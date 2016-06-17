#ifndef SSL_H
#define SSL_H

#include <openssl/ssl.h> 
#include <openssl/err.h> 

#include "Type.h"
#include "Mydb.h"
#include "Dns.h"
#include "Match.h"
#include "Socket.h"
#include "Queue.h"
#include "Threadpool.h"
#include "Bloomfilter.h"
#include "Regex.h"
#include "Ssl.h"
#include "Depress.h"

VOID ShowCerts(SSL *ssl);

VOID https_socket(CHAR *ip, CHAR *url, INT urllen, Automachine *match, BloomFilter *Bf, Queue *Urlqueue, Regex *regex);

UINT SSL_Requestsend(SSL *ssl, INT sockfd, CHAR *url, UINT urllen);

UINT SSL_Responserecv(SSL *ssl, INT sockfd, Automachine *match, BloomFilter *Bf, Queue *Urlqueue, Regex *regex);

#endif

