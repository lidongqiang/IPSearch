
// IPSearchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IPSearch.h"
#include "IPSearchDlg.h"
#include "afxtempl.h"
#include "Winnetwk.h"
#include "c_socket.h"
#include "cmd_process.h"
#include "./label/Label.h"
#include "cmFile.h"
#include "UidDlg.h"
#include <iostream>
#include <WinSock2.h>  
#include <Windows.h>  
#include <json/json.h>
#include <json/reader.h>

#define TEST_PORT 9090

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_BUFFER 256  
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

UINT ScanDeviceThread(LPVOID lpParam)
{
	ThreadInfo*   pParam   =   (ThreadInfo   *)lpParam; 
	CIPSearchDlg* pMainDlg = (CIPSearchDlg*)pParam->pDlg;
	pMainDlg->ScanDeviceProc(pParam->ip);
	return 0;
}
UINT TestDeviceThread(LPVOID lpParam)
{
	CIPSearchDlg* pMainDlg = (CIPSearchDlg*)lpParam;
	pMainDlg->TestProc();
	return 0;
}
UINT NextTestThread(LPVOID lpParam)
{
	TestInfo*   pParam   =   (TestInfo   *)lpParam; 
	CIPSearchDlg* pMainDlg = (CIPSearchDlg*)pParam->pDlg;
	pMainDlg->NextTestProc(&pParam->nResult);
	return 0;
}
UINT RecvDeviceThread(LPVOID lpParam)
{
	CIPSearchDlg* pMainDlg = (CIPSearchDlg*)lpParam;
	pMainDlg->RecvProc();
	return 0;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang);
private:
	CLabel          m_rklink;
	CLabel          m_AppName;
	CIniSettingBase &m_Configs;
	CIniLocalLan    &m_LocalLang;
// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	std::wstring GetLocalString(std::wstring strKey)
	{
		return m_LocalLang.GetLanStr(strKey);
	}
};

CAboutDlg::CAboutDlg(CIniSettingBase &Configs,CIniLocalLan &LocalLang) : CDialog(CAboutDlg::IDD)
	 ,m_Configs(Configs),m_LocalLang(LocalLang)
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_rklink.SetTransparent(TRUE);
	m_rklink.SetFontSize(8);
	m_rklink.SetFontBold(TRUE);
	m_rklink.SetText(TEXT("福州瑞芯微电子有限公司"), 0xFF1111);
	m_rklink.SetLink(TRUE,FALSE);
	m_rklink.SetHyperLink(CString(TEXT("http://www.rock-chips.com/")));
	/*m_rklink.SetLinkCursor( (HCURSOR)IDC_IBEAM); **/
	/*m_rklink.FlashText(TRUE);**/
	m_AppName.SetTransparent(TRUE);
	m_AppName.SetFontSize(8);
	m_AppName.SetFontBold(TRUE);
	m_AppName.SetText((GetLocalString(TEXT("IDS_TEXT_APPNAME"))+ TEXT(APP_VERSION)).c_str(), 0xFF1111);
	/*GetDlgItem(IDC_STATIC_APPNAME)->SetWindowText((m_LocalLang.GetStr(TEXT("APPNAME")) + TEXT(APP_VERSION)).c_str());**/
	return FALSE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_RK, m_rklink);
	DDX_Control(pDX, IDC_STATIC_APPNAME, m_AppName);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CIPSearchDlg 对话框




CIPSearchDlg::CIPSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPSearchDlg::IDD, pParent),m_listSelect(-1),m_bRun(false),m_bTestPass(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME4);
}

void CIPSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_listDevice);
	DDX_Control(pDX, IDC_LIST_INFO, m_listInfo);
	DDX_Control(pDX, IDC_LIST_TESTITEM, m_listTestItem);
}

BEGIN_MESSAGE_MAP(CIPSearchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_MSG,&CIPSearchDlg::OnHandleUpdateConfigMsg)
	ON_BN_CLICKED(ID_BTN_SERCH, &CIPSearchDlg::OnBnClickedBtnSerch)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CIPSearchDlg::OnBnClickedButtonTest)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DEVICE, &CIPSearchDlg::OnNMClickListDevice)
	ON_BN_CLICKED(ID_BTN_APPLY, &CIPSearchDlg::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CIPSearchDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PASS, &CIPSearchDlg::OnBnClickedButtonPass)
	ON_BN_CLICKED(IDC_BUTTON_FAIL, &CIPSearchDlg::OnBnClickedButtonFail)
	ON_WM_CLOSE()
	ON_COMMAND(ID_HELP_ABOUT, &CIPSearchDlg::OnHelpAbout)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CIPSearchDlg::OnBnClickedButtonExit)
	ON_MESSAGE(WM_UPDATE_TESTINFO_MSG,&CIPSearchDlg::OnHandleUpdateTestinfoMsg)
END_MESSAGE_MAP()


// CIPSearchDlg 消息处理程序

