
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

/* 추가 header */
#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원


#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

/* 추가 매크로 */
#define MAX_LAYER_NUMBER		0xff

#define ETHER_MAX_SIZE			1514
#define ETHER_HEADER_SIZE		14
#define ETHER_MAX_DATA_SIZE		( ETHER_MAX_SIZE - ETHER_HEADER_SIZE )

#define	IP_PROTOTYPE_UDP		0x11	// UDP : 17
#define IP_PROTOTYPE_TCP		0x06	// TCP : 6
#define	IP_PROTOTYPE_ICMP		0x1		// ICMP : 1

#define ICMP_DATA_SIZE			32
#define ICMP_TYPE_REQUEST		0x08
#define ICMP_TYPE_REPLY			0x00

#define UDP_PORT_SRC			520

#define RIP_CMD_REQUEST			0x1
#define RIP_CMD_REPLY			0x2
#define RIP_VER_RIP1			0x1
#define RIP_VER_RIP2			0x2
#define RIP_RD_DEFAULT			0x00
#define	RIP_ADDRFAMI_UNSPEC		0x0000
#define	RIP_ADDRFAMI_IP			0x0002
#define RIP_ROUTETAG_DEFAULT	0x00
#define RIP_ROUTETAG_BGP		0x01
#define RIP_ROUTETAG_EGP		0x02
#define RIP_HOP_DEFAULT			0
#define RIP_HOP_INFINITY		16

#define ARP_HARDTYPE			0x0001	// Ethernet frame
#define ARP_PROTOTYPE_IP		0x0800
#define ARP_PROTOTYPE_ARP		0x0806
#define ARP_PROTOTYPE_RARP		0x0835
#define ARP_OPCODE_REQUEST		0x0001
#define ARP_OPCODE_REPLY		0x0002
#define ARP_HEADER_SIZE			28

#define CACHE_INCOMPLETE		0x00
#define CACHE_COMPLETE			0x01
#define CACHE_TTL				10

#define	TIMER_REPLY				3000	// 30초
#define TIMER_VALID				12000	// 120초
#define	TIMER_EXPIRE			18000	// 180초
#define TIMER_UPDATE			1000	// 15초

#define TIMER_ID_UPDATE			8000
#define TIMER_ID_INIT			9999
#define TIMER_ID_REPLY			9998
#define TIMER_ID_CHK_LVALID		9997
#define TIMER_ID_CHK_RVALID		9996
#define TIMER_ID_INF_RVALID		9003
#define TIMER_ID_INF_LVALID		9002
#define TIMER_ID_LEFT_RECEIVE	9001
#define TIMER_ID_RIGHT_RECEIVE	9000

#define NI_COUNT_NIC			10

#define PM_DEVICE_NAME_LENGTH	20
#define RT_DEVICE_NAME_LENGTH	20

#define IP_HEADER_SIZE			( sizeof(unsigned short) * 4 +				\
								  sizeof(unsigned char) * 12  )
#define IP_DATA_SIZE			( ETHER_MAX_SIZE - ( IP_HEADER_SIZE  +		\
														ETHER_HEADER_SIZE ))
#define UDP_HEADER_SIZE			( sizeof(unsigned short) * 4 )
#define UDP_DATA_SIZE			( ETHER_MAX_SIZE - ( ETHER_HEADER_SIZE +	\
														IP_HEADER_SIZE +	\
														UDP_HEADER_SIZE))
#define RIP_HEADER_SIZE			( sizeof(unsigned char) * 2 +	\
									sizeof(unsigned short) )
#define RIP_DATA_SIZE			( sizeof(unsigned short) * 2 +	\
									sizeof(unsigned char) * 4 * 3 +	\
									sizeof(unsigned int) )
#define RIP_MAX_DATA_SIZE		RIP_DATA_SIZE * 25
#define ICMP_HEADER_SIZE		( sizeof(unsigned short) * 3 +	\
									sizeof(unsigned char) * 2 )

#define DELG_START				0
#define DELG_END				1
#define DELG_RESET				2
#define DELG_SETTING			3

// Ethernet Address
typedef struct _ETHERNET_ADDR
{
	union {
		struct { unsigned char e0, e1, e2, e3, e4, e5; } s_un_byte ;
		unsigned char s_ether_addr[6] ;
	} S_un ;

#define addr_e0 S_un.s_un_byte.e0
#define addr_e1 S_un.s_un_byte.e1
#define addr_e2 S_un.s_un_byte.e2
#define addr_e3 S_un.s_un_byte.e3
#define addr_e4 S_un.s_un_byte.e4
#define addr_e5 S_un.s_un_byte.e5
#define addrs_e S_un.s_ether_addr
} ETHERNET_ADDR, *LPETHERNET_ADDR ;

// IP Address
typedef struct _IP_ADDR
{
	union {
		struct { unsigned char i0, i1, i2, i3; } s_un_byte ;
		unsigned char s_ip_addr[4] ;
	} S_un ;

#define addr_i0 S_un.s_un_byte.i0
#define addr_i1 S_un.s_un_byte.i1
#define addr_i2 S_un.s_un_byte.i2
#define addr_i3 S_un.s_un_byte.i3
#define addrs_i S_un.s_ip_addr
} IP_ADDR, *LPIP_ADDR ;

// CACHE Entry
typedef struct _CACHE_ENTRY
{
	IP_ADDR			cache_ipaddr;
	ETHERNET_ADDR	cache_enetaddr;
	unsigned char	cache_interface[RT_DEVICE_NAME_LENGTH];
	unsigned char	cache_flag[4];
}CACHE_ENTRY, *LPCACHE_ENTRY;

// Linked List
typedef struct _ARP_NODE
{
	CACHE_ENTRY			key;
	struct _ARP_NODE	*next;
}ARP_NODE, *LPARP_NODE;

// PROXY Entry
typedef struct _PROXY_ENTRY
{
	IP_ADDR			proxy_ipaddr;
	ETHERNET_ADDR	proxy_enetaddr;
	unsigned char	proxy_devname[PM_DEVICE_NAME_LENGTH];
}PROXY_ENTRY, *LPPROXY_ENTRY;

// Linked List
typedef struct _PARP_NODE
{
	PROXY_ENTRY			key;
	struct _PARP_NODE	*next;
}PARP_NODE, *LPPARP_NODE;

// Router Entry
typedef struct _ROUT_ENTRY		//* router entry에 있는것들.
{
	IP_ADDR			rout_router;
	IP_ADDR			rout_subnet;
	IP_ADDR			rout_nexthop;
	unsigned int	rout_metric;
	IP_ADDR			rout_outport;
	unsigned char	rout_flag[4];
	unsigned char	rout_interface[RT_DEVICE_NAME_LENGTH];
	int	rout_ttl;
}ROUT_ENTRY, *LPROUT_ENTRY;

// Linked List
typedef struct _ROUT_NODE
{
	ROUT_ENTRY			key;
	struct _ROUT_NODE	*next;
}ROUT_NODE, *LPROUT_NODE;