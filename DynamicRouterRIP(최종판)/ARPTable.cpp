// EthernetLayer.cpp: implementation of the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicRouterRIP.h"
#include "ARPTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CARPTable::CARPTable()
{
	ResetHeader( ) ;
	cache_head = NULL;
	proxy_head = NULL;
}

CARPTable::~CARPTable()
{
}

void CARPTable::ResetHeader()
{
	m_sHeader.arp_hardtype = 0x0000;
	m_sHeader.arp_prottype = 0x0000;
	m_sHeader.arp_hardsize = 0x06;
	m_sHeader.arp_protsize = 0x04;
	m_sHeader.arp_op = 0x0000;
	memset(m_sHeader.arp_srcenet.addrs_e, 0, 6);
	memset(m_sHeader.arp_srcip.addrs_i, 0, 4);
	memset(m_sHeader.arp_dstenet.addrs_e, 0, 6);
	memset(m_sHeader.arp_dstip.addrs_i, 0, 4);
}

void CARPTable::SetHardType(unsigned short hardtype)
{
	m_sHeader.arp_hardtype = hardtype;
}
void CARPTable::SetProtType(unsigned short prottype)
{
	m_sHeader.arp_prottype = prottype;
}

void CARPTable::SetOpcode(unsigned short op)
{
	m_sHeader.arp_op = op;
}
void CARPTable::SetSenderEtherAddress(unsigned char* enet)
{
	ETHERNET_ADDR s_ether;
	s_ether.addr_e0 = enet[0];
	s_ether.addr_e1 = enet[1];
	s_ether.addr_e2 = enet[2];
	s_ether.addr_e3 = enet[3];
	s_ether.addr_e4 = enet[4];
	s_ether.addr_e5 = enet[5];

	memcpy(m_sHeader.arp_srcenet.addrs_e, s_ether.addrs_e, 6);
}
void CARPTable::SetSenderIPAddress(unsigned char* ip)
{
	IP_ADDR s_ip;
	s_ip.addr_i0 = ip[0];
	s_ip.addr_i1 = ip[1];
	s_ip.addr_i2 = ip[2];
	s_ip.addr_i3 = ip[3];

	memcpy(m_sHeader.arp_srcip.addrs_i, s_ip.addrs_i, 4);
}
void CARPTable::SetTargetEtherAddress(unsigned char* enet)
{
	ETHERNET_ADDR t_ether;
	t_ether.addr_e0 = enet[0];
	t_ether.addr_e1 = enet[1];
	t_ether.addr_e2 = enet[2];
	t_ether.addr_e3 = enet[3];
	t_ether.addr_e4 = enet[4];
	t_ether.addr_e5 = enet[5];

	memcpy(m_sHeader.arp_dstenet.addrs_e, t_ether.addrs_e, 6);
}
void CARPTable::SetTargetIPAddress(unsigned char* ip)
{
	IP_ADDR t_ip;
	t_ip.addr_i0 = ip[0];
	t_ip.addr_i1 = ip[1];
	t_ip.addr_i2 = ip[2];
	t_ip.addr_i3 = ip[3];

	memcpy(m_sHeader.arp_dstip.addrs_i, t_ip.addrs_i, 4);
}

unsigned char* CARPTable::GetSenderEtherAddress()
{
	return m_sHeader.arp_srcenet.addrs_e;
}
unsigned char* CARPTable::GetSenderIPAddress()
{
	return m_sHeader.arp_srcip.addrs_i;
}
unsigned char* CARPTable::GetTargetEtherAddress()
{
	return m_sHeader.arp_dstenet.addrs_e;
}
unsigned char* CARPTable::GetTargetIPAddress()
{
	return m_sHeader.arp_dstip.addrs_i;
}

void CARPTable::SetSrcIPAddress(unsigned char* ip)
{
	memcpy(src_ip,ip,4);
}
void CARPTable::SetOtherIPAddress(unsigned char* ip)
{
	memcpy(other_ip,ip,4);
}

