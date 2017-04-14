
// IPSearchDlg.cpp : ʵ���ļ�
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
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CIPSearchDlg �Ի���




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


// CIPSearchDlg ��Ϣ�������

BOOL CIPSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//m_PlayDlg.Create(IDD_PLAYER_DIALOG);
	m_pRecvThread = NULL;
	CRect rect;   

	// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
	m_listDevice.GetClientRect(&rect);   

	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
	m_listDevice.SetExtendedStyle(m_listDevice.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
	m_listDevice.InsertColumn(0,_T("UID"),LVCFMT_CENTER, rect.Width()/4, 0);
	m_listDevice.InsertColumn(1,_T("IP"),LVCFMT_CENTER, rect.Width()/2, 1);
	m_listDevice.InsertColumn(2,_T("�豸����"),LVCFMT_CENTER, rect.Width()/4, 2);
	m_listDevice.DeleteAllItems();
	
	CFont font;
	font.CreateFont(-13,10,0,0,FW_NORMAL,FALSE,FALSE,0,  
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,  
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,_T("����"));
	m_listInfo.SetFont(&font);
	font.Detach();
	m_listInfo.SetWindowBKColor(RGB(0,0,0));

	//����config.ini
	TCHAR MyexeFullPath[MAX_PATH] = TEXT("C:\\");
	if(GetModuleFileName(NULL,MyexeFullPath,MAX_PATH)) {
		TCHAR *pos = _tcsrchr(MyexeFullPath, TEXT('\\'));
		if(pos) {
			pos[1] = 0;
			m_strModulePath = MyexeFullPath;
		}
	}
	m_Configs.LoadToolSetting((LPCTSTR)(m_strModulePath+ TEXT("config.ini")));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CIPSearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	//int iMode = 1; //0������
	//ioctlsocket(sock,FIONBIO, (u_long FAR*) &iMode);//����������
	int nNetTimeout=3000;//1�룬
	//���÷��ͳ�ʱ
	setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int));
	//���ý��ճ�ʱ
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
		AfxMessageBox(_T("û���������κ��豸"));
	}
	else
		AfxMessageBox(_T("�������"));
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
	//�����������ü����ip�ͼ������
	CString strTemp;
	//�ṹ�������������Ϣ������������ip��ַ
	struct hostent *host;
	//�ṹ��in_add���ڱ���ip��ַ
	struct in_addr* ptr;
	//����ö����Դ��Χ
	DWORD dwScope=RESOURCE_CONTEXT;
	//NETRESOURCE�ṹ����ö�ٵ�������Դ�б���Ϣ
	NETRESOURCE *NetResource=NULL;
	HANDLE hEnum;
	//������������Դ����ö��
	WNetOpenEnum(dwScope,NULL,NULL,NULL,&hEnum);
	//wsadata�ṹ����winsock��Ϣ
	WSADATA wsaData;
	//��ʼ��winsock
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (hEnum)
	{
		DWORD Count=0xFFFFFFF;
		DWORD BufferSize=8192;
		LPVOID Buffer=new char[8192];
		//ö��������Դ
		WNetEnumResource(hEnum,&Count,Buffer,&BufferSize);
		//��ȡö�ٽ��
		NetResource=(NETRESOURCE*)Buffer;
		//char szHostName[200];
		//��ȡö����Ŀ
		int nmax=(BufferSize)/(sizeof(NETRESOURCE));
		//�����������
		for (int i=0;i<nmax;i++,NetResource++)
		{
			//����ǰ��Դ������������Դ����Ϊ������Դ�����������Դ
			if (NetResource->dwUsage==RESOURCEUSAGE_CONTAINER&&NetResource->dwType==RESOURCETYPE_ANY)
			{
				//��ȡ�����������Ϣ
				CString strFullName=NetResource->lpRemoteName;
				//��ȡ�������ƣ�������������󲿷�
				if (0==strFullName.Left(2).Compare(_T("\\\\")))
				{
					int nlen=strFullName.GetLength();
					strFullName=strFullName.Right(nlen-2);

					//��ȡ����ip��ַ
					host=gethostbyname(wstr2str(strFullName.GetString()).c_str());
					if (host==NULL)continue;
					//��ȡip��ַÿ����ֵ
					ptr=(struct in_addr*)host->h_addr_list[0];
					int x=ptr->S_un.S_un_b.s_b1;
					int b=ptr->S_un.S_un_b.s_b2;
					int c=ptr->S_un.S_un_b.s_b3;
					int d=ptr->S_un.S_un_b.s_b4;
					//��ʽ��ip��ַ�������
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
		////���������豸�˵���Ϣ���̣߳�ר�����ڽ����豸����Ϣ
		//m_pRecvThread = AfxBeginThread(RecvDeviceThread,this);
		//if(NULL == m_pRecvThread) {
		//	goto OnStartTestExit; /*generrally never to here **/
		//}
		TestProc();
	}
	else
	{
		AfxMessageBox(_T("��ѡ��һ��ip��ַ"));
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
		this->SetDlgItemText(IDC_BUTTON_TEST,_T("ֹͣ��"));
		m_bRun = false;
		closesocket(m_TestSocket);
		if(m_pRecvThread!=NULL)
		{
			WaitForSingleObject(m_pRecvThread->m_hThread,INFINITE);
			m_pRecvThread = NULL;
		}
		this->SetDlgItemText(IDC_BUTTON_TEST,_T("��ʼ����"));
		AddPrompt(_T("�û�ȡ��"),TRUE);
	}
	else
	{
		if (OnStartTest())
		{
			this->SetDlgItemText(IDC_BUTTON_TEST,_T("ֹͣ"));
		}
	}

}

void CIPSearchDlg::OnNMClickListDevice(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString strLangName;    // ѡ�����Ե������ַ���   
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR; 
	if (-1 != pNMListView->iItem)        // ���iItem����-1����˵�����б��ѡ��   
	{   
		m_listSelect = pNMListView->iItem;
		// ��ȡ��ѡ���б����һ��������ı�   
		strLangName = m_listDevice.GetItemText(pNMListView->iItem, 0);   
		// ��ѡ���������ʾ��༭����   
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
		//AfxMessageBox(_T("�������óɹ���"));
	}

}
BOOL CIPSearchDlg::TestProc()
{
	//1.��ʼ����  ���豸�˷�����������{"TYPE":"CMD", "CMD":"ENTER" }
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
	//1.�����豸
	strPrompt.Format(_T("�����豸(%s)..."),m_strIp);
	AddPrompt(strPrompt);
	ret = connect_dev();
	if (ret<0)
	{
		strPrompt.Format(_T("�����豸ʧ�ܣ�������:%d"),ret);
		AddPrompt(strPrompt,TRUE);
		//AfxMessageBox(_T("�����豸ʧ�ܣ�������:%d"));
		return FALSE;
	}
	strPrompt.Format(_T("�����豸�ɹ�"));
	AddPrompt(strPrompt);

	//strPrompt.Format(_T("�������ģʽ..."));
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

	////2.��ȡ�豸�˷��صĽ����ȷ���ɹ���񣬳ɹ��Ż�{"TYPE":"RES", "STATUS":"PASS"}
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
	//	strPrompt.Format(_T("�������ģʽʧ�ܣ�������:%d"),nErrCode);
	//	AddPrompt(strPrompt,TRUE);
	//	return FALSE;
	//}
	//strPrompt.Format(_T("�������ģʽ�ɹ�"));
	//AddPrompt(strPrompt);

	//��rtsp��
	//RtspPlay();

	//3.���������豸�˵���Ϣ���̣߳�ר�����ڽ����豸����Ϣ
	//m_pRecvThread = AfxBeginThread(RecvDeviceThread,this);


	//4.���ز��Գ��򣬷��Ϳ�ʼ�������{"TYPE":"CMD", "TEST_ITEM":"KEY-TEST", "CMD":"START" }
	//SD������
	if (m_Configs.bSdcardTest)
	{
		strPrompt.Format(_T("��ʼSD������..."));
		AddPrompt(strPrompt);
		ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_Configs.strSdcardName));
		if (ret<0)
		{
			strPrompt.Format(_T("SD������ʧ�ܣ�������(%d)"),ret);
			AddPrompt(strPrompt,TRUE);
		}
		else
		{
			nSdCardTest=0;
		}
	}
	if (m_Configs.bWifiTest)
	{
		strPrompt.Format(_T("��ʼWifi����..."));
		AddPrompt(strPrompt);
		ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_Configs.strWifiName));
		if (ret<0)
		{
			strPrompt.Format(_T("Wifi����ʧ�ܣ�������(%d)"),ret);
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
			strPrompt.Format(_T("%s��ʼ����..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			m_TestCaseList[i].nTestStatus = 1;
			ret = DevTest.StartTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			if (ret<0)
			{
				strPrompt.Format(_T("%s��ʼ����ʧ��,������(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
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
						strPrompt.Format(_T("%s����ʧ��,������(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
						AddPrompt(strPrompt,TRUE);
						break;
					}
					else if (ret == 2)
					{
						strPrompt.Format(_T("%s����:%s��������"),m_TestCaseList[i].TestName.c_str(),str2wstr(strOutput).c_str());
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
		if (nSdCardTest==0)		//������
		{
			ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_Configs.strSdcardName),strOutput);
			if (ret==1)
			{
				ret = m_DevTest.CommitResult(m_TestSocket,wstr2str(m_Configs.strSdcardName));
				if (ret<0)
				{
					strPrompt.Format(_T("%s����ʧ��,������(%d)"),m_Configs.strSdcardName.c_str(),ret);
					AddPrompt(strPrompt,TRUE);
				}
				else
				{
					strPrompt.Format(_T("%s���Գɹ�"),m_Configs.strSdcardName.c_str());
					AddPrompt(strPrompt);
				}
				nSdCardTest=1;
			}else if (ret<0)
			{
				strPrompt.Format(_T("%s����ʧ��,������(%d)"),m_Configs.strSdcardName.c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				nSdCardTest=1;
			}
		}

		if (nWifiTest==0)		//������
		{
			ret = m_DevTest.QueryTestItem(m_TestSocket,wstr2str(m_Configs.strWifiName),strOutput);
			if (ret==1)
			{
				ret = m_DevTest.CommitResult(m_TestSocket,wstr2str(m_Configs.strWifiName));
				if (ret<0)
				{
					strPrompt.Format(_T("%s����ʧ��,������(%d)"),m_Configs.strWifiName.c_str(),ret);
					AddPrompt(strPrompt,TRUE);
				}
				else
				{
					strPrompt.Format(_T("%s���Գɹ�"),m_Configs.strWifiName.c_str());
					AddPrompt(strPrompt);
				}
				nWifiTest=1;
			}else if (ret<0)
			{
				strPrompt.Format(_T("%s����ʧ��,������(%d)"),m_Configs.strWifiName.c_str(),ret);
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
	m_TestSocket = socket(AF_INET,SOCK_STREAM,0);//�����׽��֣�socket��
	if (INVALID_SOCKET==m_TestSocket)
	{
		return -2;
	}

	//int nNetTimeout=3000;//5��
	//���÷��ͳ�ʱ
	//setsockopt(m_TestSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int));
	//���ý��ճ�ʱ
	//setsockopt(m_TestSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
	Timeout(m_TestSocket,3000);

	SOCKADDR_IN addrSrv; 
	addrSrv.sin_addr.S_un.S_addr=inet_addr(wstr2str((LPCTSTR)m_strIp).c_str()); 
	//addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	addrSrv.sin_family=AF_INET; 
	addrSrv.sin_port=htons((u_short)6666 ); 
	//connect(m_TestSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//���豸������������connect) 
	if (connect(m_TestSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		closesocket(m_TestSocket);
		return -3;
	}
	return 0;
}
//�Զ������������Ϣ����
void CIPSearchDlg::UpdateTestInfo(std::string strMsg)
{
	std::string strSta,strRes,strTestItem,strResult;
	int nErrCode;
	CString strInfo;
	m_Json.JsontoItem("STATUS",strSta,"TEST_ITEM",strTestItem,"RES",strRes,"RESULT",strResult,strMsg);
	m_Json.JsontoInt("ERR_CODE",nErrCode,strMsg);

	//1.Ӧ���쳣 STATUS NAK������������+ʧ��+ERR_CODE��
	//2.Ӧ������������ʧ�� ������������+ʧ��+ERR_CODE��
	if (strSta.compare("NAK")==0||strResult.compare("FAIL")==0)
	{
		strInfo.Format(_T("%s ����ʧ��(������:%d)"),str2wstr(strTestItem).c_str(),nErrCode);
		AddPrompt(strInfo,TRUE);
		m_bTestPass = false;
	}

	//3.���Խ�����������Խ���ɹ� ������������+�ɹ���
	if (strRes.compare("SAVE")==0&&strSta.compare("ACK")==0)
	{
		strInfo.Format(_T("%s ���Գɹ�"),str2wstr(strTestItem).c_str());
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
			//AddPrompt(_T("�豸Ӧ��ʱ!"),TRUE);
			return FALSE;
		}
		//���½��������Ϣ
		UpdateTestInfo(msg);

		//������Ϣ��������Ϣ���з���
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
		//�������ģʽ Ӧ������������
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
		AfxMessageBox(_T("�Ѿ������һ̨��"));
	}

}

void CIPSearchDlg::OnBnClickedButtonPass()
{
	// TODO: Add your control notification handler code here
	int ret;
	int i;
	CString strPrompt;
	std::string strOutput;
	//1.��ȡ��ǰ�����������Խ�������ò���״̬Ϊ�Ѳ���
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 1)
		{
			strPrompt.Format(_T("%s������Խ��..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			ret = m_DevTest.CommitResult(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			if (ret<0)
			{
				strPrompt.Format(_T("%s������Խ��ʧ��"),m_TestCaseList[i].TestName.c_str());
				AddPrompt(strPrompt,TRUE);
				m_TestCaseList[i].nTestStatus = -1;
				m_bTestPass = false;
				break;
			}

			strPrompt.Format(_T("%s������Խ���ɹ�"),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			m_TestCaseList[i].nTestStatus = 2;
			break;
		}
	}

	//2.��ȡ��һ��δ���ԵĲ�����
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 0)
		{
			strPrompt.Format(_T("%s��ʼ����..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			m_TestCaseList[i].nTestStatus = 1;
			if (ret<0)
			{
				strPrompt.Format(_T("%s��ʼ����ʧ��,������(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
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
				strPrompt.Format(_T("%s����ʧ��,������(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
				AddPrompt(strPrompt,TRUE);
				break;
			}
			else if (ret == 2)
			{
				strPrompt.Format(_T("%s����:%s��������"),m_TestCaseList[i].TestName.c_str(),str2wstr(strOutput).c_str());
				AddPrompt(strPrompt,TRUE);
				break;
			}
			Sleep(1000);
		}
	}
	if (i==m_TestCaseList.size()&&m_bTestPass)
	{
		//���в�����������,д��
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
	//��ȡ��һ��δ���ԵĲ�����
	for (i=0;i<m_TestCaseList.size();i++)
	{
		if (m_TestCaseList[i].nTestStatus == 1)
		{
			m_TestCaseList[i].nTestStatus = -1;
		}
		if (m_TestCaseList[i].nTestStatus == 0)
		{
			strPrompt.Format(_T("%s��ʼ����..."),m_TestCaseList[i].TestName.c_str());
			AddPrompt(strPrompt);
			m_TestCaseList[i].nTestStatus = 1;
			ret = m_DevTest.StartTestItem(m_TestSocket,wstr2str(m_TestCaseList[i].TestName));
			if (ret<0)
			{
				strPrompt.Format(_T("%s��ʼ����ʧ��,������(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
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
						strPrompt.Format(_T("%s����ʧ��,������(%d)"),m_TestCaseList[i].TestName.c_str(),ret);
						AddPrompt(strPrompt,TRUE);
						break;
					}
					else if (ret == 2)
					{
						strPrompt.Format(_T("%s����:%s��������"),m_TestCaseList[i].TestName.c_str(),str2wstr(strOutput).c_str());
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
		strPrompt.Format(_T("���в������"));
		AddPrompt(strPrompt);
	}
}
