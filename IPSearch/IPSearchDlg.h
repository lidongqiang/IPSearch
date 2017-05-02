
// IPSearchDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "settings/SettingBase.h"
#include "ConfigDlg.h"
#include "VideoDlg.h"
#include "JsonConvert.h"
#include "XListBox/XListBox.h"
#include "debug.h"
#include "PcbaTest .h"
#include "AVPlayer.h"

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
private:
	CAVPlayer   m_cAVPlayer;        // ��������
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
	CJsonConvert m_Json;
	CPcbaTest	m_DevTest;
	//CVideoDlg	m_VideoDlg;
	CXListBox   m_listInfo;
	CLogger     *m_pLog;
public:
	void initUi();
	void GetOtherIp();
	void initTestCase();
	std::wstring GetLocalString(std::wstring strKey);
	void ParseDevInfo(char *msg,CString &strUid,CString &strAddr,CString &strDevname);
	bool OnStartTest();
	BOOL TestProc();
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
public:
	afx_msg void OnBnClickedBtnSerch();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnNMClickListDevice(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnPlayer();
	afx_msg void OnBnClickedButtonNext();
	afx_msg LRESULT OnHandleUpdateConfigMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedButtonPass();
	afx_msg void OnBnClickedButtonFail();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHelpAbout();
	afx_msg void OnBnClickedButtonExit();
};
