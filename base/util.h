#ifndef UTIL_H
#define UTIL_H
#include <boring.h>
NAMESPACE_BEGIN
class CUtil
{
public:
	static bool isBigEndian();
	static uint16 hton16(uint16 host16);
	static uint32 hton32(uint32 host32);
	static uint64 hton64(uint64 host64);
};
NAMESPACE_END
#endif // UTIL_H
