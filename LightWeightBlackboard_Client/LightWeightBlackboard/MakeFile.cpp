// MakeFile.cpp: 구현 파일
//

#include "stdafx.h"
#include "LightWeightBlackboard.h"
#include "MakeFile.h"
#include "afxdialogex.h"
#include "LightWeightBlackboardDlg.h"

CLightWeightBlackboardDlg *pppDlg;

// MakeFile 대화 상자

IMPLEMENT_DYNAMIC(MakeFile, CDialogEx)

MakeFile::MakeFile(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{

}

MakeFile::~MakeFile()
{
}

void MakeFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MAKE_EDIT, make_edit);
}


BEGIN_MESSAGE_MAP(MakeFile, CDialogEx)
	ON_BN_CLICKED(IDOK, &MakeFile::OnBnClickedOk)
END_MESSAGE_MAP()


// MakeFile 메시지 처리기


BOOL MakeFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	pppDlg = (CLightWeightBlackboardDlg *)AfxGetApp()->m_pMainWnd;

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void MakeFile::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString filename;
	GetDlgItemText(IDC_MAKE_EDIT, filename);

	char *senddata = new char[filename.GetLength()];

	int data_len = WideCharToMultiByte(CP_ACP, 0, filename, -1, NULL, 0, NULL, NULL);	// 변환할 문자열의 길이
	WideCharToMultiByte(CP_ACP, 0, filename, -1, senddata, data_len, NULL, NULL);	// 문자 변환

	char sendbuf[1024] = { 0 };
	sprintf(sendbuf, "9%s", senddata);

	send(pppDlg->mh_socket, sendbuf, 1024, 0);

	CDialogEx::OnOK();
}
