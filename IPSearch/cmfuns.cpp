#include "stdafx.h"
#include "cmfuns.h"
/*by lanshh 2015-06-18*/
/**************************************************************
* CHAR:0--------9  A-------F    a---------f
* HEX :30      39  41     46   61--------66
*
***************************************************************/
#ifdef _UNICODE
    #define IsNumberString  IsNumberStringW
    #define IsHexString     IsHexStringW
#else
    #define IsNumberString  IsNumberStringA
    #define IsHexString     IsHexStringA
#endif
BOOL IsNumberStringW(const wchar_t *str)
{
    int     nIndex;
    int     nLen;
    if(NULL == str){
        return FALSE;
    }
    nLen    = _tcslen( str );
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = _tcsspn( str, L"0123456789");
    return ( nLen == nIndex ) ? TRUE : FALSE;
}
BOOL IsNumberStringA(const char *str)
{
    int     nIndex;
    int     nLen;
    if(NULL == str){
        return FALSE;
    }
    nLen    = strlen( str );
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = strspn( str, "0123456789" );
    return ( nLen == nIndex ) ? TRUE : FALSE;
}
BOOL IsHexStringW(const wchar_t *str)
{
    int         nIndex;
    int         nLen;
    if(NULL == str){
        return FALSE;
    }    
    nLen    = _tcslen( str );
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = _tcsspn( str, L"0123456789ABCDEFabcdef");
    return ( nLen == nIndex ) ? TRUE : FALSE;
}
BOOL IsHexStringA(const char *str)
{
    int         nIndex;
    int         nLen;
    if(NULL == str){
        return FALSE;
    }    
    nLen    = strlen( str );
    if ( 0 == nLen ) {
        return FALSE;
    }
    nIndex = strspn( str, "0123456789ABCDEFabcdef");
    return ( nLen == nIndex ) ? TRUE : FALSE;
}
DWORD intToHexLen(DWORD dwValue)
{
    DWORD       dwLen;
    dwLen       = 0;
    dwValue     /=16;
    while(dwValue){
        dwLen ++;
        dwValue /=16;
    }
    return dwLen;
}
DWORD intToDecLen(DWORD dwValue)
{
    DWORD       dwLen;
    dwLen       = 0;
    dwValue     /=10;
    while(dwValue){
        dwLen ++;
        dwValue /=10;
    }
    return dwLen;
}
BOOL IntStrIncrease(TCHAR *strValue,int spn,TCHAR *strNEW,int len)
{
    TCHAR       szChar;
    TCHAR       newChar;
    TCHAR       *STR;
    BOOL        isINC;
    int         i;
    int         nLen;
    if (!IsNumberString(strValue)||spn > 10 ||spn < -10) {
		return FALSE;
	}
    if(0 > spn){
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
	nLen    = _tcslen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW,TEXT("%s"),strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
    	for (i = nLen-1; i >= 0; i-- ) {
    		szChar  = STR[i];
            newChar = szChar + spn;
    		if (newChar <=_T('9') ) {
    			szChar  = newChar;
                spn     =  0;
    		} else if (newChar > _T('9')) {
    		    szChar  = newChar - 10;
                STR[i]  = szChar;
                spn     =  1;
                continue;
            }
            STR[i] = szChar;
            break;
        }
    } else {
        for (i = nLen-1; i >= 0; i-- ) {
    		szChar  = STR[i];
            newChar = szChar - spn;
    		if (newChar >=_T('0')) {
    			szChar  = newChar;
                spn     =  0;
    		} else if ( newChar < _T('0') ){
    		    szChar  = newChar + 10 ;
                STR[i]  = szChar;
                spn     =  1;
                continue;
            }
            STR[i] = szChar;
            break;
        }
    }
    return TRUE;
}
/**************************************************************
* CHAR:0--------9  A-------F    a---------f
* HEX :30      39  41     46   61--------66
*
***************************************************************/
/*
*for plus result calculating,the input must in range 0 ~ 31
*
*
*/
static BOOL HEXTOCHARP(int hex,int *out)
{
    if(hex < 10){                   /*0 ---- 9 */
        *out = hex + TEXT('0');
        return FALSE;
    }else if (hex < 16 ){           /*10----15*/
        *out = hex - 10 + TEXT('A');
        return FALSE;
    }else if (hex < 26){
        *out = hex - 16 + TEXT('0');/*16----25 carry out */
        return TRUE;
    }else if (hex < 32){
        *out = hex - 26 + TEXT('A');/*26----31 carry out */
        return TRUE;
    }
    return FALSE;
}
/*
*for minus result calculating,the input must in range -15 ~ +15
*
*
*/
static BOOL HEXTOCHARM(int hex,int *out)
{
    if(hex >= 0 ){
        if(hex < 10){                   /*0 ---- 9*/
            *out = hex + TEXT('0');
        }else if (hex < 16 ){           /*10----15*/
            *out = hex - 10 + TEXT('A');
        }
        return FALSE;   //hex > 0 ,just the result
    } else {
        if(hex < -6 ){  // -7 ---- -15 16 + hex = 9 8 7 6 5 4 3 2 1 0 
            *out = 16 + hex + TEXT('0');
        } else {        // -1 ---- -6  16 + hex = f e d c b a 
            *out = 1 + hex + TEXT('F');
        }
        return TRUE;    //hex < 0 ,need borrow 
    }
}
BOOL HexStrIncreaseA(char *strValue,int spn,char *strNEW,int len)
{
    int         szChar;
    char        *STR;
    int         newChar;
    BOOL        isINC;
    int         i;
    int         nLen;
    if (!IsHexStringA(strValue)||spn > 16 ||spn < -16 ) {  //here must ensure input is hex string and the span is less than 16,
                                                           //otherwise  will get wrong result 
        return FALSE;
    }
    if(0 > spn) {
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
    nLen = strlen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        sprintf(strNEW, "%s" ,strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
        for (i = nLen - 1;i >= 0;i-- ) {
            szChar  = STR[i];
            if(szChar >= '0'&&szChar <= '9'){
                newChar = szChar -  '0' + spn;
            }else if (szChar >= 'A'&&szChar <= 'F'){
                newChar = szChar -  'A' + spn + 10;
            }else if (szChar >= 'a' &&szChar <= 'f' ){
                newChar = szChar - 'a' + spn + 10;
            }
            if(HEXTOCHARP(newChar,&szChar)){    //the input of HEXTOCHARP range is from 0 + 0 to 15 + 16
                                                
                STR[i]  = szChar;
                spn     = 1;
                continue;
            } 
            STR[i] =  szChar;
            break;
        }
    } else {
        for (i = nLen-1;i >= 0;i--) {
            szChar  = STR[i];
            if(szChar >= '0' &&szChar <= '9' ){
                newChar = szChar - '0' - spn;
            }else if (szChar >= 'A' &&szChar <= 'F' ){
                newChar = szChar - 'A' - spn + 10;
            }else if (szChar >= 'a' &&szChar <= 'f' ){
                newChar = szChar - 'a' - spn + 10;
            }
            if(HEXTOCHARM(newChar,&szChar)){ //the input of HEXTOCHARP range is from -16 to 15
                STR[i]  = szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = szChar;
            break;
    	}
    }
	return TRUE;
}
BOOL HexStrIncrease(TCHAR *strValue,int spn,TCHAR *strNEW,int len)
{
    int         szChar;
    TCHAR       *STR;
    int         newChar;
    BOOL        isINC;
    int         i;
    int         nLen;
    if (!IsHexString(strValue)||spn > 16 ||spn < -16 ) {    //here must ensure input is hex string and the span is less than 16,
                                                            //otherwise  will get wrong result 
        return FALSE;
    }
    if(0 > spn){
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
    nLen = _tcslen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW,TEXT("%s"),strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
        for (i = nLen - 1;i >= 0;i-- ) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') + spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') + spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') + spn + 10;
            }
            if(HEXTOCHARP(newChar,&szChar)){    //the input of HEXTOCHARP range is from 0 + 0 to 15 + 16
                                                
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
        }
    } else {
        for (i = nLen-1;i >= 0;i--) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') - spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') - spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') - spn + 10;
            }
            if(HEXTOCHARM(newChar,&szChar)){ //the input of HEXTOCHARP range is from -16 to 15
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
    	}
    }
	return TRUE;
}
BOOL HexStrIncreaseSkipAlpha(TCHAR *strValue,int spn,TCHAR *strNEW,int len)
{
    int         szChar;
    TCHAR       *STR;
    int         newChar;
    BOOL        isINC;
    int         i;
    int         nLen;
    if ( spn > 16 ||spn < -16 ) {  //here must ensure input is hex string and the span is less than 16,
                                                           //otherwise  will get wrong result 
        return FALSE;
    }
    if(0 > spn){
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
    nLen = _tcslen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW,TEXT("%s"),strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
        for (i = nLen - 1;i >= 0;i-- ) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') + spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') + spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') + spn + 10;
            } else {
                continue;
            }
            if(HEXTOCHARP(newChar,&szChar)){    //the input of HEXTOCHARP range is from 0 + 0 to 15 + 16
                                                
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
        }
    } else {
        for (i = nLen-1;i >= 0;i--) {
            szChar  = STR[i];
            if(szChar >= TEXT('0')&&szChar <= TEXT('9')){
                newChar = szChar -  TEXT('0') - spn;
            }else if (szChar >= TEXT('A')&&szChar <= TEXT('F')){
                newChar = szChar -  TEXT('A') - spn + 10;
            }else if (szChar >= TEXT('a')&&szChar <= TEXT('f')){
                newChar = szChar -  TEXT('a') - spn + 10;
            } else {
                continue;
            }
            if(HEXTOCHARM(newChar,&szChar)){ //the input of HEXTOCHARP range is from -16 to 15
                STR[i]  =(TCHAR)szChar;
                spn     = 1;
                continue;
            } 
            STR[i] = (TCHAR)szChar;
            break;
    	}
    }
	return TRUE;
}

