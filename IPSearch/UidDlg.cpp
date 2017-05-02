// UidDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPSearch.h"
#include "UidDlg.h"


// CUidDlg dialog

IMPLEMENT_DYNAMIC(CUidDlg, CDialog)

CUidDlg::CUidDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang,CWnd* pParent /*=NULL*/)
	: CDialog(CUidDlg::IDD, pParent),m_Configs(Configs),m_LocalLang(LocalLang)
{

}

CUidDlg::~CUidDlg()
{
}

void CUidDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUidDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUidDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUidDlg message handlers

void CUidDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	this->GetDlgItemText(IDC_EDIT_UID,strUid);
	this->GetDlgItemText(IDC_EDIT_MAC,strLanmac);
	OnOK();
}

BOOL CUidDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if (!m_Configs.bWriteUid)
	{
		GetDlgItem(IDC_EDIT_UID)->EnableWindow(FALSE);
	}
	if (!m_Configs.bWriteMac)
	{
		GetDlgItem(IDC_EDIT_MAC)->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
