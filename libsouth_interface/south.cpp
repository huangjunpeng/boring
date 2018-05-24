#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "south.h"
#include "southxml.h"

void target_destroy(struct target **pt)
{
	struct target *temp = *pt;
	if (NULL == temp) {
		return;
	}

	target_destroy(&temp->_next);

	_destory(temp->_label);

	_destory(temp->_value);

	_destory(temp->_unit);

	_destory(temp);

	*pt = NULL;
}

struct	monitor *monitor_new(const char *label, int _stat)
{
	struct monitor *pm = NULL;
	struct target *pt = NULL;
	if (NULL == label) {
		printf("传入空指针...\n");
		return NULL;
	}
	pm = _new(struct monitor);
	if (NULL == pm) {
		printf("分配内存失败...\n");
		return pm;
	}

	pt = _new(struct target);
	if (NULL == pt) {
		printf("分配内存失败...\n");
		free(pm);
		pm = NULL;
		return pm;
	}
	
	pt->_label	= _strdup(XML_PROP_VALUE_WORK_STATUS);
	pt->_unit	= NULL;
	pt->_type	= DATA_TYPE_INT32;
	pt->_value	= _strdup(status[_stat]);
	pt->_next	= NULL;
	pt->_pm		= pm;

	pm->_label		= _strdup(label);
	pm->_stat		= _stat;
	pm->_next		= NULL;
	pm->_target		= pt;
	pm->_cru		= pt;
	pm->_itarget	= 1;
	return pm;
}

int monitor_add_target(struct monitor *pm, const char *label, const char * value, const char *unit, int _type)
{
	struct target *pt = NULL;
	
	if (NULL == pm || NULL == label || NULL == unit || value == NULL) {
		printf("传入空指针...\n");
		return -1;
	}
	pt = _new(struct target);
	if (NULL == pt) {
		printf("分配内存失败...\n");
		return -1;
	}
	pt->_label	= _strdup(label);
	pt->_unit	= _strdup(unit);
	pt->_type	= _type;
	pt->_value	= _strdup(value);
	pt->_next	= NULL;
	pt->_pm		= pm;
	
	pm->_cru->_next	= pt;
	pm->_cru = pt;
	++(pm->_itarget);
	return 0;
}

int monitor_del_target(struct monitor *pm, const char *label)
{
	struct target *pt = NULL, *ptemp = NULL;
	if (NULL == pm || NULL == label) {
		printf("传入空指针...\n");
		return -1;
	}
	pt		= pm->_target->_next;
	ptemp	= pm->_target;
	while (NULL != pt) {
		if (strcmp(pt->_label, label) == 0) {
			ptemp->_next = pt->_next;
			pt->_next = NULL;
			--(pm->_itarget);
			target_destroy(&pt);
			return 0;
		}
		ptemp	= pt;
		pt		= pt->_next;
	}
	return 0;
}

int monitor_set_stat(struct monitor *pm, int _stat)
{
	if (NULL == pm) {
		printf("传入空指针...\n");
		return -1;
	}
	memcpy(pm->_target->_value, status[_stat], 1);
	return 0;
}

void monitor_destory(struct monitor **pm)
{
	struct monitor *temp = *pm;
	if (NULL == temp) {
		return;
	}

	monitor_destory(&temp->_next);

	_destory(temp->_label);

	target_destroy(&temp->_target);

	_destory(temp);

	*pm = NULL;
}


struct device *device_new(const char *ip, const char *label)
{
	struct device *pd = NULL;
	if (NULL == label || NULL == ip) {
		printf("传入空指针...\n");
		return NULL;
	}
	pd = _new(struct device);
	if (NULL == pd) {
		printf("分配内存失败...\n");
		return pd;
	}
	pd->_label		= _strdup(label);
	pd->_ip			= _strdup(ip);
	pd->_monitor	= NULL;
	pd->_curmon		= NULL;
	pd->_imonitor	= 0;
	return pd;
}

int device_add_monitor(struct device *pd, struct monitor *pm)
{
	if (NULL == pd || NULL == pm) {
		printf("传入空指针...\n");
		return -1;
	}
	pm->_pd = pd;
	if (NULL == pd->_monitor) {
		pd->_monitor		= pm;
	} else {
		pd->_curmon->_next	= pm;
	}
	pd->_curmon = pm;
	++(pd->_imonitor);
	return 0;
}

int device_del_monitor(struct device *pd, const char *label)
{
	struct monitor *pm = NULL, *ptemp = NULL;
	if (NULL == pd || NULL == label) {
		printf("传入空指针...\n");
		return -1;
	}
	pm	= pd->_monitor;
	while (NULL != pm) {
		if (strcmp(pm->_label, label) == 0) {
			if (NULL == ptemp) 
			{
				pd->_monitor = pm->_next;
			} 
			else 
			{
				ptemp->_next = pm->_next;
			}
			--(pd->_imonitor);
			pm->_next = NULL;
			monitor_destory(&pm);
			return 0;
		}
		ptemp	= pm;
		pm		= pm->_next;
	}
	return 0;
}

void device_destory(struct device **pd)
{
	struct device *temp = *pd;
	if (NULL == temp) {
		return;
	}

	_destory(temp->_label);

	_destory(temp->_ip);

	monitor_destory(&temp->_monitor);

	_destory(temp);

	*pd = NULL;
}