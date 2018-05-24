#include "transformEncode.h"
#include <Windows.h>


bool UnicodeToANSI(const wchar_t* pwcSource, std::string& strDestination)
{
	if (!pwcSource)
	{
		return false;
	}

	int dwNum = WideCharToMultiByte(CP_OEMCP,NULL,pwcSource,-1,NULL,0,NULL,FALSE);
	if (dwNum <= 0)
	{
		return false;
	}

	char *psText;
	psText = new char[dwNum+1];
	if (!psText)
	{
		return false;
	}

	int nRe = WideCharToMultiByte (CP_OEMCP,NULL,pwcSource,-1,psText,dwNum,NULL,FALSE);
	if (dwNum != nRe)
	{
		delete []psText;
		psText = NULL;
		return false;
	}

	psText[dwNum] = '\0';
	strDestination = psText;
	delete []psText;
	psText = NULL;

	return true;
}

bool UnicodeToUTF8(const wchar_t* pwcSource, std::string& strDestination)
{
	if (!pwcSource)
	{
		return false;
	}

	int dwNum_ = WideCharToMultiByte(CP_UTF8,NULL,pwcSource,-1,NULL,0,NULL,NULL);
	if (dwNum_ <= 0)
	{
		return false;
	}

	char* psText_;
	psText_ = (char *)malloc((dwNum_+1)*sizeof(char));
	if (!psText_)
	{
		return false;
	}

	int nRe = WideCharToMultiByte (CP_UTF8,NULL,pwcSource,-1,psText_,dwNum_,NULL,NULL);
	if (nRe != dwNum_)
	{
		free(psText_);
		psText_ = NULL;
		return false;
	}

	psText_[dwNum_] = '\0';
	strDestination = psText_;
	free(psText_);
	psText_ = NULL;

	return true;
}

bool ANSIToUnicode(const char* pcSource, std::wstring& wstrDestination)
{
	if (!pcSource)
	{
		return false;
	}

	int dwNum = MultiByteToWideChar (CP_ACP, 0, pcSource, -1, NULL, 0);
	if (dwNum <= 0)
	{
		return false;
	}
	wchar_t *pwText;
	pwText = new wchar_t[dwNum+1];
	if (!pwText)
	{
		return false;
	}

	int nRe = MultiByteToWideChar (CP_ACP, 0, pcSource, -1, pwText, dwNum);
	if (nRe != dwNum)
	{
		delete []pwText;
		pwText = NULL;
		return false;
	}
	pwText[dwNum] = '\0';
	wstrDestination = pwText;
	delete []pwText;
	pwText = NULL;

	return true;
}

bool UTF8ToUnicode(const char* pcSource, std::wstring& wstrDestination)
{
	if (!pcSource)
	{
		return false;
	}

	int dwNum_ = MultiByteToWideChar (CP_UTF8, 0, pcSource, -1, NULL, 0);  
	if (dwNum_ <= 0)
	{
		return false;
	}

	wchar_t *pwText_;
	pwText_ = new wchar_t[dwNum_+1];
	if (!pwText_)
	{
		return false;
	}

	int nRe = MultiByteToWideChar (CP_UTF8, 0, pcSource, -1, pwText_, dwNum_);
	if (nRe != dwNum_)
	{
		delete []pwText_;
		pwText_ = NULL;
		return false;
	}

	pwText_[dwNum_] = '\0';
	wstrDestination = pwText_;
	delete []pwText_;
	pwText_ = NULL;

	return true;
}

bool ANSIToUTF8(const char* pcSource, std::string& strDestination)
{
	std::wstring wstrTmp;
	bool bRe = ANSIToUnicode(pcSource, wstrTmp);
	if (!bRe)
	{
		return bRe;
	}

	bRe = UnicodeToUTF8(wstrTmp.c_str(), strDestination);

	return bRe;
}

bool UTF8ToANSI(const char* pcSource, std::string& strDestination)
{
	std::wstring wstrTmp;
	bool bRe = UTF8ToUnicode(pcSource, wstrTmp);
	if (!bRe)
	{
		return bRe;
	}

	bRe = UnicodeToANSI(wstrTmp.c_str(), strDestination);

	return bRe;
}

std::string  ANSIToUTF8(const char* pcSource)
{
	std::string strRet;
	::ANSIToUTF8(pcSource, strRet);
	return strRet;
}

std::string UTF8ToANSI(const char* pcSource)
{
	std::string strDestination;
	UTF8ToANSI(pcSource, strDestination);
	return strDestination;
}