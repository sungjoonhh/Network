#include "stdafx.h"		//분석완료(한 부분 빼고)
#include "DynamicRouterRIP.h"
#include "RouterTable.h"

CRouterTable::CRouterTable()
{
	ResetHeader( ) ;
	rout_head = NULL;
}

CRouterTable::~CRouterTable()
{
}

void CRouterTable::ResetHeader()	//* 아이피 주소 & metric & outport 초기화.
{
	memset(m_sHeader.rout_router.addrs_i, 0, 4);
	m_sHeader.rout_metric = 0;
	m_sHeader.rout_outport = 0;
}

// Router Table
void CRouterTable::RouterAddItem(ROUT_ENTRY rout_entry)
{
	LPROUT_NODE currNode;
	LPROUT_NODE newNode = (LPROUT_NODE)malloc(sizeof(ROUT_NODE));

	newNode->key = rout_entry;
	newNode->next = NULL;
	
	CCriticalSection cs;

	cs.Lock();
	if(SearchIPAddress(rout_entry.rout_router.addrs_i))	//* 엔트리에 동일 IP같 있으면 return;
		return;

	// Router entry에 추가
	currNode = rout_head;
	if(rout_head == NULL)	//* 현재 테이블이 비어있을 경우.
	{
		newNode->next = rout_head;
		rout_head = newNode;
	}
	else					//* 테이블에 엔트리가 있을 경우.
	{
		while(currNode->next!=NULL)
			currNode = currNode->next;
		newNode->next = NULL;		
		currNode->next = newNode;	//마지막 부분에 새로운 엔트리 추가.
	}
	cs.Unlock();
}

void CRouterTable::RouterDeleteItem(IP_ADDR ip)
{
	LPROUT_NODE prevNode;
	LPROUT_NODE currNode;
	LPROUT_NODE tempNode;

	// Router entry에서 삭제
	prevNode = rout_head;
	currNode = rout_head;

	CCriticalSection cs;

	cs.Lock();
	if(rout_head->next==NULL)	//*엔트리가 한개 있을경우 맨 위에꺼 삭제
	{
		tempNode = rout_head;
		rout_head = rout_head->next;
		free(tempNode);
	}
	else
	{
		while(currNode!=NULL){
			if(!memcmp(currNode->key.rout_router.addrs_i,ip.addrs_i,4)){	//* 현재 노드가 지우려는것과 같은경우
				if(currNode == rout_head){	//* 현재 노드가 헤드인 경우
					tempNode = currNode;
					rout_head = currNode->next;
					free(tempNode);
					currNode = rout_head;
				}
				else{						//* 현재 노드가 헤드가 아닌 경우
					tempNode = currNode;
					prevNode->next = currNode->next;
					free(tempNode);
					currNode = prevNode;
				}
			}
			else{	//* 현재 노드가 지우려는것과 다른경우 다음으로 넘어감
				prevNode = currNode;
				currNode = currNode->next;
			}
		}
	}
	cs.Unlock();
}

void CRouterTable::RouterHopUpdateByTimeout(IP_ADDR ip)		//* 30초가 지나도 송신이 안오면(?) metric 수정하는 함수
{
	// Router table entry Hop 갱신
	LPROUT_NODE currNode;
	currNode = rout_head;

	CCriticalSection cs;

	cs.Lock();
	while(currNode!=NULL)		//* 테이블 엔트리 전부를 돈다.
	{
		if(!memcmp(currNode->key.rout_outport.addrs_i,ip.addrs_i,4))	//* ip와 테이블에 있는 것이 같을경우 metric을 16으로 바꿈.
		{
			// Hop Update status
			currNode->key.rout_metric = RIP_HOP_INFINITY;
		}
		currNode = currNode->next;
	}
	cs.Unlock();
}

void CRouterTable::RouterEntryUpdate(ROUT_ENTRY entry,IP_ADDR src_ip)	//* 여기 부분 중요한듯하다. ppt에 나와있는 부분인데 좀잇다가 다시 봐야함.
{
	// Router table entry Hop 갱신
	LPROUT_NODE currNode;
	currNode = rout_head;

	CCriticalSection cs;

	cs.Lock();
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.rout_outport.addrs_i,src_ip.addrs_i,4) &&
			!memcmp(currNode->key.rout_router.addrs_i,entry.rout_router.addrs_i,4))
		{
			// Hop Update status
			if((entry.rout_metric-1) >= RIP_HOP_INFINITY){
				currNode->key.rout_metric = RIP_HOP_INFINITY;
			}
			else if(currNode->key.rout_metric > entry.rout_metric)
			{
				currNode->key.rout_metric = entry.rout_metric;
				currNode->key.rout_ttl = -1;
			}
		}
		currNode = currNode->next;
	}
	cs.Unlock();
}

void CRouterTable::RouterEntryUpdateForRight(ROUT_ENTRY entry,IP_ADDR src_ip) //* 위에것과 똑같은데 critical section 유지를 위해 2개 만든듯.
{
	// Router table entry Hop 갱신
	LPROUT_NODE currNode;
	currNode = rout_head;

	CCriticalSection cs;

	cs.Lock();
	
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.rout_outport.addrs_i,src_ip.addrs_i,4) &&
			!memcmp(currNode->key.rout_router.addrs_i,entry.rout_router.addrs_i,4))
		{
			// Hop Update status
			if((entry.rout_metric-1) >= RIP_HOP_INFINITY)
				currNode->key.rout_metric = RIP_HOP_INFINITY;
			else if(currNode->key.rout_metric > entry.rout_metric)
			{
				currNode->key.rout_metric = entry.rout_metric;
				currNode->key.rout_ttl = -1;
			}
		}
		currNode = currNode->next;
	}
	cs.Unlock();
}

BOOL CRouterTable::SearchIPAddress(unsigned char* ip) //* 테이블 전체 싹 뒤져서 같은것 있으면 TRUE 아니면 FALSE
{
	LPROUT_NODE currNode;

	currNode = rout_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.rout_router.addrs_i,ip,4))
			return TRUE;
		
		currNode = currNode->next;
	}

	return FALSE;
}

IP_ADDR CRouterTable::SubnetMask(u_char * ip, IP_ADDR subnet)	//* 서브넷 ( 서브넷이 언제 필요한지 확인하기)
{
	IP_ADDR masking_addr;
	masking_addr.addr_i0 = ip[0] & subnet.addr_i0;
	masking_addr.addr_i1 = ip[1] & subnet.addr_i1;
	masking_addr.addr_i2 = ip[2] & subnet.addr_i2;
	masking_addr.addr_i3 = ip[3] & subnet.addr_i3;
	return masking_addr;
}

LPROUT_NODE CRouterTable::GetRouterEntryByIP(u_char * ip_dst_addr)	//* ip로 entry 찾기
{
	LPROUT_NODE currNode;
	currNode = rout_head;

	LPROUT_NODE temp;
	IP_ADDR ab;
	ab.addr_i0 = 255;
	ab.addr_i1 = 255;
	ab.addr_i2 = 255;
	ab.addr_i3 = 0;
		
	while(currNode!=NULL)
	{
		IP_ADDR a = SubnetMask(currNode->key.rout_router.addrs_i,ab);
		IP_ADDR b = SubnetMask(ip_dst_addr,ab);
		if(!memcmp(a.addrs_i,b.addrs_i,4) && currNode->key.rout_metric!=0)
		{
			return currNode;
		}
		currNode = currNode->next;
	}
	return NULL;
}