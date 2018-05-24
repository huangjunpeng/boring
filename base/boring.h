#ifndef __TCP_H_
#define __TCP_H_

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <stdarg.h>
#include <exception>

#ifdef WIN32
#define SharedPtr std::tr1::shared_ptr
#else
#define SharedPtr std::shared_ptr  
#endif

#define BSWAP_8(x) ((x) & 0xff)
#define BSWAP_16(x) ((BSWAP_8(x) << 8) | BSWAP_8((x) >> 8))
#define BSWAP_32(x) ((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define BSWAP_64(x) ((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))

typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;
typedef uint16 u_short;

#define _free(c) \
	if (NULL != c)\
{ \
	free((void*)c);   \
	c = NULL;   \
}

#define _delete(c)  \
	if (NULL != c)  \
{   \
	delete c;   \
	c = NULL;   \
}

#define _delete_a(c) \
	if (NULL != c)  \
{   \
	delete[] c;   \
	c = NULL;   \
}

#define NAMESPACE_BEGIN namespace boring \
	{ \
	namespace base \
	{
#define NAMESPACE_END }\
	}

#define NAMESPACE_NET_BEGIN namespace boring \
{ \
	namespace net\
{
#define NAMESPACE_NET_END }\
}

#endif //__TCP_H_