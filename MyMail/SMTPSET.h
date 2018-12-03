#pragma once
// SMTPSET 对话框

class SMTPSET : public CDialogEx
{
	DECLARE_DYNAMIC(SMTPSET)

public:
	SMTPSET(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SMTPSET();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMTPSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtok();
	afx_msg void OnBnClickedBtclear();



};

