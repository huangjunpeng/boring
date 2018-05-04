#ifndef INIFILE_H
#define INIFILE_H
#include <boring.h>
class Config;
NAMESPACE_BEGIN
typedef SharedPtr<Config> ShardConfig;
#define BUFF_LEN 1024
class CIniFile
{
public:
    CIniFile();
    CIniFile(const char *pFileName);
    CIniFile(const std::string& strFileName);
    ~CIniFile();

    int init();
    int init(const char *pFileName);
    int init(const std::string& strFileName);

    int save();
    void set(const std::string& _section, const std::string& _key = "", const std::string& _value = "");
    std::string get(const std::string& _section, const std::string& _key, const std::string& _defvalue = "");
    void del(const std::string& _section, const std::string& _key);

    void dump();
private:
    int load();
    int write();
private:
    bool m_bIsChange;
    std::string m_strFileName;
    std::map<std::string, ShardConfig> m_mSection;
};
NAMESPACE_END
#endif // INIFILE_H
