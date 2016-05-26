#include "Regex.h"

//#define DEBUG

/******************************
 * Name		:Construct
 * Function :Regex compile
 * Args		:
 * Return	:
 * PS		:
******************************/
INT Regex::Construct()
{
//	const CHAR *pattern = "<(a|form|frame|iframe|layer|input|link|select|hr|bgsound|embed|fig|ilayer|note|overlay|ul|base|area|link|body|table|tr|th|td|head|meta|param|blockquote|del|ins|q|codebase|code|object|archive|sermap|codebase|classid|data)[^>]*(href|action|src|Longdesc|action|urn|cite|applet|object)=\"(https://|/|http://)([^/>'\"]*/?[^>'\"]*)\"?[^>]*>";
//	const CHAR *pattern = "(http|https|ftp)\\://([a-zA-Z0-9\\.\\-]+(\\:[a-zA-Z0-9\\.&amp;%\\$\\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|localhost|([a-zA-Z0-9\\-]+\\.)*[a-zA-Z0-9\\-]+\\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))(\\:[0-9]+)*(/($|[a-zA-Z0-9\\.\\,\\?\\'\\\\\\+&amp;%\\$#\\=~_\\-]+))*$";
	const CHAR *pattern = "href=\"[a-zA-z]+://[^\\s^\"]*\"";
	err = regcomp(&reg, pattern, REG_EXTENDED);
	 
	if(err < 0)
	{
		regerror(err, &reg, errbuf, sizeof(errbuf));
		perror(errbuf);
		return err;
	}

	return err;
}

/******************************
 * Name		:Find
 * Function :Regex find
 * Args		:
 * Return	:
 * PS		:
******************************/
INT Regex::Find(CHAR *bematch, CHAR *match, INT *matchlen)
{
	err = regexec(&reg, bematch, NM, pmatch, 0);
	*matchlen = 0;
	if(err == REG_NOMATCH)
	{
		return -1;
	}
	else if(err)
	{
		memset(errbuf, 0, 1024);
		regerror(err, &reg, errbuf, sizeof(errbuf));
		perror(errbuf);
		return -1;
	}

	for(INT i = 0; i < NM && pmatch[i].rm_so != -1; i++)
	{
		INT len = pmatch[i].rm_eo - pmatch[i].rm_so;
		if(len)
		{
				memcpy(match, bematch + pmatch[i].rm_so, len);
				*matchlen = len;
				return pmatch[i].rm_so + len;
		}
	}
	return 1;	
}

/******************************
 * Name		:Destroy
 * Function :Regex destroy
 * Args		:
 * Return	:
 * PS		:
******************************/
VOID Regex::Destroy()
{
	regfree(&reg);
}

