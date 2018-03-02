
// DynamicRouterRIPDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "DynamicRouterRIP.h"
#include "DynamicRouterRIPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDynamicRouterRIPDlg ��ȭ ����
CDynamicRouterRIPDlg::CDynamicRouterRIPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDynamicRouterRIPDlg::IDD, pParent),
	CBaseLayer("RouterDlg")
{
	m_ARPTable = new CARPTable();
	m_RoutTable = new CRouterTable();
	m_Icmp = new CICMP();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// ���� ����
	m_LayerMgr.AddLayer( new CNILayer("L_NI") );
	m_LayerMgr.AddLayer( new CNILayer("R_NI") );
	m_LayerMgr.AddLayer( new CEthernetLayer("L_Ether") );
	m_LayerMgr.AddLayer( new CEthernetLayer("R_Ether") );
	m_LayerMgr.AddLayer( new CIPLayer("L_IP") );
	m_LayerMgr.AddLayer( new CIPLayer("R_IP") );
	m_LayerMgr.AddLayer( new CARPLayer("L_ARP") );
	m_LayerMgr.AddLayer( new CARPLayer("R_ARP") );
	m_LayerMgr.AddLayer(new CRIPLayer("R_RIP"));
	m_LayerMgr.AddLayer(new CRIPLayer("L_RIP"));
	m_LayerMgr.AddLayer(new CUDPLayer("R_UDP"));
	m_LayerMgr.AddLayer(new CUDPLayer("L_UDP"));
	m_LayerMgr.AddLayer( this );

	// ���� ����
	m_LayerMgr.ConnectLayers("RouterDlg ( #R_RIP ( #R_UDP ( #R_IP ( -R_ARP ( #R_Ether ( #R_NI +R_IP ) ) -R_Ether ) ) ) #L_RIP ( #L_UDP ( #L_IP ( -L_ARP ( #L_Ether ( #L_NI +L_IP ) ) -L_Ether ) ) ) )");

	// ���� ��ü ����
	m_LIP = (CIPLayer *)m_LayerMgr.GetLayer("L_IP");
	m_RIP = (CIPLayer *)m_LayerMgr.GetLayer("R_IP");

	m_LARP = (CARPLayer *)m_LayerMgr.GetLayer("L_ARP");
	m_RARP = (CARPLayer *)m_LayerMgr.GetLayer("R_ARP");

	m_LEther = (CEthernetLayer *)m_LayerMgr.GetLayer("L_Ether");
	m_REther = (CEthernetLayer *)m_LayerMgr.GetLayer("R_Ether");

	m_LNI = (CNILayer *)m_LayerMgr.GetLayer("L_NI");
	m_RNI = (CNILayer *)m_LayerMgr.GetLayer("R_NI");

	m_LRIP = (CRIPLayer *)m_LayerMgr.GetLayer("L_RIP");
	m_RRIP = (CRIPLayer *)m_LayerMgr.GetLayer("R_RIP");

	m_LUDP = (CUDPLayer *)m_LayerMgr.GetLayer("L_UDP");
	m_RUDP = (CUDPLayer *)m_LayerMgr.GetLayer("R_UDP");

	// ���� �� ���� ��ü ����

	m_LIP->SetARPobj(m_LARP);
	m_LIP->SetICMPobj(m_Icmp);
	m_LIP->SetOtherIPobj(m_RIP);
	m_LIP->SetRouterTable(m_RoutTable);
	m_LIP->SetARPTable(m_ARPTable);

	m_RIP->SetARPobj(m_RARP);
	m_RIP->SetICMPobj(m_Icmp);
	m_RIP->SetOtherIPobj(m_LIP);
	m_RIP->SetRouterTable(m_RoutTable);
	m_RIP->SetARPTable(m_ARPTable);

	m_LARP->SetARPTableObject(m_ARPTable);
	m_RARP->SetARPTableObject(m_ARPTable);

	m_RRIP->SetRouterTable(m_RoutTable);
	m_LRIP->SetRouterTable(m_RoutTable);
}

void CDynamicRouterRIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_LINTERFACE,mCombo_LInterface);
	DDX_Text(pDX,IDC_EDIT_LMAC,mEdit_LMacAddr);
	DDX_Control(pDX,IDC_IPADDR_LEFT,mIPCtrl_LIP);
	DDX_Control(pDX,IDC_COMBO_RINTERFACE,mCombo_RInterface);
	DDX_Text(pDX,IDC_EDIT_RMAC,mEdit_RMacAddr);
	DDX_Control(pDX,IDC_IPADDR_RIGHT,mIPCtrl_RIP);
	DDX_Control(pDX,IDC_LIST_TBLARP,mList_ARP);
	DDX_Control(pDX,IDC_LIST_TBLROUTE,mList_Router);
	DDX_Text(pDX,IDC_EDIT_SRCPORT,mEdit_SourcePort);
}

