// IPLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////
// ºÐ¼®Áß
#pragma once
#include "BaseLayer.h"
#include "ICMP.h"
#include "ARPLayer.h"
#include "RouterTable.h"
#include "ARPTable.h"

class CIPLayer 
: public CBaseLayer  
{
private:
	inline void		ResetHeader( );
	unsigned char	srcip[4];
	unsigned char	destip[4];

public:
	CIPLayer( char* pName );
	virtual ~CIPLayer();

	u_short IP_Checksum(u_short* data,int len);

	void SetSrcIPAddress(unsigned char* src_ip);
	void SetDstIPAddress(unsigned char* dst_ip);

	void SetARPobj(CARPLayer* arp);	// arp °´Ã¼
	void SetICMPobj(CICMP* icmp);	// icmp °´Ã¼
	void SetOtherIPobj(CIPLayer* ip_obj); // ip °´Ã¼
	void SetRouterTable(CRouterTable * routTable); // router table °´Ã¼
	void SetARPTable(CARPTable * arpTable);

	void TransferIPPacket(u_char* ppayload);
	void ICMP_ReplyMSG(u_char * ppayload);

	BOOL Send(unsigned char* ppayload, int nlength);
	BOOL Receive(unsigned char* ppayload);

	typedef struct _IPLayer_HEADER {
		unsigned char ip_verlen;	// ip version & header length (1byte)
		unsigned char ip_tos;		// type of service	(1byte)
		unsigned short ip_len;		// total packet length	(2byte)
		unsigned short ip_id;		// datagram id			(2byte)
		unsigned short ip_fragoff;	// fragment offset		(2byte)
		unsigned char ip_ttl;		// time to live in gateway hops (1byte)
		unsigned char ip_proto;		// IP protocol			(1byte)
		unsigned short ip_cksum;	// header checksum		(2byte)
		unsigned char ip_src[4];	// IP address of source	(4byte)
		unsigned char ip_dst[4];	// IP address of destination	(4byte)
		unsigned char ip_data[IP_DATA_SIZE]; // variable length data
	} IPLayer_HEADER, *PIPLayer_HEADER ;
	
	unsigned char in_ip[4];
	unsigned char ex_ip[4];

	CARPLayer * m_ARP;
	CICMP * m_Icmp;
	CIPLayer * m_otherIp;
	CRouterTable * m_RoutTable;
	CARPTable * m_ARPTable;

	u_char multicast_addr[4];

	IPLayer_HEADER	m_sHeader ;
};
