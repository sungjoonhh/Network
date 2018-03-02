#include "stdafx.h"		//�м��Ϸ�(�� �κ� ����)
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

void CRouterTable::ResetHeader()	//* ������ �ּ� & metric & outport �ʱ�ȭ.
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
	if(SearchIPAddress(rout_entry.rout_router.addrs_i))	//* ��Ʈ���� ���� IP�� ������ return;
		return;

	// Router entry�� �߰�
	currNode = rout_head;
	if(rout_head == NULL)	//* ���� ���̺��� ������� ���.
	{
		newNode->next = rout_head;
		rout_head = newNode;
	}
	else					//* ���̺� ��Ʈ���� ���� ���.
	{
		while(currNode->next!=NULL)
			currNode = currNode->next;
		newNode->next = NULL;		
		currNode->next = newNode;	//������ �κп� ���ο� ��Ʈ�� �߰�.
	}
	cs.Unlock();
}

void CRouterTable::RouterDeleteItem(IP_ADDR ip)
{
	LPROUT_NODE prevNode;
	LPROUT_NODE currNode;
	LPROUT_NODE tempNode;

	// Router entry���� ����
	prevNode = rout_head;
	currNode = rout_head;

	CCriticalSection cs;

	cs.Lock();
	if(rout_head->next==NULL)	//*��Ʈ���� �Ѱ� ������� �� ������ ����
	{
		tempNode = rout_head;
		rout_head = rout_head->next;
		free(tempNode);
	}
	else
	{
		while(currNode!=NULL){
			if(!memcmp(currNode->key.rout_router.addrs_i,ip.addrs_i,4)){	//* ���� ��尡 ������°Ͱ� �������
				if(currNode == rout_head){	//* ���� ��尡 ����� ���
					tempNode = currNode;
					rout_head = currNode->next;
					free(tempNode);
					currNode = rout_head;
				}
				else{						//* ���� ��尡 ��尡 �ƴ� ���
					tempNode = currNode;
					prevNode->next = currNode->next;
					free(tempNode);
					currNode = prevNode;
				}
			}
			else{	//* ���� ��尡 ������°Ͱ� �ٸ���� �������� �Ѿ
				prevNode = currNode;
				currNode = currNode->next;
			}
		}
	}
	cs.Unlock();
}

void CRouterTable::RouterHopUpdateByTimeout(IP_ADDR ip)		//* 30�ʰ� ������ �۽��� �ȿ���(?) metric �����ϴ� �Լ�
{
	// Router table entry Hop ����
	LPROUT_NODE currNode;
	currNode = rout_head;

	CCriticalSection cs;

	cs.Lock();
	while(currNode!=NULL)		//* ���̺� ��Ʈ�� ���θ� ����.
	{
		if(!memcmp(currNode->key.rout_outport.addrs_i,ip.addrs_i,4))	//* ip�� ���̺� �ִ� ���� ������� metric�� 16���� �ٲ�.
		{
			// Hop Update status
			currNode->key.rout_metric = RIP_HOP_INFINITY;
		}
		currNode = currNode->next;
	}
	cs.Unlock();
}

void CRouterTable::RouterEntryUpdate(ROUT_ENTRY entry,IP_ADDR src_ip)	//* ���� �κ� �߿��ѵ��ϴ�. ppt�� �����ִ� �κ��ε� ���մٰ� �ٽ� ������.
{
	// Router table entry Hop ����
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

void CRouterTable::RouterEntryUpdateForRight(ROUT_ENTRY entry,IP_ADDR src_ip) //* �����Ͱ� �Ȱ����� critical section ������ ���� 2�� �����.
{
	// Router table entry Hop ����
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

BOOL CRouterTable::SearchIPAddress(unsigned char* ip) //* ���̺� ��ü �� ������ ������ ������ TRUE �ƴϸ� FALSE
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

IP_ADDR CRouterTable::SubnetMask(u_char * ip, IP_ADDR subnet)	//* ����� ( ������� ���� �ʿ����� Ȯ���ϱ�)
{
	IP_ADDR masking_addr;
	masking_addr.addr_i0 = ip[0] & subnet.addr_i0;
	masking_addr.addr_i1 = ip[1] & subnet.addr_i1;
	masking_addr.addr_i2 = ip[2] & subnet.addr_i2;
	masking_addr.addr_i3 = ip[3] & subnet.addr_i3;
	return masking_addr;
}

LPROUT_NODE CRouterTable::GetRouterEntryByIP(u_char * ip_dst_addr)	//* ip�� entry ã��
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