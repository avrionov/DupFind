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
#include "stdafx.h"
#include "SectionSeparator.h"
#include "VisualStylesXP.h"

static const int k_nIconSize = 16;

CSectionSeparator::CSectionSeparator(void)
{
    m_bChanging = false;
    m_bCollapsed = false;
    m_bResizeParent = true;
    m_bAnimCollapse = true;
    m_nSectionHeight = 0;
    m_fFPS = 35.f;
    m_fAnimDuration = 0.5f;
}

CSectionSeparator::~CSectionSeparator(void)
{
}

bool CSectionSeparator::IsCollapsed()
{
    return m_bCollapsed;
}

void CSectionSeparator::SetResizeParent(bool bResize)
{
    m_bResizeParent= bResize;
}

void CSectionSeparator::SetAnimOnCollapse(bool bAnim)
{
    m_bAnimCollapse = bAnim;
}

void CSectionSeparator::SetFramesPerSec(float fFPS)
{
    m_fFPS = fFPS;
}

void CSectionSeparator::SetAnimDuration(float fDuration)
{
    m_fAnimDuration = fDuration;
}

BEGIN_MESSAGE_MAP(CSectionSeparator, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_MESSAGE(WM_PRINT, OnPrint)
    ON_MESSAGE(WM_PRINTCLIENT, OnPrint)
END_MESSAGE_MAP()

void CSectionSeparator::Print(CDC* pDC, DWORD dwFlags)
{
    // draw separator
    CRect rc;
    m_ctrlSeparator.GetWindowRect(&rc);
    ScreenToClient(&rc);
    pDC->OffsetViewportOrg(rc.left, rc.top);
    m_ctrlSeparator.SendMessage(WM_PRINT, (WPARAM) pDC->GetSafeHdc(), dwFlags);
    pDC->OffsetViewportOrg(-rc.left, -rc.top);

    // write the caption
    CFont* pOldFont = pDC->SelectObject(&m_font);
    pDC->SetBkMode(TRANSPARENT);
    pDC->TextOut(k_nIconSize, 0, m_strLabel);
    pDC->SelectObject(pOldFont);

    // now the icon
    CPen pen(PS_SOLID, 1, pDC->GetTextColor());
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush brush(pDC->GetTextColor());
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    if (m_bCollapsed)
    {
        CPoint points[3];
        points[0] = CPoint(0, 3);
        points[1] = CPoint(8, 7);
        points[2] = CPoint(0, 11);
        pDC->Polygon(points, 3);
    }
    else
    {
        CPoint points[3];
        points[0] = CPoint(0, 3);
        points[1] = CPoint(8, 3);
        points[2] = CPoint(4, 11);
        pDC->Polygon(points, 3);
    }

    // cleanup
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}

void CSectionSeparator::OnPaint()
{
    // the client area is cleared here
    Default();

    // print in the dc
    CDC* pDC = GetDC();
    Print(pDC, 0);
    ReleaseDC(pDC);
}

LRESULT CSectionSeparator::OnPrint(WPARAM wParam, LPARAM lParam)
{
    Print(CDC::FromHandle((HDC)wParam), (DWORD)lParam);
    return 0;
}

void CSectionSeparator::SetNextSectionCtrl(CWnd& wnd)
{
    m_nNextSectionId = wnd.GetDlgCtrlID();
    UpdateSectionHeight();
}

void CSectionSeparator::SetNextSectionId(int nNextId)
{
    m_nNextSectionId = nNextId;
    UpdateSectionHeight();
}

void CSectionSeparator::UpdateSectionHeight()
{
    CRect rc1;
    GetWindowRect(&rc1);
    GetParent()->ScreenToClient(&rc1);

    CRect rc2;
    if (GetParent()->GetDlgItem(m_nNextSectionId))
    {
        GetParent()->GetDlgItem(m_nNextSectionId)->GetWindowRect(&rc2);
        GetParent()->ScreenToClient(&rc2);
    }
    else
    {
        GetParent()->GetClientRect(&rc2);
        rc2.top = rc2.bottom;
    }

    m_nSectionHeight = rc2.top - rc1.bottom - 5;
    ASSERT(m_nSectionHeight>0);
}

void CSectionSeparator::GetSectionTops(int& nCurrTop, int& nNextTop)
{
    // current section
    CRect rc;
    GetWindowRect(&rc);
    GetParent()->ScreenToClient(&rc);
    nCurrTop = rc.top;

    // next section
    if (GetParent()->GetDlgItem(m_nNextSectionId))
    {
        GetParent()->GetDlgItem(m_nNextSectionId)->GetWindowRect(&rc);
        GetParent()->ScreenToClient(&rc);
    }
    else
    {
        GetParent()->GetClientRect(&rc);
        rc.top = rc.bottom;
    }
    nNextTop = rc.top;
}

void CSectionSeparator::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_bChanging)
    {
        CRect rc;
        m_ctrlSeparator.GetWindowRect(&rc);
        ScreenToClient(&rc);
        if (point.x < rc.left)
        {
            if (m_bCollapsed) Uncollapse();
            else Collapse();
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CSectionSeparator::Collapse()
{
    if (!m_bCollapsed && m_nSectionHeight)
    {
        // flag
        m_bChanging = true;

        // store actions
        map<HWND,CRect> actions;

        // get the tops
        int nSectionTop;
        int nNextSectionTop;
        GetSectionTops(nSectionTop, nNextSectionTop);

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
            if (rc.top < nSectionTop)
            {
                pChild = pChild->GetWindow(GW_HWNDNEXT);
                continue;
            }

            // hide controls above options end
            if (rc.bottom < nNextSectionTop)
            {
                m_related[pChild->GetSafeHwnd()] = rc.top-nSectionTop;
                pChild->ShowWindow(SW_HIDE);
                pChild->MoveWindow(rc.left, nSectionTop, rc.Width(), rc.Height());
            }
            else
            {
                // move
                actions[pChild->GetSafeHwnd()] = CRect(0, -m_nSectionHeight, 0, 0);
            }

            // next
            pChild = pChild->GetWindow(GW_HWNDNEXT);
        }

        // update size of dialog
        if (m_bResizeParent)
            actions[GetParent()->GetSafeHwnd()] = CRect(0, 0, 0, -m_nSectionHeight);

        // now move everyone
        MoveControls(actions, m_bAnimCollapse ? m_fAnimDuration : -1.0f);

        // update icon
        m_bCollapsed = true;
        m_bChanging = false;
        GetParent()->RedrawWindow();
    }
}

void CSectionSeparator::Uncollapse()
{
    if (m_bCollapsed && m_nSectionHeight)
    {
        // flag
        m_bChanging = true;

        // get the tops
        int nSectionTop;
        int nNextSectionTop;
        GetSectionTops(nSectionTop, nNextSectionTop);

        // store actions
        map<HWND,CRect> actions;

        // update size of dialog
        if (m_bResizeParent)
            actions[GetParent()->GetSafeHwnd()] = CRect(0, 0, 0, m_nSectionHeight);

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

            // skip related controls
            if (m_related.find(pChild->GetSafeHwnd()) != m_related.end())
            {
                pChild = pChild->GetWindow(GW_HWNDNEXT);
                continue;
            }

            // get rect and move
            CRect rc;
            pChild->GetWindowRect(&rc);
            GetParent()->ScreenToClient(&rc);

            // do not touch controls above us
            if (rc.top < nSectionTop)
            {
                pChild = pChild->GetWindow(GW_HWNDNEXT);
                continue;
            }

            // okay!
            actions[pChild->GetSafeHwnd()] = CRect(0, m_nSectionHeight, 0, 0);

            // next
            pChild = pChild->GetWindow(GW_HWNDNEXT);
        }

        // now  move them
        MoveControls(actions, m_fAnimDuration);

        // show the related controls
        for (map<HWND,int>::iterator it = m_related.begin(); it != m_related.end(); ++it)
        {
            // get rect and move
            CRect rc;
            CWnd *pChild = CWnd::FromHandle(it->first);
            pChild->GetWindowRect(&rc);
            GetParent()->ScreenToClient(&rc);
            pChild->MoveWindow(rc.left, nSectionTop+it->second, rc.Width(), rc.Height());
            pChild->ShowWindow(SW_SHOW);
        }

        // update icon
        m_bCollapsed = false;
        m_bChanging = false;
        GetParent()->RedrawWindow();
    }
}

