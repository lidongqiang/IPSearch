/******************************************************************/
/*	Copyright (C)  ROCK-CHIPS FUZHOU . All Rights Reserved. 	  */
/*******************************************************************
File    :   RKUpgradeDll.h
Desc    :   Rockchip upgrade tool DLL
Author  :   Steven Chen
Date    :   2009-12-30
Notes   :   

Revision 1.00  2009-12-30 10:18:54	Steven Chen
********************************************************************/

#ifndef __RKUPGRADEDLL_H__
#define __RKUPGRADEDLL_H__

#ifdef RKUPGRADEDLL_EXPORTS
#define RKUPGRADE_API __declspec(dllexport)
#else
#define RKUPGRADE_API __declspec(dllimport)
#endif

/***************************************Const Declaration****************************************/
#define MAX_DEVICE				8
/****************************************Type Declaration****************************************/


#pragma pack(1)
// Struct of initialize parameter
typedef struct _INIT_DEV_INFO
{	
    BOOL	bScan4FsUsb;				// Scan for full speed usb device
    USHORT	usRockusbVid;
    USHORT	usRockusbPid;
    USHORT	usRockMscVid;
    USHORT	usRockMscPid;
    UINT	uiRockusbTimeout;
    UINT	uiRockMscTimeout;
    UINT	emSupportDevice;
}INIT_DEV_INFO,PINIT_DEV_INFO;

typedef struct _INIT_LOG_INFO_W
{
    BOOL bLogEnable;
    LPWSTR lpszLogPathName;
}INIT_LOG_INFO_W,PINIT_LOG_INFO_W;
typedef struct _INIT_LOG_INFO_A
{
    BOOL bLogEnable;
    LPSTR lpszLogPathName;
}INIT_LOG_INFO_A,PINIT_LOG_INFO_A;
#ifdef _UNICODE
	#define INIT_LOG_INFO INIT_LOG_INFO_W
	#define PINIT_LOG_INFO PINIT_LOG_INFO_W
#else
	#define INIT_LOG_INFO INIT_LOG_INFO_A
	#define PINIT_LOG_INFO PINIT_LOG_INFO_A
#endif

typedef struct _INIT_CALLBACK_INFO
{
    LPVOID pUpgradeStepPromptProc;	// Prompt certain step status is started, passed or failed.
    LPVOID pProgressPromptProc;		// Prompt progress of certain operation which may take long time.
}INIT_CALLBACK_INFO,PINIT_CALLBACK_INFO;

// Struct of device information
typedef struct _STRUCT_DEVICE_DESC_W
{
    USHORT	usVid;
    USHORT	usPid;
    WCHAR	szDrive;
    USHORT	usbcdUsb;
    DWORD	dwDeviceInstance;
    WCHAR   szLinkName[MAX_PATH];
    WCHAR	szLayer[MAX_PATH];
    UINT	emUsbType;
    UINT	emDeviceType;
	BOOL	bUsb20;
}STRUCT_DEVICE_DESC_W,*PSTRUCT_DEVICE_DESC_W;
typedef struct _STRUCT_DEVICE_DESC_A
{
    USHORT	usVid;
    USHORT	usPid;
    CHAR	szDrive;
    USHORT	usbcdUsb;
    DWORD	dwDeviceInstance;
    CHAR    szLinkName[MAX_PATH];
    CHAR	szLayer[MAX_PATH];
    UINT	emUsbType;
    UINT	emDeviceType;
	BOOL	bUsb20;
}STRUCT_DEVICE_DESC_A,*PSTRUCT_DEVICE_DESC_A;
#ifdef _UNICODE
	#define STRUCT_DEVICE_DESC STRUCT_DEVICE_DESC_W
	#define PSTRUCT_DEVICE_DESC PSTRUCT_DEVICE_DESC_W
#else
	#define STRUCT_DEVICE_DESC STRUCT_DEVICE_DESC_A
	#define PSTRUCT_DEVICE_DESC PSTRUCT_DEVICE_DESC_A
