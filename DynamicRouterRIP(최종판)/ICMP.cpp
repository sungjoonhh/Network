// ICMP.cpp: implementation of the CICMP class.
//
//////////////////////////////////////////////////////////////////////

#pragma once		//분석완료
#include "stdafx.h"
#include "DynamicRouterRIP.h"
#include "ICMP.h"

CICMP::CICMP()
{
	ResetHeader( );
}

CICMP::~CICMP()
{
}

void CICMP::ResetHeader()
{
	m_sHeader.icmp_type = 0x00;
	m_sHeader.icmp_code = 0x00;
	m_sHeader.icmp_cksum = 0x0000;
	m_sHeader.icmp_id = 0x0000;
	m_sHeader.icmp_seq = 0x0000;
	memset(m_sHeader.icmp_data,0,ICMP_DATA_SIZE);
}

// ICMP의 checksum을 계산
int CICMP::checksum(u_short* data,int len)	//* (이부분은 종이에 직접 한번 해 봤으면 함.)
{
	register u_short answer;
	register long sum = 0;
	u_short odd_byte=0;

	while(len>1){
		sum += *data++;
		len -= 2;
	}

	if(len==1){	//* 1byte 남은 경우에 처리.
		*(u_char*)(&odd_byte) = *(u_char*)data;
		sum += odd_byte;
	}

	sum = (sum>>16) + (sum&0xffff); // 상위 16비트와 하위 16비트를 더한다.
	sum += (sum>>16); // add carry
	return ~sum; // 1의 보수
}

u_char* CICMP::ICMP_SetReplyMSG(u_char* ppayload)		//* payload 받아서 t_sHeader에 복사해주고, checksum 계산해서 return
{
	PICMP_HEADER pFrame = (PICMP_HEADER) ppayload ;
	ICMP_HEADER t_sHeader;
	t_sHeader.icmp_type = ICMP_TYPE_REPLY;
	t_sHeader.icmp_code = pFrame->icmp_code;
	t_sHeader.icmp_cksum = 0;
	t_sHeader.icmp_id = pFrame->icmp_id;
	t_sHeader.icmp_seq = pFrame->icmp_seq;
	memcpy(t_sHeader.icmp_data,pFrame->icmp_data,ICMP_DATA_SIZE);
	t_sHeader.icmp_cksum = checksum((u_short*)&t_sHeader,ICMP_HEADER_SIZE+ICMP_DATA_SIZE);
	

	return (u_char*)&t_sHeader;
}