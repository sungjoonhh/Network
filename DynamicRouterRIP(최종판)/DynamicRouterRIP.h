// by th
// DynamicRouterRIP.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CDynamicRouterRIPApp:
// �� Ŭ������ ������ ���ؼ��� DynamicRouterRIP.cpp�� �����Ͻʽÿ�.
//

class CDynamicRouterRIPApp : public CWinApp
{
public:
	CDynamicRouterRIPApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CDynamicRouterRIPApp theApp;