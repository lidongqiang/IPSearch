#pragma once
#include "settings/SettingBase.h"

// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang,CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();

// Dialog Data
	enum { IDD = IDD_SETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CIniSettingBase &m_Configs;
	CIniLocalLan    &m_LocalLang;
public:
	afx_msg void OnBnClickedBtnOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnCancel();
};
