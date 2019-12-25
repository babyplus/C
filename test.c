#include <stdio.h>
#include <string.h>
#include "delivery.c"

#define PRINT_RED(str)      printf("\033[0;31m%s\033[0;39m", str);
#define PRINT_GREEN(str)    printf("\033[0;32m%s\033[0;39m", str);
#define PRINT_YELLOW(str)   printf("\033[0;33m%s\033[0;39m", str);
#define PRINT_BULE(str)     printf("\033[0;34m%s\033[0;39m", str);
#define PRINT_PURPLE(str)   printf("\033[0;35m%s\033[0;39m", str);

#define PRINT_SEPARATOR     PRINT_BULE("-------------------------------------------\n")

typedef int (*deliver_cb)(void);

void deliver(deliver_cb deliver_cb, int is_run, char* desc, char* delivery_c, int separator) 
{
	printf("%*.*s|%*.*s|%*.*s\n", 16, 16, desc, 8, 6, is_run ? "执行" : "忽略", 16, 12, delivery_c);
	if (separator) PRINT_SEPARATOR
	if (is_run) {
		PRINT_YELLOW("执行结果: \n\n")
		deliver_cb();
		PRINT_SEPARATOR
	}

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
	deliver(go_test,      0, "callback_test",        delivery_c[goodProgress],     1);
	deliver(data_length,  0, "data_length",          delivery_c[goodProgress],     1);
	deliver(str_to_hex,   0, "str_to_hex",           delivery_c[goodProgress],     1);
	deliver(send_message, 1, "send_message",         delivery_c[goodProgress],     1);
	PRINT_PURPLE("===============================================================================\n")

}
