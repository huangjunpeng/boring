#ifndef _IO_BASE_H_
#define _IO_BASE_H_

#define BSWAP_8(x) ((x) & 0xff)
#define BSWAP_16(x) ((BSWAP_8(x) << 8) | BSWAP_8((x) >> 8))
#define BSWAP_32(x) ((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define BSWAP_64(x) ((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))

#if (defined(_WIN32) || defined(_WIN64)) && !defined(__WIN__)
#define __WIN__
#endif

typedef char						io_int8;
typedef short						io_int16;
typedef int							io_int32;
#ifdef _WIN64
	typedef __int64					io_int64;
#else
	#ifdef _LINUX64
		typedef long				io_int64;
	#else
		typedef long long			io_int64;
	#endif
#endif

typedef unsigned char				io_uint8;
typedef unsigned short				io_uint16;
typedef unsigned int				io_uint32;
#ifdef _WIN64
	typedef unsigned __int64		io_uint64;
#else
	#ifdef _LINUX64
		typedef unsigned long		io_uint64;
	#else
		typedef unsigned long long	io_uint64;
	#endif
#endif

#ifdef  _WIN64
typedef io_uint64					io_uintptr_t;
#else
typedef io_uint32					io_uintptr_t;
#endif

#ifdef  _WIN64
typedef io_uint64					io_size_t;
#else
typedef io_uint32					io_size_t;
#endif

#define _free(c) if (NULL != c)	{ free((void*)c); c = NULL;	}

#define _delete(c)	if (NULL != c){	delete c;c = NULL;}

#define _delete_a(c) if (NULL != c)	{delete[] c;c = NULL;}

#endif //__TCP_H_