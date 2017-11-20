#include "stdafx.h"
#include "PcbaTest .h"
#include "c_socket.h"
#include "scriptexe.h"

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

CPcbaTest::CPcbaTest():m_pLogger(NULL)
{

}
int CPcbaTest::EnterTestMode(SOCKET TestSocket)
{
	std::string strMsg,strSta,strRes,strErrCode;
	int ret,nErrCode;

	LOGER((CLogger::DEBUG_DUT,"EnterTestMode()"));

	//1.发送命令给设备端进入测试模式{"TYPE":"CMD", "CMD":"ENTER" }
	m_Json.ItemtoJson("TYPE","CMD","CMD","ENTER",strMsg);
	LOGER((CLogger::DEBUG_DUT,"send_msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:send data failed,err=%d\r\n",ret));
		return -101;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "RES":"ENTER", "STATUS":"ACK"}
	ret = socket_read(TestSocket,strMsg);
	if (ret <= 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:recv data failed,err\r\n",ret));
		return -102;
	}
	LOGER((CLogger::DEBUG_DUT,"recv_msg%s\r\n",strMsg.c_str()));
	m_Json.JsontoItem("RES",strRes,strMsg);
	m_Json.JsontoItem("STATUS",strSta,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"ENTER"))
	{
		return -103;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return atoi(strErrCode.c_str());
	}

	//3.读取当前设备端的模式，是否已经进入测试模式
	return 0;
}

int CPcbaTest::ExitTest(SOCKET TestSocket,std::string Msg)
{
	std::string strMsg,strSta,strRes,strErrCode;
	int ret,nErrCode;

	LOGER((CLogger::DEBUG_DUT,"ExitTest()"));

	//1.发送命令给设备端退出测试模式{"TYPE":"CMD", "CMD":"EXIT" }
	m_Json.ItemtoJson("TYPE","CMD","CMD","EXIT","MSG",Msg,strMsg);
	LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:send data failed,err=%d\r\n",ret));
		return -101;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "RES":"EXIT"，"STATUS":"ACK"}
	ret = socket_read(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:recv data failed,err=%d\r\n",ret));
		return -102;
	}
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	m_Json.JsontoItem("RES",strRes,strMsg);
	m_Json.JsontoItem("STATUS",strSta,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"EXIT"))
	{
		return -103;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return atoi(strErrCode.c_str());
	}
	return 0;
}

int CPcbaTest::StartTestItem(SOCKET TestSocket,std::string TestName,std::string Msg)
{
	std::string strMsg,strSta,strRes,strTestItem,strErrCode;
	int ret,nErrCode;

	ret = UploadFile(TestName);
	//if (ret < 0)
	//{
	//	return -101;
	//}
	LOGER((CLogger::DEBUG_DUT,"StartTestItem()"));

	//1.发送命令给设备端开始测试 {"TYPE":"CMD", "TEST ITEM":"KEY-TEST", "CMD":"START" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","START","MSG",Msg,strMsg);
	LOGER((CLogger::DEBUG_DUT,"send msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed.err=%d\r\n",TestName.c_str(),ret));
		return -102;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST ITEM":"KEY-TEST", "RES":"START", "STATUS":"ACK"}
	ret = socket_read(TestSocket,strMsg);
	if (ret <= 0)
	{
		LOGER((CLogger::DEBUG_DUT,"recv data failed,err=%d\r\n",ret));
		return -103;
	}
	LOGER((CLogger::DEBUG_DUT,"recv msg:%s\n",strMsg.c_str()));
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"START")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -104;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return atoi(strErrCode.c_str());
	}
	return 0;
}

