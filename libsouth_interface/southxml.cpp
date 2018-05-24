#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <Windows.h>
using namespace std;

#include "southxml.h"
#include "south.h"
#include "transformEncode.h"

//转成UTF8编码
std::string toUtf8(const char * pt)
{
	static string str = "";
	ANSIToUTF8(pt, str);
	return str;
}


int target_make_xml(struct target *pt, TiXmlNode *pTiXmlNode)
{
	if (NULL == pt) {
		return 0;
	}

	if (NULL == pTiXmlNode) {
		return -1;
	}

	//添加新节点
	TiXmlElement *pNewElemt = new TiXmlElement(toUtf8(XML_NODE_PARAM).c_str());
	pTiXmlNode->LinkEndChild(pNewElemt);

	//设置名称属性
	if (NULL != pt->_label) {
		pNewElemt->SetAttribute(toUtf8(XML_PROP_NAME).c_str(), toUtf8(pt->_label).c_str());
	}

	//设置值
	if (NULL != pt->_value) {
		TiXmlText *pNodeText = new TiXmlText(toUtf8(pt->_value).c_str());
		pNewElemt->LinkEndChild(pNodeText);
	}

	//设置单位属性
	if (NULL != pt->_unit && strcmp(pt->_unit, "") != 0) {
		pNewElemt->SetAttribute(toUtf8(XML_PROP_UNIT).c_str(), toUtf8(pt->_unit).c_str());
	}

	//设置类型
	pNewElemt->SetAttribute(toUtf8(XML_PROP_TYPE).c_str(), pt->_type);

	//返回
	return 0;
}


int monitor_make_xml(struct monitor *pm, TiXmlNode *pTiXmlNode)
{
	if (NULL == pm) {
		return 0;
	}

	if (NULL == pTiXmlNode) {
		return -1;
	}

	//创建新节点
	TiXmlElement *pNewElemt = new TiXmlElement(toUtf8(XML_NODE_CTL_CMD).c_str());
	pTiXmlNode->LinkEndChild(pNewElemt);

	//添加属性
	if (NULL != pm->_label) {
		pNewElemt->SetAttribute(toUtf8(XML_PROP_SOFT_NAME).c_str(), toUtf8(pm->_label).c_str());
	}

	//添加属性
	if (NULL != pm->_pd->_label) {
		pNewElemt->SetAttribute(toUtf8("类型名称").c_str(), toUtf8(pm->_pd->_label).c_str());
	}

	//添加指标xml
	struct target *pt = pm->_target;
	while (NULL != pt) {
		target_make_xml(pt, pNewElemt);
		pt = pt->_next;
	}
	
	//返回
	return 0;
}

std::string monitor_to_string(struct monitor *pm)
{
	if (NULL == pm) {
		return 0;
	}

	//创建xml文档对象
	TiXmlDocument* doc = new TiXmlDocument();
	if (NULL == doc) {
		printf("new TiXmlDocument failed.");
		return 0;
	}

	//添加头部描述
	TiXmlDeclaration* decl = NULL;
	decl = new TiXmlDeclaration("1.0","UTF-8","");
	doc->LinkEndChild(decl);

	//添加监控服务节点
	TiXmlElement *pServiceElemt = new TiXmlElement(toUtf8(XML_NODE_MNTR_SERIVICE).c_str());
	doc->LinkEndChild(pServiceElemt);

	//添加监控信息
	TiXmlElement *pInfoElemt = new TiXmlElement(toUtf8(XML_NODE_MNTR_INFO).c_str());
	pServiceElemt->LinkEndChild(pInfoElemt);
	
	//添加监测点
	monitor_make_xml(pm, pInfoElemt);
		
	//输出文件
	FILE *pFile = NULL;
	pFile = fopen("libsouth.xml", "w");
	if (pFile) {
		doc->Print(pFile);
		fclose(pFile);
		pFile = NULL;
	}

	//转成字符串
	TiXmlString strReturn;
	doc->PrintS(strReturn);
	
	//销毁对象
	_delete(doc);

	//打印到屏幕
	printf("\n%s\n", strReturn.c_str());

	return strReturn.c_str();
}


int upload(struct device *pd)
{
	if (NULL == pd) {
		printf("传入空指针...\n");
		return -1;
	}
	static fun pFun	= NULL;
	if (NULL == pFun) {
		std::string strDllPath	= "UploadStatus.dll";
		std::string strFunName	= "PassiveInterface";
		HMODULE hd		= NULL;
		hd = ::LoadLibrary(strDllPath.c_str());
		if (NULL == hd) {
			printf("LoadLibrary Failed! dll = %s, error = %d", strDllPath.c_str(), GetLastError());
			return -1;
		}
		pFun = (fun)GetProcAddress(hd, strFunName.c_str());
		if (NULL == pFun) {
			printf("GetProcAddress Failed! fun = %s, error = %d", strFunName.c_str(), GetLastError());
			return -1;
		}
	}
	
	//添加监测点
	struct monitor *pm = pd->_monitor;
	while (NULL != pm) {
		//make xml
		std::string szXml = monitor_to_string(pm);

		//upload ts
		std::string szRet = pFun(pd->_ip, szXml);
		if (!szRet.empty()) {
			printf("ip = %s, ret = %s\n", pd->_ip, szRet.c_str());
		} else {
			printf("success\n");
		}

		//continue
		pm = pm->_next;
	}
	return 0;
}