// Cache Table
int CARPTable::CacheAddItem(unsigned char* ppayload)
{
	CACHE_ENTRY cache;
	LPARP_NODE currNode;
	LPARP_NODE newNode = (LPARP_NODE)malloc(sizeof(ARP_NODE));

	if(ppayload == NULL)
	{
		// Cache entry 노드 생성
		cache.cache_enetaddr = m_sHeader.arp_dstenet;
		cache.cache_ipaddr = m_sHeader.arp_dstip;
		memcpy(cache.cache_interface,"Unknown",RT_DEVICE_NAME_LENGTH);
		memcpy(cache.cache_flag,"U",4);
	}
	else
	{
		LPROUT_NODE rout_node;
		PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
		cache.cache_enetaddr = pFrame->arp_srcenet;
		cache.cache_ipaddr = pFrame->arp_srcip;
		
		if( FALSE /*(rout_node = m_RoutTable->GetRoutNodeByGate(pFrame->arp_srcip))!=NULL*/)
		{
			// 장비이름과 flag 설정
//			memcpy(cache.cache_interface,rout_node->key.rout_interface,RT_DEVICE_NAME_LENGTH);
//			memcpy(cache.cache_flag,rout_node->key.rout_flag,4);
		}
		else
		{
			memcpy(cache.cache_interface,"Unknown",RT_DEVICE_NAME_LENGTH);
			memcpy(cache.cache_flag,"U",4);
		}
	}	
	newNode->key = cache;
	newNode->next = NULL;

	// IP 중복 검사
	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,newNode->key.cache_ipaddr.addrs_i,4))
			return -1;
		currNode = currNode->next;
	}
	if(!memcmp(cache.cache_ipaddr.addrs_i,src_ip,4) || !memcmp(cache.cache_ipaddr.addrs_i,other_ip,4))
		return -1;
	
	if(!memcmp(m_sHeader.arp_dstip.addrs_i,src_ip,4) || !memcmp(m_sHeader.arp_dstip.addrs_i,other_ip,4))
		return -1;

	// Cache entry에 추가
	currNode = cache_head;
	if(cache_head == NULL)
	{
		newNode->next = cache_head;
		cache_head = newNode;
	}
	else
	{
		while(currNode->next!=NULL)
			currNode = currNode->next;
		newNode->next = currNode->next;
		currNode->next = newNode;
	}

	return newNode->key.cache_ipaddr.addr_i3;
}

void CARPTable::CacheDeleteItem(int index)
{
	LPARP_NODE currNode;
	LPARP_NODE tempNode;

	int nodeIndex = 0;

	currNode = cache_head;
	// Cache entry에서 삭제	
	if(currNode==NULL)
		return;

	if(index==0)
	{
		tempNode = cache_head;
		cache_head = cache_head->next;
		free(tempNode);
	}
	else
	{
		index--;
		while(currNode->next!=NULL){
			if(index == nodeIndex)
				break;
			currNode = currNode->next;
			nodeIndex++;
		}
		tempNode = currNode->next;
		currNode->next = tempNode->next;
		free(tempNode);
	}
}

void CARPTable::CacheAllDelete()
{
	// Cache entry 모두 삭제
	LPARP_NODE tempNode;
	while(cache_head!=NULL)
	{
		tempNode = cache_head;
		cache_head = cache_head->next;
		free(tempNode);
	}
}

void CARPTable::CacheDeleteByTimer(int tID)
{
	LPARP_NODE currNode;
	LPARP_NODE tempNode;

	int ipIndex = 0;

	// Cache entry에서 삭제
	currNode = cache_head;
	if(currNode==NULL)
		return;

	if(currNode->next==NULL)
	{
		tempNode = cache_head;
		cache_head = cache_head->next;
		free(tempNode);
	}
	else
	{
		while(currNode->next!=NULL){
			if(currNode == cache_head && currNode->key.cache_ipaddr.addr_i3==tID)
			{
				tempNode = currNode;
				currNode = tempNode->next;
				free(tempNode);
				cache_head = currNode;
				break;
			}
			if(currNode->next->key.cache_ipaddr.addr_i3==tID)
			{
				tempNode = currNode->next;
				currNode->next = tempNode->next;
				free(tempNode);
				break;
			}		
			currNode = currNode->next;
		}
	}
}

