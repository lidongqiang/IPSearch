#pragma once


// CUidDlg dialog

class CUidDlg : public CDialog
{
	DECLARE_DYNAMIC(CUidDlg)

public:
	CUidDlg(CWnd* pParent = NULL);   // standard constructor
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
	afx_msg void OnBnClickedOk();
};
