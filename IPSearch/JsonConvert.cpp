#include "stdafx.h"
#include "JsonConvert.h"

//CJsonConvert::CJsonConvert()
//{
//
//}
//
//CJsonConvert::~CJsonConvert()
//{
//
//}

BOOL CJsonConvert::ItemtoJson(std::string strItemName,std::string strItemValue,std::string &strJson)
{
	root.clear();
	root[strItemName] = strItemValue;
	strJson = style_write.write(root);
	return TRUE;
}

BOOL CJsonConvert::ItemtoJson(std::string strItemName1,std::string strItemValue1,
							  std::string strItemName2,std::string strItemValue2,std::string &strJson)
{
	root.clear();
	root[strItemName1] = strItemValue1;
	root[strItemName2] = strItemValue2;
	strJson = style_write.write(root);
	return TRUE;
}

BOOL CJsonConvert::ItemtoJson(std::string strItemName1,std::string strItemValue1, 
							  std::string strItemName2,std::string strItemValue2,
							  std::string strItemName3,std::string strItemValue3,std::string &strJson)
{
	root.clear();
	root[strItemName1] = strItemValue1;
	root[strItemName2] = strItemValue2;
	root[strItemName3] = strItemValue3;
	strJson = style_write.write(root);
	return TRUE;
}

BOOL CJsonConvert::ItemtoJson(std::string strItemName1,std::string strItemValue1, 
							  std::string strItemName2,std::string strItemValue2,
							  std::string strItemName3,std::string strItemValue3,
							  std::string strItemName4,std::string strItemValue4,std::string &strJson)
{
	root.clear();
	root[strItemName1] = strItemValue1;
	root[strItemName2] = strItemValue2;
	root[strItemName3] = strItemValue3;
	root[strItemName4] = strItemValue4;
	strJson = style_write.write(root);
	return TRUE;
}

BOOL CJsonConvert::JsontoItem(std::string strItemName,std::string &strItemValue,std::string strJson)
{
	root.clear();
	if (reader.parse(strJson,root))
	{
		strItemValue = root[strItemName].asString();
	}
	return TRUE;
}

BOOL CJsonConvert::JsontoItem(std::string strItemName1,std::string &strItemValue1, 
							  std::string strItemName2,std::string &strItemValue2, std::string strJson)
{
	root.clear();
	if (reader.parse(strJson,root))
	{
		strItemValue1 = root[strItemName1].asString();
		strItemValue2 = root[strItemName2].asString();
	}
	return TRUE;
}

BOOL CJsonConvert::JsontoItem(std::string strItemName1,std::string &strItemValue1, 
							  std::string strItemName2,std::string &strItemValue2, 
							  std::string strItemName3,std::string &strItemValue3,std::string strJson)
{
	root.clear();
	if (reader.parse(strJson,root))
	{
		strItemValue1 = root[strItemName1].asString();
		strItemValue2 = root[strItemName2].asString();
		strItemValue3 = root[strItemName3].asString();
	}
	return TRUE;
}

BOOL CJsonConvert::JsontoItem(std::string strItemName1,std::string &strItemValue1, 
							  std::string strItemName2,std::string &strItemValue2, 
							  std::string strItemName3,std::string &strItemValue3,
							  std::string strItemName4,std::string &strItemValue4,std::string strJson)
{
	root.clear();
	if (reader.parse(strJson,root))
	{
		strItemValue1 = root[strItemName1].asString();
		strItemValue2 = root[strItemName2].asString();
		strItemValue3 = root[strItemName3].asString();
		strItemValue4 = root[strItemName4].asString();
	}
	return TRUE;
}

BOOL CJsonConvert::JsontoInt(std::string strItemName,int &strItemValue,std::string strJson)
{
	root.clear();
	if (reader.parse(strJson,root))
	{
		strItemValue = root[strItemName].asInt();
	}
	return TRUE;
}