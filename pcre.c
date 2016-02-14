#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
 
int main(int argc,char **argv)
{
 char *pText = "https://64.81.85.161/site/file.php?";
 char *pPattern = "^(http|https|ftp)\\://([a-zA-Z0-9\\.\\-]+(\\:[a-zA-Z0-9\\.&amp;%\\$\\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|localhost|([a-zA-Z0-9\\-]+\\.)*[a-zA-Z0-9\\-]+\\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))(\\:[0-9]+)*(/($|[a-zA-Z0-9\\.\\,\\?\\'\\\\\\+&amp;%\\$#\\=~_\\-]+))*$";
    const char * pErrMsg = NULL;
    pcre * pPcre = NULL;
    int nOffset = -1; 
                                      
    if (NULL == (pPcre = pcre_compile(pPattern, 0, &pErrMsg, &nOffset, NULL)))
    {   
        printf("ErrMsg=%s, Offset=%d\n", pErrMsg, nOffset);
        return 1;
    }   
    else
    {   
        if (pcre_exec(pPcre, NULL, pText, strlen(pText), 0, 0, NULL, 0) < 0)
        {   
            printf("%s doesn't match %s\n", pText, pPattern);
        }   
        else
        {   
            printf("%s matches %s\n", pText, pPattern);
        }
    }
return 0;
}