void CSectionSeparator::PreSubclassWindow()
{
    CWnd::PreSubclassWindow();
    ModifyStyle(0, SS_NOTIFY, 0);

    // set bold font;
    LOGFONT logFont;
    GetFont()->GetLogFont(&logFont);
    logFont.lfWeight = FW_BOLD;
    m_font.CreateFontIndirect(&logFont);
    SetFont(&m_font);

    // get label
    CString strLabel;
    GetWindowText(strLabel);
    SetWindowText("");
    m_strLabel = strLabel;

    OnUpdateContents();
}

void CSectionSeparator::OnUpdateContents()
{
    if (m_ctrlSeparator.GetSafeHwnd())
        m_ctrlSeparator.DestroyWindow();

    CRect rect;
    GetClientRect(&rect);
    m_nOldWidth = rect.Width();

    CDC *pDC = GetDC();
    CFont *pFont = pDC->SelectObject(GetFont());
    CString strText = m_strLabel;
    if (strText != "") strText += " ";
    CSize size = pDC->GetTextExtent(strText);
    pDC->SelectObject(pFont);
    ReleaseDC(pDC);

    rect.left += size.cx;
    rect.left += k_nIconSize;

    if (g_xpStyle.IsAppThemed())
        rect.top += size.cy/3;

    m_ctrlSeparator.Create(WS_CHILD|WS_VISIBLE, rect, this);
}

