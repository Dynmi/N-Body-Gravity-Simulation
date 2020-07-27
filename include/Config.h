#ifndef __Config_h
#define __Config_h

void trim(char *strIn, char *strOut);

void getValue(char * keyAndValue, char * key, char * value);

void readCFG(const char *filename/*in*/, const char *key/*in*/, const char **value/*out*/);

#endif