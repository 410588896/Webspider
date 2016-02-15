#include<stdio.h>
#include<sys/types.h>
#include<regex.h>
#include<memory.h>
#include<stdlib.h>

int main(){

char *bematch = "<td><a href=\"http://camel.hrbeu.edu.cn/infopub/1-3.htm\" target=\"_blank\">宣传部、</a><a href=\"http://www.gongxue.cn/tongzhanbu/index.asp\" target=\"_blank\">统战部</a></td>";
//char *pattern = "(http|https|ftp)\\://([a-zA-Z0-9\\.\\-]+(\\:[a-zA-Z0-9\\.&amp;%\\$\\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|localhost|([a-zA-Z0-9\\-]+\\.)*[a-zA-Z0-9\\-]+\\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))(\\:[0-9]+)*(/($|[a-zA-Z0-9\\.\\,\\?\\'\\\\\\+&amp;%\\$#\\=~_\\-]+))*";
//char *pattern = "(http://|https://)?([^/]*)";
char *pattern = "<(a|form|frame|iframe|layer|input|link|select|hr|bgsound|embed|fig|ilayer|note|overlay|ul|base|area|link|body|table|tr|th|td|head|meta|param|blockquote|del|ins|q|codebase|code|object|archive|sermap|codebase|classid|data)[^>]*(href|action|src|Longdesc|action|urn|cite|applet|object)=\"(https://|/|http://)([^/>'\"]*/?[^>'\"]*)\"?[^>]*>";
char errbuf[1024];
char match[100];
regex_t reg;
int err,nm = 10;
regmatch_t pmatch[nm];

if(regcomp(&reg,pattern,REG_EXTENDED) < 0){
regerror(err,&reg,errbuf,sizeof(errbuf));
printf("err:%s\n",errbuf);
}

err = regexec(&reg,bematch,nm,pmatch,0);

if(err == REG_NOMATCH){
printf("no match\n");
exit(-1);
}else if(err){
regerror(err,&reg,errbuf,sizeof(errbuf));
printf("err:%s\n",errbuf);
exit(-1);
}

for(int i=0;i<10 && pmatch[i].rm_so!=-1;i++){
int len = pmatch[i].rm_eo-pmatch[i].rm_so;
if(len){
memset(match,'\0',sizeof(match));
memcpy(match,bematch+pmatch[i].rm_so,len);
printf("asd%s\n",match);
}
}
return 0;
}