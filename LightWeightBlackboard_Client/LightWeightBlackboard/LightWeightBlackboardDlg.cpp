
// LightWeightBlackboardDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "LightWeightBlackboard.h"
#include "LightWeightBlackboardDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "EnterListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLightWeightBlackboardDlg 대화 상자



CLightWeightBlackboardDlg::CLightWeightBlackboardDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LIGHTWEIGHTBLACKBOARD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLightWeightBlackboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, id_edit);
	DDX_Control(pDX, IDC_EDIT2, pw_edit);
	DDX_Control(pDX, IDC_STATIC1, id_text);
	DDX_Control(pDX, IDC_STATIC2, pw_text);
}

BEGIN_MESSAGE_MAP(CLightWeightBlackboardDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CLightWeightBlackboardDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLightWeightBlackboardDlg 메시지 처리기

BOOL CLightWeightBlackboardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_font.CreateFont(23, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	GetDlgItem(IDC_STATIC1)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC2)->SetFont(&m_font);

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CLightWeightBlackboardDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLightWeightBlackboardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLightWeightBlackboardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLightWeightBlackboardDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (mh_socket != INVALID_SOCKET) {
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}
}

void CLightWeightBlackboardDlg::OnBnClickedOk()
{
	CString id, pw;
	DWORD recvTimeout = 2000;

	GetDlgItemText(IDC_EDIT1, id);
	GetDlgItemText(IDC_EDIT2, pw);

	char *c_id = new char[id.GetLength()];
	char *c_pw = new char[pw.GetLength()];

	int id_len = WideCharToMultiByte(CP_ACP, 0, id, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, id, -1, c_id, id_len, NULL, NULL);

	int pw_len = WideCharToMultiByte(CP_ACP, 0, pw, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pw, -1, c_pw, pw_len, NULL, NULL);

	if ((mh_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		AfxMessageBox(_T("adfafdsafsfasd"));
	}
	struct sockaddr_in serv_addr;	// socketaddr_in 구조체 선언
	memset(&serv_addr, 0, sizeof(serv_addr));	// 구조체 초기화
	serv_addr.sin_family = AF_INET;	// IPv4 주소체계
	serv_addr.sin_addr.s_addr = inet_addr("192.168.45.141");	// IP
	serv_addr.sin_port = htons(42196);		// Port
	
	int nSockOpt = 1;

	setsockopt(mh_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeout, sizeof(recvTimeout));
	setsockopt(mh_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&nSockOpt, sizeof(nSockOpt));

	if (connect(mh_socket, (LPSOCKADDR)&serv_addr, sizeof(serv_addr)) != 0) {	// 서버에 접속요청, 실패시 메시지 출력
		AfxMessageBox(_T("서버접속에 실패하였습니다."));
		SetDlgItemText(IDC_EDIT1, _T(""));
		SetDlgItemText(IDC_EDIT2, _T(""));
		return;
	}

	char temp[5] = { 0 };
	recv(mh_socket, temp, 5, 0);

	char senddata[33] = { 0 };
	senddata[0] = '1';

	memcpy(&senddata[1], c_id, 16);
	memcpy(&senddata[16], c_pw, 16);

	if (send(mh_socket, senddata, 33, 0) < 0) {
		AfxMessageBox(_T("Send error"));
	}

	char recvdata[6] = { 0 };

	if (recv(mh_socket, recvdata, 6, 0) < 0) 
	{
		AfxMessageBox(_T("서버로 부터의 응답이 없습니다."));
		return;
	}

	roomcount = atoi(recvdata);

	if (roomcount > -1) 
	{
		EnterListDlg dlg;
		dlg.DoModal();
		OnClose();
		//ShowWindow(SW_SHOWMINIMIZED);
		//PostMessage(WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
		//PostMessage(WM_SHOWWINDOW, TRUE, SW_OTHERUNZOOM);
	}
	else
	{
		AfxMessageBox(_T("로그인 실패"));
		SetDlgItemText(IDC_EDIT2, _T(""));
		return;
	}
}