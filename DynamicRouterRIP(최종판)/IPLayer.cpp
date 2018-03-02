// IPLayer.cpp: implementation of the CIPLayer class.
//
//////////////////////////////////////////////////////////////////////
// 분석중
#include "stdafx.h"
#include "DynamicRouterRIP.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPLayer::CIPLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
	
	multicast_addr[0] = 224;
	multicast_addr[1] = 0;
	multicast_addr[2] = 0;
	multicast_addr[3] = 9;
	
}

CIPLayer::~CIPLayer()
{
}

void CIPLayer::ResetHeader()
{
	m_Icmp = new CICMP();

	m_sHeader.ip_verlen = 0x45;
	m_sHeader.ip_tos = 0x00;
	m_sHeader.ip_len = 0x0000;
	m_sHeader.ip_id = 0x0000;
	m_sHeader.ip_fragoff = 0x0000;
	m_sHeader.ip_ttl = 0x01;
	m_sHeader.ip_proto = IP_PROTOTYPE_UDP;
	m_sHeader.ip_cksum = 0x00;
	memset(m_sHeader.ip_src, 0, 4);
	m_sHeader.ip_dst[0] = 224;	//
	m_sHeader.ip_dst[1] = 0;	//
	m_sHeader.ip_dst[2] = 0;	//
	m_sHeader.ip_dst[3] = 9;	//* 멀티캐스팅 주소를 사용하기 위함.

	memset(m_sHeader.ip_data, 0, IP_DATA_SIZE);
}

// IP의 checksum 계산
u_short CIPLayer::IP_Checksum(u_short* data, int len)	//* ICMP의 checksum과 동일
{
	u_long usChksum = 0;
	while (len>1)
	{
		usChksum += *data++;
		len -= sizeof(u_short);
	}

	if (len)
		usChksum += *(u_char*)data;

	//Complete the calculations
	usChksum = (usChksum >> 16) + (usChksum & 0xffff);
	usChksum += (usChksum >> 16);

	// Return the value (inversed)
	return (u_short)(~usChksum);
}

void CIPLayer::SetSrcIPAddress(unsigned char* src_ip)
{
	memcpy(m_sHeader.ip_src, src_ip, 4);
}

void CIPLayer::SetDstIPAddress(unsigned char* dst_ip)
{
	memcpy(m_sHeader.ip_dst, dst_ip, 4);
}

void CIPLayer::SetARPobj(CARPLayer * arp)
{
	m_ARP = arp;
}
void CIPLayer::SetICMPobj(CICMP* icmp)
{
	m_Icmp = icmp;
}

void CIPLayer::SetOtherIPobj(CIPLayer* ip)
{
	m_otherIp = ip;
}

void CIPLayer::SetRouterTable(CRouterTable * routTable)
{
	m_RoutTable = routTable;
}

void CIPLayer::SetARPTable(CARPTable * arpTable)
{
	m_ARPTable = arpTable;
}

void CIPLayer::TransferIPPacket(u_char* ppayload)	//*패킷 전달하는거
{
	CCriticalSection cs;
	PIPLayer_HEADER pFrame = (PIPLayer_HEADER)ppayload;

	if (!memcmp(pFrame->ip_dst, m_sHeader.ip_src, 4))
	{
		m_otherIp->ICMP_ReplyMSG(ppayload);
	}

	// IP Header 생성
	IPLayer_HEADER ip_header;
	ip_header.ip_verlen = pFrame->ip_verlen;
	ip_header.ip_tos = pFrame->ip_tos;
	ip_header.ip_len = pFrame->ip_len;
	ip_header.ip_id = pFrame->ip_id;
	ip_header.ip_fragoff = pFrame->ip_fragoff;
	ip_header.ip_ttl = pFrame->ip_ttl - 1;		//* 전달이기 때문에 ttl값 1 감소
	ip_header.ip_proto = pFrame->ip_proto;
	ip_header.ip_cksum = 0;
	memcpy(ip_header.ip_src, pFrame->ip_src, 4);
	memcpy(ip_header.ip_dst, pFrame->ip_dst, 4);
	ip_header.ip_cksum = IP_Checksum((u_short*)&ip_header, IP_HEADER_SIZE);	//*IP헤더 부분의 Check부분만 검사
	memcpy(ip_header.ip_data, pFrame->ip_data, ICMP_HEADER_SIZE + ICMP_DATA_SIZE);

	// 목적 주소에 대한 엔트리가 테이블에 있는지 검사
	LPROUT_NODE rout_entry = m_RoutTable->GetRouterEntryByIP(pFrame->ip_dst);	//* 목적지 ip로 route entry 찾는다.
	IP_ADDR nexthop_addr;
	memset(nexthop_addr.addrs_i, 0, 4);

	if (rout_entry == NULL){		//* 엔트리에 해당 ip가 없을 경우
		IP_ADDR aa; memcpy(aa.addrs_i, pFrame->ip_dst, 4);
		LPARP_NODE arp_entry = m_ARPTable->GetCacheNode(aa);
		if (arp_entry != NULL)
			nexthop_addr = arp_entry->key.cache_ipaddr;
	}
	else{
		nexthop_addr = rout_entry->key.rout_nexthop;
	}

	m_ARP->SetSenderIPAddress(pFrame->ip_dst);
	m_ARP->SetTargetIPAddress(nexthop_addr.addrs_i);
	m_ARP->Send((unsigned char*)&ip_header, IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMP_DATA_SIZE);
}