BEGIN_MESSAGE_MAP(CDynamicRouterRIPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SETTING, &CDynamicRouterRIPDlg::OnBnClickedBtnSetting)
	ON_BN_CLICKED(IDC_BTN_RESET, &CDynamicRouterRIPDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_START, &CDynamicRouterRIPDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_END, &CDynamicRouterRIPDlg::OnBnClickedBtnEnd)
	ON_CBN_SELCHANGE(IDC_COMBO_LINTERFACE, &CDynamicRouterRIPDlg::OnCbnSelchangeComboLinterface)
	ON_CBN_SELCHANGE(IDC_COMBO_RINTERFACE, &CDynamicRouterRIPDlg::OnCbnSelchangeComboRinterface)
END_MESSAGE_MAP()


// CDynamicRouterRIPDlg �޽��� ó����

BOOL CDynamicRouterRIPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	// ����Ʈ ��Ʈ�� Column �߰�
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLROUTE))->InsertColumn(0,"Router",LVCFMT_LEFT,110);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLROUTE))->InsertColumn(1,"Hop",LVCFMT_LEFT,40);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLROUTE))->InsertColumn(2,"OutPort",LVCFMT_LEFT,110);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLROUTE))->InsertColumn(3,"Nexthop",LVCFMT_LEFT,110);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLROUTE))->InsertColumn(4,"Subnet",LVCFMT_LEFT,110);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLROUTE))->InsertColumn(5,"Interface",LVCFMT_LEFT,70);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLROUTE))->InsertColumn(6,"ttl",LVCFMT_LEFT,40);

	((CListCtrl*)GetDlgItem(IDC_LIST_TBLARP))->InsertColumn(0,"IP Address",LVCFMT_LEFT,110);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLARP))->InsertColumn(1,"Ethernet Address",LVCFMT_LEFT,120);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLARP))->InsertColumn(2,"Interface",LVCFMT_LEFT,100);
	((CListCtrl*)GetDlgItem(IDC_LIST_TBLARP))->InsertColumn(3,"Flag",LVCFMT_LEFT,50);

	(CButton*)GetDlgItem(IDC_BTN_SETTING)->EnableWindow(TRUE);
	(CButton*)GetDlgItem(IDC_BTN_RESET)->EnableWindow(FALSE);
	(CButton*)GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	(CButton*)GetDlgItem(IDC_BTN_END)->EnableWindow(FALSE);

	UpdateData ( TRUE );


	// NI �̿��ؼ� Mac address ��������
	CComboBox* pMY_EtherComboBox = (CComboBox*)GetDlgItem( IDC_COMBO_LINTERFACE );
	CComboBox* pEX_EtherComboBox = (CComboBox*)GetDlgItem( IDC_COMBO_RINTERFACE );

	int i;
	CString device_description;
	for(i=0;i<NI_COUNT_NIC;i++){
		if(!m_LNI->GetAdapterObject(i))
			break;
		else{
			device_description = m_LNI->GetAdapterObject(i)->description;
			device_description.Trim();
			pMY_EtherComboBox->AddString(device_description);
		}
		if(!m_RNI->GetAdapterObject(i))
			break;
		else{
			device_description = m_RNI->GetAdapterObject(i)->description;
			device_description.Trim();
			pEX_EtherComboBox->AddString(device_description);
		}
	}

	// �޺� �ڽ� �ʱⰪ ����
	CString LNicName;
	CString RNicName;
	if(i>1){
		pMY_EtherComboBox->SetCurSel(0);
		pEX_EtherComboBox->SetCurSel(1);

		LNicName = m_LNI->GetAdapterObject(0)->name;
		RNicName = m_RNI->GetAdapterObject(1)->name;
	}
	else{
		pMY_EtherComboBox->SetCurSel(0);
		pEX_EtherComboBox->SetCurSel(0);

		LNicName = m_LNI->GetAdapterObject(0)->name;
		RNicName = m_RNI->GetAdapterObject(0)->name;
	}

	// �޺��ڽ����� 0�� �ε����� NICard�� Mac �ּҸ� ������
	mEdit_LMacAddr = m_LNI->GetNICardAddress((char *)LNicName.GetString());
	mEdit_RMacAddr = m_RNI->GetNICardAddress((char *)RNicName.GetString());

	// IPControl�� �� Mac�� �ش��ϴ� IP �ּҸ� ������
	unsigned char l_ip[4]; unsigned char r_ip[4];
	memcpy(l_ip,(*m_LNI->GetIpAddress(MacAddrToHexInt(mEdit_LMacAddr))).addrs_i,4);	
	memcpy(r_ip,(*m_RNI->GetIpAddress(MacAddrToHexInt(mEdit_RMacAddr))).addrs_i,4);

	mIPCtrl_LIP.SetAddress(l_ip[0],l_ip[1],l_ip[2],l_ip[3]);
	mIPCtrl_RIP.SetAddress(r_ip[0],r_ip[1],r_ip[2],r_ip[3]);

	mEdit_SourcePort = "520";

	UpdateData(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CDynamicRouterRIPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CDynamicRouterRIPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CDynamicRouterRIPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDynamicRouterRIPDlg::SetDeligate(int deligate)
{
	switch(deligate)
	{
	case DELG_START :
		mCombo_LInterface.EnableWindow(FALSE);
		mCombo_RInterface.EnableWindow(FALSE);

		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_LEFT)->EnableWindow(FALSE);
		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_RIGHT)->EnableWindow(FALSE);

		(CEdit*)GetDlgItem(IDC_EDIT_SRCPORT)->EnableWindow(FALSE);

		(CButton*)GetDlgItem(IDC_BTN_SETTING)->EnableWindow(FALSE);
		(CButton*)GetDlgItem(IDC_BTN_RESET)->EnableWindow(FALSE);
		(CButton*)GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
		(CButton*)GetDlgItem(IDC_BTN_END)->EnableWindow(TRUE);
		break;

	case DELG_END :
		mCombo_LInterface.EnableWindow(FALSE);
		mCombo_RInterface.EnableWindow(FALSE);

		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_LEFT)->EnableWindow(FALSE);
		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_RIGHT)->EnableWindow(FALSE);

		(CEdit*)GetDlgItem(IDC_EDIT_SRCPORT)->EnableWindow(FALSE);

		(CButton*)GetDlgItem(IDC_BTN_SETTING)->EnableWindow(FALSE);
		(CButton*)GetDlgItem(IDC_BTN_RESET)->EnableWindow(TRUE);
		(CButton*)GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		(CButton*)GetDlgItem(IDC_BTN_END)->EnableWindow(FALSE);
		break;

	case DELG_RESET :
		mCombo_LInterface.EnableWindow(TRUE);
		mCombo_RInterface.EnableWindow(TRUE);

		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_LEFT)->EnableWindow(TRUE);
		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_RIGHT)->EnableWindow(TRUE);

		(CEdit*)GetDlgItem(IDC_EDIT_SRCPORT)->EnableWindow(TRUE);

		(CButton*)GetDlgItem(IDC_BTN_SETTING)->EnableWindow(TRUE);
		(CButton*)GetDlgItem(IDC_BTN_RESET)->EnableWindow(FALSE);
		(CButton*)GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
		(CButton*)GetDlgItem(IDC_BTN_END)->EnableWindow(FALSE);
		break;

	case DELG_SETTING :
		// Layout Setting
		mCombo_LInterface.EnableWindow(FALSE);
		mCombo_RInterface.EnableWindow(FALSE);

		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_LEFT)->EnableWindow(FALSE);
		(CIPAddressCtrl*)GetDlgItem(IDC_IPADDR_RIGHT)->EnableWindow(FALSE);

		(CEdit*)GetDlgItem(IDC_EDIT_SRCPORT)->EnableWindow(FALSE);

		(CButton*)GetDlgItem(IDC_BTN_SETTING)->EnableWindow(FALSE);
		(CButton*)GetDlgItem(IDC_BTN_RESET)->EnableWindow(TRUE);
		(CButton*)GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		(CButton*)GetDlgItem(IDC_BTN_END)->EnableWindow(FALSE);
		break;
	}
}

