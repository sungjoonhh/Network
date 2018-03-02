#pragma once		//�м��Ϸ�

class CRouterTable
{
public:
	CRouterTable();
	virtual ~CRouterTable();

private:
	inline void	ResetHeader( );

public:
	void RouterAddItem(ROUT_ENTRY rout_entry);	// ����
	void RouterDeleteItem(IP_ADDR ip);			// ����

	void RouterHopUpdateByTimeout(IP_ADDR ip);	// Metric ����
	void RouterEntryUpdate(ROUT_ENTRY entry,IP_ADDR src_ip); // OutPort ����
	void RouterEntryUpdateForRight(ROUT_ENTRY entry,IP_ADDR src_ip); // OutPort ����
	
	BOOL	SearchIPAddress(unsigned char* ip);	// Routing Table�� IP�� �����ϴ��� �˻�
	IP_ADDR SubnetMask(u_char * ip, IP_ADDR subnet);

	LPROUT_NODE GetRouterEntryByIP(u_char * ip_dst_addr);

	typedef struct _ROUT_HEADER {
		IP_ADDR			rout_router;
		unsigned short	rout_metric;
		unsigned short	rout_outport;
	} ROUT_HEADER, *PROUT_HEADER ;
	
	LPROUT_NODE rout_head;

protected:
	ROUT_HEADER m_sHeader;
};
