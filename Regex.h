#ifndef REGEX_H
#define REGEX_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "Type.h"

#define NM 10

class Regex
{		
	private:
		INT err;
		
		regex_t reg;
		
		regmatch_t pmatch[NM];
	
		CHAR errbuf[1024];

	public:
		INT Construct();
		
		INT Find(CHAR *bematch, CHAR *match, INT *matchlen);

		VOID Destroy();		
};

#endif

