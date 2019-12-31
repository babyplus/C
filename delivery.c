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