BOOL CDynamicRouterRIPDlg::Send( unsigned char* ppayload, int nlength )
{
	return FALSE;
}

BOOL CDynamicRouterRIPDlg::Receive(unsigned char* ppayload)
{
	m_RRIP->m_sHeader.command = 1;
	m_LRIP->m_sHeader.command = 1;
	m_RRIP->Send();
	m_LRIP->Send();
	if(ppayload[0] == 'R'){				//* R���� data ���� ��� R�� Timer �ʱ�ȭ
		KillTimer(TIMER_ID_CHK_RVALID);
		KillTimer(TIMER_ID_RIGHT_RECEIVE);
		SetTimer(TIMER_ID_RIGHT_RECEIVE,TIMER_EXPIRE,NULL);
	}
	if (ppayload[0] == 'L'){				//* L���� data ���� ��� L�� Timer �ʱ�ȭ
		KillTimer(TIMER_ID_CHK_LVALID);
		KillTimer(TIMER_ID_LEFT_RECEIVE);
		SetTimer(TIMER_ID_LEFT_RECEIVE,TIMER_EXPIRE,NULL);
	}

	SetTimer(TIMER_ID_UPDATE,TIMER_UPDATE,NULL);
	return TRUE;
}

void CDynamicRouterRIPDlg::SendData()
{
}

