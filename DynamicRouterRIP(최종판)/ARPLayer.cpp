// EthernetLayer.cpp: implementation of the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicRouterRIP.h"
#include "ARPLayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CARPLayer::CARPLayer( char* pName )
	: CBaseLayer( pName )
{
	ResetHeader( ) ;
	memset(enet_broadcast.addrs_e,0xff,6);
}

CARPLayer::~CARPLayer()
{
}

void CARPLayer::ResetHeader()
{
	m_sHeader.arp_hardtype = 0x0001;
	m_sHeader.arp_prottype = 0x0800;
	m_sHeader.arp_hardsize = 0x06;
	m_sHeader.arp_protsize = 0x04;
	m_sHeader.arp_op = 0x0000;
	memset(m_sHeader.arp_srcenet.addrs_e, 0, 6);
	memset(m_sHeader.arp_srcip.addrs_i, 0, 4);
	memset(m_sHeader.arp_dstenet.addrs_e, 0, 6);
	memset(m_sHeader.arp_dstip.addrs_i, 0, 4);
}

void CARPLayer::SetHardType(unsigned short hardtype)
{
	m_sHeader.arp_hardtype = hardtype;
}
void CARPLayer::SetProtType(unsigned short prottype)
{
	m_sHeader.arp_prottype = prottype;
}

void CARPLayer::SetOpcode(unsigned short op)
{
	m_sHeader.arp_op = op;
}
void CARPLayer::SetSenderEtherAddress(unsigned char* enet)
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
void CARPLayer::SetSenderIPAddress(unsigned char* ip)
{
	IP_ADDR s_ip;
	s_ip.addr_i0 = ip[0];
	s_ip.addr_i1 = ip[1];
	s_ip.addr_i2 = ip[2];
	s_ip.addr_i3 = ip[3];

	memcpy(m_sHeader.arp_srcip.addrs_i, s_ip.addrs_i, 4);
}
void CARPLayer::SetTargetEtherAddress(unsigned char* enet)
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
void CARPLayer::SetTargetIPAddress(unsigned char* ip)
{
	IP_ADDR t_ip;
	t_ip.addr_i0 = ip[0];
	t_ip.addr_i1 = ip[1];
	t_ip.addr_i2 = ip[2];
	t_ip.addr_i3 = ip[3];

	memcpy(m_sHeader.arp_dstip.addrs_i, t_ip.addrs_i, 4);
}

unsigned char* CARPLayer::GetSenderEtherAddress()
{
	return m_sHeader.arp_srcenet.addrs_e;
}
unsigned char* CARPLayer::GetSenderIPAddress()
{
	return m_sHeader.arp_srcip.addrs_i;
}
unsigned char* CARPLayer::GetTargetEtherAddress()
{
	return m_sHeader.arp_dstenet.addrs_e;
}
unsigned char* CARPLayer::GetTargetIPAddress()
{
	return m_sHeader.arp_dstip.addrs_i;
}

void CARPLayer::SetARPTableObject(CARPTable* arptbl)
{
	m_ARPTable = arptbl;
}

void CARPLayer::SetOriginSrcIPAddress(unsigned char* ip)
{
	memcpy(org_srcipt.addrs_i,ip,4);
}

void CARPLayer::SendGARPMsg()
{
	m_Ether = (CEthernetLayer *)GetUnderLayer();
	m_Ether->SetEnetDstAddress(enet_broadcast.addrs_e);
	m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_ARP));

	ARP_HEADER t_sHeader;
	t_sHeader.arp_hardtype = ntohs(ARP_HARDTYPE);
	t_sHeader.arp_prottype = ntohs(ARP_PROTOTYPE_IP);
	t_sHeader.arp_hardsize = 0x06;
	t_sHeader.arp_protsize = 0x04;
	t_sHeader.arp_op = ntohs(ARP_OPCODE_REQUEST);
	memcpy(t_sHeader.arp_srcenet.addrs_e,org_srcenet.addrs_e, 6);
	memcpy(t_sHeader.arp_srcip.addrs_i, org_srcipt.addrs_i, 4);
	memset(t_sHeader.arp_dstenet.addrs_e, 0, 6);
	memcpy(t_sHeader.arp_dstip.addrs_i, org_srcipt.addrs_i, 4);

	mp_UnderLayer->Send((u_char*)&t_sHeader,ARP_HEADER_SIZE);
	m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_IP));
}

