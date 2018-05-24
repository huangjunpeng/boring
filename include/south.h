#ifndef _SOUTH_H_
#define _SOUTH_H_

#include "setup.h"

#ifdef __cplusplus
extern "C" {
#endif

//define data type
enum type
{
	DATA_TYPE_INT32 = 1,
	DATA_TYPE_INT64,
	DATA_TYPE_FLOAT,
	DATA_TYPE_STRING
};

enum statu
{
	GOODSTAT = 0,		//正常
	WARNINGSTAT,		//故障
	BADSTAT				//失控或超时
};


struct monitor;
struct device;


/**
 * 用户通过这个函数创建一个监测点结构体
 * @param const char * label	监测点名称，不能为空
 * @param unsigned short _stat	监测点状态，详细看enum statu定义
 * @return struct monitor *		返回一个监测点结构体
 */
SOUTHAPI
struct monitor *monitor_new(const char *label, int _stat);

/**
 * 用户通过这个函数向监测点结构体添加一个监测指标
 * @param struct monitor * pm	监测点对象指针，通过monitor_new构造出来的
 * @param const char * label	指标名称，不能为空
 * @param const char * unit		指标单位，不能为空
 * @param unsigned short _type	指标数据类型，详细看enum type定义
 * @param const char * value	指标值，不能为空
 * @return int					返回一个状态 0为正常
 */
SOUTHAPI
int monitor_add_target(struct monitor *pm, const char *label, const char * value, const char *unit, int _type);

/**
 * 用户通过这个函数向监测点结构体删除一个监测指标
 * @param struct monitor * pm	监测点对象指针，通过monitor_new构造出来的
 * @param const char * label	指标名称，不能为空
 * @return int					返回一个状态 0为正常
 */
SOUTHAPI
int monitor_del_target(struct monitor *pm, const char *label);

/**
 * 用户通过这个函数修改监测点状态指标
 * @param struct monitor * pm	监测点对象指针，通过monitor_new构造出来的
 * @param unsigned short _stat	监测点状态，详细看enum statu定义
 * @return int					返回一个状态 0为正常
 */
SOUTHAPI
int monitor_set_stat(struct monitor *pm, int _stat);

/**
 * 用户通过这个函数销毁监测点结构体
 * @param struct monitor * pm	监测点对象指针，通过monitor_new构造出来的
 */
SOUTHAPI
void monitor_destory(struct monitor **pm);

/**
 * 用户通过这个函数创建一个设备结构体
 * @param const char *ip	设备名称，不能为空
 * @param const char *label	设备ip
 * @return struct device *		返回一个设备结构体
 */
SOUTHAPI
struct device *device_new(const char *ip, const char *label);

/**
 * 用户通过这个函数向设备添加一个监测点
 * @param struct device *		设备结构体
 * @param struct monitor *pm	监测点结构体指针
 * @return int					返回一个状态 0为正常
 */
SOUTHAPI
int device_add_monitor(struct device *pd, struct monitor *pm);

/**
 * 用户通过这个函数向设备删除一个监测点
 * @param struct device *		设备结构体
 * @param const char * label	监测点名称，不能为空
 * @return int					返回一个状态 0为正常
 */
SOUTHAPI
int device_del_monitor(struct device *pd, const char *label);

/**
 * 用户通过这个函数销毁设备结构体
 * @param struct device **pd	设备结构体
 */
SOUTHAPI
void device_destory(struct device **pd);

SOUTHAPI
int upload(struct device *pd);

#ifdef __cplusplus
}
#endif

#endif	//_SOUTH_H_