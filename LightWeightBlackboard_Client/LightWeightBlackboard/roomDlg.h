#pragma once


// roomDlg 대화 상자

class roomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(roomDlg)

public:
	roomDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~roomDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	CListBox chat_list;
	CEdit chat_edit;
	CEdit file_edit;
	CEdit compile_edit;
	SOCKET sock;
	CWinThread *m_pThread;

	bool mod = false;

	enum ThreadWorking
	{
		STOP = 0,
		RUNNING = 1,
		PAUSE = 2

	};

	ThreadWorking m_ThreadWorkType;
	virtual BOOL OnInitDialog();
	void AddEventString(CString ap_string);

	void OnLbnSelchangeChatList();
	afx_msg void OnBnClickedButton2();
	static UINT ThreadFunction(LPVOID _mothod);
	bool m_bThreadStart;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	CButton m_cButton_Test1;
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
};
