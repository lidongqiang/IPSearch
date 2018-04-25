#pragma once
#include "PcbaTest .h"

// CameraTestDlg dialog

class CameraTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CameraTestDlg)

public:
	CameraTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CameraTestDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CAMERA_TEST };
public:
	SOCKET		m_CameraSocket;
	CPcbaTest	m_CameraTest;
	int	 connect_dev();
	void DrawImg(CDC* pDC,CString strPath, int x, int y);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnTest();
};
