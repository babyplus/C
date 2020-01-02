#include <stdio.h>
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
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>

#include <errno.h>            // errno, perror()
#define ETH_P_DEAN 0x8100 //自己定义的以太网协议type

int send_oam_ais_pdu(void)
{
    int i, datalen,frame_length, sd, bytes;
    char *interface="enp0s3";
    uint8_t data[IP_MAXPACKET];
    uint8_t src_mac[6];
    uint8_t dst_mac[6];
    uint8_t ether_frame[IP_MAXPACKET];
    struct sockaddr_ll device;
    struct ifreq ifr;

    // Submit request for a socket descriptor to look up interface.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {//第一次创建socket是为了获取本地网卡信息
        perror ("socket() failed to get socket descriptor for using ioctl() ");
        exit (EXIT_FAILURE);
    }

    // Use ioctl() to look up interface name and get its MAC address.
    memset (&ifr, 0, sizeof (ifr));
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
    if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
        perror ("ioctl() failed to get source MAC address ");
        return (EXIT_FAILURE);
    }
    close (sd);

    // Copy source MAC address.
    memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6);

    // Report source MAC address to stdout.
    printf ("MAC address for interface %s is ", interface);
    for (i=0; i<5; i++) {
        printf ("%02x:", src_mac[i]);
    }
    printf ("%02x\n", src_mac[5]);

    // Find interface index from interface name and store index in
    // struct sockaddr_ll device, which will be used as an argument of sendto().
    memset (&device, 0, sizeof (device));
    if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
        perror ("if_nametoindex() failed to obtain interface index ");
        exit (EXIT_FAILURE);
    }
    printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);

    // Set destination MAC address: you need to fill these out
    dst_mac[0] = 0x01;//设置目的网卡地址
    dst_mac[1] = 0x80;
    dst_mac[2] = 0xc2;
    dst_mac[3] = 0x00;
    dst_mac[4] = 0x00;
    dst_mac[5] = 0x37;

    // Fill out sockaddr_ll.
    device.sll_family = AF_PACKET;
    memcpy (device.sll_addr, src_mac, 6);
    device.sll_halen = htons (6);

    // 发送的data，长度能够随意，可是抓包时看到最小数据长度为46，这是以太网协议规定以太网帧数据域部分最小为46字节，不足的自己主动补零处理
    datalen = 9;
   
    // vlan = 2
    data[0] = 0x20;
    data[1] = 0x02;
    data[2] = 0x89;
    data[3] = 0x02;

    // cfm ais pdu
    data[4] = 0x80;
    data[5] = 0x21;
    
    data[6] = 0x04;

    // Fill out ethernet frame header.
    frame_length = 6 + 6 + 2   + datalen;
    // Destination and Source MAC addresses
    memcpy (ether_frame, dst_mac, 6);
    memcpy (ether_frame + 6, src_mac, 6);

    ether_frame[12] = ETH_P_DEAN / 256;
    ether_frame[13] = ETH_P_DEAN % 256;

    // data
    memcpy (ether_frame + 14 , data, datalen);
    
    // Submit request for a raw socket descriptor.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {//创建正真发送的socket
        perror ("socket() failed ");
        exit (EXIT_FAILURE);
    }
    // Send ethernet frame to socket.
    if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
        perror ("sendto() failed");
        exit (EXIT_FAILURE);
    }
    printf ("send num=%d,read num=%d\n",frame_length,bytes);     
    // Close socket descriptor.
    close (sd);

    return (EXIT_SUCCESS);
}

int receive_oam_ltr_pdu(void)
{
	return (EXIT_SUCCESS);
}

