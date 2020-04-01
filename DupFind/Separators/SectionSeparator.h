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
#pragma once
#include "afxwin.h"
#include "StaticSeparator.h"

#include <map>
using namespace std;

class CSectionSeparator : public CWnd
{
public:
    CSectionSeparator(void);
    ~CSectionSeparator(void);

    void OnUpdateContents();

    void SetNextSectionCtrl(CWnd& wnd);
    void SetNextSectionId(int nNextId);

    void SetAnimOnCollapse(bool bAnim);
    void SetResizeParent(bool bResize);
    void SetFramesPerSec(float fFPS);
    void SetAnimDuration(float fDuration);

    void Collapse();
    void Uncollapse();

    bool IsCollapsed();

protected:
    CString m_strLabel;
    CSeparator m_ctrlSeparator;
    bool m_bCollapsed;
    map<HWND,int> m_related;
    bool m_bChanging;
    bool m_bResizeParent;
    bool m_bAnimCollapse;
    int m_nNextSectionId;
    int m_nSectionHeight;

    CFont m_font;
    int m_nOldWidth;

    float m_fFPS;
    float m_fAnimDuration;

    virtual void UpdateSectionHeight();
    virtual void GetSectionTops(int& nCurrTop, int& nNextTop);

    virtual void MoveControls(map<HWND,CRect>& actions, float fDuration);
    virtual void PumpMessages();

    virtual void Print(CDC* pDC, DWORD dwFlags);

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg LRESULT OnPrint(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSize( UINT nType, int cx, int cy );
    afx_msg void OnMove(int x, int y);
    afx_msg void OnDestroy();
protected:
    virtual void PreSubclassWindow();
};