void CDynamicRouterRIPDlg::Send_INIT_GARPmsg()
{
	m_LARP->SendGARPMsg();
	m_RARP->SendGARPMsg();
}

void CDynamicRouterRIPDlg::Send_FREQ_ARPmsg()
{

}
//
//	Button ���� �Լ�
//
void CDynamicRouterRIPDlg::OnBnClickedBtnSetting()	// ���� ��ư Ŭ��
{
	LPROUT_NODE newNode = (LPROUT_NODE)malloc(sizeof(ROUT_NODE));

	memcpy(right_enet.addrs_e,MacAddrToHexInt(mEdit_RMacAddr),6);
	memcpy(left_enet.addrs_e,MacAddrToHexInt(mEdit_LMacAddr),6);

	unsigned char l_ip[4]; unsigned char r_ip[4];
	mIPCtrl_LIP.GetAddress(l_ip[0],l_ip[1],l_ip[2],l_ip[3]);
	mIPCtrl_RIP.GetAddress(r_ip[0],r_ip[1],r_ip[2],r_ip[3]);

	memcpy(left_ip.addrs_i,l_ip,4);
	memcpy(right_ip.addrs_i,r_ip,4);

	memcpy(m_LRIP->ip, right_ip.addrs_i, 4);
	memcpy(m_LRIP->ip2, left_ip.addrs_i, 4);

	memcpy(m_RRIP->ip, left_ip.addrs_i, 4);
	memcpy(m_RRIP->ip2, right_ip.addrs_i, 4);
	
	memcpy(newNode->key.rout_router.S_un.s_ip_addr,left_ip.S_un.s_ip_addr,4);
	newNode->key.rout_router.S_un.s_ip_addr[3]=0;
	newNode->key.rout_metric = 0;
	memset(newNode->key.rout_outport.S_un.s_ip_addr,0,4);
	memset(newNode->key.rout_nexthop.S_un.s_ip_addr,0,4);
	memset(newNode->key.rout_subnet.S_un.s_ip_addr,255,3);
	memset(newNode->key.rout_subnet.S_un.s_ip_addr + 3, 0 ,1);
	memcpy(newNode->key.rout_interface,"-\n",20);

	m_RoutTable->RouterAddItem(newNode->key);

	memcpy(newNode->key.rout_router.S_un.s_ip_addr,right_ip.S_un.s_ip_addr,4);
	newNode->key.rout_router.S_un.s_ip_addr[3]=0;
	newNode->key.rout_metric = 0;
	memset(newNode->key.rout_outport.S_un.s_ip_addr,0,4);
	memset(newNode->key.rout_nexthop.S_un.s_ip_addr,0,4);
	memset(newNode->key.rout_subnet.S_un.s_ip_addr,255,3);
	memset(newNode->key.rout_subnet.S_un.s_ip_addr + 3, 0 ,1);
	memcpy(newNode->key.rout_interface,"-\n",20);

	m_RoutTable->RouterAddItem(newNode->key);
	free(newNode);
	

	unsigned char broadcast[6];
	memset(broadcast,0xff,6);
	m_LEther->SetEnetDstAddress(broadcast);
	m_REther->SetEnetDstAddress(broadcast);

	//////////////////// ���θ� ���� ////////////////////
	int nIndex = mCombo_LInterface.GetCurSel();	// ������ NICard�� Adapter ���
	m_LNI->SetAdapterNumber(nIndex);

	nIndex = mCombo_RInterface.GetCurSel();// ������ NICard�� Adapter ���
	m_RNI->SetAdapterNumber(nIndex);

	//////////////////// ARP ���� //////////////////
	m_LARP->SetSenderEtherAddress(left_enet.addrs_e);
	m_LARP->SetSenderIPAddress(left_ip.addrs_i);
	m_LARP->SetOriginSrcIPAddress(left_ip.addrs_i);
	m_LARP->SetOriginSrcEtherEnet(left_enet.addrs_e);

	m_RARP->SetSenderEtherAddress(right_enet.addrs_e);
	m_RARP->SetSenderIPAddress(right_ip.addrs_i);
	m_RARP->SetOriginSrcIPAddress(right_ip.addrs_i);
	m_RARP->SetOriginSrcEtherEnet(right_enet.addrs_e);

	m_ARPTable->SetSrcIPAddress(left_ip.addrs_i);
	m_ARPTable->SetOtherIPAddress(right_ip.addrs_i);
	SetDeligate(DELG_SETTING);


	//////////////////// ��Ÿ ���� ///////////////////////
	m_RIP->SetSrcIPAddress(right_ip.addrs_i);
	m_LIP->SetSrcIPAddress(left_ip.addrs_i);

	m_REther->SetEnetSrcAddress(right_enet.addrs_e);
	m_LEther->SetEnetSrcAddress(left_enet.addrs_e);
	//////////////////////UDP PORT//////////////////////////////
	CString port_str;
   GetDlgItemText( IDC_EDIT_SRCPORT, port_str );   
   m_LUDP->udp_portnumber = port_str;
   m_RUDP->udp_portnumber = port_str;

}

