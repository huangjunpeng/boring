#include "inifile.h"
#include "string.h"
#include "stringutil.h"

class Config
{
public:
	friend class CIniFile;
public:
	Config(const std::string& _MainKey);
	~Config();

	std::string get(const std::string& _key);
	void set(const std::string& _key, const std::string& _value);
	void del(const std::string& _key);
	int write(FILE *pFile);
private:
	std::string m_strMainKey;
	std::map<std::string, std::string> m_mconf;
};

Config::Config(const std::string &_MainKey) : m_strMainKey(_MainKey)
{

}

Config::~Config()
{
	m_mconf.clear();
}

std::string Config::get(const std::string& _key)
{
	std::map<std::string, std::string>::iterator it = m_mconf.find(_key);
	if (it != m_mconf.end())
	{
		return (*it).second;
	}
	return "";
}

void Config::set(const std::string &_key, const std::string &_value)
{
	if (!_key.empty())
		m_mconf[_key] = _value;
}

void Config::del(const std::string &_key)
{
	std::map<std::string, std::string>::iterator it = m_mconf.find(_key);
	if (it != m_mconf.end())
	{
		m_mconf.erase(it);
	}
}

int Config::write(FILE *pFile)
{
	if (pFile == NULL)
	{
		return -1;
	}
	fprintf(pFile, "[%s]\n", m_strMainKey.c_str());
	for (std::map<std::string, std::string>::iterator it = m_mconf.begin(); it != m_mconf.end(); ++it)
	{
		fprintf(pFile, "%s=%s\n", (*it).first.c_str(), (*it).second.c_str());
	}
	fprintf(pFile, "\n");
	return 0;
}

CIniFile::CIniFile()
    : m_bIsChange(false), m_strFileName("")
{
}

CIniFile::CIniFile(const char *pFileName)
    : m_bIsChange(false), m_strFileName(pFileName)
{

}

CIniFile::CIniFile(const std::string &strFileName)
    : m_bIsChange(false), m_strFileName(strFileName)
{

}

CIniFile::~CIniFile()
{
    if (m_bIsChange)
    {
        save();
    }
    m_mSection.clear();
}

int CIniFile::init()
{
    return load();
}

int CIniFile::init(const char *pFileName)
{
    m_strFileName = pFileName;
    return load();
}

int CIniFile::init(const std::string &strFileName)
{
    m_strFileName = strFileName;
    return load();
}

int CIniFile::save()
{
    return write();
}

void CIniFile::set(const std::string& _section, const std::string& _key, const std::string& _value)
{
    if (!_section.empty() && !_key.empty())
    {
        std::map<std::string, ShardConfig>::iterator it = m_mSection.find(_section);
        if (it == m_mSection.end())
        {
            m_mSection[_section] = ShardConfig(new Config(_section));
        }
        ShardConfig& conf = m_mSection[_section];
        conf->set(_key, _value);
        m_bIsChange = true;
    }
}

std::string CIniFile::get(const std::string& _section, const std::string& _key, const std::string& _defvalue)
{
    std::string _value = _defvalue;
    if (m_mSection.count(_section))
    {
         ShardConfig& conf = m_mSection[_section];
         _value = conf->get(_key);
    }
    return _value;
}

void CIniFile::del(const std::string& _section, const std::string& _key)
{
    if (m_mSection.count(_section))
    {
         ShardConfig& conf = m_mSection[_section];
         conf->del(_key);
		 m_bIsChange = true;
    }
}

void CIniFile::dump()
{
    std::map<std::string, ShardConfig>::iterator it = m_mSection.begin();
    while (it != m_mSection.end()) {
        printf("[%s]\n", (*it).first.c_str());
        ShardConfig& conf = (*it).second;
        std::map<std::string, std::string>::iterator ls = conf->m_mconf.begin();
        while (ls != conf->m_mconf.end()) {
            printf("%s=%s\n", (*ls).first.c_str(), (*ls).second.c_str());
            ++ls;
        }
        ++it;
    }
}

int CIniFile::load()
{
    FILE *pFile = NULL;
    if (NULL == (pFile = fopen(m_strFileName.c_str(), "r")))
    {
        return -1;
    }
    std::string _section;
    std::string _key;
    std::string _val;
    std::string _line;
    char buf[BUFF_LEN] = {0};
    while (!feof(pFile)) {
        memset(buf, 0, BUFF_LEN);
        if (NULL == fgets(buf, BUFF_LEN, pFile)) {
            break;
        }
        _line = buf;
        StringUtil::trim(_line);
        if (_line[0] == '#') {
            continue;
        }

        size_t nPos;
        if (_line[0] == '[') {
            if (std::string::npos != (nPos = _line.find_first_of("]"))) {
                _section = _line.substr(1, nPos - 1);
            }
        }
        else if (std::string::npos != (nPos = _line.find_first_of("="))) {
             std::vector<std::string> temp = StringUtil::str2vector(StringUtil::trim(_line), '=');
             _key = StringUtil::trim(temp[0]);
             _val = StringUtil::trim(temp[1]);
             if (!_section.empty()) {
				 std::map<std::string, ShardConfig>::iterator it = m_mSection.find(_section);
				 if (it == m_mSection.end())
				 {
					 m_mSection[_section] = ShardConfig(new Config(_section));
				 }
				 ShardConfig& conf = m_mSection[_section];
				 conf->set(_key, _val);
             }
        }
    }
    fclose(pFile);
    pFile = NULL;
    return 0;
}

int CIniFile::write()
{
    FILE *pFile = NULL;
    if (NULL == (pFile = fopen(m_strFileName.c_str(), "w")))
    {
        return -1;
    }
    for (std::map<std::string, ShardConfig>::iterator it = m_mSection.begin(); it != m_mSection.end(); ++it)
    {
        ShardConfig& conf = (*it).second;
        conf->write(pFile);
    }
    m_bIsChange = false;
    return fclose(pFile);
}