BOOL CARPLayer::Send(unsigned char *ppayload, int nlength)
{
	BOOL bSuccess = FALSE ;
	int index;
	unsigned char ether[6];
	memset(ether,0,6);

	LPARP_NODE arp_node;

	// ARP Table�� ���� �ּҰ� �������� �ʴ� ���
	if((arp_node = m_ARPTable->GetCacheNode(m_sHeader.arp_dstip))==NULL)
	{
		// ARP �޽����� ������ �ּҸ� �˾� �´�.
		// ARP �޽��� �ۼ�
		unsigned char broadcast[6];
		memset(broadcast,0xff,6);

		// Ethernet �����ּҿ� frame type ����
		m_Ether = (CEthernetLayer *)mp_UnderLayer;
		m_Ether->SetEnetDstAddress(broadcast);
		m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_ARP));

		// ARP table�� �߰�
		m_ARPTable->SetHardType(ntohs(ARP_HARDTYPE));
		m_ARPTable->SetProtType(ntohs(ARP_PROTOTYPE_IP));
		m_ARPTable->SetOpcode(ntohs(ARP_OPCODE_REQUEST));
		m_ARPTable->SetSenderEtherAddress(org_srcenet.addrs_e);
		m_ARPTable->SetSenderIPAddress(org_srcipt.addrs_i);
		memset(broadcast,0,6);
		m_ARPTable->SetTargetEtherAddress(broadcast);
		m_ARPTable->SetTargetIPAddress(m_sHeader.arp_dstip.addrs_i);
		
		// ARP �޽��� �ۼ�
		SetHardType(ntohs(ARP_HARDTYPE));
		SetProtType(ntohs(ARP_PROTOTYPE_IP));
		SetOpcode(ntohs(ARP_OPCODE_REQUEST));
		SetSenderIPAddress(org_srcipt.addrs_i);
		memset(broadcast,0,6);
		SetTargetEtherAddress(broadcast);
		SetTargetIPAddress(m_sHeader.arp_dstip.addrs_i);

		// Cache Table Update
		index = m_ARPTable->CacheAddItem(NULL);

		// ARP �޽��� ����
		bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
		m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_IP));
	}
	else if(memcmp(arp_node->key.cache_enetaddr.addrs_e,ether,6))// ARP Table�� ���� �ּҰ� �����ϴ� ���
	{
		// ICMP�� ������.
		m_Ether = (CEthernetLayer *)GetUnderLayer();
		m_Ether->SetEnetDstAddress(arp_node->key.cache_enetaddr.addrs_e);
		m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_IP));

		// Cache Table Update
		index = m_ARPTable->CacheAddItem(NULL);

		bSuccess = mp_UnderLayer->Send((unsigned char*) ppayload,nlength);
	}

	return bSuccess ;
}

