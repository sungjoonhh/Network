#pragma once
#include "RouterTable.h"
//분석중
class CARPTable
{
private:
	inline void		ResetHeader( );

public:
	CARPTable();
	virtual ~CARPTable();
	
	// Set Type, code , Address
	void	SetHardType(unsigned short hardtype);
	void	SetProtType(unsigned short prottype);
	void	SetOpcode(unsigned short op);
	void	SetSenderEtherAddress(unsigned char* enet);
	void	SetSenderIPAddress(unsigned char* ip);
	void	SetTargetEtherAddress(unsigned char* enet);
	void	SetTargetIPAddress(unsigned char* ip);
	
	// Get Address
	unsigned char*	GetSenderEtherAddress();
	unsigned char*	GetSenderIPAddress();
	unsigned char*	GetTargetEtherAddress();
	unsigned char*	GetTargetIPAddress();

	// Cache Table Data Struct : Linked List
	int			CacheAddItem(unsigned char* ppayload);// 삽입
	void		CacheDeleteItem(int index);	// 삭제 ( 개별 )
	void		CacheDeleteByTimer(int tID);
	void		CacheAllDelete();			// 삭제 ( 전체 )
	int			CacheUpdate(unsigned char* ppayload); // 갱신
	BOOL		CacheSearchExist(IP_ADDR ip);
	LPARP_NODE	GetCacheNode(IP_ADDR ip);

	// Proxy Table Data Struct : Linked List
	void	ProxyAddItem(PROXY_ENTRY proxy);// 삽입
	void	ProxyDeleteItem(int index);	// 삭제 ( 개별 )
	BOOL	ProxySearchExist(IP_ADDR ip);

	// ARP Header
	typedef struct _ARP_HEADER {
		unsigned short	arp_hardtype;
		unsigned short	arp_prottype;
		unsigned char	arp_hardsize;
		unsigned char	arp_protsize;
		unsigned short	arp_op;
		ETHERNET_ADDR	arp_srcenet;	// sender ethernet
		IP_ADDR			arp_srcip;		// sender ip
		ETHERNET_ADDR	arp_dstenet;	// target ethernet
		IP_ADDR			arp_dstip;		// target ip
	} ARP_HEADER, *PARP_HEADER ;

	void	SetSrcIPAddress(unsigned char* ip);
	void	SetOtherIPAddress(unsigned char* ip);
	unsigned char	src_ip[4];
	unsigned char	other_ip[4];

	void	SetRoutTableObject(CRouterTable* routetbl);
	CRouterTable* m_RoutTable;

protected:
	ARP_HEADER	m_sHeader ;

public:
	LPARP_NODE cache_head;
	LPPARP_NODE proxy_head;
};