// roomDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "LightWeightBlackboard.h"
#include "roomDlg.h"
#include "afxdialogex.h"
#include "LightWeightBlackboardDlg.h"
#include "MakeFile.h"
#include "OpenFile1.h"
#include <locale.h>

// roomDlg 대화 상자

CLightWeightBlackboardDlg *ppDlg;

IMPLEMENT_DYNAMIC(roomDlg, CDialogEx)

roomDlg::roomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{
	
}

roomDlg::~roomDlg()
{
}

void roomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, chat_list);
	DDX_Control(pDX, IDC_EDIT4, chat_edit);
	DDX_Control(pDX, IDC_EDIT2, file_edit);
	DDX_Control(pDX, IDC_EDIT3, compile_edit);
}


BEGIN_MESSAGE_MAP(roomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &roomDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDCANCEL, &roomDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT2, &roomDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON1, &roomDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &roomDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &roomDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &roomDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &roomDlg::OnBnClickedButton7)
END_MESSAGE_MAP()


// roomDlg 메시지 처리기


BOOL roomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ppDlg = (CLightWeightBlackboardDlg*)AfxGetMainWnd();
	sock = ppDlg->mh_socket;

	m_pThread = AfxBeginThread(ThreadFunction, this);

	if (m_pThread == NULL) {
		AfxMessageBox(L"Thread Start Error");
	}

	file_edit.SetReadOnly(true);
	mod = FALSE;


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void roomDlg::AddEventString(CString ap_string)
{
	while (chat_list.GetCount() > 100) {
		chat_list.DeleteString(0);
	}
	int index = chat_list.InsertString(-1, ap_string);
	chat_list.SetCurSel(index);
}

void roomDlg::OnBnClickedButton2()	// 전송
{
	char buffer[1024];

	while (chat_list.GetCount() > 100) {
		chat_list.DeleteString(0);
	}

	CString str;
	GetDlgItemText(IDC_EDIT4, str);

	char *senddata = new char[str.GetLength()];

	int data_len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);	// 변환할 문자열의 길이
	WideCharToMultiByte(CP_ACP, 0, str, -1, senddata, data_len, NULL, NULL);	// 문자 변환

	//senddata[strlen(senddata)] = '\0';

	char sendbuf[1024] = { 0 };
	sprintf(sendbuf, "2%s", senddata);

	if (send(ppDlg->mh_socket, sendbuf, 1024, 0) < 0) {
		AfxMessageBox(_T("Send error"));
	}

	SetDlgItemText(IDC_EDIT4, _T(""));
}

