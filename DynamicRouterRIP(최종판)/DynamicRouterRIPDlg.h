
// DynamicRouterRIPDlg.h : 헤더 파일
//

#pragma once
#include "LayerManager.h"	// Added by ClassView
#include "IPLayer.h"		// Added by ClassView
#include "ARPLayer.h"		// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "NILayer.h"		// Added by ClassView
#include "RouterTable.h"	// Added by ClassView
#include "ARPTable.h"		// Added by ClassView
#include "ICMP.h"			// Added by ClassView
#include "RIPLayer.h"
#include "UDPLayer.h"

// CDynamicRouterRIPDlg 대화 상자
class CDynamicRouterRIPDlg : public CDialogEx, public CBaseLayer
{
// 생성입니다.
public:
	CDynamicRouterRIPDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DYNAMICROUTERRIP_DIALOG };
	CComboBox		mCombo_LInterface;
	CString			mEdit_LMacAddr;
	CIPAddressCtrl	mIPCtrl_LIP;
	CComboBox		mCombo_RInterface;
	CString			mEdit_RMacAddr;
	CIPAddressCtrl	mIPCtrl_RIP;
	CListCtrl		mList_ARP;
	CListCtrl		mList_Router;
	CButton			mBtn_Setting;
	CButton			mBtn_Reset;
	CButton			mBtn_Start;
	CButton			mBtn_End;

	CString			mEdit_SourcePort;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnEnd();
	afx_msg void OnCbnSelchangeComboLinterface();
	afx_msg void OnCbnSelchangeComboRinterface();
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnBnClickedBtnReset();

protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL	Receive( unsigned char* ppayload );
	BOOL	Send( unsigned char* ppayload, int nlength );

	unsigned char*	MacAddrToHexInt(CString ether);
	void			RouterTableUpdate();
	void			CacheTableUpdate();

	// 제거해야될 것
	void	Debuging(u_char * in_ip, u_char * ex_ip);

	void	SetDeligate(int deligate);

	void	SendData();
	void	Send_INIT_GARPmsg();
	void	Send_FREQ_ARPmsg();
	void	Send_INIT_RIPmsg();

	CARPTable*		m_ARPTable;
	CRouterTable*	m_RoutTable;
	CICMP*			m_Icmp;

private:
	CLayerManager	m_LayerMgr;

// Object Layer	
	CIPLayer*		m_LIP;
	CARPLayer*		m_LARP;
	CEthernetLayer*	m_LEther;
	CNILayer*		m_LNI;
	CRIPLayer*		m_LRIP;
	CUDPLayer*		m_LUDP;
	
	CIPLayer*		m_RIP;
	CARPLayer*		m_RARP;
	CEthernetLayer*	m_REther;
	CNILayer*		m_RNI;
	CRIPLayer*		m_RRIP;
	CUDPLayer*		m_RUDP;

// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;

// Etc
	IP_ADDR right_ip;
	IP_ADDR left_ip;

	ETHERNET_ADDR right_enet;
	ETHERNET_ADDR left_enet;
};
