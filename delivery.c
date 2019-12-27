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
}

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
