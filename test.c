#include <stdio.h>
#include <string.h>
#include "delivery.c"
#include "delivery_oam.c"

#define PRINT_ENTER         printf("\n");
#define PRINT_RED(str)      printf("\033[0;31m%s\033[0;39m", str);
#define PRINT_GREEN(str)    printf("\033[0;32m%s\033[0;39m", str);
#define PRINT_YELLOW(str)   printf("\033[0;33m%s\033[0;39m", str);
#define PRINT_BULE(str)     printf("\033[0;34m%s\033[0;39m", str);
#define PRINT_PURPLE(str)   printf("\033[0;35m%s\033[0;39m", str);
#define PRINT_SEPARATOR     PRINT_BULE("-----------------------------------------------------------------\n")
#define BULE_DIVISION       "\033[0;34m|\033[0;39m"

#define DELIVER(func,is_run,delivery,is_show_desc,desc)  deliver(func, is_run, #func, delivery_c[delivery],is_show_desc, desc);

#define SHOW(what)          SHOW_##what(what)
#define RUN(what)           RUN_##what(what)

#define SHOW_desc(desc)          do {  \
	PRINT_GREEN("方法描述:\n") \
       	printf("%s\n", desc);  \
	PRINT_ENTER  \
	PRINT_SEPARATOR \
} while(0);
#define RUN_deliver_cb(deliver_cb)     do {  \
	PRINT_YELLOW("执行结果: \n")  \
	if (deliver_cb() > 0)  \
	PRINT_RED("执行不成功")  \
	PRINT_ENTER  \
	PRINT_SEPARATOR  \
} while(0);

#define PRINT_INFO          printf("%*.*s %s 是否执行: %*.*s %s 进度: %*.*s\n", \
		28, 28,  func_name, BULE_DIVISION, 8, 6, is_run ? "执行" : "忽略", BULE_DIVISION, 16, 12, delivery_c);

typedef int (*deliver_cb)(void);

void deliver(deliver_cb deliver_cb, int is_run, char* func_name, char* delivery_c, int is_show_desc, char* desc) 
{
	PRINT_INFO
	PRINT_SEPARATOR
	if (is_show_desc) SHOW(desc)
	if (is_run) RUN(deliver_cb)
}

void main(void)
{
	enum delivery_e {
		started,
		betaAvailable,
		slowProgress,
		deprecated,
		notStarted,
		blocked,
		goodProgress
	};

	char* delivery_c[] = {
		"正在实现",
		"存在问题",
		"进展缓慢",
		"已经废弃",
		"等待实现",
		"此路不通",
		"已经实现"
	};

	PRINT_PURPLE("===============================================================================\n")
	PRINT_SEPARATOR
	DELIVER(go_test,                     0, goodProgress, 0, "测试回调函数")
	DELIVER(data_length,                 0, goodProgress, 0, "计算数据长度")
	DELIVER(str_to_hex,                  0, goodProgress, 0, "字符串转16进制")
	DELIVER(get_src_mac_by_str,          0, goodProgress, 0, "获取网卡enp0s3的地址")
	DELIVER(count_char_of_hex,           0, goodProgress, 0, "统计出字符串中0-9,a-f,A-F的个数")
	DELIVER(get_time,                    0, goodProgress, 0, "获取时间并输出")
	DELIVER(timing_kill_by_signal,       0, betaAvailable,0, "定时结束死循环\n子进程不加sleep,控制台输出会乱序")
	DELIVER(timing_kill_by_timestamp,    0, goodProgress, 0, "通过时间戳定时结束死循环")
	PRINT_PURPLE("===============================================================================\n")
	PRINT_SEPARATOR
	DELIVER(send_oam_ais_pdu,            0, goodProgress, 0, "获取网卡enp0s3的地址并从该网卡发送Y1731 AIS报文")
	DELIVER(send_oam_ltm_pdu_by_str,     0, goodProgress, 0, "根据字符串发送Y1731 LTM报文")
	DELIVER(receive_oam_pdu,             0, deprecated,   0, "enp0s8接收CFM报文并处理,存在问题,会阻塞住程序")
	DELIVER(receive_oam_pdu_by_while,    0, betaAvailable,0, "enp0s8接收CFM报文并处理,存在问题,会阻塞住程序")
	DELIVER(receive_oam_pdu_by_select,   1, started,      1, "enp0s8接收CFM报文并处理")
	DELIVER(capture_oam_pdu,             1, started,      1, "捕获CFM报文并处理")
	PRINT_PURPLE("===============================================================================\n")
	
}