BOOL CARPLayer::Receive( unsigned char* ppayload )
{
	// ���� �������� ���� payload�� ���� ������ header������ �°� ����.
	PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;

	m_Ether = (CEthernetLayer *)GetUnderLayer();
	
	// ARP
	if(!memcmp((char *)pFrame->arp_dstip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4) &&
		memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4) &&
		memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)pFrame->arp_dstip.addrs_i,4))
	{
		if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST) // ���κ��� ��û ���� ���
		{
			m_Ether->SetEnetDstAddress(pFrame->arp_srcenet.addrs_e);
			m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_ARP));

			m_ARPTable->SetHardType(ntohs(ARP_HARDTYPE));
			m_ARPTable->SetProtType(ntohs(ARP_PROTOTYPE_IP));
			m_ARPTable->SetOpcode(ntohs(ARP_OPCODE_REPLY));
			m_ARPTable->SetSenderEtherAddress(org_srcenet.addrs_e);
			m_ARPTable->SetSenderIPAddress(org_srcipt.addrs_i);
			m_ARPTable->SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
			m_ARPTable->SetTargetIPAddress(pFrame->arp_srcip.addrs_i);

			SetHardType(ntohs(ARP_HARDTYPE));
			SetProtType(ntohs(ARP_PROTOTYPE_IP));
			SetOpcode(ntohs(ARP_OPCODE_REPLY));
			SetSenderEtherAddress(org_srcenet.addrs_e);
			SetSenderIPAddress(org_srcipt.addrs_i);
			SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
			SetTargetIPAddress(pFrame->arp_srcip.addrs_i);

			// Cache Table�� entry �߰�
			m_ARPTable->CacheAddItem(ppayload);

			bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
			m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_IP));
		}
		else if(ntohs(pFrame->arp_op) == ARP_OPCODE_REPLY) // ��뿡�� ���� ���� ���
		{
			m_ARPTable->SetHardType(pFrame->arp_hardtype);
			m_ARPTable->SetProtType(pFrame->arp_prottype);
			m_ARPTable->SetOpcode(pFrame->arp_op);
			m_ARPTable->SetSenderEtherAddress(org_srcenet.addrs_e);
			m_ARPTable->SetSenderIPAddress(org_srcipt.addrs_i);
			m_ARPTable->SetTargetEtherAddress(pFrame->arp_dstenet.addrs_e);
			m_ARPTable->SetTargetIPAddress(pFrame->arp_dstip.addrs_i);

			m_ARPTable->CacheUpdate(ppayload);
		}
	}
	// GARP�� ���
	else if(!memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)pFrame->arp_dstip.addrs_i,4))
	{
		if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST){
			if(!memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)org_srcipt.addrs_i,4)){
				SetOpcode(ntohs(ARP_OPCODE_REPLY));
				mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
			}
			if(m_ARPTable->CacheSearchExist(pFrame->arp_dstip)==FALSE)
				m_ARPTable->CacheAddItem(ppayload);
			else
				m_ARPTable->CacheUpdate(ppayload);
		}
	}
	// Proxy ARP�� ���
	else
	{
		if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST &&
			m_ARPTable->ProxySearchExist(pFrame->arp_dstip)) // ���κ��� ��û ���� ���
		{
			// ��밡 ��û�� IP�� ���ּҸ� ����� ������.
			m_Ether->SetEnetDstAddress(pFrame->arp_srcenet.addrs_e);
			m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_ARP));
			m_ARPTable->SetHardType(ntohs(ARP_HARDTYPE));
			m_ARPTable->SetProtType(ntohs(ARP_PROTOTYPE_IP));
			m_ARPTable->SetOpcode(ntohs(ARP_OPCODE_REPLY));
			// Sender�� Ether�� IP Address�� Proxy���� ����
			m_ARPTable->SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
			m_ARPTable->SetTargetIPAddress(pFrame->arp_srcip.addrs_i);

			SetHardType(ntohs(ARP_HARDTYPE));
			SetProtType(ntohs(ARP_PROTOTYPE_IP));
			SetOpcode(ntohs(ARP_OPCODE_REPLY));
			// Sender�� Ether�� IP Address�� Proxy���� ����
			SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
			SetTargetIPAddress(pFrame->arp_srcip.addrs_i);

			// Cache Table�� entry �߰�
			m_ARPTable->CacheAddItem(ppayload);
			bSuccess = mp_aUpperLayer[1]->Receive((unsigned char*) 256);

			mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
			m_Ether->SetEnetFrameType(ntohs(ARP_PROTOTYPE_IP));
		}
	}
	return bSuccess;
}

void CARPLayer::SetOriginSrcEtherEnet(unsigned char* enet)
{
	memcpy(org_srcenet.addrs_e,enet,6);
}