BOOL CIPSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//m_PlayDlg.Create(IDD_PLAYER_DIALOG);
	m_pRecvThread = NULL;
	//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	initUi();

	//加载config.ini
	if (!LoadConfig())
	{
		MessageBox(_T("Loading config file failed!"),_T("ERROR"),MB_ICONERROR|MB_OK);
		exit(-1);
	}
	if (m_Configs.strLogPath.empty())
	{
		m_Configs.strLogPath=m_strModulePath + _T("Log\\");
	}
	if (m_Configs.strTestPath.empty())
	{
		m_Configs.strTestPath=m_strModulePath + _T("test\\");
	}
	
	if(m_Configs.nLogLevel != DLEVEL_NONE ) {
		CLogger::DEBUG_LEVEL level = m_Configs.nLogLevel == DLEVEL_DEBUG?CLogger::DEBUG_ALL:
			(m_Configs.nLogLevel  == DLEVEL_INFO ?CLogger::DEBUG_INFO:CLogger::DEBUG_ERROR);
	m_pLog = CLogger::StartLog((m_Configs.strLogPath + TEXT("PCBATool-") + CLogger::TimeStr(true, true)).c_str(), level);    
	}

	LDEGMSGW((CLogger::DEBUG_INFO, _T("PCBATool(echo) start run")));
	UpdateTestitemList();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIPSearchDlg::initUi()
{
	m_pRecvThread = NULL;
	CRect rect;   
	// 获取编程语言列表视图控件的位置和大小   
	m_listTestItem.GetClientRect(&rect);   

	// 为列表视图控件添加全行选中和栅格风格   
	m_listTestItem.SetExtendedStyle(m_listTestItem.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
	m_listTestItem.InsertColumn(0,_T("测试项"),LVCFMT_CENTER, rect.Width()/2, 0);
	m_listTestItem.InsertColumn(1,_T("类别"),LVCFMT_CENTER, rect.Width()/4, 1);
	m_listTestItem.InsertColumn(2,_T("测试状态"),LVCFMT_CENTER, rect.Width()/4, 2);
	m_listTestItem.DeleteAllItems();
	LOGFONT logFont;
	ZeroMemory(&logFont,sizeof(logFont));
	logFont.lfHeight = -20;
	logFont.lfWidth = 10;
	logFont.lfWeight = FW_NORMAL;
	logFont.lfCharSet = GB2312_CHARSET;
	_tcscpy(logFont.lfFaceName,_T("宋体"));

	m_listTestItem.SetColFont(0,logFont);
	m_listTestItem.SetColFont(1,logFont);

	logFont.lfHeight = -20;
	logFont.lfWidth = 10;
	logFont.lfWeight = FW_BOLD;
	m_listTestItem.SetColFont(2,logFont);
	
	HFONT hFont;
	hFont = CreateFontIndirect(&logFont);
	GetDlgItem(IDC_LIST_TESTITEM)->SendMessage(WM_SETFONT, (WPARAM)hFont, TRUE);

	m_listTestItem.SetHeaderCtrlFont(logFont);

	// 获取编程语言列表视图控件的位置和大小   
	m_listDevice.GetClientRect(&rect); 
	m_listDevice.SetExtendedStyle(m_listDevice.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
	m_listDevice.InsertColumn(0,_T("UID"),LVCFMT_CENTER, rect.Width()/4, 0);
	m_listDevice.InsertColumn(1,_T("IP"),LVCFMT_CENTER, rect.Width()/2, 1);
	m_listDevice.InsertColumn(2,_T("设备名称"),LVCFMT_CENTER, rect.Width()/4, 2);
	m_listDevice.DeleteAllItems();

	CFont font;
	font.CreateFont(-15,10,0,0,FW_NORMAL,FALSE,FALSE,0,  
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,  
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,_T("宋体"));
	m_listInfo.SetFont(&font);
	font.Detach();
	m_listInfo.SetWindowBKColor(RGB(0,0,0));
	//m_lbVideo.SetBackground(RGB(0,0,0));

	GetDlgItem(IDC_BUTTON_EXIT)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(FALSE);
}

LRESULT CIPSearchDlg::OnHandleUpdateConfigMsg(WPARAM wParam,LPARAM lParam)
{

	switch(wParam) {
	case UPDATE_CONFIG: 
		//UpdateConfigInterface();
		//StoreConfig(STORE_ALL);
		break;
	case UPDATE_WINDOW:
		RedrawWindow();
		break;
	case UPDATE_TEST_FINISH:
		//UpdateTestCount2UI();
		break;
	case UPDATE_TEST_EXIT:
		/*if(m_Configs.basic.bStressTest ||m_Configs.basic.bAutoTest) **/
		//m_bStarWrite = FALSE;
		//GetDlgItem(IDC_BUTTON_CONFIG)->EnableWindow(TRUE);
		//SetDlgItemText(IDC_BUTTON_TEST,GetLocalString(_T("IDS_TEXT_TEST_BUTTON")));
		break;
	case UPDATE_PROMPT:
		//if (lParam == PROMPT_TESTING) {
		//	m_lblPrompt.SetTextColor(RGB(0,0,0));
		//	m_lblPrompt.SetWindowText(GetLocalString(_T("IDS_TEXT_TEST")));
		//} else if (lParam==PROMPT_PASS) {
		//	m_lblPrompt.SetTextColor(RGB(0,255,0));
		//	m_lblPrompt.SetWindowText(GetLocalString(_T("IDS_TEXT_SUCCESS")));
		//} else if(lParam==PROMPT_FAIL) {
		//	if( m_Configs.PS.bStopIfFail&& m_Configs.basic.bStressTest) {
		//		OnBnClickedButtonTest();
		//	}
		//	m_lblPrompt.SetTextColor(RGB(255,0,0));
		//	m_lblPrompt.SetWindowText(GetLocalString(_T("IDS_TEXT_FAIL")));
		//} else {
		//	m_lblPrompt.SetWindowText(_T(""));
		//}
		//m_lblPrompt.RedrawWindow();
		break;
	case UPDATE_LIST:
		if (lParam == LIST_EMPTY) {
			m_listInfo.ResetContent();
			/*m_lblCurrentSN.SetWindowText(_T("")); **/
			/*m_lblCurrentSN.SetFocus(); **/
		} else {
			PSTRUCT_LIST_LINE pLine = (PSTRUCT_LIST_LINE)lParam;
			if (pLine->flag == LIST_INFO) {
				m_listInfo.AddLine(CXListBox::White,CXListBox::Black,pLine->pszLineText); 
			} else if (pLine->flag == LIST_TIME) {
				m_listInfo.AddLine(CXListBox::White,CXListBox::Purple,pLine->pszLineText);
			} else if (pLine->flag == LIST_WARN){ 
				m_listInfo.AddLine(CXListBox::Yellow,CXListBox::Black,pLine->pszLineText);
			} else if (pLine->flag == LIST_PASS){ 
				m_listInfo.AddLine(CXListBox::Green,CXListBox::Black,pLine->pszLineText);
			} else {
				m_listInfo.AddLine(CXListBox::Red,CXListBox::Black,pLine->pszLineText);
			}
			m_listInfo.SetCurSel(m_listInfo.GetCount()-1);
			delete pLine;
		}
		break;
	default:
		break;        
	}

	return 0;
}

void CIPSearchDlg::AddPrompt(CString strPrompt,BOOL bError,INT iColor)
{
	PSTRUCT_LIST_LINE   pLine=NULL;
	CString             strTime;
	SYSTEMTIME          curTime;
	LDEGMSGW((CLogger::DEBUG_INFO, TEXT("Info:%s"),(LPCTSTR)strPrompt));
	GetLocalTime( &curTime );
	strTime.Format(_T("%02d:%02d:%02d %03d\t"),curTime.wHour,curTime.wMinute,curTime.wSecond,curTime.wMilliseconds);
	strPrompt   = strTime + strPrompt;
	pLine       = new STRUCT_LIST_LINE;
	if (!pLine) {
		return;
	}
	_tcscpy_s(pLine->pszLineText,(LPTSTR)(LPCTSTR)strPrompt);
	if(iColor) {
		pLine->flag = iColor;
		PostMessage(WM_UPDATE_MSG,UPDATE_LIST,(LPARAM)pLine);
	} else {
		if (bError) {
			pLine->flag = LIST_ERROR;
			PostMessage(WM_UPDATE_MSG,UPDATE_LIST,(LPARAM)pLine);
		} else {
			pLine->flag = LIST_INFO;
			PostMessage(WM_UPDATE_MSG,UPDATE_LIST,(LPARAM)pLine);
		}
	}
}

void CIPSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout(m_Configs,m_LocalLan);
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIPSearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIPSearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIPSearchDlg::OnBnClickedBtnSerch()
{
	// TODO: Add your control notification handler code here

	std::string strUid,strAddr,strDevname,strMac;
	STRUCT_DEV_INFO DevInfo;
	m_DevList.clear();
	m_listDevice.DeleteAllItems();
	CWinThread *pA = NULL;
	HANDLE hThread[3];
	int i = 0;
	ThreadInfo param;

	this->GetDlgItem(ID_BTN_SERCH)->EnableWindow(FALSE);

	WORD wVersionRequested = MAKEWORD(2, 2);  
	WSADATA wsaData;  
	if(0 != WSAStartup(wVersionRequested, &wsaData))  
	{  
		AfxMessageBox(_T("WSAStartup failed with error: %d/n"), GetLastError());
		return ;  
	}  
	if(2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))  
	{  
		AfxMessageBox(_T("Socket version not supported./n"));
		WSACleanup();  
		return ;  
	}
	char hostname[1024] = {0};
	gethostname(hostname, sizeof(hostname));    //获得本地主机名
	PHOSTENT hostinfo = gethostbyname(hostname);//信息结构体
	while(*(hostinfo->h_addr_list) != NULL)        //输出IP地址
	{
		param.ip = inet_ntoa(*(struct in_addr *) *hostinfo->h_addr_list);
		param.pDlg = this;
		pA = AfxBeginThread(ScanDeviceThread,&param);
		hThread[i++] = pA->m_hThread;
		hostinfo->h_addr_list++;
		::WaitForSingleObject(pA->m_hThread,INFINITE);
	}
	//Wait until all threads have terminated.
	//::WaitForMultipleObjects(i-1,hThread,TRUE,INFINITE);
	int nIndex;
	for (nIndex=0;nIndex<m_DevList.size();nIndex++)
	{
		m_listDevice.InsertItem(nIndex,_T(""));
		m_listDevice.SetItemText(nIndex,0,m_DevList[nIndex].strUid.c_str());
		m_listDevice.SetItemText(nIndex,1,m_DevList[nIndex].strIP.c_str());
		m_listDevice.SetItemText(nIndex,2,m_DevList[nIndex].strDevName.c_str());
	}

	WSACleanup();

	if (nIndex==0)
	{
		MessageBox(GetLocalString(_T("IDS_SEARCH_NODEVICE")).c_str(),_T("IPCamera"),MB_OK|MB_ICONWARNING);
	}
	//else
		//AfxMessageBox(_T("搜索完成"));
		//MessageBox(GetLocalString(_T("IDS_SEARCH_DEVICE_FINISH")).c_str(),_T("IPCamera"),MB_OK|MB_ICONWARNING);
	this->GetDlgItem(ID_BTN_SERCH)->EnableWindow(TRUE);
}

void CIPSearchDlg::ParseDevInfo(char *msg,CString &strUid,CString &strAddr,CString &strDevname)
{
	CString strMsg(msg);
	Json::Reader reader;
	Json::Value root;
	std::string strTest;
	strTest = "DEVICENAME";
	if (reader.parse(wstr2str(strMsg.GetBuffer(strMsg.GetLength())),root))
	{
		strUid = root["UID"].asCString();
		strAddr = root["IP"].asCString();
		strDevname = root[strTest].asCString();
	}
	strMsg.ReleaseBuffer(strMsg.GetLength());
}
void CIPSearchDlg::GetOtherIp()
{
	int nIndex = 0;
	m_listDevice.DeleteAllItems();
	//保存搜索到得计算机ip和计算机名
	CString strTemp;
	//结构保存给定主机信息，如主机名、ip地址
	struct hostent *host;
	//结构体in_add用于保存ip地址
	struct in_addr* ptr;
	//设置枚举资源范围
	DWORD dwScope=RESOURCE_CONTEXT;
	//NETRESOURCE结构返回枚举的网络资源列表信息
	NETRESOURCE *NetResource=NULL;
	HANDLE hEnum;
	//启动对网络资源进行枚举
	WNetOpenEnum(dwScope,NULL,NULL,NULL,&hEnum);
	//wsadata结构包含winsock信息
	WSADATA wsaData;
	//初始化winsock
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (hEnum)
	{
		DWORD Count=0xFFFFFFF;
		DWORD BufferSize=8192;
		LPVOID Buffer=new char[8192];
		//枚举网络资源
		WNetEnumResource(hEnum,&Count,Buffer,&BufferSize);
		//获取枚举结果
		NetResource=(NETRESOURCE*)Buffer;
		//char szHostName[200];
		//获取枚举数目
		int nmax=(BufferSize)/(sizeof(NETRESOURCE));
		//进行输出设置
		for (int i=0;i<nmax;i++,NetResource++)
		{
			//若当前资源包含有其他资源并且为任意资源，则输出该资源
			if (NetResource->dwUsage==RESOURCEUSAGE_CONTAINER&&NetResource->dwType==RESOURCETYPE_ANY)
			{
				//获取计算机名称信息
				CString strFullName=NetResource->lpRemoteName;
				//获取机器名称，机器名称在最后部分
				if (0==strFullName.Left(2).Compare(_T("\\\\")))
				{
					int nlen=strFullName.GetLength();
					strFullName=strFullName.Right(nlen-2);

					//获取机器ip地址
					host=gethostbyname(wstr2str(strFullName.GetString()).c_str());
					if (host==NULL)continue;
					//获取ip地址每部分值
					ptr=(struct in_addr*)host->h_addr_list[0];
					int x=ptr->S_un.S_un_b.s_b1;
					int b=ptr->S_un.S_un_b.s_b2;
					int c=ptr->S_un.S_un_b.s_b3;
					int d=ptr->S_un.S_un_b.s_b4;
					//格式化ip地址进行输出
					strTemp.Format(_T("%d.%d.%d.%d"),x,b,c,d);
					m_listDevice.InsertItem(nIndex,strTemp,1);
					strTemp.Format(_T("%s"),host->h_name);
					m_listDevice.SetItemText(nIndex,2,strTemp);
				}
			}   
		}
		delete Buffer;
		//
		WNetCloseEnum(hEnum);
	}
	WSACleanup();
}

bool CIPSearchDlg::OnStartTest()
{
	CString     strPromt;

	if (m_listSelect != -1)
	{
		initTestCase();
		if (m_TestCaseList.size()==0&&!m_Configs.bWriteMac&&!m_Configs.bWriteUid)
		{
			strPromt.Format(GetLocalString(_T("IDS_ERROR_NO_TEST")).c_str());
			goto OnStartTestExit;
		}
		m_pTestThread = AfxBeginThread(TestDeviceThread,this);
		if(NULL == m_pTestThread) {
			goto OnStartTestExit; /*generrally never to here **/
		}
	}
	else
	{
		strPromt.Format(GetLocalString(_T("IDS_ERROR_NO_DEVICE")).c_str());
		goto OnStartTestExit; /*generrally never to here **/
	}
	return TRUE;
OnStartTestExit:
	if(!strPromt.IsEmpty()) {
		MessageBox(strPromt,GetLocalString(_T("IDS_ERROR_CAPTION")).c_str(),MB_OK|MB_ICONERROR);
	}
	return FALSE;
}
void CIPSearchDlg::OnBnClickedButtonTest()
{
	// TODO: Add your control notification handler code here
	if (m_bRun)
	{
		m_bRun = false;
		this->SetDlgItemText(IDC_BUTTON_TEST,GetLocalString(_T("IDS_TEXT_STOPING_BUTTON")).c_str());
		Sleep(500);
		ExitTest();
		closesocket(m_TestSocket);
		UpdateTestitemList();

		//m_cAVPlayer.Stop();
		this->SetDlgItemText(IDC_BUTTON_TEST,GetLocalString(_T("START")).c_str());
		this->GetDlgItem(IDC_LIST_DEVICE)->EnableWindow(TRUE);
		this->GetDlgItem(ID_BTN_APPLY)->EnableWindow(TRUE);
		this->GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);
		this->GetDlgItem(ID_BTN_SERCH)->EnableWindow(TRUE);
	
		AddPrompt(GetLocalString(_T("IDS_INFO_USER_ABORT")).c_str(),TRUE);
	}
	else
	{
		if (OnStartTest())
		{
			this->SetDlgItemText(IDC_BUTTON_TEST,GetLocalString(_T("IDS_TEXT_STOP_BUTTON")).c_str());
			GetDlgItem(ID_BTN_APPLY)->EnableWindow(FALSE);
			this->GetDlgItem(IDC_LIST_DEVICE)->EnableWindow(FALSE);
			this->GetDlgItem(ID_BTN_SERCH)->EnableWindow(FALSE);
		}
	}

}

