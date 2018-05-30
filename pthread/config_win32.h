#ifndef _CONFIG_WIN32_H_
#define _CONFIG_WIN32_H_

#if (defined(_WIN32) || defined(_WIN64)) && !defined(__WIN__)
#define __WIN__
#endif

#endif //_CONFIG_WIN32_H_