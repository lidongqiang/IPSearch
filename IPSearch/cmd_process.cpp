#include "stdafx.h"
#include <stdio.h>
#include "JsonConvert.h"
#include "cmd_process.h"
#include "c_socket.h"

CMD_PROCESS_T g_cmd_list[] =
{
	{"video_param", set_video_param},
	{"set_value", set_value},
	{"KEY_TEST", key_test},
	{"SDCARD_TEST", sdcard_test},
	{"WIFI_TEST", wifi_test},
	{"PTZ_TEST", ptz_test},
	{"IRCUT_TEST", ircut_test},
	{NULL, NULL},
};

int set_video_param(SOCKET c_sid,char *data)
{
	return 0;
}

int set_value(SOCKET c_sid,char *data)
{
	return 0;
}

/*测试项
	自动流程：
	PC								设备端
	开始测试------------------------>应答
	收到应答，查询测试状态<----------
	-------------------------------->返回测试结果
	收到确认的测试结果，保存<--------
	-------------------------------->保存结果应答

	人工流程：
	pc								设备端
	开始测试------------------------>应答

	人工确认测试结果，保存---------->保存结果应答

*/
int key_test(SOCKET c_sid,char *data)
{
	return 0;
}

int sdcard_test(SOCKET c_sid,char *data)
{
	CJsonConvert strJson;
	std::string strRes,strStat,strResult,strTestItem;
	std::string strMsg;
	int nErrCode=0;
	
	strJson.JsontoItem("RES",strRes,"STATUS",strStat,"RESULT",strResult,"TEST_ITEM",strTestItem,data);
	strJson.JsontoInt("ERR_CODE",nErrCode,data);

	//1.开始测试应答--------wifi卡测试中...（界面更新）
	if (strRes.compare("START")==0)
	{
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}
	//2.查询测试状态应答，解析测试状态，如果为测试中继续查询
	if (strRes.compare("QUERY")==0&&strResult.compare("TESTING")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}

	//3.查询测试状态，当测试状态为成功时，发命令保存测试结果
	if (strRes.compare("QUERY")==0&&strResult.compare("PASS")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","SAVE",strMsg);
		socket_write(c_sid,strMsg);
	}

	//4.保存测试结果应答------------sd卡测试成功

	return 0;
}

int wifi_test(SOCKET c_sid,char *data)
{
	CJsonConvert strJson;
	std::string strRes,strStat,strResult,strTestItem;
	std::string strMsg;

	strJson.JsontoItem("RES",strRes,"STATUS",strStat,"RESULT",strResult,"TEST_ITEM",strTestItem,data);

	//1.开始测试应答--------sd卡测试中..
	if (strRes.compare("START")==0)
	{
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM","test_item","CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}
	//2.查询测试状态应答，解析测试状态，如果为测试中继续查询
	if (strRes.compare("QUERY")==0&&strResult.compare("TESTING")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}

	//3.查询测试状态，当测试状态为成功时，发命令保存测试结果
	if (strRes.compare("QUERY")==0&&strResult.compare("PASS")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","SAVE",strMsg);
		socket_write(c_sid,strMsg);
	}

	//4.保存测试结果应答------------sd卡测试成功
	return 0;
}

int ptz_test(SOCKET c_sid,char *data)
{
	return 0;
}

int ircut_test(SOCKET c_sid,char *data)
{
	return 0;
}