void CIPSearchDlg::OnNMClickListDevice(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString strLangName;    // 选择语言的名称字符串   
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR; 
	if (-1 != pNMListView->iItem)        // 如果iItem不是-1，就说明有列表项被选择   
	{   
		m_listSelect = pNMListView->iItem;
		// 获取被选择列表项第一个子项的文本   
		strLangName = m_listDevice.GetItemText(pNMListView->iItem, 0);   
		// 将选择的语言显示与编辑框中   
		SetDlgItemText(IDC_EDIT_UID, m_listDevice.GetItemText(pNMListView->iItem, 0));
		SetDlgItemText(IDC_EDIT_IP, m_listDevice.GetItemText(pNMListView->iItem, 1));
		SetDlgItemText(IDC_EDIT_DEVNAME, m_listDevice.GetItemText(pNMListView->iItem, 2));
		SetDlgItemText(IDC_EDIT_PORT,m_DevList.at(m_listSelect).strMac.c_str());
		m_strIp = m_listDevice.GetItemText(pNMListView->iItem, 1);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_UID,_T(""));
		SetDlgItemText(IDC_EDIT_IP,_T(""));
		SetDlgItemText(IDC_EDIT_DEVNAME, _T(""));
		m_listSelect = -1;
		m_strIp = _T("");
	}
}

