// UidDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPSearch.h"
#include "UidDlg.h"
#include "cmfuns.h"


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
std::wstring CUidDlg::GetLocalString(std::wstring strKey)
{
	return m_LocalLang.GetLanStr(strKey);
}
void CUidDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString strPromt;
	if (m_Configs.bWriteUid)
	{
		this->GetDlgItemText(IDC_EDIT_UID,strUid);
		if (strUid.IsEmpty())
		{
			strPromt.Format(GetLocalString(_T("IDS_ERROR_INVALID_UID")).c_str());
			goto OnBnClickedOk_Exit;
		}
	}
	if (m_Configs.bWriteMac)
	{
		this->GetDlgItemText(IDC_EDIT_MAC,strLanmac);
		if(!CheckMacStr(strLanmac)) 
		{
			strPromt.Format(GetLocalString(_T("IDS_ERROR_INVALID_LANMAC")).c_str(),strLanmac);
			goto OnBnClickedOk_Exit;
		}
	}
	OnOK();
OnBnClickedOk_Exit:
	if(!strPromt.IsEmpty()) MessageBox(strPromt,GetLocalString(_T("IDS_ERROR_CAPTION")).c_str(),MB_OK|MB_ICONERROR);
	return;
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
