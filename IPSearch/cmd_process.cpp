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

/*������
	�Զ����̣�
	PC								�豸��
	��ʼ����------------------------>Ӧ��
	�յ�Ӧ�𣬲�ѯ����״̬<----------
	-------------------------------->���ز��Խ��
	�յ�ȷ�ϵĲ��Խ��������<--------
	-------------------------------->������Ӧ��

	�˹����̣�
	pc								�豸��
	��ʼ����------------------------>Ӧ��

	�˹�ȷ�ϲ��Խ��������---------->������Ӧ��

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

	//1.��ʼ����Ӧ��--------wifi��������...��������£�
	if (strRes.compare("START")==0)
	{
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}
	//2.��ѯ����״̬Ӧ�𣬽�������״̬�����Ϊ�����м�����ѯ
	if (strRes.compare("QUERY")==0&&strResult.compare("TESTING")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}

	//3.��ѯ����״̬��������״̬Ϊ�ɹ�ʱ�����������Խ��
	if (strRes.compare("QUERY")==0&&strResult.compare("PASS")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","SAVE",strMsg);
		socket_write(c_sid,strMsg);
	}

	//4.������Խ��Ӧ��------------sd�����Գɹ�

	return 0;
}

int wifi_test(SOCKET c_sid,char *data)
{
	CJsonConvert strJson;
	std::string strRes,strStat,strResult,strTestItem;
	std::string strMsg;

	strJson.JsontoItem("RES",strRes,"STATUS",strStat,"RESULT",strResult,"TEST_ITEM",strTestItem,data);

	//1.��ʼ����Ӧ��--------sd��������..
	if (strRes.compare("START")==0)
	{
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM","test_item","CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}
	//2.��ѯ����״̬Ӧ�𣬽�������״̬�����Ϊ�����м�����ѯ
	if (strRes.compare("QUERY")==0&&strResult.compare("TESTING")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","QUERY",strMsg);
		socket_write(c_sid,strMsg);
	}

	//3.��ѯ����״̬��������״̬Ϊ�ɹ�ʱ�����������Խ��
	if (strRes.compare("QUERY")==0&&strResult.compare("PASS")==0)
	{
		Sleep(300);
		strJson.ItemtoJson("TYPE","CMD","TEST_ITEM",strTestItem,"CMD","SAVE",strMsg);
		socket_write(c_sid,strMsg);
	}

	//4.������Խ��Ӧ��------------sd�����Գɹ�
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