
// LightWeightBlackboard.cpp: 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "LightWeightBlackboard.h"
#include "LightWeightBlackboardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLightWeightBlackboardApp

BEGIN_MESSAGE_MAP(CLightWeightBlackboardApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLightWeightBlackboardApp 생성

CLightWeightBlackboardApp::CLightWeightBlackboardApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CLightWeightBlackboardApp 개체입니다.

CLightWeightBlackboardApp theApp;


// CLightWeightBlackboardApp 초기화

BOOL CLightWeightBlackboardApp::InitInstance()
{
	CWinApp::InitInstance();

	WSADATA temp;
	WSAStartup(0x0202, &temp);

	CLightWeightBlackboardDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	WSACleanup();

	return FALSE;
}

