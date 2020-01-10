int go_test(void) { return 0; }

int data_length(void)
{
	char * data = "0X4100c010200002000000050000000000100000180C200003100000011111188a800c8890220640346000000000010010302ABCD000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	int data_len = 0;
	while (data[data_len] != 0) ++data_len;
	printf("%d\n", data_len);
	return 0;
}

int str_to_hex(void)
{
	char * data = "4100c010200002000000050000000000100000180C200003100000011111188a800c8890220640346000000000010010302ABCD000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	int data_len = 0;
	while (data[data_len] != 0) ++data_len;
	/* Convert char to value */
	char tmp, data_iter;
	int data_base = 0;
	int i = 0;
	while (i < data_len) {
		data_iter = data[data_base + i];
		if (('0' <= data_iter) && (data_iter <= '9')) {
			tmp = data_iter - '0';
		}
		else if (('a' <= data_iter) && (data_iter <= 'f')) {
			tmp = data_iter - 'a' + 10;
		}
		else if (('A' <= data_iter) && (data_iter <= 'F')) {
			tmp = data_iter - 'A' + 10;
		}
		else {
			printf("Unexpected char: %c\n", data_iter);
			return 1;
		}
		printf("%x", tmp);
		++i;
	}
	printf("\n");
	return 0;
}

#include <stdlib.h>
#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()
#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_ICMP, INET_ADDRSTRLEN
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/ip_icmp.h>  // struct icmp, ICMP_ECHO
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <errno.h>            // errno, perror()

int  get_src_mac_by_str(void) 
{
	// 通过网卡名获取源地址
	char * interface = "enp0s3";
	int sd;
	uint8_t src_mac[6];
	struct ifreq ifr;
	if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
	        perror ("socket() failed to get socket descriptor for using ioctl() ");
	        exit (EXIT_FAILURE);
	}
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
	if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
		perror ("ioctl() failed to get source MAC address ");
		return (EXIT_FAILURE);
	}
	close (sd);
	memcpy (src_mac, ifr.ifr_hwaddr.sa_data, sizeof(src_mac));
	
	printf ("MAC address for interface %s is ", interface);
	int i = 0;
	for(i = 0; i < sizeof(src_mac); i++){
		printf( i==5 ? "%.2x\n" : "%.2x:", src_mac[i]);
	}
	return (EXIT_SUCCESS);
}

int count_char_of_hex(void)
{
	char * data_str = "20028902 8021:aD";
	int num = 0;
	int i = 0;
	int data_len = 0;
	while (data_str[data_len] != 0) ++data_len;
	for ( i=0; i < data_len; i++ ) {
		if (('0' <= data_str[i]) && (data_str[i] <= '9') 
			|| ('a' <= data_str[i]) && (data_str[i] <= 'f') 
			|| ('A' <= data_str[i]) && (data_str[i] <= 'F') 
		) num++;
	}
	printf("%s --> %d", data_str, num);	
	return (EXIT_SUCCESS);
}

#include<time.h>
int get_time(void)
{
	time_t timep;
	memset(&timep, 0, sizeof(time_t));
	timep = time(NULL);
	printf("time: %ld\n\n", time(&timep));
	printf("time: %s\n", ctime(&timep));
	struct tm tm;
	memset(&tm, 0, sizeof(struct tm));
	localtime_r(&timep, &tm);
	printf("time: %i-%i-%i\n\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	printf("begin time: %ld\n", time(&timep));
	printf("wait 3 seconds\n");
	sleep(3);
	printf("end time: %ld\n\n", time(&timep));
	return (EXIT_SUCCESS);
}

#include <signal.h>
int timing_kill_by_signal(void)
{
	pid_t pid;
	pid = fork();
	void handler(int i)
	{
		sleep(2); // 如果不加这句, 控制台输出会乱序
		exit(0);
	}
	if ( pid < 0 ) {
		perror("fork()");
		exit(EXIT_FAILURE);
	}
	else if ( ! pid ) {
		sleep(6);
	} else {
		signal(SIGALRM, handler);
		alarm(5);
		while (1) {
			printf("wait\n");
			sleep(1);
		} 
	}
	return (EXIT_SUCCESS);
}

#include <time.h>
int timing_kill_by_timestamp(void)
{
	time_t timep;
	short timeout = 5;
	memset(&timep, 0, sizeof(time_t));
	timep = time(NULL);
	long tmp = time(&timep);
	while(time(&timep)-tmp < timeout){
		printf("wait %ld\n", time(&timep)-tmp+1);
		sleep(1);
	}
	return (EXIT_SUCCESS);
}

#include <sys/time.h>
int delay(void)
{
	struct timeval time_first;
	struct timeval time_second;
	
	gettimeofday(&time_first, NULL);
	sleep(2);
	gettimeofday(&time_second, NULL);

	printf("%.3f ms\n", (time_second.tv_sec -
					time_first.tv_sec) * 1000 +
					(time_second.tv_usec - time_first.tv_usec) / 1000.0);
	printf("%.3ld \n", time_second.tv_sec);
	printf("%.3ld \n", time_first.tv_sec);
	printf("%.3ld \n", time_second.tv_usec);
	printf("%.3ld \n", time_first.tv_usec);
	return (EXIT_SUCCESS);
}

int delay_pro(void)
{
	typedef struct _delay {
		struct timeval * time_first;
		struct timeval * time_second;
	} delay_t;

	delay_t _delay ;
	delay_t * delay = &_delay;

	struct timeval tmp_first;	
	gettimeofday(&tmp_first, NULL);
	delay->time_first = &tmp_first;
	printf("%p\n", delay->time_first);
	printf("%ld\n", (*delay->time_first).tv_sec);
	sleep(2);
	struct timeval tmp_second;	
	gettimeofday(&tmp_second, NULL);
	delay->time_second = &tmp_second;
	printf("%p\n", delay->time_second);
	printf("%ld\n", (*delay->time_second).tv_sec);

	printf("%.3f ms\n", ((*delay->time_second).tv_sec -
					(*delay->time_first).tv_sec) * 1000 +
					((*delay->time_second).tv_usec - (*delay->time_first).tv_usec) / 1000.0);

	return (EXIT_SUCCESS);
}
