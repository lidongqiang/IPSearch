// UidDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPSearch.h"
#include "UidDlg.h"


// CUidDlg dialog

IMPLEMENT_DYNAMIC(CUidDlg, CDialog)

CUidDlg::CUidDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUidDlg::IDD, pParent)
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