#endif
typedef struct  _STRUCT_DEVICESET_W
{
	BYTE nDevice;
	STRUCT_DEVICE_DESC_W deviceSet[MAX_DEVICE];
	BOOL bRun[MAX_DEVICE];
	BOOL bConnected[MAX_DEVICE];
	CHAR nNextPos;
}STRUCT_DEVICESET_W,*PSTRUCT_DEVICESET_W;
typedef struct  _STRUCT_DEVICESET_A
{
	BYTE nDevice;
	STRUCT_DEVICE_DESC_A deviceSet[MAX_DEVICE];
	BOOL bRun[MAX_DEVICE];
	BOOL bConnected[MAX_DEVICE];
	CHAR nNextPos;
}STRUCT_DEVICESET_A,*PSTRUCT_DEVICESET_A;
#ifdef _UNICODE
	#define STRUCT_DEVICESET STRUCT_DEVICESET_W
	#define PSTRUCT_DEVICESET PSTRUCT_DEVICESET_W
#else
	#define STRUCT_DEVICESET STRUCT_DEVICESET_A
	#define PSTRUCT_DEVICESET PSTRUCT_DEVICESET_A
#endif

typedef struct _STRUCT_UPGRADE_PARAM_A 
{
	BOOL bEnableFormat; //format user disk  
	BOOL bEnableCopyData;//copy file or dir to user disk 
	BOOL bEnableFormatDataDisk;//format data disk  <android not support>
	BOOL bEnableCopyDataDiskData;//copy file or dir to data disk <android not support>
	LPSTR lpszDiskVolume;//user disk volume,can be NULL									  
	LPSTR lpszDataPath;//path of file|dir copying to user disk,can be NULL
	LPSTR lpszDataDiskVolume;//data disk volume,can be NULL
	LPSTR lpszDataDiskPath;//path of file|dir copying to data disk,can be NULL
	UINT  uiMiscModifyFlag;//0:not change misc.img,1:wipe_all 2:wipe_data(only support android)
	BOOL	bMscReset;//true:after copying data,device reset automatically,default is false
	BOOL	bResetAfterUpgrade;//true:after finishing upgrade,device reset(default),false:not to reset device
	USHORT usIgnoreIDBUpgradeFlag;//0x5AA5:upgrade firmware without upgrading idblock only in upgrade function
}STRUCT_UPGRADE_PARAM_A,*PSTRUCT_UPGRADE_PARAM_A;

typedef struct _STRUCT_UPGRADE_PARAM_W 
{
	BOOL bEnableFormat; 
	BOOL bEnableCopyData;
	BOOL bEnableFormatDataDisk;
	BOOL bEnableCopyDataDiskData;
	LPWSTR lpszDiskVolume;									  
	LPWSTR lpszDataPath;
	LPWSTR lpszDataDiskVolume;
	LPWSTR lpszDataDiskPath;
	UINT  uiMiscModifyFlag;
	BOOL	bMscReset;
	BOOL	bResetAfterUpgrade;
	USHORT usIgnoreIDBUpgradeFlag;
}STRUCT_UPGRADE_PARAM_W,*PSTRUCT_UPGRADE_PARAM_W;

typedef struct _STRUCT_SEC3_DATA
{
	PBYTE pSn;
	USHORT usSnSize;
	PBYTE pUid;
	BYTE  btUidSize;
	PBYTE pImei;
	BYTE  btImeiSize;
	PBYTE pBt;
	BYTE  btBlueToothSize;
	PBYTE pMac;
	BYTE  btMacSize;
	PBYTE pWifi;
	BYTE  btWifiSize;
	PBYTE pReserved;
	BYTE  btReservedSize;
}STRUCT_SEC3_DATA,*PSTRUCT_SEC3_DATA;

#ifdef _UNICODE
#define STRUCT_UPGRADE_PARAM STRUCT_UPGRADE_PARAM_W
#define PSTRUCT_UPGRADE_PARAM PSTRUCT_UPGRADE_PARAM_W
#else
#define STRUCT_UPGRADE_PARAM STRUCT_UPGRADE_PARAM_A
#define PSTRUCT_UPGRADE_PARAM PSTRUCT_UPGRADE_PARAM_A
#endif

// Struct of upgrade information
typedef struct _MD_RESULT
{
	BOOL  bAllFail;//True:all devices failed,False:some devices failed
	DWORD dwEachLayer[MAX_DEVICE];//each device layer value
    BOOL  bEachResult[MAX_DEVICE];//True:Succeed False:Fail
}MD_RESULT,*PMD_RESULT;

