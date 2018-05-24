#ifndef _SOUTH_XML_H_
#define _SOUTH_XML_H_
#include <string>
#include "tinyxml.h"

#ifdef __cplusplus
extern "C" {
#endif

//定义监测指标
struct target
{
	char *_label;
	char *_unit;
	int  _type;
	char *_value;
	struct target *_next;
	struct monitor *_pm;
};

//定义监测点
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

//定义设备
struct device
{
	char *_label;
	char *_ip;
	struct monitor *_monitor;
	struct monitor *_curmon;
	unsigned int _imonitor;
};


//xml键值
const char XML_NODE_MNTR_SERIVICE[]			=	"监控服务";
const char XML_NODE_MNTR_INFO[]				=	"监控信息";
const char XML_NODE_CTL_CMD[]				=	"控制命令";
const char XML_NODE_PARAM[]					=	"参数";
const char XML_PROP_SOFT_NAME[]				=	"软件名称";
const char XML_PROP_NAME[]					=	"名称";
const char XML_PROP_TYPE[]					=	"类型";
const char XML_PROP_UNIT[]					=	"单位";
const char XML_PROP_VALUE_WORK_STATUS[]		=   "工作状态";
const char XML_PROP_VALUE_EXP_DSCR[]		=   "异常描述";
const char XML_PROP_VALUE_START_TIME[]		=	"启动时间";
const char XML_PROP_VALUE_IMPT_RCRD[]		=	"重要日志";

//分配空间
#define _new(type) (type*)malloc(sizeof(type))

//销毁对象
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

//状态
static const char* const status[] = {
	"0",
	"1",
	"2"
};

//函数指针
typedef std::string (*fun)(std::string, std::string);

/**
 * 用户通过这个函数生成监测点结xml
 * @param struct monitor *pm	设备结构体
 * @return std::string	返回生成字符串
 */
std::string monitor_to_string(struct monitor *pm);

/**
 * 用户通过这个函数生成检查指标xml节点
 * @param struct target *pt	监测指标结构体
 * @param TiXmlNode *pTiXmlNode	存放生监测指标的xml对象
 * @return int	返回0为成功
 */
int target_make_xml(struct target *pt, TiXmlNode *pTiXmlNode);

/**
 * 用户通过这个函数生成检查指标xml节点
 * @param struct monitor *pm	监测点结构体
 * @param TiXmlNode *pTiXmlNode	存放生监测点的xml对象
 * @return int	返回0为成功
 */
int monitor_make_xml(struct monitor *pm, TiXmlNode *pTiXmlNode);

#ifdef __cplusplus
}
#endif

#endif //_SOUTH_XML_H_