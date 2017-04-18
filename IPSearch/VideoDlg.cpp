// VideoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPSearch.h"
#include "VideoDlg.h"
#include <windows.h>
#include <iostream>

// Convert wstring to string
static std::string wstr2str(const std::wstring& arg)
{
	int requiredSize;
	requiredSize = WideCharToMultiByte(CP_ACP,0,arg.c_str(),arg.length(),NULL,0,NULL,NULL);
	std::string res;
	if (requiredSize<=0) {
		res = "";
		return res;
	}
	res.assign(requiredSize,'\0');
	WideCharToMultiByte(CP_ACP,0,arg.c_str(),arg.length(),const_cast<char*>(res.data()),requiredSize,NULL,NULL);
	return res;
}
// Convert string to wstring
static std::wstring str2wstr(const std::string& arg)
{
	int requiredSize;
	requiredSize = MultiByteToWideChar(CP_ACP,0,arg.c_str(),arg.length(),NULL,0);
	std::wstring res;
	if (requiredSize<=0) {
		res = L"";
		return res;
	}
	res.assign(requiredSize,L'\0');
	MultiByteToWideChar(CP_ACP,0,arg.c_str(),arg.length(),const_cast<wchar_t *>(res.data()),requiredSize);
	return res;
}
// CVideoDlg dialog

IMPLEMENT_DYNAMIC(CVideoDlg, CDialog)

CVideoDlg::CVideoDlg(CString strUrl,CWnd* pParent /*=NULL*/)
	: CDialog(CVideoDlg::IDD, pParent),m_strUrl(strUrl)
{

}

CVideoDlg::~CVideoDlg()
{
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_PLAY, &CVideoDlg::OnBnClickedBtnPlay)
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_BTN_CONFIG, &CVideoDlg::OnBnClickedBtnConfig)
END_MESSAGE_MAP()


// CVideoDlg message handlers

void CVideoDlg::OnBnClickedBtnPlay()
{
	// TODO: Add your control notification handler code here
	//GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
	const char * const vlc_args[] = {  
		"--demux=h264",  
		"--ipv4",  
		"--no-prefer-system-codecs",  
		"--rtsp-caching=300",  
		"--network-caching=500",  
		"--rtsp-tcp",  
		//"--verbose=2",  
		//"--extraintf=logger"  
	};  
	CString strUrl = _T("");
	//char *strUrl ;
	this->GetDlgItemText(IDC_EDIT_URL,strUrl);
	if (strUrl.IsEmpty())
	{
		AfxMessageBox(_T("please fix path of url !"));
		return;
	}
	if (m_vlcInst == NULL)  
	{  
		m_vlcInst = libvlc_new(sizeof(vlc_args)/sizeof(vlc_args[0]), vlc_args);  
	}  
	if (m_vlcMedia == NULL)  
	{  
		//  "rtsp://127.0.0.1:1234/vedio"  
		//m_vlcMedia = libvlc_media_new_location(m_vlcInst,wstr2str((LPCTSTR)strUrl).c_str());
		m_vlcMedia = libvlc_media_new_location(m_vlcInst,"rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov");

	}  
	if (m_vlcMplay == NULL)  
	{  
		m_vlcMplay = libvlc_media_player_new_from_media(m_vlcMedia);  
	}  

	HWND hWndVedio = GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd();
	//HWND hWndVedio = GetDlgItem(IDC_LIST_VIDEO)->GetSafeHwnd();
	libvlc_media_player_set_hwnd(m_vlcMplay, hWndVedio);  
	libvlc_media_player_play(m_vlcMplay);
	Sleep(10000);

	if (m_vlcMplay != NULL)  
	{  
		libvlc_media_player_release(m_vlcMplay);  
		m_vlcMplay = NULL;  
	}  
	if (m_vlcMedia != NULL)  
	{  
		libvlc_media_release(m_vlcMedia);  
		m_vlcMedia = NULL;  
	}  
	if (m_vlcInst != NULL)  
	{  
		libvlc_release(m_vlcInst);  
		m_vlcInst = NULL;  
	}  
}

BOOL CVideoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_vlcInst = NULL;
	m_vlcMedia = NULL;
	m_vlcMplay = NULL;
	
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(rect);
	m_nDlgWidth = rect.right - rect.left; 
	m_nDlgHeight = rect.bottom - rect.top;
	//计算分辨率
	m_nWidth = GetSystemMetrics(SM_CXSCREEN); 
	m_nHeight = GetSystemMetrics(SM_CYSCREEN);

	//计算放大倍数
	m_Multiple_width = float(m_nWidth)/float(m_nDlgWidth); 
	m_Mutiple_heith = float(m_nHeight)/float(m_nDlgHeight); 
	change_flag=TRUE;//这个是成员变量bool形，用来判断onsize执行时oninitdlg是否已经执行了

	this->SetDlgItemText(IDC_EDIT_URL,m_strUrl);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	//if (change_flag)//如果确定oninitdlg已经调用完毕.
	//{
	//	ReSize(IDC_STATIC_URL);
	//	ReSize(IDC_EDIT_URL);
	//	ReSize(ID_BTN_PLAY);
	//	ReSize(ID_BTN_CONFIG);
	//	ReSize(IDC_STATIC_VIDEO);
	//	//ReSize(IDC_LIST_VIDEO);

	//	//恢复放大倍数，并保存 (确保还原时候能够还原到原来的大小);

	//	m_Multiple_width = float(1)/ m_Multiple_width ; 
	//	m_Mutiple_heith = float(1)/m_Mutiple_heith ; 
	//}
}

void CVideoDlg::ReSize(int nID)
{
	CRect Rect; 
	GetDlgItem(nID)->GetWindowRect(Rect); 
	ScreenToClient(Rect); 
	//计算控件左上角点 
	CPoint OldTLPoint,TLPoint; 
	OldTLPoint = Rect.TopLeft(); 
	TLPoint.x = long(OldTLPoint.x *m_Multiple_width); 
	TLPoint.y = long(OldTLPoint.y * m_Mutiple_heith );

	//计算控件右下角点
	CPoint OldBRPoint,BRPoint; 
	OldBRPoint = Rect.BottomRight(); 
	BRPoint.x = long(OldBRPoint.x *m_Multiple_width); 
	BRPoint.y = long(OldBRPoint.y * m_Mutiple_heith ); 
	//移动控件到新矩形

	Rect.SetRect(TLPoint,BRPoint); 
	GetDlgItem(nID)->MoveWindow(Rect,TRUE); 
}

void CVideoDlg::OnBnClickedBtnConfig()
{
	// TODO: Add your control notification handler code here
	CBitConfigDlg bitConfdlg;
	bitConfdlg.DoModal();
}