void CIPSearchDlg::RtspPlay()
{
	int ret;
	CString strPrompt;
	
	ret = m_DevTest.CameraTest(m_TestSocket,wstr2str(m_Configs.strCameraName));
	//if (ret<0)
	//{
	//	strPrompt.Format(GetLocalString(_T("FAILED")).c_str(),GetLocalString(m_Configs.strCameraName).c_str(),ret);
	//	AddPrompt(strPrompt,True);
	//}
	Sleep(1000);
	CString strUrl;
	strUrl.Format(_T("rtsp://%s/webcam"),m_strIp);
	//m_cAVPlayer.Play(wstr2str((LPCTSTR)strUrl));
}
void CIPSearchDlg::OnBnClickedBtnApply()
{
	// TODO: Add your control notification handler code here
	CConfigDlg ConfgDlg(m_Configs,m_LocalLan);
	if (IDOK == ConfgDlg.DoModal())
	{
		m_Configs.SaveToolSetting(std::wstring(TEXT("")));
		UpdateTestitemList();
	}

}
int CIPSearchDlg::ScanDeviceProc(LPVOID lpParameter)
{
	std::string strUid,strAddr,strDevname,strMac;
	STRUCT_DEV_INFO DevInfo;
	char	szIp[16] = {0};
	strcpy (szIp, (char*)lpParameter);
	WORD wVersionRequested = MAKEWORD(2, 2);  
	WSADATA wsaData;  
	if(0 != WSAStartup(wVersionRequested, &wsaData))  
	{  
		AfxMessageBox(_T("WSAStartup failed with error: %d/n"), GetLastError());
		return -1;  
	}  
	if(2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))  
	{  
		AfxMessageBox(_T("Socket version not supported./n"));
		WSACleanup();  
		return -2;  
	}  

	SOCKET sockClient	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sockClient)
	{
		//printf ("socket create failed. ip=[%s] errno=[%d] ", szIp, WSAGetLastError());
		AfxMessageBox(_T("socket create failed. ip=[%s] errno=[%d] "));
		return	-2;
	}
	BOOL bBroadcast = TRUE;                             
	if (0 != setsockopt ( sockClient,SOL_SOCKET,SO_BROADCAST, (CHAR *)&bBroadcast, sizeof(BOOL)))
	{
		AfxMessageBox (_T("setsockopt failed. ip=[%s] errno=[%d]"));
		return	-3;
	}
	//int iMode = 1; //0：阻塞
	//ioctlsocket(sock,FIONBIO, (u_long FAR*) &iMode);//非阻塞设置
	int nNetTimeout=3000;//1秒，
	//设置发送超时
	setsockopt(sockClient,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int));
	//设置接收超时
	setsockopt(sockClient,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
	SOCKADDR_IN	addrClient	= {0};
	addrClient.sin_family	= AF_INET;
	addrClient.sin_addr.s_addr	= inet_addr(szIp);
	addrClient.sin_port	= 0;	/// 0 表示由系统自动分配端口号
	if (0 != bind (sockClient, (sockaddr*)&addrClient, sizeof(addrClient)))
	{
		AfxMessageBox (_T("bind failed.ip=[%s] errno=[%d]\n"));
	}
	SOCKADDR_IN addrServer; 
	memset(&addrServer, 0, sizeof(addrServer));  
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	addrServer.sin_port = htons (18888);
	int len = sizeof(addrServer);

	char szBuf[MAX_BUFFER] = {0};
	char buf[] = {"CMD_DISCOVER"};
	int n = 10;
	int nIndex = 0;
	if(SOCKET_ERROR == sendto(sockClient, buf, sizeof(buf), 0, (LPSOCKADDR)&addrServer, sizeof(SOCKADDR_IN)))  
	{  
		AfxMessageBox(_T("sendto failed with error: %d\n"));
	}
	while(1)  
	{  
		memset(szBuf,0,255);
		if (SOCKET_ERROR == recvfrom(sockClient, szBuf, 255, 0, (LPSOCKADDR)&addrServer, &len))
		{
			break;
		}
		if (strlen(szBuf) != 0)
		{
			m_Json.JsontoItem("UID",strUid,"IP",strAddr,"DEVICENAME",strDevname,"MAC",strMac,szBuf);
			DevInfo.strUid = str2wstr(strUid);
			DevInfo.strIP = str2wstr(strAddr);
			DevInfo.strDevName = str2wstr(strDevname);
			DevInfo.strMac = str2wstr(strMac);
			m_DevList.push_back(DevInfo);
			//this->SetDlgItemText(IDC_EDIT_PORT,str2wstr(strMac).c_str());
			//m_listDevice.InsertItem(nIndex,_T(""));
			//m_listDevice.SetItemText(nIndex,0,str2wstr(strUid).c_str());
			//m_listDevice.SetItemText(nIndex,1,str2wstr(strAddr).c_str());
			//m_listDevice.SetItemText(nIndex,2,str2wstr(strDevname).c_str());
			nIndex++;
		}
		Sleep(100);  
	}  
	closesocket(sockClient); 
	WSACleanup();
	return	0;
}
BOOL CIPSearchDlg::TestProc()
{
	//1.开始测试  向设备端发送命令命令{"TYPE":"CMD", "CMD":"ENTER" }
	int ret;
	int i;
	int nSdCardTest=-1;
	int nWifiTest=-1;
	std::string strMsg,strSta,strOutput;
	bool bQuery = true;
	bool bMaunlTest=false;
	//int nErrCode;
	CString strPrompt;
	CLogger         *logger = NULL;

	m_bRun = true;
	if (m_listInfo.GetCount()>0) {
		PostMessage(WM_UPDATE_MSG,UPDATE_LIST,LIST_EMPTY);
	}
	CLogger::DEBUG_LEVEL level = CLogger::DEBUG_ALL;
	logger = CLogger::StartLog((m_Configs.strLogPath +  m_strIp.GetBuffer(m_strIp.GetLength()) + TEXT("-") + CLogger::TimeStr(true, true)).c_str(), level);
	m_strIp.ReleaseBuffer(m_strIp.GetLength());
	if(logger) m_DevTest.StartLog(logger);

	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(TRUE);
	m_DevTest.SetTestPath(m_Configs.strTestPath);
	m_DevTest.SetDevIp(m_strIp.GetBuffer(m_strIp.GetLength()));
	m_DevTest.SetWifiTest(m_Configs.strWifiName);
	m_strIp.ReleaseBuffer(m_strIp.GetLength());
	//1.连接设备
	strPrompt.Format(GetLocalString(_T("IDS_INFO_CONN_DEVICE")).c_str(),m_strIp);
	AddPrompt(strPrompt);
	ret = connect_dev();
	if (ret<0)
	{
		strPrompt.Format(GetLocalString(_T("IDS_ERROR_CONN_FAIL")).c_str(),ret);
		AddPrompt(strPrompt,TRUE);
		return FALSE;
	}
	strPrompt.Format(GetLocalString(_T("IDS_INFO_CONN_PASS")).c_str());
	AddPrompt(strPrompt);

	if (!m_bRun)
	{
		goto TestExit;
	}
	strPrompt.Format(GetLocalString(_T("IDS_INFO_ENTER_TEST")).c_str());
	AddPrompt(strPrompt);
	ret = m_DevTest.EnterTestMode(m_TestSocket);
	if (ret < 0)
	{
		strPrompt.Format(GetLocalString(_T("IDS_INFO_ENTER_TEST_FAIL")).c_str(),ret);
		AddPrompt(strPrompt,TRUE);
		return FALSE;
	}
	else
	{
		strPrompt.Format(GetLocalString(_T("IDS_INFO_ENTER_TEST_PASS")).c_str());
		AddPrompt(strPrompt);
	}

	//打开rtsp流
	if (!m_bRun)
	{
		goto TestExit;
	}

	//4.下载测试程序，发送开始测试命令，{"TYPE":"CMD", "TEST_ITEM":"KEY-TEST", "CMD":"START" }
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 0)
		{
			strPrompt.Format(_T("%s:%s"),GetLocalString(m_TestCaseList[i].TestName).c_str(),GetLocalString(_T("IDS_TESTING")).c_str());
			AddPrompt(strPrompt,FALSE,LIST_WARN);
			m_TestCaseList[i].nTestStatus = 1;
			PostMessage(WM_UPDATE_TESTINFO_MSG,0,0);
			ret = DoTestItem(m_TestCaseList[i].TestName,strOutput);
			if (ret<0)
			{
				m_TestCaseList[i].nTestStatus = -1;
				strPrompt.Format(GetLocalString(_T("FAILED")).c_str(),GetLocalString(m_TestCaseList[i].TestName).c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				PostMessage(WM_UPDATE_TESTINFO_MSG,0,0);
			}
			else if (ret == 2)
			{
				if (m_TestCaseList[i].TestName.compare(m_Configs.strKeyName)==0)
				{
					strPrompt.Format(GetLocalString(_T("IDS_KEY_DOWN")).c_str());
					AddPrompt(strPrompt,FALSE,LIST_WARN);
					m_listTestItem.SetItemText(i,2,strPrompt);
				}
				else
				{
					strPrompt.Format(GetLocalString(_T("IDS_ROTARY")).c_str());
					AddPrompt(strPrompt,FALSE,LIST_WARN);
					m_listTestItem.SetItemText(i,2,strPrompt);
				}
			}
			//所有自动测试项完，第一个人工测试项
			if (!m_TestCaseList[i].bAuto)
			{
				bMaunlTest = true;
				break;
			}
		}
	}
	while(bQuery)
	{
		bQuery = false;
		for (i=0;i<m_TestCaseList.size();i++)
		{
			if (m_TestCaseList[i].nTestStatus == 1 &&m_TestCaseList[i].bAuto)
			{
				bQuery = true;
				ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName),strOutput);
				if (ret == 1)
				{
					ret = m_DevTest.StopTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
					if (ret<0)
					{
						m_TestCaseList[i].nTestStatus = -1;
						strPrompt.Format(GetLocalString(_T("IDS_STOP_TEST_FAIL")).c_str(),GetLocalString(m_TestCaseList[i].TestName).c_str(),ret);
						AddPrompt(strPrompt,TRUE);
						PostMessage(WM_UPDATE_TESTINFO_MSG,0,0);
					}
					else
					{
						m_TestCaseList[i].nTestStatus = 2;
						strPrompt.Format(GetLocalString(_T("SUCCESS")).c_str(),GetLocalString(m_TestCaseList[i].TestName).c_str());
						AddPrompt(strPrompt,FALSE,LIST_PASS);
						PostMessage(WM_UPDATE_TESTINFO_MSG,0,0);
					}
				}
				else if (ret < 0)
				{
					strPrompt.Format(GetLocalString(_T("FAILED")).c_str(),GetLocalString(m_TestCaseList[i].TestName).c_str(),ret);
					AddPrompt(strPrompt,TRUE);
					m_TestCaseList[i].nTestStatus = -1;
					PostMessage(WM_UPDATE_TESTINFO_MSG,0,0);
				}
			}
		}
		if (!m_bRun)
		{
			goto TestExit;
		}
		Sleep(200);
	}
	
	if (!m_bRun)
	{
		goto TestExit;
	}

