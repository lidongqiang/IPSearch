#include "StdAfx.h"
#include "GridListCtrlColor.h"

CGridListCtrlColor::CGridListCtrlColor(void)
{
	m_mapRowColor.clear();
	m_mapColumnFont.clear();
}

CGridListCtrlColor::~CGridListCtrlColor(void)
{
	m_mapRowColor.clear();
	m_mapColumnFont.clear();
}
VOID CGridListCtrlColor::SetRowColor(int nRow,COLORREF color)
{
	if (nRow<GetItemCount())
	{
		m_mapRowColor[nRow] = color;
		RedrawItems(nRow,nRow);
	}
}
VOID CGridListCtrlColor::DelRowColor(int nRow)
{
	map<int,COLORREF>::iterator iter;
	iter = m_mapRowColor.find(nRow);
	if (iter!=m_mapRowColor.end())
	{
		m_mapRowColor.erase(iter);
		RedrawItems(nRow,nRow);
		
	}
}
VOID CGridListCtrlColor::SetColFont(int nCol,LOGFONT &font)
{
	if (nCol<GetHeaderCtrl()->GetItemCount())
	{
		m_mapColumnFont[nCol] = font;
		RedrawItems(0,GetItemCount()-1);
	}
}
VOID CGridListCtrlColor::DelColFont(int nCol)
{
	map<int,LOGFONT>::iterator iter;
	iter = m_mapColumnFont.find(nCol);
	if (iter!=m_mapColumnFont.end())
	{
		m_mapColumnFont.erase(iter);
		RedrawItems(0,GetItemCount()-1);
	}
}

bool CGridListCtrlColor::OnDisplayRowColor(int nRow, COLORREF& textColor, COLORREF& backColor)
{
	map<int,COLORREF>::iterator iter;
	iter = m_mapRowColor.find(nRow);
	if (iter!=m_mapRowColor.end())
	{
		backColor = iter->second;
		return true;
	}
	return false;
}
bool CGridListCtrlColor::OnDisplayCellFont(int nRow, int nCol, LOGFONT& font)
{
	map<int,LOGFONT>::iterator iter;
	iter = m_mapColumnFont.find(nCol);
	if (iter!=m_mapColumnFont.end())
	{
		font = iter->second;
		return true;
	}
	return false;
}
BOOL CGridListCtrlColor::DeleteItem( int nItem )
{
	DelRowColor(nItem);
	return CListCtrl::DeleteItem(nItem);
}
BOOL CGridListCtrlColor::DeleteAllItems()
{
	m_mapRowColor.clear();
	return CListCtrl::DeleteAllItems();
}
VOID CGridListCtrlColor::SetRowHeight(LONG height)
{
	LOGFONT logFont;
	CFont *font=NULL,newFont;
	font = GetFont();
	if (!font)
	{
		return;
	}
	font->GetLogFont(&logFont);
	int nLogicPixelsY;
	nLogicPixelsY = GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY);
	logFont.lfHeight = -MulDiv(height, 72, nLogicPixelsY);
	//font->DeleteObject();
	newFont.CreateFontIndirect(&logFont);
	SetFont(&newFont);
	newFont.Detach();
	if ((HFONT)m_headerCtrlFont)
	{
		GetHeaderCtrl()->SetFont(&m_headerCtrlFont);
	}
}
VOID CGridListCtrlColor::SetHeaderCtrlFont(LOGFONT &font)
{
	if ((HFONT)m_headerCtrlFont)
	{
		m_headerCtrlFont.DeleteObject();
	}

	m_headerCtrlFont.CreateFontIndirect(&font);
	GetHeaderCtrl()->SetFont(&m_headerCtrlFont);
}