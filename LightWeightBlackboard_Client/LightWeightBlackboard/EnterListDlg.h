#pragma once


// EnterListDlg 대화 상자

class EnterListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(EnterListDlg)

public:
	EnterListDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~EnterListDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox roomlist;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
