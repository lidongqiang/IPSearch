
// IPSearch.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#define WM_UPDATE_MSG		(WM_USER+111)
#define UPDATE_CONFIG 0
#define UPDATE_WINDOW 1
#define UPDATE_TEST_EXIT 2
#define UPDATE_PROMPT 3
#define UPDATE_SN 4
#define UPDATE_LIST 5
#define UPDATE_TEST_FINISH 6
#define UPDATE_CABLELOSS   7

#define PROMPT_TESTING 1
#define PROMPT_PASS 2
#define PROMPT_FAIL 3
#define PROMPT_EMPTY 4

#define LIST_INFO  1
#define LIST_ERROR 2
#define LIST_EMPTY 3
#define LIST_TIME  4
#define LIST_WARN  5  

#define RF_TEST_PASS 1
#define RF_TEST_FAIL 2
#define RF_TEST_ABORT 3

#define SN_SHOW 1
#define SN_HIDE 2

// CIPSearchApp:
// �йش����ʵ�֣������ IPSearch.cpp
//

class CIPSearchApp : public CWinAppEx
{
public:
	CIPSearchApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CIPSearchApp theApp;