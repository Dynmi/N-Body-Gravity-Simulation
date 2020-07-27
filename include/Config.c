#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"


void trim(char * strIn, char * strOut)
{
    char *start, *end, *tmp;
    tmp = strIn;

    while(*tmp == ' ')
    {
        ++tmp;
    }

    start = tmp;

    tmp = strIn + strlen(strIn) - 1;

    while(*tmp== ' ')
    {
        --tmp;
    }

    end = tmp;
    strIn = start;

    while(strIn<=end)
    {
        *strOut++ = *strIn++;
    }

    *strOut = '\0';
}

void getValue(char * keyAndValue, char * key, char * value)
{

    char *p = keyAndValue;

    p = strstr(keyAndValue, key);//找到key的位置，指针
    if(p == NULL){
        printf("没有key %s\n", key);
        return ;
    }

    p += strlen(key);//指针后移到key后面
    trim(p, value);

    p = strstr(value, "=");//找等号的位置
    if(p == NULL){
        printf("没有找到=\n");
        return;
    }
    p+= strlen("=");//指针后移到等号后面
    trim(p, value);//删除字符串前后的空格

}

void readCFG(const char *cfg_file/*in*/, const char *key/*in*/, const char **value/*out*/)
{
    FILE *pf;
    char line[1024] = {0}, vtemp[1024] = {0};

    pf = fopen(cfg_file, "r");
    if(pf == NULL) 
    {
        perror("Can't open Config file!\n");
        return;
    }

    while(1){
        if( feof(pf) )
        {
            break;
        }

        fgets(line, 1024, pf);
        getValue(line, key, vtemp);
        if(strlen(vtemp) != 0)
            break;
    }

    if(strlen(vtemp) != 0){
        *value = (char *)malloc(sizeof(char) * strlen(vtemp) + 1);
        strcpy(*value, vtemp);
        printf("%s\n",*value);
    }    
    else
        value = NULL;

    fclose(pf);
}