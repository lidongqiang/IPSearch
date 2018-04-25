
// IPSearchDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "settings/SettingBase.h"
#include "ConfigDlg.h"
#include "JsonConvert.h"
#include "XListBox/XListBox.h"
#include "label/FontStatic.h"
#include "debug.h"
#include "PcbaTest .h"
//#include "AVPlayer.h"
#include "ListCtrlEx/GridListCtrlColor.h"
#include "scriptexe.h"

typedef struct  
{
	int     flag;
	TCHAR   pszLineText[MAX_PATH];
}STRUCT_LIST_LINE,*PSTRUCT_LIST_LINE;

typedef struct
{
	std::wstring TestName;
	bool bAuto;//TRUE �Զ����ԣ�FALSE �ֶ�����
	int nTestStatus;//0--δ���ԣ�1---�����У�2----���Գɹ���-1----����ʧ��,3--------�˹��ж�
}TestCase,STRUCT_TEST_CASE;
typedef vector<STRUCT_TEST_CASE> TEST_CASE_VECTOR;

typedef struct  
{
	std::wstring strDevName;
	std::wstring strIP;
	std::wstring strUid;
	std::wstring strMac;
}DevInfo,STRUCT_DEV_INFO;
typedef vector<STRUCT_DEV_INFO> DEV_INFO_VECTOR;


// CIPSearchDlg �Ի���
class CIPSearchDlg : public CDialog
{
// ����
public:
	CIPSearchDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IPSEARCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
//private:
//	CAVPlayer   m_cAVPlayer;        // ��������
public:
	CIniSettingBase m_Configs;
	CIniLocalLan	m_LocalLan;

	CString         m_strModulePath;
	CListCtrl	m_listDevice;
	CString		m_strIp;
	int			m_listSelect;
	bool		m_bNext;
	bool		m_bExit;
	bool		m_bRun;
	bool		m_bTestPass;//��¼���в�������Խ����ȫ��ͨ��ΪTrue���в���û��ͨ����ΪFalse
	CWinThread  *m_pTestThread;
	CWinThread  *m_pRecvThread;
	SOCKET		m_TestSocket;
	TEST_CASE_VECTOR m_TestCaseList;
	DEV_INFO_VECTOR m_DevList;
	CJsonConvert m_Json;
	CPcbaTest	m_DevTest;
	//CVideoDlg	m_VideoDlg;
	CXListBox   m_listInfo;
	CFontStatic m_lbVideo;
	CLogger     *m_pLog;
	CGridListCtrlColor m_listTestItem;

	CWinThread		*m_pScanThread;
	CEvent			*m_pScanEvent;
	CCriticalSection m_csScanLock;
	BOOL			m_bUpgradeDllInitOK;
	BOOL			m_bTerminated;
	UINT			m_nDeviceCount;
	BOOL			m_bExistMsc;
	BOOL			m_bExistAdb;
	BOOL			m_bExistLoader;
	BOOL            m_bRedLedLight;
	HBITMAP         m_hGreenLedBitmap;
	HBITMAP         m_hRedLedBitmap;
public:
	void initUi();
	void GetOtherIp();
	void initTestCase();
	std::wstring GetLocalString(std::wstring strKey);
	void ParseDevInfo(char *msg,CString &strUid,CString &strAddr,CString &strDevname);
	bool OnStartTest();
	BOOL TestProc();
	BOOL NextTestProc(int nTestResult);
	int ScanDeviceProc(LPVOID lpParameter);
	void ScanAdbDeviceProc();
	BOOL RecvProc();
	//void RecvThread();
	int	 connect_dev();
	void RtspPlay();
	void ExitTest();
	void AddPrompt(CString strPrompt,BOOL bError=FALSE,INT iColor = 0);
	void UpdateTestInfo(std::string strMsg);
	void WritePara();
	void SaveTestResult();
	BOOL LoadConfig();
	VOID WalkMenu(CMenu *pMenu,CString strMainKeyPart);
	int DoTestItem(std::wstring strTestName,std::string &strInfo);
	void UpdateTestitemList();
	void GetDeviceList();
	void UpdateDevInfo(std::string strInfo);
public:
	afx_msg void OnBnClickedBtnSerch();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnNMClickListDevice(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedButtonNext();
	afx_msg LRESULT OnHandleUpdateConfigMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedButtonPass();
	afx_msg void OnBnClickedButtonFail();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHelpAbout();
	afx_msg void OnBnClickedButtonExit();
	afx_msg LRESULT OnHandleUpdateTestinfoMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedBtnCameraTest();
};
typedef struct{
	CIPSearchDlg *pDlg;
	char *ip;
}ThreadInfo;
typedef struct{
	CIPSearchDlg *pDlg;
	int nResult;
}TestInfo;