//返回值
//测试中------0
//测试成功----1
//需人工判断---2
//测试失败----errcode
int CPcbaTest::QueryTestItem(SOCKET TestSocket,std::string TestName,std::string &strOutput)
{
	std::string strMsg,strSta,strRes,strTestItem,strResult,strErrCode;
	int ret,nErrCode;

	LOGER((CLogger::DEBUG_DUT,"QueryTestItem()"));

	//1.发送命令给设备端查询测试状态 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"QUERY" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","QUERY",strMsg);
	LOGER((CLogger::DEBUG_DUT,"send_msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed,errcode=%d\r\n",TestName.c_str(),ret));
		return -101;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"QUERY", "MSG":"msg", "STATUS":"ACK", "RESULT":"PASS"}
	ret = socket_read(TestSocket,strMsg);
	if (ret <= 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed,errcode=%d\r\n",TestName.c_str(),ret));
		return -102;
	}
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s",strMsg.c_str()));
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,"RESULT",strResult,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"QUERY")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -103;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK")||!stringCompareIgnoreCase(strResult,"FAIL"))
	{
		return atoi(strErrCode.c_str());
	}
	if (!stringCompareIgnoreCase(strResult,"PASS"))
	{
		return 1;
	}
	if (!stringCompareIgnoreCase(strResult,"verify"))
	{
		m_Json.JsontoItem("MSG",strOutput,strMsg);
		return 2;
	}
	return 0;
}

int CPcbaTest::CommitResult(SOCKET TestSocket,std::string TestName,std::string strResult)
{
	std::string strMsg,strSta,strRes,strTestItem,strErrCode;
	int ret,nErrCode;

	LOGER((CLogger::DEBUG_DUT,"CommitResult()"));

	//1.发送命令给设备端保存测试结果 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"SAVE" }

	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"MSG",strResult,"CMD","START",strMsg);
	LOGER((CLogger::DEBUG_DUT,"send_msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:send data failed,err=%d\r\n",ret));
		return -101;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"SAVE", "STATUS":"ACK"}
	ret = socket_read(TestSocket,strMsg);
	if (ret <= 0)
	{
		LOGER((CLogger::DEBUG_DUT,"recv data failed,ret=%d\r\n",ret));
		return -102;
	}
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"START"))
	{
		return -103;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return atoi(strErrCode.c_str());
	}
	return 0;
}

int CPcbaTest::StopTestItem(SOCKET TestSocket,std::string TestName)
{
	std::string strMsg,strSta,strRes,strTestItem,strErrCode;
	int ret,nErrCode;

	LOGER((CLogger::DEBUG_DUT,"StopTestItem()"));

	//1.发送命令给设备端保存测试结果 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"STOP" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","STOP",strMsg);

	LOGER((CLogger::DEBUG_DUT,"send_msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed,errcode=%d\r\n",TestName.c_str(),ret));
		return -101;
	}
	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"STOP", "STATUS":"ACK"}
	ret = socket_read(TestSocket,strMsg);
	if (ret <= 0)
	{
		LOGER((CLogger::DEBUG_DUT,"recv data failed,errcode=%d\r\n",ret));
		return -102;
	}
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"STOP")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -103;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return atoi(strErrCode.c_str());
	}
	return 0;
}


int CPcbaTest::WriteUidAndLanMac(SOCKET TestSocket,std::string TestName,std::string strWriteMsg)
{
	std::string strMsg,strSta,strRes,strTestItem,strResult,strErrCode;
	int ret,nErrCode;
	int n=5;

	LOGER((CLogger::DEBUG_DUT,"WriteUidAndLanMac()"));

	ret = UploadFile(TestName);
	if (ret < 0)
	{
		return -101;
	}

	//1.发送命令给设备端开始测试 {"TYPE":"CMD", "TEST ITEM":"WRITE_TEST", "CMD":"START" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"MSG",strWriteMsg,"CMD","START",strMsg);
	LOGER((CLogger::DEBUG_DUT,"send_msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -102;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST ITEM":"KEY-TEST", "RES":"START", "STATUS":"ACK"}
	ret = socket_read(TestSocket,strMsg);
	if (ret <= 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg));
		return -103;
	}
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"START")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -4;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return atoi(strErrCode.c_str());
	}
	//while(n--)
	//{
	//	//3.发送命令给设备端查询测试状态 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"QUERY" }
	//	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","QUERY",strMsg);
	//	LOGER((CLogger::DEBUG_DUT,"send_msg:%s\r\n",strMsg.c_str()));
	//	ret = socket_write(TestSocket,strMsg);
	//	if (ret < 0)
	//	{
	//		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName.c_str()));
	//		return -5;
	//	}

	//	//4.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"QUERY", "MSG":"msg", "STATUS":"ACK", "RESULT":"PASS"}
	//	socket_read(TestSocket,strMsg);
	//	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	//	if (ret < 0)
	//	{
	//		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg.c_str()));
	//		return -6;
	//	}
	//	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,"RESULT",strResult,strMsg);
	//	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	//	if (stringCompareIgnoreCase(strRes,"START")||stringCompareIgnoreCase(strTestItem,TestName))
	//	{
	//		return -7;
	//	}
	//	if (!stringCompareIgnoreCase(strSta,"NAK")||!stringCompareIgnoreCase(strResult,"FAIL"))
	//	{
	//		return -atoi(strErrCode.c_str());
	//	}
	//	if (!stringCompareIgnoreCase(strResult,"PASS"))
	//	{
	//		break;
	//	}
	//	Sleep(500);
	//}
	return 0;
}

