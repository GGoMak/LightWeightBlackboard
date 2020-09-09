#pragma once


// listDlg 대화 상자

class listDlg : public CDialogEx
{
	DECLARE_DYNAMIC(listDlg)

public:
	listDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~listDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox list_edit;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
