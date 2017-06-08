// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPSearch.h"
#include "ConfigDlg.h"


// CConfigDlg dialog

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang,CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent),m_Configs(Configs),m_LocalLang(LocalLang)
{

}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_OK, &CConfigDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(ID_BTN_CANCEL, &CConfigDlg::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CConfigDlg message handlers

void CConfigDlg::OnBnClickedBtnOk()
{
	// TODO: Add your control notification handler code here

	m_Configs.bSdcardTest    = (((CButton*)GetDlgItem(IDC_CHECK_SDCARD))->GetCheck() == BST_CHECKED);
	m_Configs.bWifiTest		 = (((CButton*)GetDlgItem(IDC_CHECK_WIFI))->GetCheck() == BST_CHECKED);
	m_Configs.bKeyTest	    = (((CButton*)GetDlgItem(IDC_CHECK_KEY))->GetCheck() == BST_CHECKED);
	m_Configs.bLedTest	    = (((CButton*)GetDlgItem(IDC_CHECK_LED))->GetCheck() == BST_CHECKED);
	m_Configs.bMonitorTest    = (((CButton*)GetDlgItem(IDC_CHECK_MONITOR))->GetCheck() == BST_CHECKED);
	m_Configs.bInterphoneTest    = (((CButton*)GetDlgItem(IDC_CHECK_PHONE))->GetCheck() == BST_CHECKED);
	m_Configs.bPtzTest    = (((CButton*)GetDlgItem(IDC_CHECK_PTZ))->GetCheck() == BST_CHECKED);
	m_Configs.bIrcutTest    = (((CButton*)GetDlgItem(IDC_CHECK_IRCUT))->GetCheck() == BST_CHECKED);
	m_Configs.bHdmiTest    = (((CButton*)GetDlgItem(IDC_CHECK_HDMI))->GetCheck() == BST_CHECKED);
	//m_Configs.bWriteUid    = (((CButton*)GetDlgItem(IDC_CHECK_UID))->GetCheck() == BST_CHECKED);
	//m_Configs.bWriteMac    = (((CButton*)GetDlgItem(IDC_CHECK_LANMAC))->GetCheck() == BST_CHECKED);
	m_Configs.bBtTest		 = (((CButton*)GetDlgItem(IDC_CHECK_BT))->GetCheck() == BST_CHECKED);
	m_Configs.bEmmcTest		 = (((CButton*)GetDlgItem(IDC_CHECK_EMMC))->GetCheck() == BST_CHECKED);
	m_Configs.bDdrTest		 = (((CButton*)GetDlgItem(IDC_CHECK_DDR))->GetCheck() == BST_CHECKED);
	m_Configs.bRtcTest		 = (((CButton*)GetDlgItem(IDC_CHECK_RTC))->GetCheck() == BST_CHECKED);
	m_Configs.bRotaryTest		 = (((CButton*)GetDlgItem(IDC_CHECK_ROTARY))->GetCheck() == BST_CHECKED);
	CConfigDlg::OnOK();
}

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_LocalLang.TreeControls(m_hWnd,m_Configs.bDebug?TRUE:FALSE,this->IDD,false);
	((CButton*)GetDlgItem(IDC_CHECK_SDCARD    ))->SetCheck(m_Configs.bSdcardTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_WIFI    ))->SetCheck(m_Configs.bWifiTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_KEY    ))->SetCheck(m_Configs.bKeyTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_LED    ))->SetCheck(m_Configs.bLedTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_MONITOR    ))->SetCheck(m_Configs.bMonitorTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_PHONE    ))->SetCheck(m_Configs.bInterphoneTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_PTZ    ))->SetCheck(m_Configs.bPtzTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_IRCUT    ))->SetCheck(m_Configs.bIrcutTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_HDMI    ))->SetCheck(m_Configs.bHdmiTest               ?BST_CHECKED:BST_UNCHECKED);
	//((CButton*)GetDlgItem(IDC_CHECK_UID    ))->SetCheck(m_Configs.bWriteUid                ?BST_CHECKED:BST_UNCHECKED);
	//((CButton*)GetDlgItem(IDC_CHECK_LANMAC    ))->SetCheck(m_Configs.bBtTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_BT    ))->SetCheck(m_Configs.bBtTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_EMMC    ))->SetCheck(m_Configs.bEmmcTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_DDR    ))->SetCheck(m_Configs.bDdrTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_RTC    ))->SetCheck(m_Configs.bRtcTest                ?BST_CHECKED:BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_ROTARY    ))->SetCheck(m_Configs.bRotaryTest                ?BST_CHECKED:BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::OnBnClickedBtnCancel()
{
	// TODO: Add your control notification handler code here
	CConfigDlg::OnCancel();
}