TestExit:
	if (m_bRun&&!bMaunlTest)
	{
		SaveTestResult();
		GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);
	}

    m_pTestThread = NULL;
	return TRUE;
}
std::wstring CIPSearchDlg::GetLocalString(std::wstring strKey)
{
	return m_LocalLan.GetLanStr(strKey);
}

int CIPSearchDlg::connect_dev()
{
	WORD wVersionRequested = MAKEWORD(2, 2);  
	WSADATA wsaData;  
	if(0 != WSAStartup(wVersionRequested, &wsaData))  
	{  
		AfxMessageBox(_T("WSAStartup failed with error: %d/n"), GetLastError());
		return -1;  
	}  
	if(2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))  
	{  
		AfxMessageBox(_T("Socket version not supported./n"));
		WSACleanup();  
		return -1;  
	}
	m_TestSocket = socket(AF_INET,SOCK_STREAM,0);//创建套接字（socket）
	if (INVALID_SOCKET==m_TestSocket)
	{
		return -2;
	}

	//int nNetTimeout=3000;//5秒
	//设置发送超时
	//setsockopt(m_TestSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int));
	//设置接收超时
	//setsockopt(m_TestSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
	Timeout(m_TestSocket,5000);

	SOCKADDR_IN addrSrv; 
	addrSrv.sin_addr.S_un.S_addr=inet_addr(wstr2str((LPCTSTR)m_strIp).c_str()); 
	//addrSrv.sin_addr.S_un.S_addr=inet_addr("172.16.14.156");
	addrSrv.sin_family=AF_INET; 
	addrSrv.sin_port=htons((u_short)6666 ); 
	//connect(m_TestSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//向设备发出连接请求（connect) 
	if (connect(m_TestSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		closesocket(m_TestSocket);
		return -3;
	}
	return 0;
}
//自动测试项测试信息更新
void CIPSearchDlg::UpdateTestInfo(std::string strMsg)
{
	std::string strSta,strRes,strTestItem,strResult;
	int nErrCode;
	CString strInfo;
	m_Json.JsontoItem("STATUS",strSta,"TEST_ITEM",strTestItem,"RES",strRes,"RESULT",strResult,strMsg);
	m_Json.JsontoInt("ERR_CODE",nErrCode,strMsg);

	//1.应答异常 STATUS NAK（测试项名称+失败+ERR_CODE）
	//2.应答正常，测试失败 （测试项名称+失败+ERR_CODE）
	if (strSta.compare("NAK")==0||strResult.compare("FAIL")==0)
	{
		strInfo.Format(_T("%s 测试失败(错误码:%d)"),str2wstr(strTestItem).c_str(),nErrCode);
		AddPrompt(strInfo,TRUE);
		m_bTestPass = false;
	}

	//3.测试结束，保存测试结果成功 （测试项名称+成功）
	if (strRes.compare("SAVE")==0&&strSta.compare("ACK")==0)
	{
		strInfo.Format(_T("%s 测试成功"),str2wstr(strTestItem).c_str());
		AddPrompt(strInfo);
	}
	
}

