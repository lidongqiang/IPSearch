#include "stdafx.h"
#include "PcbaTest .h"
#include "c_socket.h"

int CPcbaTest::EnterTestMode(SOCKET TestSocket)
{
	std::string strMsg,strSta,strRes,strErrCode;
	int ret,nErrCode;

	//LOGER((CLogger::DEBUG_DUT,"EnterTestMode()\r\n"));

	//1.发送命令给设备端进入测试模式{"TYPE":"CMD", "CMD":"ENTER" }
	m_Json.ItemtoJson("TYPE","CMD","CMD","ENTER",strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"socket:send data failed\r\n"));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "RES":"ENTER", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"socket:recv data failed\r\n"));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,strMsg);
	m_Json.JsontoItem("STATUS",strSta,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (strRes.compare("EXIT"))
	{
		return -3;
	}
	if (strSta.compare("NAK")==0)
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

	//LOGER((CLogger::DEBUG_DUT,"ExitTest()\r\n"));

	//1.发送命令给设备端退出测试模式{"TYPE":"CMD", "CMD":"EXIT" }
	m_Json.ItemtoJson("TYPE","CMD","CMD","EXIT",strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"socket:send data failed\r\n"));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "RES":"EXIT"，"STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"socket:recv data failed\r\n"));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,strMsg);
	m_Json.JsontoItem("STATUS",strSta,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (strRes.compare("EXIT"))
	{
		return -3;
	}
	if (strSta.compare("NAK")==0)
	{
		return -atoi(strErrCode.c_str());
	}
	return 0;
}

int CPcbaTest::StartTestItem(SOCKET TestSocket,std::string TestName)
{
	std::string strMsg,strSta,strRes,strTestItem,strErrCode;
	int ret,nErrCode;

	//LOGER((CLogger::DEBUG_DUT,"StartTestItem()\r\n"));

	//1.发送命令给设备端开始测试 {"TYPE":"CMD", "TEST ITEM":"KEY-TEST", "CMD":"START" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","START","MSG","msg",strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST ITEM":"KEY-TEST", "RES":"START", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (strRes.compare("START")||strTestItem.compare(TestName))
	{
		return -3;
	}
	if (strSta.compare("NAK")==0)
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

	//LOGER((CLogger::DEBUG_DUT,"QueryTestItem()\r\n"));

	//1.发送命令给设备端查询测试状态 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"QUERY" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","QUERY",strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"QUERY", "MSG":"msg", "STATUS":"ACK", "RESULT":"PASS"}
	socket_read(TestSocket,strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,"RESULT",strResult,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (strRes.compare("QUERY")||strTestItem.compare(TestName))
	{
		return -3;
	}
	if (strSta.compare("NAK")==0||strResult.compare("FAIL")==0)
	{
		return -atoi(strErrCode.c_str());
	}
	if (strResult.compare("pass")==0)
	{
		return 1;
	}
	if (strResult.compare("VERIFY")==0)
	{
		m_Json.JsontoItem("MSG",strOutput,strMsg);
		return 2;
	}
	return 0;
}

int CPcbaTest::CommitResult(SOCKET TestSocket,std::string TestName)
{
	std::string strMsg,strSta,strRes,strTestItem,strErrCode;
	int ret,nErrCode;

	//LOGER((CLogger::DEBUG_DUT,"CommitResult()\r\n"));

	//1.发送命令给设备端保存测试结果 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"SAVE" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","SAVE",strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"SAVE", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoItem("ERR_CODE",strErrCode,strMsg);
	if (strRes.compare("SAVE")||strTestItem.compare(TestName))
	{
		return -3;
	}
	if (strSta.compare("NAK")==0)
	{
		return -atoi(strErrCode.c_str());
	}
	return 0;
}

int CPcbaTest::WriteUidAndLanMac(SOCKET TestSocket,std::string TestName,std::string strWriteMsg)
{
	std::string strMsg,strSta,strRes,strTestItem,strResult;
	int ret,nErrCode;
	int n=5;

	//LOGER((CLogger::DEBUG_DUT,"WriteUidAndLanMac()\r\n"));

	//1.发送命令给设备端开始测试 {"TYPE":"CMD", "TEST ITEM":"WRITE_TEST", "CMD":"START" }
	m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"MSG",strWriteMsg,"CMD","START",strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	ret = socket_write(TestSocket,strMsg);
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
		return -1;
	}

	//2.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST ITEM":"KEY-TEST", "RES":"START", "STATUS":"ACK"}
	socket_read(TestSocket,strMsg);
	//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
	if (ret < 0)
	{
		//LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg));
		return -2;
	}
	m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,strMsg);
	m_Json.JsontoInt("ERR_CODE",nErrCode,strMsg);
	if (strRes.compare("START")||strTestItem.compare(TestName))
	{
		return -3;
	}
	if (strSta.compare("NAK")==0)
	{
		return nErrCode;
	}
	while(n--)
	{
		//3.发送命令给设备端查询测试状态 {"TYPE":"CMD", "TEST_ITEM":"test_item", "CMD":"QUERY" }
		m_Json.ItemtoJson("TYPE","CMD","TEST_ITEM",TestName,"CMD","QUERY",strMsg);
		//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
		ret = socket_write(TestSocket,strMsg);
		if (ret < 0)
		{
			//LOGER((CLogger::DEBUG_DUT,"%s:socket:send data failed\r\n",TestName));
			return -4;
		}

		//4.读取设备端返回的结果，确定成功与否，成功返回{"TYPE":"RES", "TEST_ITEM":"test_item", "RES":"QUERY", "MSG":"msg", "STATUS":"ACK", "RESULT":"PASS"}
		socket_read(TestSocket,strMsg);
		//LOGER((CLogger::DEBUG_DUT,"%s\r\n",strMsg));
		if (ret < 0)
		{
			//LOGER((CLogger::DEBUG_DUT,"%s:recv data failed\r\n",strMsg));
			return -5;
		}
		m_Json.JsontoItem("RES",strRes,"STATUS",strSta,"TEST_ITEM",strTestItem,"RESULT",strResult,strMsg);
		m_Json.JsontoInt("ERR_CODE",nErrCode,strMsg);
		if (strRes.compare("START")||strTestItem.compare(TestName))
		{
			return -6;
		}
		if (strSta.compare("NAK")==0||strResult.compare("FAIL")==0)
		{
			return nErrCode;
		}
		if (strResult.compare("PASS")==0)
		{
			break;
		}
		Sleep(500);
	}
	return 0;
}