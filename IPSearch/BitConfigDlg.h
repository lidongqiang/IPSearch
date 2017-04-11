#pragma once


// CBitConfigDlg dialog

class CBitConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CBitConfigDlg)

public:
	CBitConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBitConfigDlg();

// Dialog Data
	enum { IDD = IDD_BITCONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
