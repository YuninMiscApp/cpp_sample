#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   
#include <time.h>
#include <unistd.h>
#include <assert.h>   
#include <ctype.h>

#include <stdarg.h>

#include "shell.h"





char *shell_cmd_parse(char *targetOutput,unsigned int targetSize,
	const char *format, ...)
{
	char cmd[1024*2] = {0,};
    FILE *fp = NULL;
    char *pResult = NULL;
    
    assert(targetOutput && targetSize > 0);
    
	memset(cmd,0,sizeof(cmd));
	va_list args;
    va_start(args, format);
    vsnprintf(cmd,sizeof(cmd),format,args);
    va_end(args);
    fp = popen (cmd, "r");
    //printf("cmd: %s\n",cmd);
    assert(fp);
	memset(targetOutput,0,targetSize);
	pResult = fgets (targetOutput, targetSize, fp);
    assert(pResult);
    pclose(fp);
    fp = NULL;
	return targetOutput;
}













