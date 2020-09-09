// EnterListDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "LightWeightBlackboard.h"
#include "EnterListDlg.h"
#include "afxdialogex.h"
#include "LightWeightBlackboardDlg.h"
#include "roomDlg.h"

// EnterListDlg 대화 상자

CLightWeightBlackboardDlg *pDlg;

IMPLEMENT_DYNAMIC(EnterListDlg, CDialogEx)

EnterListDlg::EnterListDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

EnterListDlg::~EnterListDlg()
{
}

void EnterListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, roomlist);
}


BEGIN_MESSAGE_MAP(EnterListDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &EnterListDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &EnterListDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &EnterListDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// EnterListDlg 메시지 처리기

BOOL EnterListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	pDlg = (CLightWeightBlackboardDlg*)AfxGetApp()->m_pMainWnd;

	int recv_len;
	char buf[256] = { 0 };

	for (int i = 0; i < pDlg->roomcount; i++) {
		recv(pDlg->mh_socket, buf, 256, 0);
		CString data(buf);
		CString num;
		num.Format(_T("%d"), i + 1);
		data.Insert(0, num); 
		roomlist.AddString(data);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void EnterListDlg::OnBnClickedOk()	// 방 입장
{
	int index = roomlist.GetCurSel();

	if (index == LB_ERR) {
		AfxMessageBox(_T("방을 선택해주세요"));
		return;
	}

	char senddata[16] = { 0 };
	char recvdata[5] = { 0 };
	sprintf(senddata, "4%d", index);

	send(pDlg->mh_socket, senddata, 16, 0);

	recv(pDlg->mh_socket, recvdata, 5, 0);

	if (strcmp(recvdata, "FULL")) {
		roomDlg dlg;
		dlg.DoModal();
		OnClose();
	}
	else {
		AfxMessageBox(_T("방이 가득 찼습니다."));
		return;
	}

}

void EnterListDlg::OnBnClickedButton1()	// 방 만들기
{
	char senddata[6];

	sprintf(senddata, "4MAKE");

	send(pDlg->mh_socket, senddata, 6, 0);

	roomDlg dlg;
	dlg.DoModal();
	OnClose();
}

void EnterListDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
	CDialogEx::OnCancel();
}
