// ARPLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARPLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ARPLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#pragma once
#include "BaseLayer.h"
#include "RouterTable.h"
#include "ARPTable.h"
#include "EthernetLayer.h"

class CARPLayer
: public CBaseLayer
{
private:
	inline void		ResetHeader( );

public:
	BOOL			Receive( unsigned char* ppayload ) ;
	BOOL			Send( unsigned char* ppayload, int nlength );

	static UINT		ARPTimer(LPVOID pParam);

	CARPLayer( char* pName );
	virtual ~CARPLayer();

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

	void	SetARPTableObject(CARPTable* arptbl);
	CARPTable*	m_ARPTable;
	
	void	SetOriginSrcEtherEnet(unsigned char* enet);
	ETHERNET_ADDR	org_srcenet;
	void	SetOriginSrcIPAddress(unsigned char* ip);
	IP_ADDR	org_srcipt;

	void	SendGARPMsg();
	void	SendARPMsg();

protected:
	ARP_HEADER	m_sHeader ;
	ETHERNET_ADDR	enet_broadcast;

protected:
	CEthernetLayer* m_Ether;
};

#endif // !defined(AFX_ARPLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
