// ICMP.cpp: implementation of the CICMP class.
//
//////////////////////////////////////////////////////////////////////

#pragma once		//�м��Ϸ�
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

// ICMP�� checksum�� ���
int CICMP::checksum(u_short* data,int len)	//* (�̺κ��� ���̿� ���� �ѹ� �� ������ ��.)
{
	register u_short answer;
	register long sum = 0;
	u_short odd_byte=0;

	while(len>1){
		sum += *data++;
		len -= 2;
	}

	if(len==1){	//* 1byte ���� ��쿡 ó��.
		*(u_char*)(&odd_byte) = *(u_char*)data;
		sum += odd_byte;
	}

	sum = (sum>>16) + (sum&0xffff); // ���� 16��Ʈ�� ���� 16��Ʈ�� ���Ѵ�.
	sum += (sum>>16); // add carry
	return ~sum; // 1�� ����
}

u_char* CICMP::ICMP_SetReplyMSG(u_char* ppayload)		//* payload �޾Ƽ� t_sHeader�� �������ְ�, checksum ����ؼ� return
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