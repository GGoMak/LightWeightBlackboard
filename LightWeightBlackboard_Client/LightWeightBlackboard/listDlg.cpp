// listDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "LightWeightBlackboard.h"
#include "listDlg.h"
#include "afxdialogex.h"
#include "LightWeightBlackboardDlg.h"

CLightWeightBlackboardDlg *aDlg;
// listDlg 대화 상자

IMPLEMENT_DYNAMIC(listDlg, CDialogEx)

listDlg::listDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG5, pParent)
{

}

listDlg::~listDlg()
{
}

void listDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_BOX, list_edit);
}


BEGIN_MESSAGE_MAP(listDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &listDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// listDlg 메시지 처리기


BOOL listDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	aDlg = (CLightWeightBlackboardDlg*)AfxGetApp()->m_pMainWnd;
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void listDlg::OnBnClickedOk()	// 파일 목록 열기
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
