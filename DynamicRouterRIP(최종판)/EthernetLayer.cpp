// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicRouterRIP.h"
#include "EthernetLayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset( m_sHeader.enet_dstaddr.addrs_e, 0, 6 ) ;
	memset( m_sHeader.enet_srcaddr.addrs_e, 0, 6 ) ;
	memset( m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE ) ;
	m_sHeader.enet_frametype = ntohs(ARP_PROTOTYPE_IP);
}

unsigned char* CEthernetLayer::GetEnetDstAddress() // Ethernet Destination Address
{
	return m_sHeader.enet_srcaddr.addrs_e;
}

unsigned char* CEthernetLayer::GetEnetSrcAddress() // Ethernet Source Address
{
	return m_sHeader.enet_dstaddr.addrs_e;
}

unsigned short CEthernetLayer::GetEnetFrameType()
{
	return m_sHeader.enet_frametype;
}

void CEthernetLayer::SetEnetSrcAddress(unsigned char *pAddress)
{
	ETHERNET_ADDR src_ether;
	src_ether.addr_e0 = pAddress[0];
	src_ether.addr_e1 = pAddress[1];
	src_ether.addr_e2 = pAddress[2];
	src_ether.addr_e3 = pAddress[3];
	src_ether.addr_e4 = pAddress[4];
	src_ether.addr_e5 = pAddress[5];

	memcpy( m_sHeader.enet_srcaddr.addrs_e, src_ether.addrs_e, 6 ) ;
}

void CEthernetLayer::SetEnetDstAddress(unsigned char *pAddress)
{
	ETHERNET_ADDR dst_ether;
	dst_ether.addr_e0 = pAddress[0];
	dst_ether.addr_e1 = pAddress[1];
	dst_ether.addr_e2 = pAddress[2];
	dst_ether.addr_e3 = pAddress[3];
	dst_ether.addr_e4 = pAddress[4];
	dst_ether.addr_e5 = pAddress[5];

	memcpy( m_sHeader.enet_dstaddr.addrs_e, dst_ether.addrs_e, 6 ) ;
}

void CEthernetLayer::SetEnetFrameType( unsigned short frame )
{
	m_sHeader.enet_frametype = frame;
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;
	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,nlength+ETHER_HEADER_SIZE);

	return bSuccess ;
}

BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	// 하위 계층에서 받은 payload를 현재 계층의 header구조에 맞게 읽음.
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload ;

	BOOL bSuccess = FALSE ;

	// broadcast 주소 (ff:ff:ff:ff:ff:ff)
	unsigned char broadcast[6];
	memset(broadcast,0xff,6);

	// 나에게 온 Ethernet Address가 아니면 discard 한다.
	if((!memcmp((char *)pFrame->enet_dstaddr.addrs_e,(char *)m_sHeader.enet_srcaddr.addrs_e,6) &&
		memcmp((char *)pFrame->enet_srcaddr.addrs_e,(char *)m_sHeader.enet_srcaddr.addrs_e,6)) ||
		!memcmp((char *)pFrame->enet_dstaddr.addrs_e,(char *)broadcast,6))
	{
		if(ntohs(pFrame->enet_frametype) == ARP_PROTOTYPE_IP)
			bSuccess = mp_aUpperLayer[1]->Receive((unsigned char*) pFrame->enet_data);
		else if(ntohs(pFrame->enet_frametype) == ARP_PROTOTYPE_ARP)
			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) pFrame->enet_data);
	}
	return bSuccess ;
}