void CIPSearchDlg::initTestCase()
{
	STRUCT_TEST_CASE TestCase;
	m_TestCaseList.clear();
	if (m_Configs.bSdcardTest)
	{
		TestCase.TestName = m_Configs.strSdcardName;
		TestCase.bAuto = true;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bWifiTest)
	{
		TestCase.TestName = m_Configs.strWifiName;
		TestCase.bAuto = true;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bBtTest)
	{
		TestCase.TestName = m_Configs.strBtTest;
		TestCase.bAuto = true;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bDdrTest)
	{
		TestCase.TestName = m_Configs.strDdrTest;
		TestCase.bAuto = true;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bEmmcTest)
	{
		TestCase.TestName = m_Configs.strEmmcTest;
		TestCase.bAuto = true;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bRtcTest)
	{
		TestCase.TestName = m_Configs.strRtcTest;
		TestCase.bAuto = true;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bLedTest)
	{
		TestCase.TestName = m_Configs.strLedName;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bIrcutTest)
	{
		TestCase.TestName = m_Configs.strIrcutName;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bMonitorTest)
	{
		TestCase.TestName = m_Configs.strMonitorName;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bInterphoneTest)
	{
		TestCase.TestName = m_Configs.strInterphoneName;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bPtzTest)
	{
		TestCase.TestName = m_Configs.strPtzName;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bHdmiTest)
	{
		TestCase.TestName = m_Configs.strHdmiName;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bKeyTest)
	{
		TestCase.TestName = m_Configs.strKeyName;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bRotaryTest)
	{
		TestCase.TestName = m_Configs.strRotaryTest;
		TestCase.bAuto = false;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
}
void CIPSearchDlg::UpdateTestitemList()
{
	initTestCase();
	m_listTestItem.DeleteAllItems();
	for (int i=0;i<m_TestCaseList.size();i++)
	{
		m_listTestItem.InsertItem(i,_T(""));
		m_listTestItem.SetItemText(i,0,GetLocalString(m_TestCaseList[i].TestName).c_str());
		if (m_TestCaseList[i].bAuto)
		{
			m_listTestItem.SetItemText(i,1,GetLocalString(_T("IDS_AUTOTEST")).c_str());
		}
		else
		{
			m_listTestItem.SetItemText(i,1,GetLocalString(_T("IDS_MANUL")).c_str());
		}
	}
}
BOOL CIPSearchDlg::RecvProc()
{
	int ret;
	int i_index = 0;
	char msg[255];
	std::string strTestCase,strRes,strStat;
	Json::Reader reader;
	Json::Value root;
	while(m_bRun)
	{
		ret = recv(m_TestSocket,msg,255,0);
		if (ret < 0)
		{
			//AfxMessageBox(_T("RecvProc:recv data failed!"));
			//AddPrompt(_T("设备应答超时!"),TRUE);
			return FALSE;
		}
		//更新界面测试信息
		UpdateTestInfo(msg);

		//解析消息包，对消息进行分派
		m_Json.JsontoItem("RES",strRes,"STATUS",strStat,"TEST_ITEM",strTestCase,msg);
		//if (reader.parse(msg,root))
		//{
		//	strTestCase = root["TEST_ITEM"].asString();
		//	strRes = root["RES"].asString();
		//}
		if (strStat.compare("NAK")==0||strRes.compare("EXIT")==0)
		{
			break;
		}
		//进入测试模式 应答，无需做处理
		if (strRes.compare("ENTER")==0)
		{
			continue;
		}

		while(g_cmd_list && g_cmd_list[i_index].tag)
		{
			if(0 == strcmp(g_cmd_list[i_index].tag, strTestCase.c_str()))
			{
				g_cmd_list[i_index].cmd_handle(m_TestSocket,msg);
				//if((g_cmd_list[i_index].set_param_func) && web_page_table[i_index].set_param_func())
				//{
				//	printf("set_param_func failed:%s", cgl_Env->remote_addr);
				//	goto FAILED;
				//}
				//break;
			}
			i_index++;
		}

	}

	//m_pRecvThread = NULL;
	//socket_close(m_TestSocket);
	return TRUE;
}


void CIPSearchDlg::OnBnClickedButtonNext()
{
	// TODO: Add your control notification handler code here
	int nCount = m_listDevice.GetItemCount();
	if (m_listSelect!=-1)
	{
		m_listSelect++;
	}

	if (m_listSelect>0&&m_listSelect<nCount)
	{
		//m_bRun = false;
		m_strIp = m_listDevice.GetItemText(m_listSelect, 1);
		if (OnStartTest())
		{
			this->SetDlgItemText(IDC_BUTTON_TEST,GetLocalString(_T("IDS_TEXT_STOP_BUTTON")).c_str());
			GetDlgItem(ID_BTN_APPLY)->EnableWindow(FALSE);
		}
	}
	else
	{
		AfxMessageBox(_T("已经是最后一台！"));
	}

}

void CIPSearchDlg::SaveTestResult()
{
	int i,nRet;
	int nResult = 0;
	CString strPromt;
	CString strTmp;
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].TestName.compare(m_Configs.strSdcardName)==0&&m_TestCaseList[i].nTestStatus==2)
		{
			nResult|=ITEM_SDCARD_MASK;
		}
		else if (m_TestCaseList[i].TestName.compare(m_Configs.strWifiName)==0&&m_TestCaseList[i].nTestStatus==2)
		{
			nResult|=ITEM_WIFI_MASK;
		}
		else if (m_TestCaseList[i].TestName.compare(m_Configs.strKeyName)==0&&m_TestCaseList[i].nTestStatus==2)
		{
			nResult|=ITEM_KEY_MASK;
		}
		else if (m_TestCaseList[i].TestName.compare(m_Configs.strInterphoneName)==0&&m_TestCaseList[i].nTestStatus==2)
		{
			nResult|=ITEM_INTERPHONE_MASK;
		}
		else if (m_TestCaseList[i].TestName.compare(m_Configs.strIrcutName)==0&&m_TestCaseList[i].nTestStatus==2)
		{
			nResult|=ITEM_IRCUT_MASK;
		}
		else if (m_TestCaseList[i].TestName.compare(m_Configs.strPtzName)==0&&m_TestCaseList[i].nTestStatus==2)
		{
			nResult|=ITEM_PTZ_MASK;
		}
		else if (m_TestCaseList[i].TestName.compare(m_Configs.strLedName)==0&&m_TestCaseList[i].nTestStatus==2)
		{
			nResult|=ITEM_LED_MASK;
		}
	}
	nResult|= 0x8000;
	strTmp.Format(_T("-s TESTRESULT -v %d"),nResult);
	nRet = m_DevTest.CommitResult(m_TestSocket,wstr2str(m_Configs.strWriteName),wstr2str((LPCTSTR)strTmp));
	if (nRet < 0)
	{
		strPromt.Format(GetLocalString(_T("IDS_INFO_SAVE_RESULT_FAIL")).c_str(),nRet);
	}
}
void CIPSearchDlg::WritePara()
{
	CUidDlg uiddlg(m_Configs,m_LocalLan);
	CString strPrompt;
	CString strTmp;
	int nRet;
	if (m_Configs.bWriteUid||m_Configs.bWriteMac)
	{
		if (IDOK == uiddlg.DoModal())
		{
			if (m_Configs.bWriteUid)
			{
				strPrompt.Format(_T("烧写UID.."));
				AddPrompt(strPrompt);
				strTmp.Format(_T("-s UID -v %s"),uiddlg.strUid);
				nRet = m_DevTest.WriteUidAndLanMac(m_TestSocket,wstr2str(m_Configs.strWriteName),wstr2str((LPCTSTR)strTmp));
				if (nRet<0)
				{
					strPrompt.Format(_T("烧写UID失败,错误码(%d)"),nRet);
					AddPrompt(strPrompt,TRUE);
				}
				else
				{
					strPrompt.Format(_T("烧写UID成功"));
					AddPrompt(strPrompt);
				}
			}
			if (m_Configs.bWriteMac)
			{
				strPrompt.Format(_T("烧写LanMac.."));
				AddPrompt(strPrompt);
				strTmp.Format(_T("-s LANMAC -v %s"),uiddlg.strLanmac);
				nRet = m_DevTest.WriteUidAndLanMac(m_TestSocket,wstr2str(m_Configs.strWriteName),wstr2str((LPCTSTR)strTmp));
				if (nRet<0)
				{
					strPrompt.Format(_T("烧写LanMac失败,错误码(%d)"),nRet);
					AddPrompt(strPrompt);
				}
				else
				{
					strPrompt.Format(_T("烧写LanMac成功"));
					AddPrompt(strPrompt);
				}
			}
		}
	}
}

