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
// CheckBoxSeparator.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBoxSeparator.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCheckBoxSeparator::CCheckBoxSeparator()
{
}

CCheckBoxSeparator::~CCheckBoxSeparator()
{
    DeleteObject(m_font);
}

BEGIN_MESSAGE_MAP(CCheckBoxSeparator, CButton)
    ON_WM_MOVE()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckBoxSeparator message handlers

void CCheckBoxSeparator::PreSubclassWindow() 
{
	__super::PreSubclassWindow();

    // set bold font;
    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    logFont.lfWeight = FW_BOLD;
    m_font.CreateFontIndirect(&logFont);
    SetFont(&m_font);

    OnUpdateContents();
    UpdateControlledDlgItems();
}

void CCheckBoxSeparator::SetNextSectionCtrl(CWnd& wnd)
{
    SetNextSectionId(wnd.GetDlgCtrlID());
}

void CCheckBoxSeparator::SetNextSectionId(int nNextId)
{
    CRect rc1;
    GetWindowRect(&rc1);
    GetParent()->ScreenToClient(&rc1);

    CRect rc2;
    if (GetParent()->GetDlgItem(nNextId))
    {
        GetParent()->GetDlgItem(nNextId)->GetWindowRect(&rc2);
        GetParent()->ScreenToClient(&rc2);
    }
    else
    {
        GetParent()->GetClientRect(&rc2);
        rc2.top = rc2.bottom;
    }

    // iterate on children
    CWnd* pChild = GetParent()->GetWindow(GW_CHILD);
    while (pChild)
    {
        // do not touch ourself
        if (pChild == this)
        {
            pChild = pChild->GetWindow(GW_HWNDNEXT);
            continue;
        }

        // get rect
        CRect rc;
        pChild->GetWindowRect(&rc);
        GetParent()->ScreenToClient(&rc);

        // do not touch controls above us
        if (rc.top < rc1.top)
        {
            pChild = pChild->GetWindow(GW_HWNDNEXT);
            continue;
        }

        // save controls above next section end
        if (rc.bottom < rc2.top)
            AddControlledDlgItem(pChild->GetDlgCtrlID());

        // next
        pChild = pChild->GetWindow(GW_HWNDNEXT);
    }
}

void CCheckBoxSeparator::AddControlledDlgItem(int id)
{
    m_ControlledIDs.insert(id);
}

void CCheckBoxSeparator::OnUpdateContents()
{
    if (m_ctrlSeparator.GetSafeHwnd())
        m_ctrlSeparator.DestroyWindow();

    CRect rect;
    GetClientRect(&rect);

    CDC *pDC = GetDC();
    CFont *pFont = pDC->SelectObject(GetFont());
    CString strText;
    GetWindowText(strText);
    strText = " " + strText + " ";
    CSize size = pDC->GetTextExtent(strText);
    pDC->SelectObject(pFont);
    ReleaseDC(pDC);

    // 15 is the checkbox width
    rect.left += 15 + size.cx;

    if (g_xpStyle.IsAppThemed())
        rect.top += size.cy/3;

    m_ctrlSeparator.Create(WS_CHILD|WS_VISIBLE, rect, this);
}

BOOL CCheckBoxSeparator::OnClicked() 
{
    UpdateControlledDlgItems();
    return FALSE;
}

void CCheckBoxSeparator::UpdateControlledDlgItems()
{
    BOOL bEnable = TRUE;
	if (GetCheck() == 0)
        bEnable = FALSE;

    for (set<int>::iterator it = m_ControlledIDs.begin(); it != m_ControlledIDs.end(); it++)
    {
        CWnd *pWnd = GetParent()->GetDlgItem(*it);
        if (pWnd)
            pWnd->EnableWindow(bEnable);
    }
}

void CCheckBoxSeparator::OnMove(int x, int y)
{
    __super::OnMove(x, y);
    OnUpdateContents();
}

LRESULT CCheckBoxSeparator::OnSetText(WPARAM wParam, LPARAM lParam)
{
    Default();
    OnUpdateContents();
    RedrawWindow();
    return TRUE;
}

