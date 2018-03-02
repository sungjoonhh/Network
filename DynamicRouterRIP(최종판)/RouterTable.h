#pragma once		//분석완료

class CRouterTable
{
public:
	CRouterTable();
	virtual ~CRouterTable();

private:
	inline void	ResetHeader( );

public:
	void RouterAddItem(ROUT_ENTRY rout_entry);	// 삽입
	void RouterDeleteItem(IP_ADDR ip);			// 삭제

	void RouterHopUpdateByTimeout(IP_ADDR ip);	// Metric 갱신
	void RouterEntryUpdate(ROUT_ENTRY entry,IP_ADDR src_ip); // OutPort 갱신
	void RouterEntryUpdateForRight(ROUT_ENTRY entry,IP_ADDR src_ip); // OutPort 갱신
	
	BOOL	SearchIPAddress(unsigned char* ip);	// Routing Table에 IP가 존재하는지 검사
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
