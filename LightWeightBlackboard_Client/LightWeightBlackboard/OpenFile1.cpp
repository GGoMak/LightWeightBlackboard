// OpenFile1.cpp: 구현 파일
//

#include "stdafx.h"
#include "LightWeightBlackboard.h"
#include "OpenFile1.h"
#include "afxdialogex.h"
#include "LightWeightBlackboardDlg.h"

CLightWeightBlackboardDlg *ppppDlg;
// OpenFile1 대화 상자

IMPLEMENT_DYNAMIC(OpenFile1, CDialogEx)

OpenFile1::OpenFile1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG4, pParent)
{

}

OpenFile1::~OpenFile1()
{
}

void OpenFile1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_OPEN_EDIT, open_edit);
}


BEGIN_MESSAGE_MAP(OpenFile1, CDialogEx)
	ON_BN_CLICKED(IDOK, &OpenFile1::OnBnClickedOk)
END_MESSAGE_MAP()


// OpenFile1 메시지 처리기


void OpenFile1::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString filename;
	GetDlgItemText(IDC_OPEN_EDIT, filename);

	char *senddata = new char[filename.GetLength()];

	int data_len = WideCharToMultiByte(CP_ACP, 0, filename, -1, NULL, 0, NULL, NULL);	// 변환할 문자열의 길이
	WideCharToMultiByte(CP_ACP, 0, filename, -1, senddata, data_len, NULL, NULL);	// 문자 변환

	char sendbuf[1024] = { 0 };
	sprintf(sendbuf, "a%s", senddata);

	send(ppppDlg->mh_socket, sendbuf, 1024, 0);

	CDialogEx::OnOK();
}


BOOL OpenFile1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ppppDlg = (CLightWeightBlackboardDlg *)AfxGetApp()->m_pMainWnd;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
