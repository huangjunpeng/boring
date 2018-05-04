#include "util.h"

bool CUtil::isBigEndian()
{
    const uint16 p = 0x0001;
    if (*(int8*)&p)
    {
        return false;
    }
    return true;
}

uint16 CUtil::hton16(uint16 host16)
{
    if (isBigEndian())
        return host16;
    return (0xff & host16) << 8 | (host16 >> 8) & 0xff;
}

uint32 CUtil::hton32(uint32 host32)
{
    if (isBigEndian())
        return host32;
    return (0x000000ff & host32) << 24 |
           (0x0000ff00 & host32) << 8  |
           (0x00ff0000 & host32) >> 8  |
           (0xff000000 & host32) >> 24;
}

uint64 CUtil::hton64(uint64 host64)
{
    if (isBigEndian())
        return host64;
    return (0x00000000000000ff & host64) << 56 |
           (0x000000000000ff00 & host64) << 40 |
           (0x0000000000ff0000 & host64) << 24 |
           (0x00000000ff000000 & host64) << 8  |
           (0x000000ff00000000 & host64) >> 8  |
           (0x0000ff0000000000 & host64) >> 24 |
           (0x00ff000000000000 & host64) >> 40 |
           (0xff00000000000000 & host64) >> 56;
}