void CSectionSeparator::OnSize( UINT nType, int cx, int cy )
{
    CRect rect;
    m_ctrlSeparator.GetClientRect(&rect);
    m_ctrlSeparator.SetWindowPos(NULL, 0, 0, rect.Width() + cx - m_nOldWidth, cy, SWP_NOMOVE|SWP_NOZORDER);
    m_nOldWidth = cx;
    CWnd::OnSize(nType, cx, cy);
}

void CSectionSeparator::OnMove(int x, int y)
{
    CWnd::OnMove(x, y);
    OnUpdateContents();
}

void CSectionSeparator::OnDestroy()
{
    m_font.DeleteObject();
    m_ctrlSeparator.DestroyWindow();
    CWnd::OnDestroy();
}

LRESULT CSectionSeparator::OnSetText(WPARAM wParam, LPARAM lParam)
{
    m_strLabel = (LPCTSTR) lParam;
    RedrawWindow();
    return TRUE;
}

void CSectionSeparator::PumpMessages()
{
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (!PreTranslateMessage(&msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void CSectionSeparator::MoveControls(map<HWND,CRect>& actions, float fDuration)
{
    // we use this a lot
    HWND hwndParent = GetParent()->GetSafeHwnd();

    // for parent
    bool bResizeParent = false;
    int nParentOrigWidth = 0;
    int nParentOrigHeight = 0;
    int nParentWidthInc = 0;
    int nParentHeightInc = 0;

    // save the initial rect of each control
    map<HWND,CRect> initial;
    for (map<HWND,CRect>::iterator it = actions.begin(); it != actions.end(); ++it)
    {
        CRect rc;
        CWnd* pWnd = CWnd::FromHandle(it->first);
        pWnd->GetWindowRect(&rc);
        if (it->first != hwndParent)
        {
            GetParent()->ScreenToClient(&rc);
            initial[it->first] = rc;
        }
        else
        {
            bResizeParent = true;
            nParentOrigWidth = rc.Width();
            nParentOrigHeight = rc.Height();
            nParentWidthInc = it->second.right;
            nParentHeightInc = it->second.bottom;
        }
    }

    // no animation if not visible
    if (!GetParent()->IsWindowVisible())
        fDuration = -1.0f;

    // animation?
    if (fDuration>=0)
    {
        // params
        int nFrames = (fDuration==0) ? int(m_fFPS+0.5f) : int(m_fFPS*fDuration+0.5f);

        // timer
        fDuration *= 1000;
        LARGE_INTEGER liF, liS, now;
        __int64 freq, startTime;
        QueryPerformanceFrequency(&liF);
        freq = liF.QuadPart;
        QueryPerformanceCounter(&liS);
        startTime = liS.QuadPart;

        // draw each frame
        for (int f=1; f<nFrames; f++)
        {
            // offset
            float fOffset = float(f)/float(nFrames);

            // we use this to avoid flickering
            HDWP hdwp = BeginDeferWindowPos(int(actions.size()));

            // iterate
            for (map<HWND,CRect>::iterator it = actions.begin(); it != actions.end(); ++it)
            {
                CRect rc = initial[it->first];
                rc.left += int(it->second.left*fOffset+0.5);
                rc.top += int(it->second.top*fOffset+0.5);
                rc.right += int((it->second.left + it->second.right)*fOffset+0.5);
                rc.bottom += int((it->second.top + it->second.bottom)*fOffset+0.5);
                if (it->first != hwndParent)
                    hdwp = DeferWindowPos(hdwp, it->first, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_NOREDRAW);
            }

            // do it
            EndDeferWindowPos(hdwp);

            // resize parent
            if (bResizeParent)
            {
                CRect rcParent;
                GetParent()->GetWindowRect(&rcParent);
                rcParent.right = rcParent.left + nParentOrigWidth + int(nParentWidthInc*fOffset+0.5);
                rcParent.bottom = rcParent.top + nParentOrigHeight + int(nParentHeightInc*fOffset+0.5);
                GetParent()->MoveWindow(rcParent,TRUE);
            }
            
            // draw
            CRect rc;
            CDC dcMem;
            CBitmap bmp;
            CDC *pDC = GetParent()->GetWindowDC();
            dcMem.CreateCompatibleDC(pDC);
            GetParent()->GetWindowRect(&rc);
            bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
            CBitmap *pOldBitmap = dcMem.SelectObject(&bmp);
            ::DefWindowProc(hwndParent, WM_PRINT, (WPARAM) dcMem.GetSafeHdc(), PRF_CLIENT|PRF_NONCLIENT|PRF_CHILDREN|PRF_ERASEBKGND);
            pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
            dcMem.SelectObject(pOldBitmap);
            GetParent()->ReleaseDC(pDC);
            bmp.DeleteObject();

            // check timing
            PumpMessages();
            if (fDuration != 0)
            {
                QueryPerformanceCounter(&now);
                float fCurrentTime = (float)((now.QuadPart - startTime) / (double)(freq/1000.0));
                float fNormalTime = fDuration*fOffset;
                if (fCurrentTime<fNormalTime)
                {
                    while (fCurrentTime<fNormalTime)
                    {
                        PumpMessages();
                        QueryPerformanceCounter(&now);
                        fCurrentTime = (float)((now.QuadPart - startTime) / (double)(freq/1000.0));
                    }
                }
                else
                {
                    f = int(fCurrentTime/fDuration*float(nFrames)+0.5f)-1;
                }
            }
        }
    }

    // now one last frame to position everyone precisely
    HDWP hdwp = BeginDeferWindowPos(int(actions.size()));

    // iterate
    for (map<HWND,CRect>::iterator it = actions.begin(); it != actions.end(); ++it)
    {
        CRect rc = initial[it->first];
        rc.left += it->second.left;
        rc.top += it->second.top;
        rc.right += it->second.left + it->second.right;
        rc.bottom += it->second.top + it->second.bottom;
        if (it->first != hwndParent)
            hdwp = DeferWindowPos(hdwp, it->first, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_NOREDRAW);
    }

    // do it
    EndDeferWindowPos(hdwp);

    // resize parent
    if (bResizeParent)
    {
        CRect rcParent;
        GetParent()->GetWindowRect(&rcParent);
        rcParent.right = rcParent.left + nParentOrigWidth + nParentWidthInc;
        rcParent.bottom = rcParent.top + nParentOrigHeight + nParentHeightInc;
        GetParent()->MoveWindow(rcParent,TRUE);
    }
}