int _translate_str_to_uint8_t(char * str, uint8_t * uint8, int len)
{
	char tmp, data_iter;
	int data_len = 0;
        while (str[data_len] != 0) ++data_len;
	int i = 0;
	int r = 0;
	int uint8_tmp_index = 0;
	uint8_t uint8_tmp[len];
	memset(uint8_tmp, 0, len);
        while (i < data_len) {
                data_iter = str[i];
                if (('0' <= data_iter) && (data_iter <= '9')) {
                        tmp = data_iter - '0';
			uint8_tmp[uint8_tmp_index] = r ? ((uint8_tmp[uint8_tmp_index] << 4) + tmp) :  (uint8_tmp[uint8_tmp_index] + tmp);
                }
                else if (('a' <= data_iter) && (data_iter <= 'f')) {
                        tmp = data_iter - 'a' + 10;
			uint8_tmp[uint8_tmp_index] = r ? ((uint8_tmp[uint8_tmp_index] << 4) + tmp) :  (uint8_tmp[uint8_tmp_index] + tmp);
                }
                else if (('A' <= data_iter) && (data_iter <= 'F')) {
                        tmp = data_iter - 'A' + 10;
			uint8_tmp[uint8_tmp_index] = r ? ((uint8_tmp[uint8_tmp_index] << 4) + tmp) :  (uint8_tmp[uint8_tmp_index] + tmp);
                }
		else if ( ':' == data_iter | ' ' == data_iter | '|' == data_iter ) {
			++i;
			continue;
		}
                else {
                        printf("Unexpected char: %c\n", data_iter);
                        return 1;
                }
		if (r) {
			uint8_tmp_index++;
			r = 0;
		}else r=1;
                ++i;
        }
	memcpy (uint8 , uint8_tmp, sizeof(uint8_tmp));
}

int _count_char_of_hex(char * data_str, int * p_hex_num)
{
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
	*p_hex_num = num;
        printf("%s --> %d\n", data_str, num);
        return (EXIT_SUCCESS);
}

int  send_oam_ltm_pdu_by_str(void) 
{

	uint8_t tmp_data[] = {};
	int tmp_data_len = 0;
	// 通过网卡名获取源地址
	char * interface = "enp0s3";
	int sd;
	uint8_t src_mac[6];
	uint8_t dst_mac[6];
	struct ifreq ifr;
	struct sockaddr_ll device;

	if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
	        perror ("socket() failed to get socket descriptor for using ioctl() ");
	        exit (EXIT_FAILURE);
	}
	memset(&ifr, 0, sizeof(ifr));
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
	if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
		perror ("ioctl() failed to get source MAC address ");
		return (EXIT_FAILURE);
	}
	close (sd);
	memcpy (src_mac, ifr.ifr_hwaddr.sa_data, sizeof(src_mac));
	int i = 0;
	for(i = 0; i < sizeof(src_mac); i++){
		printf( i==5 ? "%.2x\n" : "%.2x:", src_mac[i]);
	}

	memset(&device, 0 ,sizeof(device));
	if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
		perror("if_nametoindex() failed to obtain interface index.");
		return (EXIT_FAILURE);
	}
	printf("Index for interface %s is %i \n", interface, device.sll_ifindex);

	device.sll_family = AF_PACKET;
	memcpy(device.sll_addr, src_mac, 6);
	device.sll_halen = htons(6);

	char * dst_mac_str = "01:80:c2:00:00:37";
	_translate_str_to_uint8_t(dst_mac_str, dst_mac, 6);
        for(i = 0; i < sizeof(dst_mac); i++){
                printf( i==5 ? "%.2x\n" : "%.2x:", dst_mac[i]);
        }

	char * eth_type_str = "8100";
	uint8_t eth_type[2] = {0};
	_translate_str_to_uint8_t(eth_type_str, eth_type, 2);
        for(i = 0; i < sizeof(eth_type); i++){
                printf("%.2x", eth_type[i]);
        }
	printf("\n");
	
	// vlan = 2 | is a ltm pdu | pdu data
	char * data_str = "20028902 | 8005 | 80 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01";
	int hex_num = 0;
	int data_len = 0;
	_count_char_of_hex(data_str, &hex_num);
	data_len = hex_num/2;
	uint8_t data[data_len];
	memset(data , 0, sizeof(uint8_t) * data_len);
	_translate_str_to_uint8_t(data_str, data, data_len);
        for(i = 0; i < sizeof(data); i++){
                printf("%.2x", data[i]);
        }
	printf("\n");

	int frame_length = 6+6+2+ data_len +2;
	uint8_t ether_frame[IP_MAXPACKET];
	memcpy(ether_frame, dst_mac, 6);
	memcpy(ether_frame + 6, src_mac, 6);
	memcpy(ether_frame + 12, eth_type, 2);
	memcpy(ether_frame + 14, data, data_len);

	int bytes;
	// Submit request for a raw socket descriptor.
	if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {//创建正真发送的socket
	    perror ("socket() failed ");
	    exit (EXIT_FAILURE);
	}
	// Send ethernet frame to socket.
	if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
	    perror ("sendto() failed");
	    exit (EXIT_FAILURE);
	}
	printf ("send num=%d,read num=%d\n",frame_length,bytes);     
	// Close socket descriptor.
	close (sd);
}
