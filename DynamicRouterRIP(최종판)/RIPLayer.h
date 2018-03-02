#pragma once
#include "BaseLayer.h"
#include "ICMP.h"
#include "RouterTable.h"
#include "IPLayer.h"

class CRIPLayer
	: public CBaseLayer
{
private:

public:
	inline void		ResetHeader();
	CRIPLayer(char* pName);
	virtual ~CRIPLayer();
	BOOL Send();
	BOOL Receive(unsigned char* ppayload);
	void CRIPLayer::SetRouterTable(CRouterTable * routTable);


	typedef struct _RIPLayer_ENTRY {
		unsigned short address_family;
		unsigned short route_tag;
		unsigned char dst_ip[4];
		unsigned char subnet_mask[4];
		unsigned char next_hop_ip[4];
		unsigned int metric;
	} RIPLayer_ENTRY, *PRIPLayer_ENTRY;

	typedef struct _RIP_HEADER{
		unsigned char command;
		unsigned char version;
		unsigned short routing_domain;	//* 사용 안함
		RIPLayer_ENTRY table_entry[24];
	}RIP_HEADER, *PRIP_HEADER;

	RIP_HEADER m_sHeader;
	CRouterTable * m_RoutTable;

	BOOL dir;
	unsigned char ip[4];
	unsigned char ip2[4];
};
