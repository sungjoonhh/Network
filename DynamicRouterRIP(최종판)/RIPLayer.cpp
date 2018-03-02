#include "stdafx.h"
#include "DynamicRouterRIP.h"
#include "DynamicRouterRIPDlg.h"
#include "RIPLayer.h"

CRIPLayer::CRIPLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
	m_sHeader.command = 1;
	m_sHeader.version = 2;
	m_sHeader.table_entry[0].metric = 16;
}
CRIPLayer::~CRIPLayer()
{
}
void CRIPLayer::SetRouterTable(CRouterTable * routTable)
{
	m_RoutTable = routTable;
}


void CRIPLayer::ResetHeader()
{
	int i = 0;
	m_sHeader.command = 0;
	//m_sHeader.version = 0;
	m_sHeader.routing_domain = 0;
	for (i = 0; i < 24; i++){
		m_sHeader.table_entry[i].address_family = 0;
		memset(m_sHeader.table_entry[i].dst_ip, 0, 4);
		m_sHeader.table_entry[i].subnet_mask[0]=255;
		m_sHeader.table_entry[i].subnet_mask[1]=255;
		m_sHeader.table_entry[i].subnet_mask[2]=255;
		m_sHeader.table_entry[i].subnet_mask[3]=0;
		memset(m_sHeader.table_entry[i].next_hop_ip, 0, 4);
		m_sHeader.table_entry[i].route_tag = 0;
		m_sHeader.table_entry[i].metric = 0;
	}
}
BOOL CRIPLayer::Send(){
	BOOL bSuccess = FALSE;
	LPROUT_NODE currNode = m_RoutTable->rout_head;
	int i;
	m_sHeader.version = 2;
	if (m_sHeader.command == 1){
		for (i = 0; i < 24 && m_sHeader.table_entry[i].address_family == 1; i++){
			m_sHeader.table_entry[i].address_family = 0;
			memset(m_sHeader.table_entry[i].dst_ip, 0, 4);
			m_sHeader.table_entry[i].subnet_mask[0]=255;
			m_sHeader.table_entry[i].subnet_mask[1]=255;
			m_sHeader.table_entry[i].subnet_mask[2]=255;
			m_sHeader.table_entry[i].subnet_mask[3]=0;
			memset(m_sHeader.table_entry[i].next_hop_ip, 0, 4);
			m_sHeader.table_entry[i].route_tag = 0;
			m_sHeader.table_entry[i].metric = 0;
		}
		i = 0;
		while (currNode != NULL){
			if(!memcmp(ip, currNode->key.rout_router.S_un.s_ip_addr,4)) {
				currNode = currNode->next;
				continue;
			}
			m_sHeader.table_entry[i].metric = currNode->key.rout_metric;
			memcpy(m_sHeader.table_entry[i].dst_ip, currNode->key.rout_router.S_un.s_ip_addr,4);

			memcpy(m_sHeader.table_entry[i].next_hop_ip, ip2, 4);
			memcpy(m_sHeader.table_entry[i].subnet_mask, currNode->key.rout_subnet.S_un.s_ip_addr, 4);
			m_sHeader.table_entry[i].address_family = 1;
			i++;
			currNode = currNode->next;
		}
		bSuccess = mp_UnderLayer->Send((u_char*)&m_sHeader, sizeof(m_sHeader));
		m_sHeader.command = 2;
	}
	return bSuccess;
}
BOOL CRIPLayer:: Receive(unsigned char* ppayload){
	BOOL bSuccess = FALSE;
	BOOL check;
	PRIP_HEADER pFrame = (PRIP_HEADER)ppayload;
	LPROUT_NODE currNode;
	LPROUT_NODE newNode = (LPROUT_NODE)malloc(sizeof(ROUT_NODE));
	BOOL isChange = FALSE;
	int i = 0;
	int j = 0;
	unsigned char _interface[2];
	_interface[1] = '\0';
	if (dir)
		_interface[0] = 'L';
	else
		_interface[0] = 'R';

	while (pFrame->table_entry[i].address_family == 1){
		if( pFrame->table_entry[i].metric < 16)
			pFrame->table_entry[i].metric++;
		i++;
	}
	i = 0;

	currNode = m_RoutTable->rout_head;

	while (pFrame->table_entry[i].address_family == 1){
		while (currNode != NULL){
			if (currNode->key.rout_metric != 0 && !memcmp(pFrame->table_entry[i].dst_ip, currNode->key.rout_router.S_un.s_ip_addr, 4))
				currNode->key.rout_ttl = 18;
			currNode = currNode->next;
		}
		i++;
	}
	i = 0;
	while (pFrame->table_entry[i].address_family == 1){
		currNode = m_RoutTable->rout_head;
		while (currNode != NULL){
			if (!memcmp(currNode->key.rout_router.S_un.s_ip_addr, pFrame->table_entry[i].dst_ip, 4))
				break;
			currNode = currNode->next;
		}
		//없을시
		if (currNode == NULL){

			newNode->key.rout_metric = pFrame->table_entry[i].metric;
			memcpy(newNode->key.rout_subnet.S_un.s_ip_addr, pFrame->table_entry[i].subnet_mask, 4);
			memcpy(newNode->key.rout_router.S_un.s_ip_addr, pFrame->table_entry[i].dst_ip, 4);
			memcpy(newNode->key.rout_nexthop.S_un.s_ip_addr, pFrame->table_entry[i].next_hop_ip, 4);
			newNode->key.rout_router.S_un.s_ip_addr[3] = 0;
			memset(newNode->key.rout_outport.S_un.s_ip_addr,0, 4);
			newNode->key.rout_ttl = 18;
			memcpy(newNode->key.rout_interface, _interface, 2);
			m_RoutTable->RouterAddItem(newNode->key);
			//R,L 위로 올리기
			isChange = TRUE;
		}
		//있을시
		else{
			if (!memcmp(currNode->key.rout_nexthop.S_un.s_ip_addr, pFrame->table_entry[i].next_hop_ip,4)){// next-hop 이 같을 때.
				m_RoutTable->RouterDeleteItem(currNode->key.rout_router);								//next-hop = 0이라서 여기서 무조건 걸림
				newNode->key.rout_metric = pFrame->table_entry[i].metric;
				memcpy(newNode->key.rout_router.S_un.s_ip_addr, pFrame->table_entry[i].dst_ip, 4);
				memcpy(newNode->key.rout_subnet.S_un.s_ip_addr, pFrame->table_entry[i].subnet_mask, 4);
				memcpy(newNode->key.rout_nexthop.S_un.s_ip_addr, pFrame->table_entry[i].next_hop_ip, 4);
				newNode->key.rout_router.S_un.s_ip_addr[3] = 0;
				memset(newNode->key.rout_outport.S_un.s_ip_addr, 0, 4);
				newNode->key.rout_ttl = 18;
				memcpy(newNode->key.rout_interface, _interface, 2);
				m_RoutTable->RouterAddItem(newNode->key);
				//isChange = TRUE;
			}
			else{
				if (currNode->key.rout_metric > pFrame->table_entry[i].metric){
					m_RoutTable->RouterDeleteItem(currNode->key.rout_router);
					newNode->key.rout_metric = pFrame->table_entry[i].metric;
					memcpy(newNode->key.rout_router.S_un.s_ip_addr, pFrame->table_entry[i].dst_ip, 4);
					memcpy(newNode->key.rout_subnet.S_un.s_ip_addr, pFrame->table_entry[i].subnet_mask, 4);
					memcpy(newNode->key.rout_nexthop.S_un.s_ip_addr, pFrame->table_entry[i].next_hop_ip, 4);
					newNode->key.rout_router.S_un.s_ip_addr[3] = 0;
					memset(newNode->key.rout_outport.S_un.s_ip_addr, 0, 4);
					newNode->key.rout_ttl = 18;
					memcpy(newNode->key.rout_interface, _interface, 2);
					m_RoutTable->RouterAddItem(newNode->key);
					isChange = TRUE;
				}
			}
		}
		i++;
	}
	if (isChange){
		if (pFrame->command)
		m_sHeader.command = 1;
		mp_aUpperLayer[0]->Receive(_interface);
		isChange = FALSE;
	}
	return bSuccess;
}