BOOL IntStrIncreaseSkipAlpha(TCHAR *strValue,int spn,TCHAR *strNEW,int len)
{
    TCHAR       szChar;
    TCHAR       newChar;
    TCHAR       *STR;
    BOOL        isINC;
    int         i;
    int         nLen;
    if ( spn > 10 ||spn < -10) {
		return FALSE;
	}
    if(0 > spn){
        spn   = -spn;
        isINC   = FALSE;
    } else if(0 < spn){
        isINC   = TRUE;
    } else {
        return FALSE;
    }
	nLen    = _tcslen(strValue);
    if( NULL != strNEW ){
        if(nLen + 1 > len ){
            return FALSE;
        }
        wsprintf(strNEW,TEXT("%s"),strValue);
        STR = strNEW;
    } else {
        STR = strValue;
    }
    if(isINC) {
    	for (i = nLen-1; i >= 0; i-- ) {
    		szChar  = STR[i];
            if(!isdigit(szChar)){
                continue;
            }
            newChar = szChar + spn;
    		if (newChar <=_T('9') ) {
    			szChar  = newChar;
                spn     =  0;
    		} else if (newChar > _T('9')) {
    		    szChar  = newChar - 10;
                STR[i]  = szChar;
                spn     =  1;
                continue;
            }else {
                continue;
            }
            STR[i] = szChar;
            break;
        }
    } else {
        for (i = nLen-1; i >= 0; i-- ) {
    		szChar  = STR[i];
            if(!isdigit(szChar)){
                continue;
            }
            newChar = szChar - spn;
    		if (newChar >=_T('0')) {
    			szChar  = newChar;
                spn     =  0;
    		} else if ( newChar < _T('0') ){
    		    szChar  = newChar + 10 ;
                STR[i]  = szChar;
                spn     =  1;
                continue;
            }
            STR[i] = szChar;
            break;
        }
    }
    return TRUE;
}
TCHAR ComputeCD(INT64 nImei)
{
    INT64   d;
    int     nTime       = 0;
    INT64   nCD         = 0;  
    while (nImei != 0) {
        d = nImei % 10;
        if(nTime % 2 == 0) {
            d *= 2;
            if(d >= 10) {
                d = d % 10 + d / 10;
            }
        }   
        nCD     += d;   
        nTime++;
        nImei   /= 10;
    }  
    nCD %= 10;
    if(nCD != 0) {
        nCD = 10 - nCD;
    }  
    return (TCHAR)nCD;
}
BOOL CheckImeiStrFt(const TCHAR *strImei)
{
    INT64       int64Imei;
    size_t      nLen;
    if (NULL == strImei) {
        return FALSE;
    }
    nLen    = _tcslen( strImei );
    if (15 == nLen) {
        return FALSE;
    } else if (14 == nLen ) {
        return nLen == _tcsspn( strImei,TEXT("0123456789"));
    }
    return FALSE;
}

