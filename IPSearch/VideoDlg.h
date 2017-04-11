#pragma once


// CVideoDlg dialog
#include <vlc/vlc.h>
#include "BitConfigDlg.h"
class CVideoDlg : public CDialog
{
	DECLARE_DYNAMIC(CVideoDlg)

public:
	CVideoDlg(CString strUrl,CWnd* pParent = NULL);   // standard constructor
	virtual ~CVideoDlg();

// Dialog Data
	enum { IDD = IDD_VIDEO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	libvlc_media_t *m_vlcMedia;
	libvlc_media_player_t *m_vlcMplay;
	libvlc_instance_t *m_vlcInst;
	LONG m_nWidth;
	LONG m_nHeight;
	LONG m_nDlgWidth ;
	LONG m_nDlgHeight;
	float m_Multiple_width;
	float m_Mutiple_heith;
	bool change_flag;
	CString m_strUrl;
public:
	afx_msg void OnBnClickedBtnPlay();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ReSize(int nID);
	afx_msg void OnBnClickedBtnConfig();
};