void CDynamicRouterRIPDlg::OnBnClickedBtnReset()	// �缳�� ��ư Ŭ��
{

	SetDeligate(DELG_RESET);
}

void CDynamicRouterRIPDlg::OnBnClickedBtnStart()	// ���� ��ư Ŭ��
{
	// �ʱ� �ڽ��� ��Ʈ�� �߰�
//	Debuging(right_ip.addrs_i,left_ip.addrs_i);

	m_LNI->PacketStartDriver();	// Packet Driver ����
	m_RNI->PacketStartDriver();	// Packet Driver ����

	// GARP �޽��� ����
	SetTimer(12523,10000,NULL);
	Send_INIT_GARPmsg();

	SetTimer(123,1000,NULL);
	SetDeligate(DELG_START);


	m_LRIP->dir = TRUE;
	m_RRIP->dir = FALSE;

	m_LRIP->Send();
	m_RRIP->Send();

	RouterTableUpdate();
	SetTimer(54, 0, NULL);
	//* Initialize : �ڽ��� ����Ǿ��ְ� ����ִ� ������ RIP ������.
	//* point-to-point�� ������.
	//* broadcast�� ������. (UDP 520����)
}

void CDynamicRouterRIPDlg::OnBnClickedBtnEnd()	// ���� ��ư Ŭ��
{
	m_RRIP->m_sHeader.command = 1;
	m_LRIP->m_sHeader.command = 1;
	// ��Ŷ ����̹� ����
	m_LNI->PacketEndDriver();
	m_RNI->PacketEndDriver();

	// �ֱ��� Ÿ�̸� ����
	KillTimer(TIMER_ID_INIT);
	KillTimer(TIMER_ID_REPLY);
	KillTimer(TIMER_ID_CHK_LVALID);
	KillTimer(TIMER_ID_CHK_RVALID);
	KillTimer(TIMER_ID_LEFT_RECEIVE);
	KillTimer(TIMER_ID_RIGHT_RECEIVE);
	KillTimer(54);
	SetDeligate(DELG_END);

	mList_Router.DeleteAllItems();
	mList_ARP.DeleteAllItems();
	m_RoutTable->rout_head = NULL;
	m_ARPTable->cache_head = NULL;
}

void CDynamicRouterRIPDlg::OnCbnSelchangeComboLinterface()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );

	// NICard �̸����� Mac address ��������
	CComboBox*	pMY_EtherComboBox = (CComboBox*)GetDlgItem( IDC_COMBO_LINTERFACE );

	// ComboBox���� ���õ� �ε��� ��ȣ ��������
	int sIndex = pMY_EtherComboBox->GetCurSel();

	// ������ �ε��� ��ȣ�� NIC�� Adapter �̸� ��������
	CString nicName = m_LNI->GetAdapterObject(sIndex)->name;

	// Adapter �̸����� Mac Address�� ��������
	mEdit_LMacAddr = m_LNI->GetNICardAddress((char *)nicName.GetString());

	// Mac Address�� IP�� ��������
	unsigned char in_ip[4];
	memcpy(in_ip,(*m_LNI->GetIpAddress(MacAddrToHexInt(mEdit_LMacAddr))).addrs_i,4);	
	mIPCtrl_LIP.SetAddress(in_ip[0],in_ip[1],in_ip[2],in_ip[3]);

	UpdateData ( FALSE );
}