UINT roomDlg::ThreadFunction(LPVOID _mothod)
{
	roomDlg *th = (roomDlg*)_mothod;
	CLightWeightBlackboardDlg *my = (CLightWeightBlackboardDlg *)AfxGetApp()->m_pMainWnd;
	//CLightWeightLight *my = (roomDlg*)AfxGetMainWnd(); // thread에서 다이얼로그의 변수 사용을 위한 코드
	
	char buffer[1024];
	int recv_len;
	
	while (1) {
		memset(buffer, 0, sizeof(buffer));
		recv_len = recv(my->mh_socket, buffer, 1024, 0);

		if (buffer[0] == '1') {
			char id[16] = { 0 };
			memcpy(id, &buffer[1], 16);

			CString str1(id);
			str1.Append(_T("님이 입장하였습니다."));
			th->chat_list.InsertString(-1, str1);
			th->chat_list.SetCurSel(th->chat_list.GetCount() - 1);
		}

		else if (buffer[0] == '2') {	//채팅
			char id[16] = { 0 };
			char chat[128] = { 0 };

			memcpy(id, &buffer[1], 16);
			memcpy(chat, &buffer[18], 128);

			CString str1, str2;
			str1 = (LPSTR)id;
			str2 = (LPSTR)chat;

			str1.Append(_T(" : "));
			str1.Append(str2);

			if (!str1.IsEmpty()) {
				th->chat_list.InsertString(-1, str1);
				th->chat_list.SetCurSel(th->chat_list.GetCount()-1);
			}
		}

		else if (buffer[0] == '3') {	// 파일 전송
			CString str;
			str = (LPSTR)&buffer[1];

			if (!str.IsEmpty()) {
				th->file_edit.SetWindowText(str);
			}
		}

		else if (buffer[0] == '5') {	// 수정권한 요청

			if (!strcmp(buffer, "5TRUE")) {
				AfxMessageBox(_T("파일 수정 권한을 부여받았습니다."));
				th->file_edit.SetReadOnly(false);
			}
			else {
				AfxMessageBox(_T("다른 사용자가 수정 중입니다."));
				th->mod = FALSE;
			}
		}

		else if (buffer[0] == '6') {	// 권한 해제
			if (!strcmp(buffer, "6TRUE")) {
				AfxMessageBox(_T("파일 수정 권한을 해제하였습니다."));
				th->file_edit.SetReadOnly(true);
				th->mod = FALSE;
			}
		}

		else if (buffer[0] == '7') {	// 파일 수신
			CString recvfile(&buffer[1]);
			th->file_edit.SetWindowTextW(recvfile);
		}

		else if (buffer[0] == '8') {	// 실행결과 수신
			CString recvfile(&buffer[1]);
			th->compile_edit.SetWindowTextW(recvfile);
		}

		else if (buffer[0] == '9') {	// 파일 만들기
			if(!strcmp(&buffer[1], "FAIL")){
				AfxMessageBox(_T("파일이 이미 존재합니다."));
			}
		}

		else if (buffer[0] == 'a') {	// 파일 열기
			if(!strcmp(&buffer[1], "FAIL")){
				AfxMessageBox(_T("파일이 존재하지 않습니다."));
			}
		}
	}

	return 0;
}

void roomDlg::OnBnClickedCancel()
{
	m_pThread->SuspendThread();
	DWORD dwResult;
	::GetExitCodeThread(m_pThread->m_hThread, &dwResult);

	delete m_pThread;
	m_pThread = NULL;
	CDialogEx::OnCancel();
}


void roomDlg::OnEnChangeEdit2()
{
	CString readfile;
	GetDlgItemText(IDC_EDIT2, readfile);

	char *senddata = new char[readfile.GetLength()];

	int data_len = WideCharToMultiByte(CP_ACP, 0, readfile, -1, NULL, 0, NULL, NULL);	// 변환할 문자열의 길이
	WideCharToMultiByte(CP_ACP, 0, readfile, -1, senddata, data_len, NULL, NULL);

	char sendstr[1024] = { 0 };
	sprintf(sendstr, "7%s", senddata);

	send(ppDlg->mh_socket, sendstr, 1024, 0);
}


void roomDlg::OnBnClickedButton1()	// 파일 수정권한 요청
{
	char senddata[1024] = { 0 };
	sprintf(senddata, "5");

	send(ppDlg->mh_socket, senddata, 1024, 0);
}

void roomDlg::OnBnClickedButton4()	// 파일 수정 권한 해제
{
	if (mod = FALSE) {
		AfxMessageBox(_T("파일 수정 권한을 가지고 있지 않습니다."));
		return;
	}

	char senddata[1024] = { 0 };
	sprintf(senddata, "5");

	send(ppDlg->mh_socket, senddata, 1024, 0);

}

void roomDlg::OnBnClickedButton3()	// 컴파일 요청
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char senddata[1024] = { 0 };
	sprintf(senddata, "8");

	send(ppDlg->mh_socket, senddata, 1024, 0);
}


void roomDlg::OnBnClickedButton6()	// 파일 만들기
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MakeFile dlg;
	dlg.DoModal();

}


void roomDlg::OnBnClickedButton7()	// 파일 열기
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OpenFile1 dlg;
	dlg.DoModal();
}