void CIPSearchDlg::OnBnClickedButtonPass()
{
	// TODO: Add your control notification handler code here
	TestInfo param;
	CWinThread *pA = NULL;
	param.nResult = 1;
	param.pDlg = this;
	pA = AfxBeginThread(NextTestThread,&param);
	pA=NULL;
}

void CIPSearchDlg::OnBnClickedButtonFail()
{
	// TODO: Add your control notification handler code here
	TestInfo param;
	CWinThread *pA = NULL;
	param.nResult = 0;
	param.pDlg = this;
	pA = AfxBeginThread(NextTestThread,&param);
	pA=NULL;
}

void CIPSearchDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_bRun)
	{
		MessageBox(GetLocalString(_T("IDS_ERROR_STILL_RUNNING")).c_str(),GetLocalString(_T("IDS_ERROR_CAPTION")).c_str(),MB_OK|MB_ICONERROR);
		return;
	}
	if(m_pLog) {
		delete m_pLog;
		m_pLog = NULL;
	}
	CDialog::OnOK();
}

BOOL CIPSearchDlg::LoadConfig()
{
	CString strConfigPath;
	TCHAR MyexeFullPath[MAX_PATH] = TEXT("C:\\");
	if(GetModuleFileName(NULL,MyexeFullPath,MAX_PATH)) {
		TCHAR *pos = _tcsrchr(MyexeFullPath, TEXT('\\'));
		if(pos) {
			pos[1] = 0;
			m_strModulePath = MyexeFullPath;
		}
	}
	strConfigPath = m_strModulePath + _T("config.ini");

	bool bLoadConfig = m_Configs.LoadToolSetting(strConfigPath.GetString());
	if (!bLoadConfig)
	{
		return FALSE;
	}
	strConfigPath = m_strModulePath + m_Configs.strLanPath.c_str();
	if (m_Configs.nCurLan == 1)
	{
		strConfigPath = strConfigPath + m_Configs.strCnFilename.c_str();
	}
	else
	{
		strConfigPath = strConfigPath + m_Configs.strEnFilename.c_str();
	}

	bLoadConfig = m_LocalLan.LoadToolSetting(strConfigPath.GetString());
	m_LocalLan.TreeControls(m_hWnd,m_Configs.bDebug?TRUE:FALSE,this->IDD,true);
	if (!bLoadConfig)
	{
		return FALSE;
	}
	if(!m_Configs.bDebug) {
		WalkMenu(GetMenu(),TEXT("MENU"));
	}
	GetLocalString(TEXT("LANG:IDS_TEXT_APPNAME"));
	return TRUE;
}

VOID CIPSearchDlg::WalkMenu(CMenu *pMenu,CString strMainKeyPart)
{
	CString strKey;
	int     id ;
	UINT    i;
	if(NULL == pMenu) return ;
	for (i=0;i < pMenu->GetMenuItemCount();i++) {
		strKey.Format(_T("%s_%d"),strMainKeyPart,i);
		id = pMenu->GetMenuItemID(i);
		if (0 == id) { 
			/*If nPos corresponds to a SEPARATOR menu item, the return value is 0. **/
		} else if (-1 == id) { 
			/*If the specified item is a pop-up menu (as opposed to an item within the pop-up menu), the return value is –1 **/
			pMenu->ModifyMenu(i, MF_BYPOSITION, i, m_LocalLan.GetStr((LPCTSTR)strKey).c_str());
			WalkMenu(pMenu->GetSubMenu(i),strKey);
		} else {
			pMenu->ModifyMenu(id, MF_BYCOMMAND, id,m_LocalLan.GetStr((LPCTSTR)strKey).c_str());
		}
	}
}

