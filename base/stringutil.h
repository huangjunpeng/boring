#ifndef STRINGUTIL_H
#define STRINGUTIL_H
#include <boring.h>
class StringUtil
{
public:
    static std::string trim(std::string& str, const std::string drop = " \t\n\r");
    static std::string ltrim(std::string& str, const std::string drop = " \t\n\r");
    static std::string rtrim(std::string& str, const std::string drop = " \t\n\r");
    static std::vector<std::string> str2vector(const std::string& str, const char delim);
    static std::list<std::string> str2list(const std::string& str, const char delim);
    static std::map<std::string, std::string> str2map(const std::string& str, const char delim, const char _kv = '=');
    static bool format(std::string& szDes, const char* pFormat, ...);
    static int c2i(char c);
    static char c2x(char c);
    static int hex2dec(char *hex);
    static std::string str2hex(const std::string& str);
    static std::string hex2str(const std::string& str);
    static int isdigit(int c);
    static int isalpha(int c);
    static int islower(int c);
    static int isupper(int c);
    static int isalnum(int c);
    static int tolower(int c);
    static int toupper(int c);
    static std::string stolower(std::string& str);
    static std::string stoupper(std::string& str);
};


#endif // STRINGUTIL_H
