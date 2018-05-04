#include "stringutil.h"
#include <string.h>
#include <algorithm>
#include <stdarg.h>
using namespace boring::base;
std::string StringUtil::trim(std::string &str, const std::string drop)
{
    str.erase(str.find_last_not_of(drop) + 1);
    return str.erase(0, str.find_first_not_of(drop));
}

std::string StringUtil::ltrim(std::string &str, const std::string drop)
{
    return str.erase(0, str.find_first_not_of(drop));
}

std::string StringUtil::rtrim(std::string &str, const std::string drop)
{
    return str.erase(str.find_last_not_of(drop) + 1);
}

std::vector<std::string> StringUtil::str2vector(const std::string &str, const char delim)
{
    std::vector<std::string> result;
    size_t      start    = 0;
    size_t      pos      = str.find(delim);
    while (pos != std::string::npos) {
        if (pos > start) {
            result.push_back(str.substr(start, pos - start));
        }
        start = pos + 1;
        pos = str.find(delim, start);
    }
    if (start < str.length())
    {
        result.push_back(str.substr(start));
    }
    return result;
}

std::list<std::string> StringUtil::str2list(const std::string &str, const char delim)
{
    char *pCur = NULL;
    char *pVal = NULL;
    char *pSrc = NULL;
    std::list<std::string> _list;
    pSrc = new char[str.size() + 2];
    if (pSrc != NULL)
    {
        memset(pSrc, 0, str.size() + 2);
        memcpy(pSrc, str.c_str(), str.size());
        pCur = pSrc;
        pVal = pCur;
        while (*pCur != '\0') {
            if (*pCur == delim)
            {
                *pCur = '\0';
                ++pCur;
                if (strlen(pVal) > 0)
                {
                    _list.push_back(pVal);
                }
                pVal = pCur;
            }
            else
            {
                ++pCur;
            }
        }
        if (pVal != pCur && strlen(pVal) > 0)
        {
            _list.push_back(pVal);
        }
        if (pSrc)
        {
            delete[] pSrc;
            pSrc = NULL;
        }
    }
    return _list;
}

std::map<std::string, std::string> StringUtil::str2map(const std::string &str, const char delim, const char split)
{
    std::string szSrc = str;
    StringUtil::trim(szSrc);
    std::vector<std::string> result = StringUtil::str2vector(szSrc, delim);
    std::map<std::string, std::string> _map;
    std::vector<std::string> temp;
    for (std::vector<std::string>::iterator it = result.begin(); it != result.end(); ++it)
    {
        temp = StringUtil::str2vector(StringUtil::trim(*it), split);
        _map[StringUtil::trim(temp[0])] = StringUtil::trim(temp[1]);
        temp.clear();
    }
    return _map;
}

bool StringUtil::format(std::string& szDes, const char* pFormat, ...)
{
    va_list args;
    int len = -1;
    va_start(args, pFormat);
    len = _vscprintf(pFormat, args) + 1;
    if (len <= 1)
    {
        return false;
    }
    char *buf = NULL;
    buf = (char*)malloc(len * sizeof(char));
    if (NULL == buf)
    {
        return false;
    }
    memset(buf, 0, len);
    vsprintf_s(buf, len, pFormat, args);
    va_end(args);
    szDes = buf;
    _free(buf);
    return true;
}

int StringUtil::c2i(char c)
{
    //判断是否是十进制整数
    if (StringUtil::isdigit(c))
    {
         // 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2
        return c - 48;//0-9 ascii 48-57
    }

    //判断是否是 A~F,a~f之间的
    if (c < 'A' || (c > 'F' && c < 'a') || c > 'f')
    {
        return -1;
    }

    if (StringUtil::isalpha(c))
    {
        //16进制A~F,a~f对应的十进制整数10~15
        //16进制A~F,a~f对应的ascii 65~70 97~102
        // 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10
        // 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10
        return StringUtil::isupper(c) ? c - 55 : c - 87;
        //return StringUtil::islower(c) ? c - 87 : c - 55;
    }
    return -1;
}

char StringUtil::c2x(char c)
{
    if (c >= 0 && c <= 9)
    {
        return c + 48;
    }
    else if (c >= 10 && c <= 15)
    {
        return c + 55;
    }
	return c;
}

int StringUtil::hex2dec(char *hex)
{
    int len = 0, num = 0, temp = 0, bits = 0, i = 0;
    const char *p = hex;
    if (*p == '0' && (*(p + 1) == 'x' || *(p + 1) =='X'))
    {
        p += 2;
    }

    len = strlen(p);

    for (; i < len; i++, temp = 0)
    {
        temp = c2i(*(p + i));
        bits = (len - i - 1) * 4;
        temp <<= bits;
        num = num | temp;
    }
    return num;
}

std::string StringUtil::str2hex(const std::string &str)
{
    std::string szRet;
    std::string::const_iterator it = str.begin();
    while (it != str.end()) {
        char c = *it;
        szRet.push_back(c2x((c & 0xf0) >> 4));
        szRet.push_back(c2x((c & 0x0f)));
        if (++it != str.end())
        {
            szRet.push_back(':');
        }
    }
    return szRet;
}

std::string StringUtil::hex2str(const std::string &str)
{
    std::string szRet;
    std::string::const_iterator it = str.begin();
    while (it != str.end()) {
        if (*it == ':'){
            ++it;
            continue;
        }
        char hig = *it;
        char low = *(it + 1);
        hig = (char)c2i(hig);
        hig = (hig << 4) | (char)c2i(low);
        szRet.push_back(hig);
        it += 2;
    }
    return szRet;
}



int StringUtil::isdigit(int c)
{
    if (c >= 48 && c <= 57)
    {
        return 1;
    }
    return 0;
}


int StringUtil::isalpha(int c)
{
    if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))
    {
        return 1;
    }
    return 0;
}


int StringUtil::islower(int c)
{
    if (c >= 97 && c <= 122)
    {
        return 1;
    }
    return 0;
}


int StringUtil::isupper(int c)
{
    if (c >= 65 && c <= 90)
    {
        return 1;
    }
    return 0;
}


int StringUtil::isalnum(int c)
{
    if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122))
    {
        return 1;
    }
    return 0;
}


int StringUtil::tolower(int c)
{
    if (c >= 65 && c <= 90)
    {
        return c + 32;
    }
    return c;
}


int StringUtil::toupper(int c)
{
    if (c >= 97 && c <= 122)
    {
        return c - 32;
    }
    return c;
}

std::string StringUtil::stolower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), StringUtil::tolower);
    return str;
}

std::string StringUtil::stoupper(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), StringUtil::toupper);
    return str;
}

