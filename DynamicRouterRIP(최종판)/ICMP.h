// ICMP.h: interface for the CICMP class.
//
//////////////////////////////////////////////////////////////////////

#pragma once		//분석완료
#include "BaseLayer.h"

class CICMP 
{
private:
	inline void		ResetHeader( );

public:
	CICMP();
	virtual ~CICMP();

	int checksum(u_short* data,int len);

	u_char *ICMP_SetReplyMSG(u_char* ppayload);

	typedef struct _ICMP_HEADER {
		unsigned char	icmp_type;
		unsigned char	icmp_code;
		unsigned short	icmp_cksum;
		unsigned short	icmp_id;
		unsigned short	icmp_seq;
		unsigned char	icmp_data[ICMP_DATA_SIZE];
	}ICMP_HEADER, *PICMP_HEADER ;

protected:
	ICMP_HEADER m_sHeader;
};