int CARPTable::CacheUpdate(unsigned char* ppayload)
{
	// Cache table 갱신
	// Cache table의 Mac address를 갱신하고, 타이머 번호를 return해준다.
	PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
	LPARP_NODE currNode;
	LPROUT_NODE rout_node;

	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,pFrame->arp_srcip.addrs_i,4))
		{
			// Update ethernet address and status
			memcpy(currNode->key.cache_enetaddr.addrs_e,pFrame->arp_srcenet.addrs_e,6);
			if( TRUE /*(rout_node = m_RoutTable->GetRoutNodeByGate(currNode->key.cache_ipaddr))==NULL*/)
			{
				break;
			}
			else
			{
				break;
			}
		}
		currNode = currNode->next;
	}
	if(currNode==NULL) // 들어온 ip주소가 Cache table에 없는 경우 : 나에게 ARP를 요청한 경우
		return pFrame->arp_srcip.addr_i3+255;
	return currNode->key.cache_ipaddr.addr_i3;
}

BOOL CARPTable::CacheSearchExist(IP_ADDR ip)
{
	// Cache table에 찾고자 하는 ip가 있는지 검사
	// 찾으면 TRUE, 못찾으면 FALSE
	LPARP_NODE currNode;

	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,ip.addrs_i,4))
		{
			return TRUE;
		}
		currNode = currNode->next;
	}
	return FALSE;
}

LPARP_NODE CARPTable::GetCacheNode(IP_ADDR ip)
{
	// Cache table에 있는 etnry를 return.
	LPARP_NODE currNode;
	unsigned char ether[6];
	memset(ether,0,6);

	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,ip.addrs_i,4))
		{
			if(memcmp(currNode->key.cache_enetaddr.addrs_e,ether,6))
			{
				return currNode;
			}
		}
		currNode = currNode->next;
	}
	return NULL;
}

// Proxy Table
void CARPTable::ProxyAddItem(PROXY_ENTRY proxy)
{
	// Proxy table에 추가
	LPPARP_NODE currNode;
	LPPARP_NODE newNode = (LPPARP_NODE)malloc(sizeof(PARP_NODE));

	newNode->key = proxy;
	newNode->next = NULL;

	// IP 중복 검사
	currNode = proxy_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.proxy_ipaddr.addrs_i,newNode->key.proxy_ipaddr.addrs_i,4))
		{
			return;
		}
		currNode = currNode->next;
	}

	// Proxy entry에 추가
	currNode = proxy_head;
	if(proxy_head == NULL)
	{
		newNode->next = proxy_head;
		proxy_head = newNode;
	}
	else
	{
		while(currNode->next!=NULL)
			currNode = currNode->next;
		newNode->next = currNode->next;
		currNode->next = newNode;
	}
}

void CARPTable::ProxyDeleteItem(int index)
{
	LPPARP_NODE currNode;
	LPPARP_NODE tempNode;

	int nodeIndex = 0;	

	// Proxy entry에서 삭제
	currNode = proxy_head;
	if(index==0)
	{
		tempNode = proxy_head;
		proxy_head = proxy_head->next;
		free(tempNode);
	}
	else
	{
		index--;
		while(currNode->next!=NULL){
			if(index == nodeIndex)
				break;
			currNode = currNode->next;
			nodeIndex++;
		}
		tempNode = currNode->next;
		currNode->next = tempNode->next;
		free(tempNode);
	}
}

BOOL CARPTable::ProxySearchExist(IP_ADDR ip)
{
	// Proxy table에 찾고자 하는 ip가 있는지 검사
	// 찾으면 TRUE, 못찾으면 FALSE
	LPPARP_NODE currNode;

	currNode = proxy_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.proxy_ipaddr.addrs_i,ip.addrs_i,4))
		{
			return TRUE;
		}
		currNode = currNode->next;
	}
	return FALSE;
}

void CARPTable::SetRoutTableObject(CRouterTable* routetbl)
{
	m_RoutTable = routetbl;
}