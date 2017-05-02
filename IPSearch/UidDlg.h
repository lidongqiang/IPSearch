#pragma once
#include "settings/SettingBase.h"

// CUidDlg dialog

class CUidDlg : public CDialog
{
	DECLARE_DYNAMIC(CUidDlg)

public:
	CUidDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang,CWnd* pParent = NULL);   // standard constructor
	virtual ~CUidDlg();

// Dialog Data
	enum { IDD = IDD_UID_DIALOG };
public:
	CString strUid;
	CString strLanmac;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CIniSettingBase &m_Configs;
	CIniLocalLan    &m_LocalLang;
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