void CIPSearchDlg::ExitTest()
{
	int i,ret;
	CString strPrompt;
	for (i=0;i<m_TestCaseList.size();i++)
	{

		if (m_TestCaseList[i].nTestStatus==1)
		{
			ret = m_DevTest.StopTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			m_TestCaseList[i].nTestStatus = -1;
			if (ret<0)
			{
				strPrompt.Format(GetLocalString(_T("IDS_STOP_TEST_FAIL")).c_str(),GetLocalString(m_TestCaseList[i].TestName).c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				m_TestCaseList[i].nTestStatus = -1;
			}
		}
	}
	ret = m_DevTest.ExitTest(m_TestSocket);
	//if (ret<0)
	//{
	//	strPrompt.Format(GetLocalString(_T("IDS_INFO_EXIT_TEST_FAIL")).c_str(),ret);
	//	AddPrompt(strPrompt,TRUE);
	//	return ;
	//}
}
BOOL CIPSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message==WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_F2:
			//调用按钮对应的事件函数，就同点击按钮相同
			if(GetDlgItem(ID_BTN_APPLY)->IsWindowEnabled())  
				OnBnClickedBtnApply();
			break;
		case VK_F3:
			//调用按钮对应的事件函数，就同点击按钮相同
			if(GetDlgItem(IDC_BUTTON_NEXT)->IsWindowEnabled())  
				OnBnClickedButtonNext();
			break;
		case VK_F4:
			//调用按钮对应的事件函数，就同点击按钮相同
			if(GetDlgItem(IDC_BUTTON_PASS)->IsWindowEnabled())  
				OnBnClickedButtonPass();
			break;
		case VK_F5:
			//调用按钮对应的事件函数，就同点击按钮相同
			if(GetDlgItem(IDC_BUTTON_FAIL)->IsWindowEnabled())  
				OnBnClickedButtonFail();
			break;
		//case VK_F6:
		//	//调用按钮对应的事件函数，就同点击按钮相同
		//	if(GetDlgItem(ID_BTN_APPLY)->IsWindowEnabled())  
		//		OnBnClickedBtnApply();
		//	break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CIPSearchDlg::OnHelpAbout()
{
	// TODO: Add your command handler code here
	CAboutDlg dlgAbout(m_Configs,m_LocalLan);
	dlgAbout.DoModal();
}
int CIPSearchDlg::DoTestItem(std::wstring strTestName,std::string &strInfo)
{
	int ret;
	//std::string strOutput;
	if (strTestName.compare(m_Configs.strKeyName)==0)
	{
		GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(TRUE);
		ret=m_DevTest.KeyTest(m_TestSocket,wstr2str(strTestName),strInfo);
	}
	else if (strTestName.compare(m_Configs.strSdcardName)==0)
	{
		ret = m_DevTest.SdCardTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strWifiName)==0)
	{
		ret = m_DevTest.WifiTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strIrcutName)==0)
	{
		ret = m_DevTest.IrcutTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strInterphoneName)==0)
	{
		ret = m_DevTest.InterphoneTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strMonitorName)==0)
	{
		ret = m_DevTest.MonitorTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strPtzName)==0)
	{
		ret = m_DevTest.PtzTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strLedName)==0)
	{
		ret = m_DevTest.LedTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strHdmiName)==0)
	{
		ret = m_DevTest.HdmiTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strBtTest)==0)
	{
		ret = m_DevTest.BtTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strEmmcTest)==0)
	{
		ret = m_DevTest.EmmcTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strDdrTest)==0)
	{
		ret = m_DevTest.DdrTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strRtcTest)==0)
	{
		ret = m_DevTest.RtcTest(m_TestSocket,wstr2str(strTestName));
	}
	else if (strTestName.compare(m_Configs.strRotaryTest)==0)
	{
		GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(TRUE);
		ret=m_DevTest.KeyTest(m_TestSocket,wstr2str(strTestName),strInfo);
	}

	return ret;
}
void CIPSearchDlg::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	int ret,i;
	bool bTest=false;
	CString strPrompt;
	
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus==1)
		{
			bTest = true;
		}
	}
	if (bTest)
	{
		strPrompt.Format(GetLocalString(_T("IDS_ERROR_EIXT_TEST")).c_str());
		MessageBox(strPrompt,GetLocalString(_T("IDS_ERROR_CAPTION")).c_str(),MB_OK|MB_ICONERROR);
		return;
	}

	strPrompt.Format(GetLocalString(_T("IDS_INFO_EXIT_TEST")).c_str());
	AddPrompt(strPrompt);
	ret = connect_dev();
	if (ret<0)
	{
		strPrompt.Format(GetLocalString(_T("IDS_ERROR_CONN_FAIL")).c_str(),ret);
		AddPrompt(strPrompt,TRUE);
		return ;
	}
	ret = m_DevTest.StopTestItem(m_TestSocket,wstr2str(m_Configs.strCameraName));
	ret = m_DevTest.ExitTest(m_TestSocket);
	if (ret<0)
	{
		strPrompt.Format(GetLocalString(_T("IDS_INFO_EXIT_TEST_FAIL")).c_str(),ret);
		AddPrompt(strPrompt,TRUE);
		return ;
	}
	strPrompt.Format(GetLocalString(_T("IDS_INFO_EXIT_TEST_SUCCESS")).c_str());
	AddPrompt(strPrompt);
}
LRESULT CIPSearchDlg::OnHandleUpdateTestinfoMsg(WPARAM wParam,LPARAM lParam)
{
	for (int i = 0; i<m_TestCaseList.size(); i++)
	{
		if (m_TestCaseList[i].nTestStatus==0)
		{
			m_listTestItem.SetItemText(i,2,_T(""));
			m_listTestItem.SetRowColor(i,RGB(255,255,255));
		}
		else if (m_TestCaseList[i].nTestStatus==1)
		{
			m_listTestItem.SetItemText(i,2,GetLocalString(_T("IDS_TESTING")).c_str());
			m_listTestItem.SetRowColor(i,RGB(255,255,0));
		}
		else if (m_TestCaseList[i].nTestStatus==2)
		{
			m_listTestItem.SetItemText(i,2,GetLocalString(_T("IDS_PASS")).c_str());
			m_listTestItem.SetRowColor(i,RGB(0,255,0));
		}
		else if (m_TestCaseList[i].nTestStatus==-1)
		{
			m_listTestItem.SetItemText(i,2,GetLocalString(_T("IDS_FAIL")).c_str());
			m_listTestItem.SetRowColor(i,RGB(255,0,0));
		}
	}
	return 0;
}

BOOL CIPSearchDlg::NextTestProc(LPVOID lpParameter)
{
	int ret;
	int i;
	int nCount=0;
	CString strPrompt;
	bool bTest=false;
	std::string strOutput;
	int *nResult = (int*)lpParameter;
	int nTest = *nResult;

	nCount = m_TestCaseList.size();
	//1.获取当前测试项，保存测试结果，设置测试状态为已测试
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 1&&!m_TestCaseList[i].bAuto)
		{
			ret = m_DevTest.StopTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			if (ret<0)
			{
				strPrompt.Format(GetLocalString(_T("IDS_STOP_TEST_FAIL")).c_str(),GetLocalString(m_TestCaseList[i].TestName).c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				m_TestCaseList[i].nTestStatus = -1;
				m_bTestPass = false;
			}
			if (nTest)
			{
				strPrompt.Format(_T("%s:%s"),GetLocalString(m_TestCaseList[i].TestName).c_str(),GetLocalString(_T("IDS_PASS")).c_str());
				AddPrompt(strPrompt,FALSE,LIST_PASS);
				m_TestCaseList[i].nTestStatus = 2;
			}
			else
			{
				strPrompt.Format(_T("%s:%s"),GetLocalString(m_TestCaseList[i].TestName).c_str(),GetLocalString(_T("IDS_FAIL")).c_str());
				AddPrompt(strPrompt,TRUE);
				m_TestCaseList[i].nTestStatus = -1;
			}
			PostMessage(WM_UPDATE_TESTINFO_MSG,0,0);
		}
		if (m_TestCaseList[i].nTestStatus == 0)
		{
			strPrompt.Format(_T("%s:%s"),GetLocalString(m_TestCaseList[i].TestName).c_str(),GetLocalString(_T("IDS_TESTING")).c_str());
			AddPrompt(strPrompt,FALSE,LIST_WARN);
			m_TestCaseList[i].nTestStatus = 1;
			PostMessage(WM_UPDATE_TESTINFO_MSG,0,0);
			ret = DoTestItem(m_TestCaseList[i].TestName,strOutput);
			if (ret<0)
			{
				strPrompt.Format(GetLocalString(_T("FAILED")).c_str(),GetLocalString(m_TestCaseList[i].TestName).c_str(),ret);
				AddPrompt(strPrompt,True);
				break;
			}
			else if (ret == 2)
			{
				if (m_TestCaseList[i].TestName.compare(m_Configs.strKeyName)==0)
				{
					strPrompt.Format(GetLocalString(_T("IDS_KEY_DOWN")).c_str());
					AddPrompt(strPrompt,FALSE,LIST_WARN);
					m_listTestItem.SetItemText(i,2,strPrompt);
				}
				else
				{
					strPrompt.Format(GetLocalString(_T("IDS_ROTARY")).c_str());
					AddPrompt(strPrompt,FALSE,LIST_WARN);
					m_listTestItem.SetItemText(i,2,strPrompt);
				}
			}
			break;
		}
	}

	if (m_TestCaseList[nCount-1].nTestStatus!=0&&m_TestCaseList[nCount-1].nTestStatus!=1)
	{
		//保存测试结果

		while(m_pTestThread!=NULL)
		{
			Sleep(100);
		}
		SaveTestResult();
		//strPrompt.Format(GetLocalString(_T("IDS_INFO_TEST_OVER")).c_str());
		//AddPrompt(strPrompt);
		for (i=0;i<m_TestCaseList.size();i++)
		{
			if (m_TestCaseList[i].nTestStatus==-1)
			{
				break;
			}
		}
		if (i>=m_TestCaseList.size())
		{
			//写号
			WritePara();
		}
		ExitTest();
		GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);
	}
	return TRUE;
}