void CIPLayer::ICMP_ReplyMSG(u_char * ppayload)
{
	PIPLayer_HEADER pFrame = (PIPLayer_HEADER)ppayload;
	IPLayer_HEADER ip_header;
	ip_header.ip_verlen = pFrame->ip_verlen;
	ip_header.ip_tos = pFrame->ip_tos;
	ip_header.ip_len = pFrame->ip_len;
	ip_header.ip_id = pFrame->ip_id;
	ip_header.ip_fragoff = pFrame->ip_fragoff;
	ip_header.ip_ttl = (pFrame->ip_ttl / 2) + 1;
	ip_header.ip_proto = pFrame->ip_proto;
	ip_header.ip_cksum = 0;
	memcpy(ip_header.ip_src, pFrame->ip_dst, 4);
	memcpy(ip_header.ip_dst, pFrame->ip_src, 4);
	ip_header.ip_cksum = IP_Checksum((u_short*)&ip_header, IP_HEADER_SIZE);
	memcpy(ip_header.ip_data, m_Icmp->ICMP_SetReplyMSG(pFrame->ip_data), ICMP_HEADER_SIZE + ICMP_DATA_SIZE);

	LPROUT_NODE rout_entry = m_RoutTable->GetRouterEntryByIP(pFrame->ip_src);
	IP_ADDR nexthop_addr;
	memset(nexthop_addr.addrs_i, 0, 4);

	if (rout_entry == NULL){
		IP_ADDR aa; memcpy(aa.addrs_i, pFrame->ip_src, 4);
		LPARP_NODE arp_entry = m_ARPTable->GetCacheNode(aa);
		if (arp_entry != NULL)
			nexthop_addr = arp_entry->key.cache_ipaddr;
	}
	else{
		nexthop_addr = rout_entry->key.rout_nexthop;
	}
	m_ARP->SetSenderIPAddress(pFrame->ip_dst);
	m_ARP->SetTargetIPAddress(nexthop_addr.addrs_i);
	m_ARP->Send((unsigned char*)&ip_header, IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMP_DATA_SIZE);
}


BOOL CIPLayer::Send(unsigned char* ppayload, int nlength)
{
	m_sHeader.ip_cksum = 0;
	memset(m_sHeader.ip_data, 0, IP_DATA_SIZE);

	m_sHeader.ip_cksum = IP_Checksum((u_short*)&m_sHeader, IP_HEADER_SIZE);
	memcpy(m_sHeader.ip_data, ppayload, nlength);

	BOOL bSuccess = FALSE;
	bSuccess = mp_UnderLayer->Send((u_char*)&m_sHeader, nlength + IP_HEADER_SIZE);

	return bSuccess;
}

BOOL CIPLayer::Receive(unsigned char* ppayload)
{
	PIPLayer_HEADER pFrame = (PIPLayer_HEADER)ppayload;
	u_char default[4];
	memset(default, 0xff, 4);

	BOOL bSuccess = FALSE;

	// 목적주소가 자신이며, 출발주소가 자신이 아닌 패킷이거나,
	// 목적주소가 0.0.0.0이며, 출발주소가 자신이 아닌 패킷만 통과
	if ((memcmp((char *)pFrame->ip_dst, (char *)m_sHeader.ip_src, 4) == 0 &&
		memcmp((char *)pFrame->ip_src, (char *)m_sHeader.ip_src, 4) != 0) ||
		(memcmp((char *)pFrame->ip_dst, (char *)default, 4) == 0 &&
		memcmp((char *)pFrame->ip_src, (char *)m_sHeader.ip_src, 4) != 0) ||
		(memcmp((char *)pFrame->ip_dst, (char *)multicast_addr, 4) == 0 &&
		memcmp((char *)pFrame->ip_src, (char *)m_sHeader.ip_src, 4) != 0))
	{
		if (pFrame->ip_proto == IP_PROTOTYPE_UDP)
			bSuccess = mp_aUpperLayer[0]->Receive((u_char *)pFrame->ip_data);
	}

	else{
		if (pFrame->ip_proto == IP_PROTOTYPE_ICMP){
			if (pFrame->ip_data[0] == ICMP_TYPE_REQUEST){
				if (!memcmp(pFrame->ip_dst, m_sHeader.ip_src, 4)){ // 자신에게 온 것이면,
					// ICMP Reply 메시지 작성하여 Send
					ICMP_ReplyMSG(ppayload);
				}
				else{ // 자신에게 온 것이 아니면,
					// RIP테이블을 검색한다.
					// pFrame의 목적주소를 이용해서,
					m_otherIp->TransferIPPacket(ppayload);
				}
			}
			if (pFrame->ip_data[0] == ICMP_TYPE_REPLY)
			{
				if (memcmp(pFrame->ip_dst, m_sHeader.ip_src, 4)){
					m_otherIp->TransferIPPacket(ppayload);
				}
			}
		}
	}

	return bSuccess;
}