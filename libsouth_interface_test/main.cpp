#include <stdio.h>
#include <stdlib.h>
#include <south.h>
#include <string>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <time.h>
using namespace std;
typedef std::string (PassiveFun)(std::string, std::string);

int main(int argc, char **argv)
{
	struct device *pd = device_new("127.0.0.1", "");
	stringstream stream;
	char buf[8] = {0};
	struct monitor *pm = NULL;
	for (int i = 0; i < 400; i++) {
		memset(buf, 0, 8);
		_itoa(i, buf, 10);
		pm = monitor_new(buf, GOODSTAT);
		monitor_add_target(pm, "数据接入速度", "100.25", "Mb/s", DATA_TYPE_FLOAT);
		monitor_add_target(pm, "接收数据量", "568", "MB", DATA_TYPE_INT32);
		device_add_monitor(pd, pm);
		pm = NULL;
	}
	printf("begin...............................\n");
	clock_t t=clock();
	upload(pd);
	clock_t tEnd=clock();
	printf("t = %d\ntEnd = %d\ntEnd-t = %d\n", t, tEnd, tEnd - t);
	device_destory(&pd);
	return 0;
}