// ameraTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPSearch.h"
#include "ameraTestDlg.h"
#include "scriptexe.h"


// CameraTestDlg dialog

IMPLEMENT_DYNAMIC(CameraTestDlg, CDialog)

CameraTestDlg::CameraTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CameraTestDlg::IDD, pParent)
{

}

CameraTestDlg::~CameraTestDlg()
{
}

void CameraTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CameraTestDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_TEST, &CameraTestDlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


// CameraTestDlg message handlers

void CameraTestDlg::OnBnClickedBtnTest()
{
	// TODO: Add your control notification handler code here
	CSpawn					ShellSpawn;
	wchar_t     strCmd[2600] = {0};  
	CString		strModulePath,strBmpPath,strPrompt;
	int			ret;
	TCHAR MyexeFullPath[MAX_PATH] = TEXT("C:\\");
	if(GetModuleFileName(NULL,MyexeFullPath,MAX_PATH)) {
		TCHAR *pos = _tcsrchr(MyexeFullPath, TEXT('\\'));
		if(pos) {
			pos[1] = 0;
			strModulePath = MyexeFullPath;
		}
	}
	strBmpPath = strModulePath + _T("test.bmp");

	//协议交互
	ret = connect_dev();
	if (ret<0)
	{
		AfxMessageBox(_T("connect device error"));
		return;
	}
	ret = m_CameraTest.CameraTest(m_CameraSocket,"");
	if (ret<0)
	{
		AfxMessageBox(_T("Camera Test error"));
		return;
	}

	//从设备中获取图像
	swprintf(strCmd,sizeof(strCmd) / sizeof(strCmd[0]),TEXT("adb.exe pull //tmp//camera_test//test.bmp %s"),strBmpPath);
	if(ShellSpawn.Exe(strCmd,25000, true)) 
	{
		if(0 != ShellSpawn.GetResult()) 
		{
			AfxMessageBox(_T("get picture fail!"));
			return ;
		}
	}
	else
	{
		AfxMessageBox(_T("update picture fail!"));
		return ;
	}


	//显示图像
	CDC *pDC=NULL;    
	pDC = GetDlgItem(IDC_STATIC_PICTURE)->GetDC();    
	DrawImg(pDC, strBmpPath, 0, 0);    

	ReleaseDC(pDC); // 记得释放资源，不然会导致内存泄露  
}

void CameraTestDlg::DrawImg(CDC* pDC,CString strPath, int x, int y)  
{  
	bool m_bPrintFlag=false;
	IPicture *pPic=NULL;     
	OleLoadPicturePath(CComBSTR(strPath.GetBuffer()), (LPUNKNOWN)NULL, 0, 0, IID_IPicture,(LPVOID*)&pPic);    
	if (NULL == pPic)    
	{    
		return;    
	}    

	// 获取图像宽和高,注意这里的宽和高不是图像的分辨率    
	OLE_XSIZE_HIMETRIC hmWidth;      
	OLE_YSIZE_HIMETRIC hmHeight;      
	pPic->get_Width(&hmWidth);      
	pPic->get_Height(&hmHeight);      

	// 获取显示图片窗口的宽度和高度    
	RECT rtWnd;    
	pDC->GetWindow()->GetWindowRect(&rtWnd);    
	int iWndWidth = rtWnd.right - rtWnd.left;    
	int iWndHeight = rtWnd.bottom - rtWnd.top;  
	if(m_bPrintFlag)  
	{  
		iWndHeight = iWndHeight * 10;  
		iWndWidth = iWndWidth * 10;  
	}  

	if(FAILED(pPic->Render(*pDC, x, y, iWndWidth, iWndHeight, 0, hmHeight, hmWidth, -hmHeight, NULL)))      
	{    
		pPic->Release();    
		return;    
	}    

	//记得释放资源，不然会导致内存泄露    
	pPic->Release();     

	return;    
}  
int CameraTestDlg::connect_dev()
{
	CSpawn		ShellSpawn;
	wchar_t     strCmd[2600] = {0};
	CString		strPort = _T("6666");
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
	m_CameraSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//创建套接字（socket）
	if (INVALID_SOCKET==m_CameraSocket)
	{
		return -2;
	}

	//adb socket
	swprintf(strCmd,nof(strCmd),TEXT("adb.exe forward tcp:8888 tcp:%s"),strPort);
	if(ShellSpawn.Exe(strCmd,25000, true)) 
	{
		if(0 != ShellSpawn.GetResult()) 
		{
			AfxMessageBox(_T("adb forward fail!"));
			return -3;
		}
	}
	else
	{
		AfxMessageBox(_T("adb forward fail!"));
		return -4;
	}

	int nNetTimeout=3000;//3秒
	//设置发送超时
	setsockopt(m_CameraSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int));
	//设置接收超时
	setsockopt(m_CameraSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));

	SOCKADDR_IN addrSrv; 
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1"); 
	//addrSrv.sin_addr.S_un.S_addr=inet_addr("172.16.14.156");
	addrSrv.sin_family=AF_INET; 
	addrSrv.sin_port=htons((u_short)8888 ); 
	//connect(m_TestSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//向设备发出连接请求（connect) 
	if (connect(m_CameraSocket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		closesocket(m_CameraSocket);
		return -3;
	}
	return 0;
}