void CDynamicRouterRIPDlg::OnCbnSelchangeComboRinterface()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );
	// NICard �̸����� Mac address ��������
	CComboBox*	pEX_EtherComboBox = (CComboBox*)GetDlgItem( IDC_COMBO_RINTERFACE );

	// ComboBox���� ���õ� �ε��� ��ȣ ��������
	int sIndex = pEX_EtherComboBox->GetCurSel();

	// ������ �ε��� ��ȣ�� NIC�� Adapter �̸� ��������
	CString nicName = m_RNI->GetAdapterObject(sIndex)->name;

	// Adapter �̸����� Mac Address�� ��������
	mEdit_RMacAddr = m_RNI->GetNICardAddress((char *)nicName.GetString());

	// Mac Address�� IP�� ��������
	unsigned char ex_ip[4];
	memcpy(ex_ip,(*m_RNI->GetIpAddress(MacAddrToHexInt(mEdit_RMacAddr))).addrs_i,4);
	mIPCtrl_RIP.SetAddress(ex_ip[0],ex_ip[1],ex_ip[2],ex_ip[3]);

	UpdateData ( FALSE );
}


//
//	Help �Լ�
//
void CDynamicRouterRIPDlg::OnTimer(UINT nIDEvent) 
{
	// Ÿ�̸Ӱ� ����� �߻� �Ǵ� �̺�Ʈ �Լ�

	// Ÿ�̸� ��ɿ� ���� ��ȣ�� �޸��Ѵ�.
	// ex) 2�� ���, 3�� ��� �Ǵ� Ȧ�� ¦�� ��...


	// �߻��� Ÿ�̸Ӹ� ����
	switch(nIDEvent){
	case 12523:
		KillTimer(12523);
		Send_INIT_GARPmsg();
		SetTimer(12523,10000,NULL);
		break;
	case 123:
		KillTimer(123);
		CacheTableUpdate();	
		SetTimer(123,1000,NULL);	//* 1�ʸ��� ������Ʈ
		break;
	case 54:
		KillTimer(54);
		m_RRIP->m_sHeader.command = 1;
		m_RRIP->Send();
		m_LRIP->m_sHeader.command = 1;
		m_LRIP->Send();
		SetTimer(54, 3000, NULL);		//* �ϴ� 3�ʸ��� ������Ʈ�� �س���
		break;
	case TIMER_ID_UPDATE:
		KillTimer( nIDEvent );
		RouterTableUpdate();
		SetTimer(TIMER_ID_UPDATE,TIMER_UPDATE,NULL);	//* 15�ʸ��� ������Ʈ -> 1�ʷ� ����������
		break;
	case TIMER_ID_INIT :
	case TIMER_ID_REPLY : // ���� �޽����� Ÿ�̸Ӱ� ����Ǿ��� ��
		KillTimer( nIDEvent );
		break;
	case TIMER_ID_LEFT_RECEIVE: // �ð��� ����Ǿ��� ���
		m_RoutTable->RouterHopUpdateByTimeout(left_ip);
		KillTimer( nIDEvent );
		SetTimer(TIMER_ID_CHK_LVALID,TIMER_VALID,NULL);
		break;	 
	case TIMER_ID_RIGHT_RECEIVE: // �ð��� ����Ǿ��� ���
		m_RoutTable->RouterHopUpdateByTimeout(right_ip);
		KillTimer( nIDEvent );
		SetTimer(TIMER_ID_CHK_RVALID,TIMER_VALID,NULL);
		break;
	case TIMER_ID_CHK_LVALID: // �̿� ������� ��ȿ ���� �ð��� ��� �Ǿ��� ��
		//m_RoutTable->RouterDeleteItem(left_ip);
		KillTimer( nIDEvent );
		break;
	case TIMER_ID_INF_RVALID: // �̿� ������� ��ȿ ���� �ð��� ��� �Ǿ��� ��
		//m_RoutTable->RouterDeleteItem(right_ip);
		KillTimer( nIDEvent );
		break;
	default:
		KillTimer( nIDEvent );
	}

	CDialog::OnTimer( nIDEvent );
}

