#pragma once
#include "CGridListCtrlEx.h"

class CGridListCtrlColor :public CGridListCtrlEx
{
public:
	CGridListCtrlColor(void);
	~CGridListCtrlColor(void);
	VOID SetRowColor(int nRow,COLORREF color);
	VOID DelRowColor(int nRow);
	VOID SetColFont(int nCol,LOGFONT &font);
	VOID DelColFont(int nCol);
	VOID SetRowHeight(LONG height);
	VOID SetHeaderCtrlFont(LOGFONT &font);
	virtual bool OnDisplayRowColor(int nRow, COLORREF& textColor, COLORREF& backColor);
	virtual bool OnDisplayCellFont(int nRow, int nCol, LOGFONT& font);
	BOOL DeleteItem( int nItem );
	BOOL DeleteAllItems();
private:
	map<int,COLORREF> m_mapRowColor;
	map<int,LOGFONT> m_mapColumnFont;
	CFont m_headerCtrlFont;
};
