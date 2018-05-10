#ifndef _PROTOCOL_HEADER_H_
#define _PROTOCOL_HEADER_H_

#include <boring.h>
#pragma pack(1)
//ÒÔÌ«Íø
typedef struct _Ethernet
{
	int8	dst_mac[6];
	int8	src_mac[6];
	uint16	type;
}Ethernet, *PEthernet;

typedef struct _vlan
{
	uint16	vlan_id;
	uint16	type;
}VLAN, *PVLAN;

typedef struct _ip
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	uint8	ip_hl:4;
	uint8	ip_v:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
	uint8	ip_v:4;
	uint8	ip_hl:4;
#else
# error	"__LITTLE_ENDIAN OR __BIG_ENDIAN?"
#endif
	uint8	ip_tos;
	uint16	ip_len;
	uint16	ip_id;
	uint16	ip_off;
#define	IP_RF 0x8000			/* reserved fragment flag */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	uint8	ip_ttl;
	uint8	ip_p;
	uint16	ip_sum;
	struct in_addr ip_src, ip_dst;	/* source and dest address */
}ip, *ptrip;

#define	IPVERSION	4               /* IP version number */
#define	IP_MAXPACKET	65535		/* maximum packet size */
#define IP_SIZE 20

/*
* Internet implementation parameters.
*/
#define	MAXTTL		255		/* maximum time to live (seconds) */
#define	IPDEFTTL	64		/* default ttl, from RFC 1340 */
#define	IPFRAGTTL	60		/* time to live for frags, slowhz */
#define	IPTTLDEC	1		/* subtracted when forwarding */

#define	IP_MSS		576		/* default maximum segment size */


struct tcphdr///tcp
{
	uint16 source;
	uint16 dest;
	uint32 seq;//sequence number
	uint32 ack_seq;//Ackowledegment number
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	uint16 res1:4;
	uint16 doff:4;//HeaderLength
	uint16 fin:1;
	uint16 syn:1;
	uint16 rst:1;
	uint16 psh:1;
	uint16 ack:1;
	uint16 urg:1;
	uint16 res2:2;
#  elif __BYTE_ORDER == __BIG_ENDIAN
	uint16 doff:4;
	uint16 res1:4;
	uint16 res2:2;
	uint16 urg:1;
	uint16 ack:1;
	uint16 psh:1;
	uint16 rst:1;
	uint16 syn:1;
	uint16 fin:1;
#  else
#error	"__LITTLE_ENDIAN OR __BIG_ENDIAN?"
#  endif
	uint16 window;
	uint16 check;
	uint16 urg_ptr;
};

enum {
	TCP_ESTABLISHED = 1,
	TCP_SYN_SENT,
	TCP_SYN_RECV,
	TCP_FIN_WAIT1,
	TCP_FIN_WAIT2,
	TCP_TIME_WAIT,
	TCP_CLOSE,
	TCP_CLOSE_WAIT,
	TCP_LAST_ACK,
	TCP_LISTEN,
	TCP_CLOSING   /* now a valid state */
};

#pragma pack()
#endif //_PROTOCOL_HEADER_H_