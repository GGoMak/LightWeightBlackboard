#pragma once


// MakeFile 대화 상자

class MakeFile : public CDialogEx
{
	DECLARE_DYNAMIC(MakeFile)

public:
	MakeFile(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~MakeFile();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
public:
	CEdit make_edit;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