int CPcbaTest::WriteUid(SOCKET TestSocket,std::string TestName,std::string strWriteMsg)
{
	return 0;
}

int CPcbaTest::WriteLanMac(SOCKET TestSocket,std::string TestName,std::string strWriteMsg)
{
	return 0;
}
int CPcbaTest::KeyTest(SOCKET TestSocket,std::string TestName,std::string &strOutput)
{
	std::string strMsg,strSta,strRes,strTestItem,strResult,strErrCode;
	int ret,nErrCode;
	int n = 5;
	LOGER((CLogger::DEBUG_DUT,"KeyTest()\n"));
	ret = StartTestItem(TestSocket,TestName);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"StartTestItem failed,errcode:%d\n",ret));
		return ret;
	}
	while(1)
	{
		ret = QueryTestItem(TestSocket,TestName,strOutput);
		if (ret < 0)
		{
			LOGER((CLogger::DEBUG_DUT,"QueryTestItem failed,errcode:%d\n",ret));
			return ret;
		}
		else if (ret == 2)
		{
			return ret;
		}
		Sleep(1000);
	}
	return -101;
}

int CPcbaTest::UploadFile(std::string strFileName)
{
	CSpawn					ShellSpawn;
	wchar_t     strCmd[2600] = {0};  

	LOGER((CLogger::DEBUG_DUT,"UploadFile()\n"));
	//tftp.exe -i 172.16.14.62 put test_ftp.txt
	swprintf(strCmd,nof(strCmd),TEXT("tftp -i %s put \"%s%s\""),m_strIp.c_str(),m_strTestPath.c_str(),str2wstr(strFileName).c_str());
	if(ShellSpawn.Exe(strCmd,25000, true)) 
	{
		if(0 != ShellSpawn.GetResult()) 
		{
			LOGER((CLogger::DEBUG_DUT,"tftp file error strCmd=%s\n",wstr2str(strCmd).c_str()));
			return -1;
		}
	}
	else
	{
		LOGER((CLogger::DEBUG_DUT,"tftp file error strCmd=%s\n",wstr2str(strCmd).c_str()));
		return -2;
	}
	//if (!stringCompareIgnoreCase(strFileName,wstr2str(m_strWifiTest)))
	//{
	//	swprintf(strCmd,nof(strCmd),TEXT("tftp.exe -i %s put %s%s"),m_strIp.c_str(),m_strTestPath.c_str(),TEXT("wifi.sh"));
	//	if(ShellSpawn.Exe(strCmd,2500, true)) 
	//	{
	//		if(0 != ShellSpawn.GetResult()) 
	//		{
	//			LOGER((CLogger::DEBUG_DUT,"tftp file error strCmd=%s\n",wstr2str(strCmd).c_str()));
	//			return -3;
	//		}
	//	}
	//	else
	//	{
	//		LOGER((CLogger::DEBUG_DUT,"tftp file error strCmd=%s\n",wstr2str(strCmd).c_str()));
	//		return -4;
	//	}
	//}
	return 0;
}

int CPcbaTest::MonitorTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName,"-c record");
}

int CPcbaTest::InterphoneTest(SOCKET TestSocket,std::string TestName)
{
	//UploadFile("play_test.wav");
	return StartTestItem(TestSocket,TestName,"-c play");
}
int CPcbaTest::SdCardTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::IrcutTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::LedTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::BtTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::EmmcTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::DdrTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::RtcTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::RotaryTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::HdmiTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::PtzTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::WifiTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::TouchTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::MicTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::CameraTest(SOCKET TestSocket,std::string TestName)
{
	return StartTestItem(TestSocket,TestName);
}
int CPcbaTest::StopCamera(SOCKET TestSocket,std::string TestName)
{
	return StopTestItem(TestSocket,TestName);
}