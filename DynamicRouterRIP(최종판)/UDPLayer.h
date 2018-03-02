// UDP.h: interface for the CUDP class.
//
//////////////////////////////////////////////////////////////////////
#include "BaseLayer.h"
#include "IpLayer.h"

#if !defined(AFX_UDP_H__C66AE427_E464_4BC7_9E3A_C0F1D43ABFF4__INCLUDED_)
#define AFX_UDP_H__C66AE427_E464_4BC7_9E3A_C0F1D43ABFF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUDPLayer
	:public CBaseLayer
{
public:
	CIPLayer *m_IP;
	typedef struct _UDP{
		unsigned short      uh_sport;
		unsigned short      uh_dport;
		unsigned short      uh_ulen;
		unsigned short      uh_sum;
		unsigned char      uh_data[UDP_DATA_SIZE];
	}UDP, *LPUDP;

	typedef struct _PseudoHeader{
		unsigned char   SourceAddress[4];
		unsigned char   DestinationAddress[4];
		unsigned char   Zeros;
		unsigned char   PTCL;
		unsigned short   Length;
	}PseudoHeader;
	CString udp_portnumber;
	unsigned char m_srcIPAddr[4];
	BOOL IsCorrectCheckSum(LPUDP header);
	void MakeCheckSum(int iPacketLength);
	char m_NICDescription[200];
	void SetAddress(char *NICDescription, unsigned char* srcIP);
	CUDPLayer(char* pName);
	virtual ~CUDPLayer();

	BOOL Send(unsigned char* ppayload, int nlength);
	BOOL Receive(unsigned char *ppayload);


	UDP m_header;
	PseudoHeader m_pseudoHeader;

};

#endif // !defined(AFX_UDP_H__C66AE427_E464_4BC7_9E3A_C0F1D43ABFF4__INCLUDED_)