BOOL CheckImeiStr(const TCHAR *strImei)
{
    INT64       int64Imei;
    size_t      nLen;
    if (NULL == strImei) {
        return FALSE;
    }
    nLen    = _tcslen( strImei );
    if (15 == nLen) {
        if( nLen == _tcsspn( strImei,TEXT("0123456789"))){
            int64Imei   = _ttoi64(strImei);
            int64Imei   /= 10;
            return  strImei[14] == (ComputeCD(int64Imei) + TEXT('0'));
        }
    } else if (14 == nLen ) {
        return nLen == _tcsspn( strImei,TEXT("0123456789"));
    }
    return FALSE;
}
BOOL CheckMacStr(const TCHAR *strMac)
{
    int         nIndex;
    int         strlen;
    int         nMac;
    if(NULL == strMac ){
        return FALSE;
    }
    if (strMac[1]>='0'&&strMac[1]<='9')
    {
        nMac = strMac[1] - '0';
    }
    else if (strMac[1]>='A'&&strMac[1]<='F')
    {
        nMac = strMac[1] - 'A'+10;
    }
    else if (strMac[1]>='a'&&strMac[1]<='f')
    {
        nMac = strMac[1] - 'a'+10;
    }
    else
    {
        return FALSE;  
    }
    if (nMac%2 != 0)
    {
        return FALSE;
    }
    //if (!(strMac[1]=='0'||strMac[1]=='8'||strMac[1]=='12'))//mac地址的第一个字节的低2bit必为0
    //{
    //    return FALSE;
    //}
    strlen = _tcslen( strMac);
    if ( 12 == strlen ) {
        nIndex = _tcsspn( strMac, _T("0123456789ABCDEFabcdef"));
        return ( strlen == nIndex ) ? TRUE : FALSE;
    }
    return FALSE;
}

