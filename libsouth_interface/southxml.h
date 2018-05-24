#ifndef _SOUTH_XML_H_
#define _SOUTH_XML_H_
#include <string>
#include "tinyxml.h"

#ifdef __cplusplus
extern "C" {
#endif

//������ָ��
struct target
{
	char *_label;
	char *_unit;
	int  _type;
	char *_value;
	struct target *_next;
	struct monitor *_pm;
};

//�������
struct monitor
{
	int  _stat;
	char *_label;
	struct target *_target;
	struct target *_cru;
	unsigned int _itarget;
	struct monitor *_next;
	struct device *_pd;
};

//�����豸
struct device
{
	char *_label;
	char *_ip;
	struct monitor *_monitor;
	struct monitor *_curmon;
	unsigned int _imonitor;
};


//xml��ֵ
const char XML_NODE_MNTR_SERIVICE[]			=	"��ط���";
const char XML_NODE_MNTR_INFO[]				=	"�����Ϣ";
const char XML_NODE_CTL_CMD[]				=	"��������";
const char XML_NODE_PARAM[]					=	"����";
const char XML_PROP_SOFT_NAME[]				=	"�������";
const char XML_PROP_NAME[]					=	"����";
const char XML_PROP_TYPE[]					=	"����";
const char XML_PROP_UNIT[]					=	"��λ";
const char XML_PROP_VALUE_WORK_STATUS[]		=   "����״̬";
const char XML_PROP_VALUE_EXP_DSCR[]		=   "�쳣����";
const char XML_PROP_VALUE_START_TIME[]		=	"����ʱ��";
const char XML_PROP_VALUE_IMPT_RCRD[]		=	"��Ҫ��־";

//����ռ�
#define _new(type) (type*)malloc(sizeof(type))

//���ٶ���
#define _destory(ptr)	\
	if (NULL != ptr)	\
	{					\
		free(ptr);		\
		ptr = NULL;		\
	}

#define _delete(ptr)\
	if (NULL != ptr)	\
	{					\
		delete ptr;		\
		ptr = NULL;		\
	}

//״̬
static const char* const status[] = {
	"0",
	"1",
	"2"
};

//����ָ��
typedef std::string (*fun)(std::string, std::string);

/**
 * �û�ͨ������������ɼ����xml
 * @param struct monitor *pm	�豸�ṹ��
 * @return std::string	���������ַ���
 */
std::string monitor_to_string(struct monitor *pm);

/**
 * �û�ͨ������������ɼ��ָ��xml�ڵ�
 * @param struct target *pt	���ָ��ṹ��
 * @param TiXmlNode *pTiXmlNode	��������ָ���xml����
 * @return int	����0Ϊ�ɹ�
 */
int target_make_xml(struct target *pt, TiXmlNode *pTiXmlNode);

/**
 * �û�ͨ������������ɼ��ָ��xml�ڵ�
 * @param struct monitor *pm	����ṹ��
 * @param TiXmlNode *pTiXmlNode	����������xml����
 * @return int	����0Ϊ�ɹ�
 */
int monitor_make_xml(struct monitor *pm, TiXmlNode *pTiXmlNode);

#ifdef __cplusplus
}
#endif

#endif //_SOUTH_XML_H_