// SMTPSET.cpp : 实现文件
// SMTPSET 对话框

#include "stdafx.h"
#include "MyMail.h"
#include "SMTPSET.h"
#include "afxdialogex.h"
extern CString Buffer1;
extern CString Buffer2;

IMPLEMENT_DYNAMIC(SMTPSET, CDialogEx)

SMTPSET::SMTPSET(CWnd* pParent)
	: CDialogEx(IDD_SMTPSET, pParent)
{
}

BOOL SMTPSET::OnInitDialog()
{
	if (Buffer1.IsEmpty() && Buffer2.IsEmpty())
	{
		GetDlgItem(IDC_EDIT7)->SetWindowText("smtp.163.com");
		GetDlgItem(IDC_EDIT8)->SetWindowText("25");
	}
	else 
	{
		GetDlgItem(IDC_EDIT7)->SetWindowText(Buffer1);
		GetDlgItem(IDC_EDIT8)->SetWindowText(Buffer2);
	}
	return true;
}

SMTPSET::~SMTPSET()
{
}

void SMTPSET::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SMTPSET, CDialogEx)
	ON_BN_CLICKED(ID_BTOK, &SMTPSET::OnBnClickedBtok)
	ON_BN_CLICKED(IDC_BTCLEAR, &SMTPSET::OnBnClickedBtclear)
END_MESSAGE_MAP()

//消息处理程序
void SMTPSET::OnBnClickedBtok()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_EDIT7)->GetWindowText(Buffer1);
	GetDlgItem(IDC_EDIT8)->GetWindowText(Buffer2);
	if (Buffer1.IsEmpty())
		MessageBox("服务器地址不能为空！", "提示", MB_OK);
	if (Buffer2.IsEmpty())
		MessageBox("端口不能为空！", "提示", MB_OK);
	if (!Buffer1.IsEmpty() && !Buffer2.IsEmpty())
	{
		OnCancel();
	}
}

void SMTPSET::OnBnClickedBtclear()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_EDIT7, "");
	SetDlgItemText(IDC_EDIT8, "");
}


