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
	GOODSTAT = 0,		//����
	WARNINGSTAT,		//����
	BADSTAT				//ʧ�ػ�ʱ
};


struct monitor;
struct device;


/**
 * �û�ͨ�������������һ������ṹ��
 * @param const char * label	�������ƣ�����Ϊ��
 * @param unsigned short _stat	����״̬����ϸ��enum statu����
 * @return struct monitor *		����һ������ṹ��
 */
SOUTHAPI
struct monitor *monitor_new(const char *label, int _stat);

/**
 * �û�ͨ��������������ṹ�����һ�����ָ��
 * @param struct monitor * pm	�������ָ�룬ͨ��monitor_new���������
 * @param const char * label	ָ�����ƣ�����Ϊ��
 * @param const char * unit		ָ�굥λ������Ϊ��
 * @param unsigned short _type	ָ���������ͣ���ϸ��enum type����
 * @param const char * value	ָ��ֵ������Ϊ��
 * @return int					����һ��״̬ 0Ϊ����
 */
SOUTHAPI
int monitor_add_target(struct monitor *pm, const char *label, const char * value, const char *unit, int _type);

/**
 * �û�ͨ��������������ṹ��ɾ��һ�����ָ��
 * @param struct monitor * pm	�������ָ�룬ͨ��monitor_new���������
 * @param const char * label	ָ�����ƣ�����Ϊ��
 * @return int					����һ��״̬ 0Ϊ����
 */
SOUTHAPI
int monitor_del_target(struct monitor *pm, const char *label);

/**
 * �û�ͨ����������޸ļ���״ָ̬��
 * @param struct monitor * pm	�������ָ�룬ͨ��monitor_new���������
 * @param unsigned short _stat	����״̬����ϸ��enum statu����
 * @return int					����һ��״̬ 0Ϊ����
 */
SOUTHAPI
int monitor_set_stat(struct monitor *pm, int _stat);

/**
 * �û�ͨ������������ټ���ṹ��
 * @param struct monitor * pm	�������ָ�룬ͨ��monitor_new���������
 */
SOUTHAPI
void monitor_destory(struct monitor **pm);

/**
 * �û�ͨ�������������һ���豸�ṹ��
 * @param const char *ip	�豸���ƣ�����Ϊ��
 * @param const char *label	�豸ip
 * @return struct device *		����һ���豸�ṹ��
 */
SOUTHAPI
struct device *device_new(const char *ip, const char *label);

/**
 * �û�ͨ������������豸���һ������
 * @param struct device *		�豸�ṹ��
 * @param struct monitor *pm	����ṹ��ָ��
 * @return int					����һ��״̬ 0Ϊ����
 */
SOUTHAPI
int device_add_monitor(struct device *pd, struct monitor *pm);

/**
 * �û�ͨ������������豸ɾ��һ������
 * @param struct device *		�豸�ṹ��
 * @param const char * label	�������ƣ�����Ϊ��
 * @return int					����һ��״̬ 0Ϊ����
 */
SOUTHAPI
int device_del_monitor(struct device *pd, const char *label);

/**
 * �û�ͨ��������������豸�ṹ��
 * @param struct device **pd	�豸�ṹ��
 */
SOUTHAPI
void device_destory(struct device **pd);

SOUTHAPI
int upload(struct device *pd);

#ifdef __cplusplus
}
#endif

#endif	//_SOUTH_H_