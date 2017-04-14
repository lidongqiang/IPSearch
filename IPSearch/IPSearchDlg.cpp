
// IPSearchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IPSearch.h"
#include "IPSearchDlg.h"
#include "afxtempl.h"
#include "Winnetwk.h"
#include "c_socket.h"
#include "cmd_process.h"
#include <iostream>
#include <WinSock2.h>  
#include <Windows.h>  
#include <json/json.h>
#include <json/reader.h>
#pragma comment(lib, "WS2_32.lib")  

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

UINT TestDeviceThread(LPVOID lpParam)
{
	CIPSearchDlg* pMainDlg = (CIPSearchDlg*)lpParam;
	pMainDlg->TestProc();
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
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CIPSearchDlg 对话框




CIPSearchDlg::CIPSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPSearchDlg::IDD, pParent),m_listSelect(-1),m_bRun(false),m_bTestPass(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIPSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_listDevice);
	DDX_Control(pDX, IDC_LIST_INFO, m_listInfo);
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
	ON_BN_CLICKED(IDC_BTN_PLAYER, &CIPSearchDlg::OnBnClickedBtnPlayer)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CIPSearchDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PASS, &CIPSearchDlg::OnBnClickedButtonPass)
	ON_BN_CLICKED(IDC_BUTTON_FAIL, &CIPSearchDlg::OnBnClickedButtonFail)
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
	CRect rect;   

	// 获取编程语言列表视图控件的位置和大小   
	m_listDevice.GetClientRect(&rect);   

	// 为列表视图控件添加全行选中和栅格风格   
	m_listDevice.SetExtendedStyle(m_listDevice.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
	m_listDevice.InsertColumn(0,_T("UID"),LVCFMT_CENTER, rect.Width()/4, 0);
	m_listDevice.InsertColumn(1,_T("IP"),LVCFMT_CENTER, rect.Width()/2, 1);
	m_listDevice.InsertColumn(2,_T("设备名称"),LVCFMT_CENTER, rect.Width()/4, 2);
	m_listDevice.DeleteAllItems();
	
	CFont font;
	font.CreateFont(-13,10,0,0,FW_NORMAL,FALSE,FALSE,0,  
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,  
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,_T("宋体"));
	m_listInfo.SetFont(&font);
	font.Detach();
	m_listInfo.SetWindowBKColor(RGB(0,0,0));

	//加载config.ini
	TCHAR MyexeFullPath[MAX_PATH] = TEXT("C:\\");
	if(GetModuleFileName(NULL,MyexeFullPath,MAX_PATH)) {
		TCHAR *pos = _tcsrchr(MyexeFullPath, TEXT('\\'));
		if(pos) {
			pos[1] = 0;
			m_strModulePath = MyexeFullPath;
		}
	}
	m_Configs.LoadToolSetting((LPCTSTR)(m_strModulePath+ TEXT("config.ini")));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
				m_listInfo.AddLine(CXListBox::Black,CXListBox::Yellow,pLine->pszLineText);
			} else {
				m_listInfo.AddLine(CXListBox::White,CXListBox::Red,pLine->pszLineText);
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
	//LDEGMSGW((CLogger::DEBUG_INFO, TEXT("Info:%s"),(LPCTSTR)strPrompt));
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
		CAboutDlg dlgAbout;
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

	//CString strUid,strAddr,strDevname;
	std::string strUid,strAddr,strDevname;
	m_listDevice.DeleteAllItems();

	WORD wVersionRequested = MAKEWORD(2, 2);  
	WSADATA wsaData;  
	if(0 != WSAStartup(wVersionRequested, &wsaData))  
	{  
		//printf("WSAStartup failed with error: %d/n", GetLastError());
		AfxMessageBox(_T("WSAStartup failed with error: %d/n"), GetLastError());
		return ;  
	}  
	if(2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))  
	{  
		//printf("Socket version not supported./n");
		AfxMessageBox(_T("Socket version not supported./n"));
		WSACleanup();  
		return ;  
	}  
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  
	if(INVALID_SOCKET == sock)  
	{  
		//printf("socket failed with error: %d/n", WSAGetLastError());
		AfxMessageBox(_T("socket failed with error: %d/n"), GetLastError());
		WSACleanup();  
		return ;  
	}  
	SOCKADDR_IN addr;  
	memset(&addr, 0, sizeof(addr));  
	addr.sin_family = AF_INET;  
	addr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);  
	addr.sin_port = htons(18889);  
	BOOL bBoardcast = TRUE; 
	int len = sizeof(addr);
	if(SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bBoardcast, sizeof(bBoardcast)))  
	{  
		printf("setsockopt failed with error code: %d/n", WSAGetLastError());  
		if(INVALID_SOCKET != sock)  
		{  
			closesocket(sock);  
			sock = INVALID_SOCKET;  
		}  
		WSACleanup();  
	}
	//int iMode = 1; //0：阻塞
	//ioctlsocket(sock,FIONBIO, (u_long FAR*) &iMode);//非阻塞设置
	int nNetTimeout=3000;//1秒，
	//设置发送超时
	setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int));
	//设置接收超时
	setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
	char szBuf[MAX_BUFFER] = {0};
	char buf[] = {"CMD_DISCOVER"};
	int n = 10;
	int nIndex = 0;
	if(SOCKET_ERROR == sendto(sock, buf, sizeof(buf), 0, (LPSOCKADDR)&addr, sizeof(addr)))  
	{  
		//printf("sendto failed with error: %d/n", WSAGetLastError());  
		AfxMessageBox(_T("sendto failed with error: %d\n"));
	}
	while(1)  
	{  
		memset(szBuf,0,255);
		if (SOCKET_ERROR == recvfrom(sock, szBuf, 255, 0, (LPSOCKADDR)&addr, &len))
		{
			break;
		}
		if (strlen(szBuf) != 0)
		{
			//{"UID" : "123423434424","IP" : "172.16.14.85","DEVNAME" : "IPC"}
			//ParseDevInfo("{\"UID\" : \"123423434424\",\"IP\" : \"172.16.14.85\",\"DEVNAME\" : \"IPC\"}",strUid,strAddr,strDevname);
			//ParseDevInfo(szBuf,strUid,strAddr,strDevname);
			m_Json.JsontoItem("UID",strUid,"IP",strAddr,"DEVICENAME",strDevname,szBuf);
			//m_listDevice.InsertItem(nIndex,_T(""));
			//m_listDevice.SetItemText(nIndex,0,strUid);
			//m_listDevice.SetItemText(nIndex,1,strAddr);
			//m_listDevice.SetItemText(nIndex,2,strDevname);
			m_listDevice.InsertItem(nIndex,_T(""));
			m_listDevice.SetItemText(nIndex,0,str2wstr(strUid).c_str());
			m_listDevice.SetItemText(nIndex,1,str2wstr(strAddr).c_str());
			m_listDevice.SetItemText(nIndex,2,str2wstr(strDevname).c_str());
			nIndex++;
		}
		Sleep(100);  
	}  
	WSACleanup();
	if (nIndex==0)
	{
		AfxMessageBox(_T("没有搜索的任何设备"));
	}
	else
		AfxMessageBox(_T("搜索完成"));
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
		//m_pTestThread = AfxBeginThread(TestDeviceThread,this);
		//if(NULL == m_pTestThread) {
		//	goto OnStartTestExit; /*generrally never to here **/
		//}
		////创建接收设备端的消息的线程，专门用于接收设备端消息
		//m_pRecvThread = AfxBeginThread(RecvDeviceThread,this);
		//if(NULL == m_pRecvThread) {
		//	goto OnStartTestExit; /*generrally never to here **/
		//}
		TestProc();
	}
	else
	{
		AfxMessageBox(_T("请选择一个ip地址"));
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
		this->SetDlgItemText(IDC_BUTTON_TEST,_T("停止中"));
		m_bRun = false;
		closesocket(m_TestSocket);
		if(m_pRecvThread!=NULL)
		{
			WaitForSingleObject(m_pRecvThread->m_hThread,INFINITE);
			m_pRecvThread = NULL;
		}
		this->SetDlgItemText(IDC_BUTTON_TEST,_T("开始测试"));
		AddPrompt(_T("用户取消"),TRUE);
	}
	else
	{
		if (OnStartTest())
		{
			this->SetDlgItemText(IDC_BUTTON_TEST,_T("停止"));
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
		m_strIp = m_listDevice.GetItemText(pNMListView->iItem, 1);
		//RtspPlay();
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
	libvlc_media_t *m_vlcMedia=NULL;
	libvlc_media_player_t *m_vlcMplay=NULL;
	libvlc_instance_t *m_vlcInst=NULL;
	const char * const vlc_args[] = {  
		"--demux=h264",  
		"--ipv4",  
		"--no-prefer-system-codecs",  
		"--rtsp-caching=300",  
		"--network-caching=500",  
		"--rtsp-tcp",  
	};
	CString strUrl;
	strUrl.Format(_T("rtsp://%s/webcam"),m_strIp);

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
		m_vlcMedia = libvlc_media_new_location(m_vlcInst,wstr2str((LPCTSTR)strUrl).c_str());

	}  
	if (m_vlcMplay == NULL)  
	{  
		m_vlcMplay = libvlc_media_player_new_from_media(m_vlcMedia);  
	}  

	HWND hWndVedio = GetDlgItem(IDC_LIST_INFO)->GetSafeHwnd();
	//HWND hWndVedio = GetDlgItem(IDC_LIST_VIDEO)->GetSafeHwnd();
	libvlc_media_player_set_hwnd(m_vlcMplay, hWndVedio);  
	libvlc_media_player_play(m_vlcMplay);
}
void CIPSearchDlg::OnBnClickedBtnApply()
{
	// TODO: Add your control notification handler code here
	//CString strUid,strIP,strDevname;

	//this->GetDlgItemText(IDC_EDIT_UID,strUid);
	//this->GetDlgItemText(IDC_EDIT_IP,strIP);
	//this->GetDlgItemText(IDC_EDIT_DEVNAME,strDevname);

	//m_listDevice.SetItemText(m_listSelect,0,strUid);
	//m_listDevice.SetItemText(m_listSelect,1,strIP);
	//m_listDevice.SetItemText(m_listSelect,2,strDevname);
	CConfigDlg ConfgDlg(m_Configs);
	if (IDOK == ConfgDlg.DoModal())
	{
		m_Configs.SaveToolSetting(std::wstring(TEXT("")));
		//AfxMessageBox(_T("保存配置成功！"));
	}

}
BOOL CIPSearchDlg::TestProc()
{
	//1.开始测试  向设备端发送命令命令{"TYPE":"CMD", "CMD":"ENTER" }
	int ret;
	int i;
	int nSdCardTest=-1;
	int nWifiTest=-1;
	std::string strMsg,strSta,strOutput;
	//int nErrCode;
	CString strPrompt;
	CPcbaTest DevTest;

	m_bRun = TRUE;
	if (m_listInfo.GetCount()>0) {
		PostMessage(WM_UPDATE_MSG,UPDATE_LIST,LIST_EMPTY);
	}
	//1.连接设备
	strPrompt.Format(_T("连接设备(%s)..."),m_strIp);
	AddPrompt(strPrompt);
	ret = connect_dev();
	if (ret<0)
	{
		strPrompt.Format(_T("连接设备失败，错误码:%d"),ret);
		AddPrompt(strPrompt,TRUE);
		//AfxMessageBox(_T("连接设备失败，错误码:%d"));
		return FALSE;
	}
	strPrompt.Format(_T("连接设备成功"));
	AddPrompt(strPrompt);

	//strPrompt.Format(_T("进入测试模式..."));
	//AddPrompt(strPrompt);
	//m_Json.ItemtoJson("TYPE","CMD","CMD","ENTER",strMsg);
	//ret = socket_write(m_TestSocket,strMsg);

	//if (ret < 0)
	//{
	//	strPrompt.Format(_T("socket:send data failed"));
	//	AddPrompt(strPrompt,TRUE);
	//	//AfxMessageBox(_T("socket:send data failed"));
	//	return FALSE;
	//}

	////2.读取设备端返回的结果，确定成功与否，成功放回{"TYPE":"RES", "STATUS":"PASS"}
	//std::string strTest;
	//socket_read(m_TestSocket,strMsg);
	////AfxMessageBox(str2wstr(strMsg).c_str());
	//if (ret < 0)
	//{
	//	AfxMessageBox(_T("socket:recv data failed"));
	//	return FALSE;
	//}
	//m_Json.JsontoItem("STATUS",strSta,strMsg);
	//m_Json.JsontoInt("ERR_CODE",nErrCode,strMsg);
	//if (strSta.compare("NAK")==0)
	//{
	//	strPrompt.Format(_T("进入测试模式失败，错误码:%d"),nErrCode);
	//	AddPrompt(strPrompt,TRUE);
	//	return FALSE;
	//}
	//strPrompt.Format(_T("进入测试模式成功"));
	//AddPrompt(strPrompt);

	//打开rtsp流
	//RtspPlay();

	//3.创建接收设备端的消息的线程，专门用于接收设备端消息
	//m_pRecvThread = AfxBeginThread(RecvDeviceThread,this);


	//4.下载测试程序，发送开始测试命令，{"TYPE":"CMD", "TEST_ITEM":"KEY-TEST", "CMD":"START" }
	//SD卡测试
	if (m_Configs.bSdcardTest)
	{
		strPrompt.Format(_T("开始SD卡测试..."));
		AddPrompt(strPrompt);
		ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_Configs.strSdcardName));
		if (ret<0)
		{
			strPrompt.Format(_T("SD卡测试失败，错误码(%d)"),ret);
			AddPrompt(strPrompt,TRUE);
		}
		else
		{
			nSdCardTest=0;
		}
	}
	if (m_Configs.bWifiTest)
	{
		strPrompt.Format(_T("开始Wifi测试..."));
		AddPrompt(strPrompt);
		ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_Configs.strWifiName));
		if (ret<0)
		{
			strPrompt.Format(_T("Wifi测试失败，错误码(%d)"),ret);
			AddPrompt(strPrompt,TRUE);
		}
		else
		{
			nWifiTest = 0;
		}
	}

	initTestCase();
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 0)
		{
			strPrompt.Format(_T("%s开始测试..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			m_TestCaseList[i].nTestStatus = 1;
			ret = DevTest.StartTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			if (ret<0)
			{
				strPrompt.Format(_T("%s开始测试失败,错误码(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
				AddPrompt(strPrompt,TRUE);
			}
			if (m_TestCaseList[i].TestName.compare(m_Configs.strKeyName)==0)
			{
				while(m_TestCaseList[i].nTestStatus==1)
				{
					ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_Configs.strKeyName),strOutput);
					if (ret < 0)
					{
						//m_TestCaseList[i].nTestStatus = -1;
						strPrompt.Format(_T("%s测试失败,错误码(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
						AddPrompt(strPrompt,TRUE);
						break;
					}
					else if (ret == 2)
					{
						strPrompt.Format(_T("%s测试:%s按键按下"),m_TestCaseList[i].TestName.c_str(),str2wstr(strOutput).c_str());
						AddPrompt(strPrompt,TRUE);
						break;
					}
					Sleep(1000);
				}
			}
			break;
		}
	}
	while ((nSdCardTest==0)||(nWifiTest==0))
	{
		if (nSdCardTest==0)		//测试中
		{
			ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_Configs.strSdcardName),strOutput);
			if (ret==1)
			{
				ret = m_DevTest.CommitResult(m_TestSocket,wstr2str(m_Configs.strSdcardName));
				if (ret<0)
				{
					strPrompt.Format(_T("%s测试失败,错误码(%d)"),m_Configs.strSdcardName.c_str(),ret);
					AddPrompt(strPrompt,TRUE);
				}
				else
				{
					strPrompt.Format(_T("%s测试成功"),m_Configs.strSdcardName.c_str());
					AddPrompt(strPrompt);
				}
				nSdCardTest=1;
			}else if (ret<0)
			{
				strPrompt.Format(_T("%s测试失败,错误码(%d)"),m_Configs.strSdcardName.c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				nSdCardTest=1;
			}
		}

		if (nWifiTest==0)		//测试中
		{
			ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_Configs.strWifiName),strOutput);
			if (ret==1)
			{
				ret = m_DevTest.CommitResult(m_TestSocket,wstr2str(m_Configs.strWifiName));
				if (ret<0)
				{
					strPrompt.Format(_T("%s测试失败,错误码(%d)"),m_Configs.strWifiName.c_str(),ret);
					AddPrompt(strPrompt,TRUE);
				}
				else
				{
					strPrompt.Format(_T("%s测试成功"),m_Configs.strWifiName.c_str());
					AddPrompt(strPrompt);
				}
				nWifiTest=1;
			}else if (ret<0)
			{
				strPrompt.Format(_T("%s测试失败,错误码(%d)"),m_Configs.strWifiName.c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				nWifiTest=1;
			}
		}
		Sleep(1000);
	}

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
	Timeout(m_TestSocket,3000);

	SOCKADDR_IN addrSrv; 
	addrSrv.sin_addr.S_un.S_addr=inet_addr(wstr2str((LPCTSTR)m_strIp).c_str()); 
	//addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
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
	if (m_Configs.bIrcutTest)
	{
		TestCase.TestName = m_Configs.strIrcutName;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bMonitorTest)
	{
		TestCase.TestName = m_Configs.strMonitorName;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bInterphoneTest)
	{
		TestCase.TestName = m_Configs.strInterphoneName;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bPtzTest)
	{
		TestCase.TestName = m_Configs.strPtzName;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
	}
	if (m_Configs.bKeyTest)
	{
		TestCase.TestName = m_Configs.strKeyName;
		TestCase.nTestStatus = 0;
		m_TestCaseList.push_back(TestCase);
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

void CIPSearchDlg::OnBnClickedBtnPlayer()
{
	// TODO: Add your control notification handler code here
	//CPlayerDlg playDlg;
	//playDlg,DoModal();
	CString strUrl;
	if (m_strIp.IsEmpty())
	{
		strUrl = _T("");
	}
	else
	{
		strUrl.Format(_T("rtsp://%s/webcam"),m_strIp);
	}
	CVideoDlg	m_VideoDlg(strUrl);
	m_VideoDlg.DoModal();
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
		m_bRun = false;
		if(m_pRecvThread!=NULL)
		{
			WaitForSingleObject(m_pRecvThread->m_hThread,INFINITE);
			closesocket(m_TestSocket);
			m_pRecvThread = NULL;
		}
		m_strIp = m_listDevice.GetItemText(m_listSelect, 1);
		TestProc();
	}
	else
	{
		AfxMessageBox(_T("已经是最后一台！"));
	}

}

void CIPSearchDlg::OnBnClickedButtonPass()
{
	// TODO: Add your control notification handler code here
	int ret;
	int i;
	CString strPrompt;
	std::string strOutput;
	//1.获取当前测试项，保存测试结果，设置测试状态为已测试
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 1)
		{
			strPrompt.Format(_T("%s保存测试结果..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			ret = m_DevTest.CommitResult(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			if (ret<0)
			{
				strPrompt.Format(_T("%s保存测试结果失败"),m_TestCaseList[i].TestName.c_str());
				AddPrompt(strPrompt,TRUE);
				m_TestCaseList[i].nTestStatus = -1;
				m_bTestPass = false;
				break;
			}

			strPrompt.Format(_T("%s保存测试结果成功"),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			m_TestCaseList[i].nTestStatus = 2;
			break;
		}
	}

	//2.获取下一个未测试的测试项
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 0)
		{
			strPrompt.Format(_T("%s开始测试..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			m_TestCaseList[i].nTestStatus = 1;
			if (ret<0)
			{
				strPrompt.Format(_T("%s开始测试失败,错误码(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
				AddPrompt(strPrompt,True);
			}
			break;
		}
	}
	if (m_TestCaseList[i].TestName.compare(m_Configs.strKeyName)==0)
	{
		while(m_TestCaseList[i].nTestStatus==1)
		{
			ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_Configs.strKeyName),strOutput);
			if (ret < 0)
			{
				strPrompt.Format(_T("%s测试失败,错误码(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				break;
			}
			else if (ret == 2)
			{
				strPrompt.Format(_T("%s测试:%s按键按下"),m_TestCaseList[i].TestName.c_str(),str2wstr(strOutput).c_str());
				AddPrompt(strPrompt,TRUE);
				break;
			}
			Sleep(1000);
		}
	}
	if (i==m_TestCaseList.size()&&m_bTestPass)
	{
		//所有测试项都测试完成,写号
		std::string strWriteMsg;
		m_DevTest.WriteUidAndLanMac(m_TestSocket,wstr2str(m_Configs.strWriteName),strWriteMsg);
	}
}

void CIPSearchDlg::OnBnClickedButtonFail()
{
	// TODO: Add your control notification handler code here
	int ret;
	int i,j;
	std::string strOutput;
	CString strPrompt;
	m_bTestPass = false;
	//获取下一个未测试的测试项
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 1)
		{
			m_TestCaseList[i].nTestStatus = -1;
		}
		if (m_TestCaseList[i].nTestStatus == 0)
		{
			strPrompt.Format(_T("%s开始测试..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			m_TestCaseList[i].nTestStatus = 1;
			ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			if (ret<0)
			{
				strPrompt.Format(_T("%s开始测试失败,错误码(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
				AddPrompt(strPrompt,True);
				break;
			}
			if (m_TestCaseList[i].TestName.compare(m_Configs.strKeyName)==0)
			{
				while(m_TestCaseList[i].nTestStatus==1)
				{
					ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_Configs.strKeyName),strOutput);
					if (ret < 0)
					{
						strPrompt.Format(_T("%s测试失败,错误码(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
						AddPrompt(strPrompt,TRUE);
						break;
					}
					else if (ret == 2)
					{
						strPrompt.Format(_T("%s测试:%s按键按下"),m_TestCaseList[i].TestName.c_str(),str2wstr(strOutput).c_str());
						AddPrompt(strPrompt,TRUE);
						break;
					}
					Sleep(1000);
				}
			}
			break;
		}
	}

	if (i==m_TestCaseList.size())
	{
		strPrompt.Format(_T("所有测试完成"));
		AddPrompt(strPrompt);
	}
}
