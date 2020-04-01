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
// Separator.cpp : implementation file
//

#include "stdafx.h"
#include "Separator.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSeparator::CSeparator()
{
}

CSeparator::~CSeparator()
{
}

BEGIN_MESSAGE_MAP(CSeparator, CStatic)
	ON_WM_PAINT()
    ON_MESSAGE(WM_PRINT, OnPrint)
    ON_MESSAGE(WM_PRINTCLIENT, OnPrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeparator message handlers

void CSeparator::Create(DWORD dwStyle, CRect rc, CWnd* pParent, int nID)
{
    CStatic::Create(NULL, dwStyle, rc, pParent, nID);
}

void CSeparator::Print(CDC* pDC, DWORD dwFlags)
{
    CRect rect;
    GetClientRect(&rect);

    bool bParentIsRealDialog = false;
   // if (dynamic_cast<CDialog*>(GetParent()))
    {
        // is this a property page
     //   if (!dynamic_cast<CPropertyPage*>(GetParent()))
            bParentIsRealDialog = true;
    }

    if (!g_xpStyle.IsAppThemed() || bParentIsRealDialog)
        pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

    bool bHoriz = true;
    if (rect.Height() > rect.Width())
        bHoriz = false;

    if (g_xpStyle.IsAppThemed())
    {
        int iPartId = bHoriz ? TP_SEPARATORVERT : TP_SEPARATOR;
        HTHEME hTheme = g_xpStyle.OpenThemeData(GetSafeHwnd(), L"TOOLBAR");
        g_xpStyle.DrawThemeBackground(hTheme, pDC->GetSafeHdc(), iPartId, XPS_TB_NORMAL, &rect, NULL);
        g_xpStyle.CloseThemeData(hTheme);
    }
    else
    {
        CPoint pt1, pt2, offset;
        int nMargin = 2;

        if (bHoriz)
        {
            pt1.x = rect.left + nMargin;
            pt1.y = rect.top +(rect.Height() - 1)/2;
            pt2.x = pt1.x;
            pt2.y = pt1.y + 1;
            offset.x = rect.Width() - 2*nMargin;
            offset.y = 0;
        }
        else
        {
            pt1.x = rect.left +(rect.Width() - 1)/2;
            pt1.y = rect.top + nMargin;
            pt2.x = pt1.x + 1;
            pt2.y = pt1.y;
            offset.x = 0;
            offset.y = rect.Height() - 2*nMargin;
        }

        COLORREF color = ::GetSysColor(COLOR_3DSHADOW);
        CPen pen(PS_SOLID, 1, color); 
        CPen *oldPen = pDC->SelectObject(&pen);
        pDC->MoveTo(pt1);
        pt1 += offset;
        pDC->LineTo(pt1);

        COLORREF color2 = ::GetSysColor(COLOR_3DHIGHLIGHT);
        CPen pen2(PS_SOLID, 1, color2);
        pDC->SelectObject(&pen2);
        pDC->MoveTo(pt2);
        pt2 += offset;
        pDC->LineTo(pt2);

        pDC->SelectObject(oldPen);
    }
}

void CSeparator::OnPaint() 
{
	// device context for painting
    CPaintDC dc(this);
    Print(&dc, 0);
}

LRESULT CSeparator::OnPrint(WPARAM wParam, LPARAM lParam)
{
    Print(CDC::FromHandle((HDC)wParam), (DWORD)lParam);
    return 0;
}

BOOL CSeparator::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.lpszName = NULL;
    cs.dwExStyle |= WS_EX_TRANSPARENT;
    return CStatic::PreCreateWindow(cs);
}

void CSeparator::PreSubclassWindow()
{
    SetWindowText(_T(""));
    CStatic::PreSubclassWindow();
    ModifyStyleEx(0, WS_EX_TRANSPARENT);
}