//struct of provisioning data
typedef struct _PROVISIONING_DATA 
{
	USHORT id;
	USHORT flag;
	USHORT size;
	USHORT reserved;
	BYTE   data[1];
}PROVISIONING_DATA,*PPROVISIONING_DATA;
#pragma pack()
/****************************************Function Declaration************************************/
/*----------------------------------------------------------------------
Name    :   RK_Initialize
Desc    :   Initialize global variables
Params  :   (IN)InitDevInfo:			Device info to be initialized
            (IN)InitLogInfo:			Log info to be initialized
            (IN)InitCallbackInfo		Callback function info to be initialized
Return  :   TRUE:						SUCCESSED
            FALSE:						FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_InitializeW(INIT_DEV_INFO InitDevInfo, 
										   INIT_LOG_INFO_W InitLogInfo,
										   INIT_CALLBACK_INFO InitCallbackInfo);
RKUPGRADE_API BOOL _stdcall RK_InitializeA(INIT_DEV_INFO InitDevInfo, 
										   INIT_LOG_INFO_A InitLogInfo,
										   INIT_CALLBACK_INFO InitCallbackInfo);
#ifdef _UNICODE
#define  RK_Initialize RK_InitializeW
#else
#define  RK_Initialize RK_InitializeA
#endif

/*----------------------------------------------------------------------
Name    :   RK_SetFirmware
Desc    :   Set and analyze firmware
Params  :   (IN)lpszFirmwarePathName:	Image file to be upgraded
            (IN)bCheckFw:true means check firmware it takes much time, default do not check the firmware
Return  :   TRUE:						SUCCESSED
            FALSE:						FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_SetFirmwareW(LPCWSTR lpszFirmwarePathName,BOOL bCheckFw=FALSE);
RKUPGRADE_API BOOL _stdcall RK_SetFirmwareA(LPCSTR lpszFirmwarePathName,BOOL bCheckFw=FALSE);
#ifdef _UNICODE
#define  RK_SetFirmware RK_SetFirmwareW
#else
#define  RK_SetFirmware RK_SetFirmwareA
#endif
/*----------------------------------------------------------------------
Name    :   RK_Uninitialize
Desc    :   Uninitialize global variables
Params  :   NONE
Return  :   TRUE:					SUCCESSED
            FALSE:					FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_Uninitialize();

/*----------------------------------------------------------------------
Name    :   RK_ScanDevice
Desc    :   Scan for the specified rockusb type(ENUM_RKUSB_TYPE)
Params  :   (OUT)nDeviceCounts: receive the count of connected device
            (OUT)bExistMsc: judge if existing msc device in the scanning devices	
Return  :   TRUE:            SUCCESSED
            FALSE:           FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ScanDevice(UINT &nDeviceCounts,BOOL &bExistMsc,BOOL &bExistAdb);

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceInfo
Desc    :   Get all of connected device layer value,calling after RK_ScanDevice
Params  :   (OUT)pLayerArray: save all layer value,the buffer is equal to connected devices count
			(OUT)pUsbTypeArray: save all usb type
			(OUT)pDeviceTypeArray: save all device type
            (IN)nArrayLen: the length of array	
Return  :   Actually the count copying to array
            
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API int _stdcall RK_GetDeviceInfo(PDWORD pLayerArray,PDWORD pUsbTypeArray,PDWORD pDeviceTypeArray,int nArrayLen);




/*----------------------------------------------------------------------
Name    :   RK_EraseFlash
Desc    :   Erase flash
Params  :	(IN)dwLayer:device layer value,0:default first device in the scan result,other:specified device's layer value
Return  :   TRUE:					SUCCESSED
            FALSE:					FAILED
Notes   :   Though the callback function, we can retrieve the progress of erase operation.
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_EraseFlash(DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_Upgrade
Desc    :   Upgrade device
Params  :   (IN)upgradeParam:  see the definition
            (IN)dwLayer:device layer value
Return  :   TRUE:             SUCCESSED
            FALSE:            FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_UpgradeW(STRUCT_UPGRADE_PARAM_W &upgradeParam,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_UpgradeA(STRUCT_UPGRADE_PARAM_A &upgradeParam,DWORD dwLayer=0);
#ifdef _UNICODE
#define  RK_Upgrade RK_UpgradeW
#else
#define  RK_Upgrade RK_UpgradeA
#endif

/*----------------------------------------------------------------------
Name    :   RK_Restore
Desc    :   Restore device
Params  :   (IN)upgradeParam:      see the definition
            (IN)dwLayer:    device layer value

Return  :   TRUE:					SUCCESSED
            FALSE:					FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_RestoreW(STRUCT_UPGRADE_PARAM_W &upgradeParam,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_RestoreA(STRUCT_UPGRADE_PARAM_A &upgradeParam,DWORD dwLayer=0);
#ifdef _UNICODE
#define  RK_Restore RK_RestoreW
#else
#define  RK_Restore RK_RestoreA
#endif


/*----------------------------------------------------------------------
Name    :   RK_ReadCustomData
Desc    :   Read Custom Data from device
Params  :   (OUT)pCustomData:			Point to CustomData buffer (Maximum=512 Bytes)
            (OUT)nCustomDataLen:			Length of CustomData
			(OUT)nCustomDataOffset:		Offset of CustomData
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadCustomData(PBYTE pCustomData, INT &nCustomDataLen,INT &nCustomDataOffset,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteCustomData
Desc    :   Write Custom Data to device
Params  :   (IN)pCustomData:        Point to CustomData buffer (Maximum=512 Bytes)
			(IN)nCustomDataOffset:  Offset of CustomData
            (IN)nCustomDataLen:     Length of CustomData
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteCustomData(PBYTE pCustomData, INT nCustomDataOffset,INT nCustomDataLen,DWORD dwLayer=0);


/*----------------------------------------------------------------------
Name    :   RK_ReadSN
Desc    :   Read SN from device
Params  :   (OUT)pSN:			Point to Sn buffer (Maximum=30 Bytes)
            (OUT)nSNLen:		Length of SN
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadSN(PBYTE pSN, INT& nSNLen,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteSN
Desc    :   Write SN to device
Params  :   (IN)pSN:        Point to Sn buffer (Maximum=30 Bytes)
            (IN)nSNLen:     Length of SN
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteSN(PBYTE pSN, INT nSNLen,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadMAC
Desc    :   Read MAC from device
Params  :   (OUT)pMac:			Point to Mac buffer (Maximum=6 Bytes)
            (OUT)nMacLen:		Length of Mac
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadMAC(PBYTE pMac, INT& nMacLen,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteMAC
Desc    :   Write Mac to device
Params  :   (IN)pMac:        Point to Mac buffer (Maximum=6 Bytes)
            (IN)nMacLen:     Length of Mac
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteMAC(PBYTE pMac, INT nMacLen=6,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadWifi
Desc    :   Read Wifi from device
Params  :   (OUT)pWifi:			Point to Wifi buffer (Maximum=6 Bytes)
            (OUT)nWifiLen:		Length of Wifi
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadWifi(PBYTE pWifi, INT& nWifiLen,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteWifi
Desc    :   Write Wifi to device
Params  :   (IN)pWifi:        Point to Wifi buffer (Maximum=6 Bytes)
            (IN)nWifiLen:     Length of Wifi
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteWifi(PBYTE pWifi, INT nWifiLen=6,DWORD dwLayer=0);


/*----------------------------------------------------------------------
Name    :   RK_ReadIMEI
Desc    :   Read IMEI from device
Params  :   (OUT)pImei:			Point to Imei buffer (Maximum=15 Bytes)
            (OUT)nImeiLen:		Length of Imei
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadIMEI(PBYTE pImei, INT& nImeiLen,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteIMEI
Desc    :   Write Imei to device
Params  :   (IN)pImei:        Point to Imei buffer (Maximum=15 Bytes)
            (IN)nImeiLen:     Length of Imei
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteIMEI(PBYTE pImei, INT nImeiLen=15,DWORD dwLayer=0);


/*----------------------------------------------------------------------
Name    :   RK_ReadBT
Desc    :   Read BT from device
Params  :   (OUT)pBT:			Point to Bt buffer (Maximum=6 Bytes)
            (OUT)nBTLen:		Length of Bt
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadBT(PBYTE pBT, INT& nBTLen,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteBT
Desc    :   Write BT to device
Params  :   (IN)pBT:        Point to BT buffer (Maximum=6 Bytes)
            (IN)nBTLen:     Length of BT
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteBT(PBYTE pBT, INT nBTLen=6,DWORD dwLayer=0);


/*----------------------------------------------------------------------
Name    :   RK_ReadAllInfo
Desc    :   Read all info of idb sector 3 
Params  :   (OUT)allInfo:		Include sn,uid,mac,bt and imei 
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadAllInfo(STRUCT_SEC3_DATA &allInfo, DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteAllInfo
Desc    :   Write all info to idb sector 3 
Params  :   (IN)allInfo:	Include sn,uid,mac,bt and imei
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteAllInfo(STRUCT_SEC3_DATA &allInfo,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadBid
Desc    :   Read bid from flash 
Params  :   (OUT)pBid:	this buffer must be larger than 96 Bytes 
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadBid(PBYTE pBid, DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteBid
Desc    :   Write bid to flash
Params  :   (IN)pBid: Bid is a 96 bytes buffer
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteBid(PBYTE pBid,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadVendorInfo
Desc    :   Read vendor info from flash,1 vendor sector = 504 Bytes
Params  :   (IN)sectorOffset:		number of sectors from begin of vendor info
			(IN)sectorCount:		number of sectors to read
			(OUT)pVendorBuffer:		buffer to save vendor info, malloc by caller,504*sectorCount(Bytes) at least
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadVendorInfo(int sectorOffset,int sectorCount,PBYTE pVendorBuffer, DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteVendorInfo
Desc    :   Write vendor info to flash,1 vendor sector = 504 Bytes
Params  :   (IN)sectorOffset:		number of sectors from begin of vendor info
			(IN)sectorCount:		number of sectors to write
			(OUT)pVendorBuffer:		buffer to write vendor info, malloc by caller,504*sectorCount(Bytes) at least
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteVendorInfo(int sectorOffset,int sectorCount,PBYTE pVendorBuffer, DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadProvisioningData
Desc    :   Read provisioning data by id
Params  :   (IN)nID					id 
			(OUT)pDataBuffer:		buffer to save data, malloc by caller
			(IN|OUT)nBufferSize:	in = size of buffer,out = actual data size
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadProvisioningData(USHORT nID,PBYTE pDataBuffer,USHORT &nBufferSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteProvisioningData
Desc    :   Write provisioning data by id
Params  :   (IN)nID					id 
			(IN)pDataBuffer:		buffer to save data, malloc by caller
			(IN)nBufferSize:		size of buffer
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteProvisioningData(USHORT nID,PBYTE pDataBuffer,USHORT nBufferSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadEfuse
Desc    :   Read efuse from device
Params  :   (OUT)pEfuse:			Point to efuse buffer (buffer must be larger than usReadSize)
			(IN)usReadSize			size of efuse data
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadEfuse(PBYTE pEfuse,USHORT usReadSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadKeyHashFromEfuse
Desc    :   Read key hash from efuse
Params  :   (OUT)pKeyHash:			Point to buffer (buffer size >= 32bytes)
			(OUT)usKeyHashSize		size of key hash
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadKeyHashFromEfuse(PBYTE pKeyHash,USHORT &usKeyHashSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteKeyHashToEfuse
Desc    :   Write key hash to efuse
Params  :  
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   get key hash from firmware,call RK_SetFirmware before.
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteKeyHashToEfuse(DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadDataFromEfuse
Desc    :   Read data from efuse
Params  :   (OUT)pBuffer:		save data reading from efuse
			(IN)usPos			position to read
			(IN)usReadSize		size to read
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadDataFromEfuse(PBYTE pBuffer,USHORT usPos,USHORT usReadSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_WriteDataToEfuse
Desc    :   Write data to efuse
Params  :   (OUT)pBuffer:		data writing to efuse
			(IN)usPos			position to write
			(IN)usWriteSize		size to write
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_WriteDataToEfuse(PBYTE pBuffer,USHORT usPos,USHORT usWriteSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadUID
Desc    :   Read UID from device
Params  :   (OUT)pUID:			Point to UID buffer (Maximum=30 Bytes)
            (OUT)nUIDLen:		Length of UID
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadUID(PBYTE pUID, INT& nUIDLen,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadParameterFile
Desc    :   Read paramter file from device
Params  :   (OUT)pParameter:		Point to parameter buffer
            (OUT)nParamSize:		Length of parameter buffer size
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   when nParamSize=-1,in order to get parameter required size save to nParamSize
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadParameterFile(PBYTE pParameter, INT& nParamSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ReadFirmwareParameterFile
Desc    :   Read paramter file from firmware
Params  :   (OUT)pParameter:		Point to parameter buffer
            (OUT)nParamSize:		Length of parameter buffer size
			(IN)dwLayer:device layer value
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   when nParamSize=-1,in order to get parameter required size save to nParamSize
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ReadFirmwareParameterFile(PBYTE pParameter, INT& nParamSize);

/*----------------------------------------------------------------------
Name    :   RK_GetFirmwareDiskSize
Desc    :   Get disk size from firmware
Params  :   (OUT)uiSysDiskSize: System disk size(MB)	
            (OUT)uiDataDiskSize:Data disk size(MB)
Return  :   TRUE:               SUCCESSED
            FALSE:              FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetFirmwareDiskSize(UINT& uiSysDiskSize,UINT& uiDataDiskSize);

/*----------------------------------------------------------------------
Name    :   RK_GetFirmwareVersion
Desc    :   Get version info from firmware
Params  :   (OUT)dwFwVer: Firmware version
            (OUT)dwBootVer:Boot version
Return  :   TRUE:             SUCCESSED
            FALSE:            FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetFirmwareVersion(DWORD &dwFwVer,DWORD &dwBootVer);

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceDiskSize
Desc    :   Get disk size from device
Params  :   (OUT)uiSysDiskSize:     System disk size(MB)
            (OUT)uiDataDiskSize:    Data disk size(MB)
			(IN)dwLayer:device layer value
Return  :   TRUE:                   SUCCESSED
            FALSE:                  FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceDiskSize(UINT& uiSysDiskSize,UINT& uiDataDiskSize,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceVersion
Desc    :   Get version info from device
Params  :   (OUT)dwFwVer: Firmware version
            (OUT)dwBootVer:Boot version
			(IN)dwLayer:device layer value
Return  :   TRUE:              SUCCESSED
            FALSE:             FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceVersion(DWORD &dwFwVer,DWORD &dwBootVer,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceIDBSector
Desc    :   Get sector 3 of IDB from device
Params  :   (OUT)pSec: Sector buffer (512 Bytes assign by caller)
            (IN)dwLayer:device layer value
Return  :   TRUE:              SUCCESSED
            FALSE:             FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceIDBSector(PBYTE pSec,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceUID
Desc    :   Get UID info from device
Params  :   (OUT)pUid: Device UID (30 Bytes assign by caller)
            (IN)dwLayer:device layer value
Return  :   TRUE:              SUCCESSED
            FALSE:             FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceUID(PBYTE pUid,DWORD dwLayer=0);


/*----------------------------------------------------------------------
Name    :   RK_GetDeviceProductModel
Desc    :   Get product model info from device
Params  :   (OUT)pProductModel: product model (32 Bytes assign by caller)
            (IN)dwLayer:device layer value
Return  :   TRUE:              SUCCESSED
            FALSE:             FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceProductModel(PBYTE pProductModel,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceSN
Desc    :   Get product SN from device
Params  :   (OUT)pProductSN: product SN (32 Bytes assign by caller)
            (IN)dwLayer:device layer value
Return  :   TRUE:              SUCCESSED
            FALSE:             FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceSN(PBYTE pProductSN,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_SwitchToRockusb
Desc    :   Switch mass storage device to rockusb device
Params  :   (IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_SwitchToRockusb(DWORD dwSubCode=0,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_AdbToRockusb
Desc    :   Switch adb device to rockusb device
Params  :   (IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
//RKUPGRADE_API BOOL _stdcall RK_AdbToRockusb(DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_SendMscCommand
Desc    :   Send extended command to msc device
Params  :   (IN)dwCmdCode,extended command code
			(OUT)pOutBuffer,malloc by caller in order to saving output data
			(IN)uiOutBufferSize,need to read size of output data
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_SendMscCommand(DWORD dwCmdCode,PBYTE pOutBuffer=NULL,UINT uiOutBufferSize=0,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_RemoveRockMsc
Desc    :   Disconnect Msc device
Params  :   (IN)dwLayer:device layer value
Return  :   TRUE:                       SUCCESSED
            FALSE:                      FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_RemoveRockMsc(DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_ResetRockusb
Desc    :   Reset rockusb device
Params  :   (IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_ResetRockusb(DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_OpenChannel(BYTE *pChannelData,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_WaitDevice(CString strLayer,UINT uiUsbType);
//call rk_setfirmware before calling RK_DownloadBoot 
RKUPGRADE_API BOOL _stdcall RK_DownloadBoot(DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_TestRockusb
Desc    :   test rockusb device
Params  :   (IN)subCode:set subcode to execute different function
			(IN)dwLayer:device layer value
Return  :   TRUE:           SUCCESSED
            FALSE:          FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_TestRockusb(BYTE subCode,DWORD dwLayer=0);

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceDrive
Desc    :   Get drive of msc device
Params  :   (OUT)drive: device drive
Return  :   TRUE:                       SUCCESSED
            FALSE:                      FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceDrive(CHAR &drive,DWORD dwLayer=0);
/*----------------------------------------------------------------------
Name    :   RK_InstallDriver
Desc    :   Install rockusb driver
Params  :   (IN)lpszDriverInfPathName:  Specify the xxx.inf of driver
			(IN)dwLayer:device layer value
Return  :   TRUE:                       SUCCESSED
            FALSE:                      FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_InstallDriverW(LPCWSTR lpszDriverInfPathName);
RKUPGRADE_API BOOL _stdcall RK_InstallDriverA(LPCSTR lpszDriverInfPathName);
#ifdef _UNICODE
#define RK_InstallDriver RK_InstallDriverW
#else
#define RK_InstallDriver RK_InstallDriverA
#endif

/*----------------------------------------------------------------------
Name    :   RK_FormatDisk
Desc    :   Format user disk
Params  :   (IN)lpszVolume:  Specify the volume of user disk
			(IN)dwLayer:device layer value
Return  :   TRUE:                       SUCCESSED
            FALSE:                      FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_FormatDiskW(LPCWSTR lpszVolume,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_FormatDiskA(LPCSTR lpszVolume,DWORD dwLayer=0);
#ifdef _UNICODE
#define RK_FormatDisk RK_FormatDiskW
#else
#define RK_FormatDisk RK_FormatDiskA
#endif


/*----------------------------------------------------------------------
Name    :   RK_CopyData
Desc    :   Copy file/dir to user disk
Params  :   (IN)lpszDataPath:  Specify the path of file/dir
			(IN)dwLayer:device layer value
Return  :   TRUE:                       SUCCESSED
            FALSE:                      FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_CopyDataW(LPCWSTR lpszDataPath,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_CopyDataA(LPCSTR lpszDataPath,DWORD dwLayer=0);
#ifdef _UNICODE
#define RK_CopyData RK_CopyDataW
#else
#define RK_CopyData RK_CopyDataA
#endif

/*----------------------------------------------------------------------
Name    :   RK_IsRockUsbPlugged
Desc    :   Check out whether RockUsb is plugged
Params  :   (OUT)lpszDeviceID: save device id
Return  :   TRUE:                       Found
            FALSE:                      Not found
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_IsRockUsbPluggedW(LPWSTR lpszDeviceID=NULL);
RKUPGRADE_API BOOL _stdcall RK_IsRockUsbPluggedA(LPSTR lpszDeviceID=NULL);
#ifdef _UNICODE
#define RK_IsRockUsbPlugged RK_IsRockUsbPluggedW
#else
#define RK_IsRockUsbPlugged RK_IsRockUsbPluggedA
#endif


/*----------------------------------------------------------------------
Name    :   RK_DownloadFile
Desc    :   Write file to device by lba
Params  :   (IN)lpszFilePath:  Specify the path of file
			(IN)dwOffset: offset address
			(IN)dwLayer:device layer value
Return  :   TRUE:                       SUCCESSED
            FALSE:                      FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_DownloadFileW(LPCWSTR lpszFilePath,DWORD dwOffset,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_DownloadFileA(LPCSTR lpszFilePath,DWORD dwOffset,DWORD dwLayer=0);
#ifdef _UNICODE
#define RK_DownloadFile RK_DownloadFileW
#else
#define RK_DownloadFile RK_DownloadFileA
#endif

/*----------------------------------------------------------------------
Name    :   MD_SwitchToRockusb
Desc    :   Switch mass storage device to rockusb device for multi devices
Params  :   (OUT)pResult:       Contain result of every device
Return  :   TRUE:               SUCCESSED
            FALSE:              FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/

RKUPGRADE_API BOOL _stdcall MD_SwitchToRockusb(PMD_RESULT& pResult);

/*----------------------------------------------------------------------
Name    :   MD_Upgrade
Desc    :   Upgrade for multi devices
Params  :   (OUT)Result:		Contain result of every device
			(IN)bEnableFormat:  Enable format disk
			(IN)bEnableCopyData:Enable copy data to disk
			(IN)lpszDataPath:   Source data directory
            (IN)szDiskVolume:   Disk volume
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall MD_UpgradeW(PMD_RESULT& pResult,STRUCT_UPGRADE_PARAM_W &upgradeParam);
										
RKUPGRADE_API BOOL _stdcall MD_UpgradeA(PMD_RESULT& pResult,STRUCT_UPGRADE_PARAM_A &upgradeParam);
										
#ifdef _UNICODE
#define  MD_Upgrade MD_UpgradeW
#else
#define  MD_Upgrade MD_UpgradeA
#endif

/*----------------------------------------------------------------------
Name    :   MD_Restore
Desc    :   Restore for multi devices
Params  :   (OUT)Result:		Contain result of every device
			(IN)bEnableFormat:  Enable format disk
			(IN)bEnableCopyData:Enable copy data to disk
			(IN)lpszDataPath:   Source data directory
            (IN)szDiskVolume:   Disk volume
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Steven Chen
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall MD_RestoreW(PMD_RESULT& pResult,STRUCT_UPGRADE_PARAM_W &upgradeParam);
										
RKUPGRADE_API BOOL _stdcall MD_RestoreA(PMD_RESULT& pResult,STRUCT_UPGRADE_PARAM_A &upgradeParam);
										
#ifdef _UNICODE
#define  MD_Restore MD_RestoreW
#else
#define  MD_Restore MD_RestoreA
#endif

/*----------------------------------------------------------------------
Name    :   GetLayerString
Desc    :   Convert value of layer to string
Params  :   (IN)dwLayer:		value of layer
			(OUT)pszLayer:		save for layer string (created by caller )
Return  :   TRUE:				SUCCESSED
            FALSE:				FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall GetLayerStringW(DWORD dwLayer,PWCHAR pszLayer);
										
RKUPGRADE_API BOOL _stdcall GetLayerStringA(DWORD dwLayer,PCHAR pszLayer);
										
#ifdef _UNICODE
#define  GetLayerString GetLayerStringW
#else
#define  GetLayerString GetLayerStringA
#endif

/*----------------------------------------------------------------------
Name    :   RK_GetDevicePath
Desc    :   return device path
Params  :   (OUT)pszDevicePath: device path (256 characters assign by caller)
            (IN)dwLayer:device layer value
Return  :   TRUE:              SUCCESSED
            FALSE:             FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDevicePathA(PCHAR pszDevicePath,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_GetDevicePathW(PWCHAR pszDevicePath,DWORD dwLayer=0);
#ifdef _UNICODE
#define  RK_GetDevicePath RK_GetDevicePathW
#else
#define  RK_GetDevicePath RK_GetDevicePathA
#endif

/*----------------------------------------------------------------------
Name    :   RK_GetDeviceLayer
Desc    :   return device layer string
Params  :   (OUT)pszDeviceLayer: device layer string (256 characters assign by caller)
            (IN)dwLayer:device layer value
Return  :   TRUE:              SUCCESSED
            FALSE:             FAILED
Notes   :   
Author  :   Seth
----------------------------------------------------------------------*/
RKUPGRADE_API BOOL _stdcall RK_GetDeviceLayerA(PCHAR pszDeviceLayer,DWORD dwLayer=0);
RKUPGRADE_API BOOL _stdcall RK_GetDeviceLayerW(PWCHAR pszDeviceLayer,DWORD dwLayer=0);
#ifdef _UNICODE
#define  RK_GetDeviceLayer RK_GetDeviceLayerW
#else
#define  RK_GetDeviceLayer RK_GetDeviceLayerA
#endif


#endif


