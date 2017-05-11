#ifndef PCBA_TEST_H
#define  PCBA_TEST_H
#include <vector>
#include "JsonConvert.h"
#include "debug.h"
#define LOG(A)
#define LOGER(a) if(m_pLogger) m_pLogger->PrintMSGA a
#define nof(x) (sizeof(x) / sizeof(x[0]))

struct STATUS_DATA
{
	unsigned int sdcard:     1;
	unsigned int wifi        :1;
	unsigned int key	     :1;
	unsigned int interphone  :1;
	unsigned int monitor     :1;
	unsigned int ircut       :1;
	unsigned int ptz	     :1;
	unsigned int led		 :1;
	unsigned int reserved1   :1;
	unsigned int reserved2   :1;
	unsigned int reserved3   :1;
	unsigned int reserved4   :1;
	unsigned int reserved5   :1;
	unsigned int valid       :1;
	unsigned int status      :2; /*0 idle 1 testing 2 done**/
};
#define ITEM_SDCARD_MASK        (0x1)
#define ITEM_WIFI_MASK			(0x2)
#define ITEM_KEY_MASK			(0x4)
#define ITEM_INTERPHONE_MASK    (0x8)
#define ITEM_MONTOR_MASK        (0x10)
#define ITEM_IRCUT_MASK         (0x20)
#define ITEM_PTZ_MASK			(0x40)
#define ITEM_LED_MASK			(0x80)
#define ITEM_RESERVED_BIT1      (0x100)
#define ITEM_RESERVED_BIT2      (0x200)
#define ITEM_RESERVED_BIT3      (0x400)
#define ITEM_RESERVED_BIT4      (0x800)
#define ITEM_RESERVED_BIT5      (0x1000)
#define ITEM_VALID_MASK         (0x2000)
#define ITEM_TEST_STATUS        (0xC000)


class CPcbaTest
{
public:
	CLogger *m_pLogger;
	CJsonConvert m_Json;
	std::wstring m_strTestPath;
	std::wstring m_strIp;
	std::wstring m_strWifiTest;
public:
	CPcbaTest();
	//~CPcbaTest();
	void StartLog(CLogger *pLogger) {
		m_pLogger = pLogger;
	}
	void StopLog(){
		m_pLogger = NULL;
	}
	void SetTestPath(std::wstring strTestPath)
	{
		m_strTestPath = strTestPath;
	}
	void SetDevIp(std::wstring strDevIp)
	{
		m_strIp = strDevIp;
	}
	void SetWifiTest(std::wstring strWifiTest)
	{
		m_strWifiTest = strWifiTest;
	}
	bool stringCompareIgnoreCase(std::string lhs,std::string rhs)
	{
		return _stricmp(lhs.c_str(),rhs.c_str());
	}
	int UploadFile(std::string strFileName);
	int EnterTestMode(SOCKET TestSocket);
	int ExitTest(SOCKET TestSocket);
	int StartTestItem(SOCKET TestSocket,std::string TestName,std::string Msg="");
	int QueryTestItem(SOCKET TestSocket,std::string TestName,std::string &strOutput);
	int CommitResult(SOCKET TestSocket,std::string TestName,std::string strResult);
	int StopTestItem(SOCKET TestSocket,std::string TestName);
	int WriteUidAndLanMac(SOCKET TestSocket,std::string TestName,std::string strWriteMsg);
	int WriteUid(SOCKET TestSocket,std::string TestName,std::string strWriteMsg);
	int WriteLanMac(SOCKET TestSocket,std::string TestName,std::string strWriteMsg);
	int KeyTest(SOCKET TestSocket,std::string TestName,std::string &strOutput);
	int SdCardTest(SOCKET TestSocket,std::string TestName);
	int WifiTest(SOCKET TestSocket,std::string TestName);
	int InterphoneTest(SOCKET TestSocket,std::string TestName);
	int IrcutTest(SOCKET TestSocket,std::string TestName);
	int LedTest(SOCKET TestSocket,std::string TestName);
	int HdmiTest(SOCKET TestSocket,std::string TestName);
	int MonitorTest(SOCKET TestSocket,std::string TestName);
	int PtzTest(SOCKET TestSocket,std::string TestName);
	int CameraTest(SOCKET TestSocket,std::string TestName);
	int StopCamera(SOCKET TestSocket,std::string TestName);

};


#endif