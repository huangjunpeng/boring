#ifndef _TRANSFORMENCODE_
#define _TRANSFORMENCODE_
#include <string>
int UTF82GBKConvert( const char* a_szSrc, char* a_szDest, size_t &a_nDestSize );
int GBK2UTF8Convert( const char* a_szSrc, size_t a_nSrcSize, char* a_szDest, size_t &a_nDestSize );
bool ANSIToUTF8(const char* pcSource, std::string& strDestination);
bool UTF8ToANSI(const char* pcSource, std::string& strDestination);
#endif

