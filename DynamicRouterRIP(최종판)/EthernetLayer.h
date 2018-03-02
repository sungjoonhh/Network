// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CEthernetLayer
: public CBaseLayer
{
private:
	inline void		ResetHeader( );

public:
	BOOL			Receive( unsigned char* ppayload ) ;
	BOOL			Send( unsigned char* ppayload, int nlength );

	void			SetEnetDstAddress( unsigned char* pAddress );
	void			SetEnetSrcAddress( unsigned char* pAddress );
	void			SetEnetFrameType( unsigned short frame );
	unsigned char*	GetEnetDstAddress( );
	unsigned char*	GetEnetSrcAddress( );
	unsigned short	GetEnetFrameType();

	CEthernetLayer( char* pName );
	virtual ~CEthernetLayer(); 

private:
	// Ethernet Header
	typedef struct _ETHERNET_HEADER {
		ETHERNET_ADDR	enet_dstaddr;		// destination address of ethernet layer
		ETHERNET_ADDR	enet_srcaddr;		// source address of ethernet layer
		unsigned short	enet_frametype;		// type of ethernet layer
		unsigned char	enet_data[ETHER_MAX_DATA_SIZE]; // frame data
	} ETHERNET_HEADER, *PETHERNET_HEADER ;

protected:
	ETHERNET_HEADER	m_sHeader ;
};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
