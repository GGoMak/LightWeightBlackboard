#pragma once


// OpenFile1 대화 상자

class OpenFile1 : public CDialogEx
{
	DECLARE_DYNAMIC(OpenFile1)

public:
	OpenFile1(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~OpenFile1();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit open_edit;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
