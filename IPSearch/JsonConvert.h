#ifndef JSONCONVERT_H
#define JSONCONVERT_H
#include <json/json.h>
#include <json/reader.h>
#include <iostream>
using namespace std;

class CJsonConvert
{
private:
	Json::Reader reader;
	Json::Value root;
	Json::StyledWriter style_write; 
public:
	BOOL ItemtoJson(std::string strItemName,std::string strItemValue,std::string &strJson);
	BOOL ItemtoJson(std::string strItemName1,std::string strItemValue1,std::string strItemName2,std::string strItemValue2,std::string &strJson);
	BOOL ItemtoJson(std::string strItemName1,std::string strItemValue1,
					std::string strItemName2,std::string strItemValue2,
					std::string strItemName3,std::string strItemValue3,std::string &strJson);
	BOOL ItemtoJson(std::string strItemName1,std::string strItemValue1,
					std::string strItemName2,std::string strItemValue2,
					std::string strItemName3,std::string strItemValue3,
					std::string strItemName4,std::string strItemValue4,std::string &strJson);

	BOOL JsontoInt(std::string strItemName,int &strItemValue,std::string strJson);
	BOOL JsontoItem(std::string strItemName,std::string &strItemValue,std::string strJson);
	BOOL JsontoItem(std::string strItemName1,std::string &strItemValue1,std::string strItemName2,std::string &strItemValue2,std::string strJson);
	BOOL JsontoItem(std::string strItemName1,std::string &strItemValue1,
					std::string strItemName2,std::string &strItemValue2,
					std::string strItemName3,std::string &strItemValue3,std::string strJson);

	BOOL JsontoItem(std::string strItemName1,std::string &strItemValue1,
					std::string strItemName2,std::string &strItemValue2,
					std::string strItemName3,std::string &strItemValue3,
					std::string strItemName4,std::string &strItemValue4,std::string strJson);
};






#endif