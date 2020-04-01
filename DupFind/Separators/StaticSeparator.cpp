// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
//
//  Copyright © 2020 Nikolay Avrionov
//  This file is part of project: DupFind
//
// StaticSeparator.cpp : implementation file
//

#include "stdafx.h"
#include "StaticSeparator.h"
#include "VisualStylesXP.h"

/////////////////////////////////////////////////////////////////////////////
// CStaticSeparator

CStaticSeparator::CStaticSeparator()
{
}

CStaticSeparator::~CStaticSeparator()
{
}


BEGIN_MESSAGE_MAP(CStaticSeparator, CStatic)
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_WM_DESTROY()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticSeparator message handlers

void CStaticSeparator::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();

    // set bold font;
    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    logFont.lfWeight = FW_BOLD;
    m_font.CreateFontIndirect(&logFont);
    SetFont(&m_font);
    
    OnUpdateContents();
}

void CStaticSeparator::OnUpdateContents()
{
    if (m_ctrlSeparator.GetSafeHwnd())
        m_ctrlSeparator.DestroyWindow();
    
    CRect rect;
    GetClientRect(&rect);
    m_nOldWidth = rect.Width();

    CDC *pDC = GetDC();
    CFont *pFont = pDC->SelectObject(GetFont());
    CString strText;
    GetWindowText(strText);
    if (strText != "")
        strText += " ";
    CSize size = pDC->GetTextExtent(strText);
    pDC->SelectObject(pFont);
    ReleaseDC(pDC);

    rect.left += size.cx;

    if (g_xpStyle.IsAppThemed())
        rect.top += size.cy/3;

    m_ctrlSeparator.Create(WS_CHILD|WS_VISIBLE, rect, this);
}

void CStaticSeparator::OnSize( UINT nType, int cx, int cy )
{
    CRect rect;
    m_ctrlSeparator.GetClientRect(&rect);
    m_ctrlSeparator.SetWindowPos(NULL, 0, 0, rect.Width() + cx - m_nOldWidth, cy, SWP_NOMOVE|SWP_NOZORDER);
    m_nOldWidth = cx;
    CStatic::OnSize(nType, cx, cy);
}

void CStaticSeparator::OnMove(int x, int y)
{
    CStatic::OnMove(x, y);
    OnUpdateContents();
}

void CStaticSeparator::OnDestroy()
{
    m_font.DeleteObject();
    m_ctrlSeparator.DestroyWindow();
    CStatic::OnDestroy();
}

LRESULT CStaticSeparator::OnSetText(WPARAM wParam, LPARAM lParam)
{
    Default();
    OnUpdateContents();
    RedrawWindow();
    return TRUE;
}

