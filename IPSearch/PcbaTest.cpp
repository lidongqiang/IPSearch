#include "stdafx.h"
#include "PcbaTest .h"
#include "c_socket.h"

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
		LOGER((CLogger::DEBUG_DUT,"socket:send data failed\r\n"));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "RES":"ENTER", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	LOGER((CLogger::DEBUG_DUT,"recv_msg%s\r\n",strMsg.c_str()));
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:recv data failed\r\n"));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,strMsg);
	m_Json.JsontoItem("STATUS",strSta,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"ENTER"))
	{
		return -3;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return -atoi(strErrCode.c_str());
	}

	//3.读取当前设备端的模式，是否已经进入测试模式
	return 0;
}

int CPcbaTest::ExitTest(SOCKET TestSocket)
{
	std::string strMsg,strSta,strRes,strErrCode;
	int ret,nErrCode;

	LOGER((CLogger::DEBUG_DUT,"ExitTest()"));

	//1.发送命令给设备端退出测试模式{"TYPE":"CMD", "CMD":"EXIT" }
	m_Json.ItemtoJson("TYPE","CMD","CMD","EXIT",strMsg);
	LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:send data failed\r\n"));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "RES":"EXIT"，"STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"socket:recv data failed\r\n"));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,strMsg);
	m_Json.JsontoItem("STATUS",strSta,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"EXIT"))
	{
		return -3;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return -atoi(strErrCode.c_str());
	}
	return 0;
}

int CPcbaTest::StartTestItem(SOCKET TestSocket,std::string TestName)
{
	std::string strMsg,strSta,strRes,strTestItem,strErrCode;
	int ret,nErrCode;


	//int n =5;
	//while (n--)
	//{
	//	Sleep(1000);
	//}

	LOGER((CLogger::DEBUG_DUT,"StartTestItem()"));

	//1.发送命令给设备端开始测试 {"TYPE":"CMD", "TEST ITEM":"KEY-TEST", "CMD":"START" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","START",/*"MSG","msg",*/strMsg);
	LOGER((CLogger::DEBUG_DUT,"send msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName.c_str()));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST ITEM":"KEY-TEST", "RES":"START", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	LOGER((CLogger::DEBUG_DUT,"recv msg:%s\n",strMsg.c_str()));
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg.c_str()));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"START")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -3;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return -atoi(strErrCode.c_str());
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
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName.c_str()));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"QUERY", "MSG":"msg", "STATUS":"ACK", "RESULT":"PASS"}
	ret = socket_read(TestSocket,strMsg);
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s",strMsg.c_str()));
	if (ret <= 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",TestName.c_str()));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,"RESULT",strResult,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"QUERY")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -3;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK")||!stringCompareIgnoreCase(strResult,"FAIL"))
	{
		return -atoi(strErrCode.c_str());
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

int CPcbaTest::CommitResult(SOCKET TestSocket,std::string TestName,bool bPass)
{
	std::string strMsg,strSta,strRes,strTestItem,strErrCode;
	int ret,nErrCode;

	LOGER((CLogger::DEBUG_DUT,"CommitResult()"));

	//1.发送命令给设备端保存测试结果 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"SAVE" }
	if (bPass)
	{
		m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","SAVE",strMsg);
	}
	else
	{
		m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","SAVE",strMsg);
	}
	LOGER((CLogger::DEBUG_DUT,"send_msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"SAVE", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg.c_str()));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"SAVE")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -3;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return -atoi(strErrCode.c_str());
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
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"STOP", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg.c_str()));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"STOP")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -3;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return -atoi(strErrCode.c_str());
	}
	return 0;
}


int CPcbaTest::WriteUidAndLanMac(SOCKET TestSocket,std::string TestName,std::string strWriteMsg)
{
	std::string strMsg,strSta,strRes,strTestItem,strResult,strErrCode;
	int ret,nErrCode;
	int n=5;

	LOGER((CLogger::DEBUG_DUT,"WriteUidAndLanMac()"));

	//1.发送命令给设备端开始测试 {"TYPE":"CMD", "TEST ITEM":"WRITE_TEST", "CMD":"START" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"MSG",strWriteMsg,"CMD","START",strMsg);
	LOGER((CLogger::DEBUG_DUT,"send_msg:%s",strMsg.c_str()));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST ITEM":"KEY-TEST", "RES":"START", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
	if (ret < 0)
	{
		LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (stringCompareIgnoreCase(strRes,"START")||stringCompareIgnoreCase(strTestItem,TestName))
	{
		return -3;
	}
	if (!stringCompareIgnoreCase(strSta,"NAK"))
	{
		return -atoi(strErrCode.c_str());
	}
	while(n--)
	{
		//3.发送命令给设备端查询测试状态 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"QUERY" }
		m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","QUERY",strMsg);
		LOGER((CLogger::DEBUG_DUT,"send_msg:%s\r\n",strMsg.c_str()));
		ret = socket_write(TestSocket,strMsg);
		if (ret < 0)
		{
			LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName.c_str()));
			return -4;
		}

		//4.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"QUERY", "MSG":"msg", "STATUS":"ACK", "RESULT":"PASS"}
		socket_read(TestSocket,strMsg);
		LOGER((CLogger::DEBUG_DUT,"recv_msg:%s\n",strMsg.c_str()));
		if (ret < 0)
		{
			LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg.c_str()));
			return -5;
		}
		m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,"RESULT",strResult,strMsg);
		m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
		if (stringCompareIgnoreCase(strRes,"START")||stringCompareIgnoreCase(strTestItem,TestName))
		{
			return -6;
		}
		if (!stringCompareIgnoreCase(strSta,"NAK")||!stringCompareIgnoreCase(strResult,"FAIL"))
		{
			return -atoi(strErrCode.c_str());
		}
		if (!stringCompareIgnoreCase(strResult,"PASS"))
		{
			break;
		}
		Sleep(500);
	}
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
	//int n = 5;
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
	return 0;
}