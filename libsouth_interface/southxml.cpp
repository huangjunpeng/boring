#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <Windows.h>
using namespace std;

#include "southxml.h"
#include "south.h"
#include "transformEncode.h"

//ת��UTF8����
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

	//����½ڵ�
	TiXmlElement *pNewElemt = new TiXmlElement(toUtf8(XML_NODE_PARAM).c_str());
	pTiXmlNode->LinkEndChild(pNewElemt);

	//������������
	if (NULL != pt->_label) {
		pNewElemt->SetAttribute(toUtf8(XML_PROP_NAME).c_str(), toUtf8(pt->_label).c_str());
	}

	//����ֵ
	if (NULL != pt->_value) {
		TiXmlText *pNodeText = new TiXmlText(toUtf8(pt->_value).c_str());
		pNewElemt->LinkEndChild(pNodeText);
	}

	//���õ�λ����
	if (NULL != pt->_unit && strcmp(pt->_unit, "") != 0) {
		pNewElemt->SetAttribute(toUtf8(XML_PROP_UNIT).c_str(), toUtf8(pt->_unit).c_str());
	}

	//��������
	pNewElemt->SetAttribute(toUtf8(XML_PROP_TYPE).c_str(), pt->_type);

	//����
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

	//�����½ڵ�
	TiXmlElement *pNewElemt = new TiXmlElement(toUtf8(XML_NODE_CTL_CMD).c_str());
	pTiXmlNode->LinkEndChild(pNewElemt);

	//�������
	if (NULL != pm->_label) {
		pNewElemt->SetAttribute(toUtf8(XML_PROP_SOFT_NAME).c_str(), toUtf8(pm->_label).c_str());
	}

	//�������
	if (NULL != pm->_pd->_label) {
		pNewElemt->SetAttribute(toUtf8("��������").c_str(), toUtf8(pm->_pd->_label).c_str());
	}

	//���ָ��xml
	struct target *pt = pm->_target;
	while (NULL != pt) {
		target_make_xml(pt, pNewElemt);
		pt = pt->_next;
	}
	
	//����
	return 0;
}

std::string monitor_to_string(struct monitor *pm)
{
	if (NULL == pm) {
		return 0;
	}

	//����xml�ĵ�����
	TiXmlDocument* doc = new TiXmlDocument();
	if (NULL == doc) {
		printf("new TiXmlDocument failed.");
		return 0;
	}

	//���ͷ������
	TiXmlDeclaration* decl = NULL;
	decl = new TiXmlDeclaration("1.0","UTF-8","");
	doc->LinkEndChild(decl);

	//��Ӽ�ط���ڵ�
	TiXmlElement *pServiceElemt = new TiXmlElement(toUtf8(XML_NODE_MNTR_SERIVICE).c_str());
	doc->LinkEndChild(pServiceElemt);

	//��Ӽ����Ϣ
	TiXmlElement *pInfoElemt = new TiXmlElement(toUtf8(XML_NODE_MNTR_INFO).c_str());
	pServiceElemt->LinkEndChild(pInfoElemt);
	
	//��Ӽ���
	monitor_make_xml(pm, pInfoElemt);
		
	//����ļ�
	FILE *pFile = NULL;
	pFile = fopen("libsouth.xml", "w");
	if (pFile) {
		doc->Print(pFile);
		fclose(pFile);
		pFile = NULL;
	}

	//ת���ַ���
	TiXmlString strReturn;
	doc->PrintS(strReturn);
	
	//���ٶ���
	_delete(doc);

	//��ӡ����Ļ
	printf("\n%s\n", strReturn.c_str());

	return strReturn.c_str();
}


int upload(struct device *pd)
{
	if (NULL == pd) {
		printf("�����ָ��...\n");
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
	
	//��Ӽ���
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