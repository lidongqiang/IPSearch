#ifndef PCBA_TEST_H
#define  PCBA_TEST_H
#include <vector>
#include "JsonConvert.h"
#include "debug.h"
#define LOG(A)
#define LOGER(a) if(m_pLogger) m_pLogger->PrintMSGA a



class CPcbaTest
{
public:
	CLogger *m_pLogger;
	CJsonConvert m_Json;

public:
	CPcbaTest();
	//~CPcbaTest();
	void StartLog(CLogger *pLogger) {
		m_pLogger = pLogger;
	}
	void StopLog(){
		m_pLogger = NULL;
	}
	bool stringCompareIgnoreCase(std::string lhs,std::string rhs)
	{
		return _stricmp(lhs.c_str(),rhs.c_str());
	}
	int EnterTestMode(SOCKET TestSocket);
	int ExitTest(SOCKET TestSocket);
	int StartTestItem(SOCKET TestSocket,std::string TestName);
	int QueryTestItem(SOCKET TestSocket,std::string TestName,std::string &strOutput);
	int CommitResult(SOCKET TestSocket,std::string TestName);
	int WriteUidAndLanMac(SOCKET TestSocket,std::string TestName,std::string strWriteMsg);
};


#endif