#ifndef _CMFUNS_H_
#define _CMFUNS_H_
BOOL            IsNumberStringW         (const wchar_t *);
BOOL            IsNumberStringA         (const char *);
BOOL            IsHexStringW            (const wchar_t *);
BOOL            IsHexStringA            (const char  *);
DWORD           intToHexLen             (DWORD );
DWORD           intToDecLen             (DWORD );
BOOL            IntStrIncrease          (TCHAR *    ,int spn = 1,TCHAR *strNEW = NULL,int len = 0);
BOOL            HexStrIncrease          (TCHAR *    ,int spn = 1,TCHAR *strNEW = NULL,int len = 0);
BOOL            HexStrIncreaseA         (char *     ,int spn = 1,char  *strNEW = NULL,int len = 0);
BOOL            HexStrIncreaseSkipAlpha (TCHAR *    ,int spn = 1,TCHAR *strNEW = NULL,int len = 0);
BOOL            IntStrIncreaseSkipAlpha (TCHAR *    ,int spn = 1,TCHAR *strNEW = NULL,int len = 0);
TCHAR           ComputeCD               (INT64 nImei);
BOOL            CheckImeiStr            (const TCHAR *strImei);
BOOL            CheckMacStr             (const TCHAR *strMac);
BOOL            CheckImeiStrFt          (const TCHAR *strImei);
#endif