unsigned char* CDynamicRouterRIPDlg::MacAddrToHexInt(CString ether)
{
	// �ݷ�(:)���� ���� �Ǿ��� Ethernet �ּҸ�
	// �ݷ�(:)�� ��ū���� �� ����Ʈ�� ���� �����ͼ� Ethernet�迭�� �־��ش�.
	CString cstr;
	unsigned char *arp_ether = (u_char *)malloc(sizeof(u_char)*6);

	for(int i=0;i<6;i++){
		AfxExtractSubString(cstr,ether,i,':');
		// strtoul -> ���ڿ��� ���ϴ� ������ ��ȯ �����ش�.
		arp_ether[i] = (unsigned char)strtoul(cstr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';

	return arp_ether;
}

void CDynamicRouterRIPDlg::CacheTableUpdate()		//* 1�ʸ��� ���ŵǴ� ARP Table�� ����.
{
	// Cache Table�� ����Ʈ �߰�
	// ����Ʈ�� �� �� ���� ������ �Ŀ�,
	// ���� ���̺��� ����Ǿ��� �ִ� ���Ḯ��Ʈ�� ������
	// IP Layer���� ������ cache_head�� ���� �߰��Ѵ�.

	mList_ARP.DeleteAllItems();

	LPARP_NODE head;
	LPARP_NODE currNode;

	head = m_ARPTable->cache_head;
	currNode = head;
	while(currNode!=NULL)
	{
		LV_ITEM lvItem;
		lvItem.iItem = mList_ARP.GetItemCount();
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;

		// IP
		CString ip;
		ip.Format("%d.%d.%d.%d",currNode->key.cache_ipaddr.addr_i0,
			currNode->key.cache_ipaddr.addr_i1,
			currNode->key.cache_ipaddr.addr_i2,
			currNode->key.cache_ipaddr.addr_i3);
		mList_ARP.SetItemText(lvItem.iItem, 0, (LPSTR)(LPCTSTR)ip);

		lvItem.pszText = (LPSTR)(LPCTSTR)ip;
		mList_ARP.InsertItem(&lvItem);

		// Ethernet
		CString ether;
		ether.Format("%.02x:%.02x:%.02x:%.02x:%.02x:%.02x",currNode->key.cache_enetaddr.addr_e0,
			currNode->key.cache_enetaddr.addr_e1,
			currNode->key.cache_enetaddr.addr_e2,
			currNode->key.cache_enetaddr.addr_e3,
			currNode->key.cache_enetaddr.addr_e4,
			currNode->key.cache_enetaddr.addr_e5);
		mList_ARP.SetItemText(lvItem.iItem, 1, (LPSTR)(LPCTSTR)ether);

		// interface
		CString _interface;
		_interface = currNode->key.cache_interface;
		mList_ARP.SetItemText(lvItem.iItem, 2, (LPSTR)(LPCTSTR)_interface);

		// flag
		CString flag;
		flag = currNode->key.cache_flag;
		mList_ARP.SetItemText(lvItem.iItem, 3, (LPSTR)(LPCTSTR)flag);

		currNode = currNode->next;
	}
}

void CDynamicRouterRIPDlg::RouterTableUpdate()
{
	// Router Table�� ����Ʈ �߰�

	mList_Router.DeleteAllItems();

	LPROUT_NODE head;
	LPROUT_NODE currNode;
	LPROUT_NODE tempNode;

	head = m_RoutTable->rout_head;
	currNode = head;
	while(currNode!=NULL)
	{
		if(currNode->key.rout_ttl==(-1) && currNode->key.rout_metric == RIP_HOP_INFINITY)	//* HOP�� 16�̰�, ttl������ �Ǿ����� ������ 120�� �Ŀ� ����
			currNode->key.rout_ttl = TIMER_VALID/1000; // �и��� -> ��

		if(currNode->key.rout_ttl>0)		//* 1�ʾ� ����
			currNode->key.rout_ttl--;

		if(currNode->key.rout_ttl == 0 && currNode->key.rout_metric == 16){	//* ttl�� 0�� �Ǹ� ���̺��� ����
			tempNode = currNode;
			currNode = currNode->next;
			m_RoutTable->RouterDeleteItem(tempNode->key.rout_router);
			continue;
		}

		if (currNode->key.rout_ttl == 0){	//* ttl�� 0�� �Ǹ� ���̺��� ����
			currNode->key.rout_ttl = 12;
			currNode->key.rout_metric = 16;
		}

		LV_ITEM lvItem;
		lvItem.iItem = mList_Router.GetItemCount();
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;

		// Destination IP Address
		CString ip;
		ip.Format("%d.%d.%d.%d",currNode->key.rout_router.addr_i0,
			currNode->key.rout_router.addr_i1,
			currNode->key.rout_router.addr_i2,
			currNode->key.rout_router.addr_i3);
		mList_Router.SetItemText(lvItem.iItem, 0, (LPSTR)(LPCTSTR)ip);

		lvItem.pszText = (LPSTR)(LPCTSTR)ip;
		mList_Router.InsertItem(&lvItem);

		// Metric
		CString metric;
		metric.Format("%d",currNode->key.rout_metric);
		mList_Router.SetItemText(lvItem.iItem, 1, (LPSTR)(LPCTSTR)metric);

		// Port Address
		CString port;
		port.Format("%d.%d.%d.%d",currNode->key.rout_outport.addr_i0,
			currNode->key.rout_outport.addr_i1,
			currNode->key.rout_outport.addr_i2,
			currNode->key.rout_outport.addr_i3);
		mList_Router.SetItemText(lvItem.iItem, 2, (LPSTR)(LPCTSTR)port);

		// Nexthop Address
		CString nexthop;
		nexthop.Format("%d.%d.%d.%d",currNode->key.rout_nexthop.addr_i0,
			currNode->key.rout_nexthop.addr_i1,
			currNode->key.rout_nexthop.addr_i2,
			currNode->key.rout_nexthop.addr_i3);
		mList_Router.SetItemText(lvItem.iItem, 3, (LPSTR)(LPCTSTR)nexthop);

		// Subnet Address
		CString subnet;
		subnet.Format("%d.%d.%d.%d",currNode->key.rout_subnet.addr_i0,
			currNode->key.rout_subnet.addr_i1,
			currNode->key.rout_subnet.addr_i2,
			currNode->key.rout_subnet.addr_i3);
		mList_Router.SetItemText(lvItem.iItem, 4, (LPSTR)(LPCTSTR)subnet);

		// Interface Name
		CString interfaceName;
		interfaceName.Format("%s",currNode->key.rout_interface);
		mList_Router.SetItemText(lvItem.iItem, 5, (LPSTR)(LPCTSTR)interfaceName);

		// ttl
		CString ttl;
		if(currNode->key.rout_ttl >= 0)
			ttl.Format("%d",currNode->key.rout_ttl);
		else
			ttl.Format(" - ");
		mList_Router.SetItemText(lvItem.iItem, 6, (LPSTR)(LPCTSTR)ttl);

		currNode = currNode->next;
	}
}

// �׽�Ʈ
void CDynamicRouterRIPDlg::Debuging(u_char * r_ip, u_char * l_ip)
{
	/* ����� �뵵 ���� ���� ���� */
	////////////////////////////////////////////////////////////////////
	ROUT_ENTRY rout_entry1;
	rout_entry1.rout_router.addr_i0 = r_ip[0];
	rout_entry1.rout_router.addr_i1 = r_ip[1];
	rout_entry1.rout_router.addr_i2 = r_ip[2];
	rout_entry1.rout_router.addr_i3 = 0;

	rout_entry1.rout_metric = 0;

	rout_entry1.rout_outport.addr_i0 = 0;
	rout_entry1.rout_outport.addr_i1 = 0;
	rout_entry1.rout_outport.addr_i2 = 0;
	rout_entry1.rout_outport.addr_i3 = 0;

	rout_entry1.rout_nexthop.addr_i0 = 0;
	rout_entry1.rout_nexthop.addr_i1 = 0;
	rout_entry1.rout_nexthop.addr_i2 = 0;
	rout_entry1.rout_nexthop.addr_i3 = 0;

	rout_entry1.rout_subnet.addr_i0 = 255;
	rout_entry1.rout_subnet.addr_i1 = 255;
	rout_entry1.rout_subnet.addr_i2 = 255;
	rout_entry1.rout_subnet.addr_i3 = 0;

	memcpy(rout_entry1.rout_interface,"Right",RT_DEVICE_NAME_LENGTH);

	rout_entry1.rout_ttl = -1;

	m_RoutTable->RouterAddItem(rout_entry1);

	////////////////////////////////////////////////////////////////////
	ROUT_ENTRY rout_entry2;
	rout_entry2.rout_router.addr_i0 = l_ip[0];
	rout_entry2.rout_router.addr_i1 = l_ip[1];
	rout_entry2.rout_router.addr_i2 = l_ip[2];
	rout_entry2.rout_router.addr_i3 = 0;

	rout_entry2.rout_metric = 0;

	rout_entry2.rout_outport.addr_i0 = 0;
	rout_entry2.rout_outport.addr_i1 = 0;
	rout_entry2.rout_outport.addr_i2 = 0;
	rout_entry2.rout_outport.addr_i3 = 0;

	rout_entry2.rout_nexthop.addr_i0 = 0;
	rout_entry2.rout_nexthop.addr_i1 = 0;
	rout_entry2.rout_nexthop.addr_i2 = 0;
	rout_entry2.rout_nexthop.addr_i3 = 0;

	rout_entry2.rout_subnet.addr_i0 = 255;
	rout_entry2.rout_subnet.addr_i1 = 255;
	rout_entry2.rout_subnet.addr_i2 = 255;
	rout_entry2.rout_subnet.addr_i3 = 0;

	memcpy(rout_entry2.rout_interface,"Left",RT_DEVICE_NAME_LENGTH);

	rout_entry2.rout_ttl = -1;

	m_RoutTable->RouterAddItem(rout_entry2);

	RouterTableUpdate();
}
