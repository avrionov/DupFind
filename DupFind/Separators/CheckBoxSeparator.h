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

#include "Separator.h"
#include <set>
using namespace std;

class CCheckBoxSeparator : public CButton
{
public:
	CCheckBoxSeparator();
    virtual ~CCheckBoxSeparator();

    void SetNextSectionCtrl(CWnd& wnd);
    void SetNextSectionId(int nNextId);

    void AddControlledDlgItem(int);
    void UpdateControlledDlgItems();

protected:
    CFont m_font;
    CSeparator m_ctrlSeparator;
    set<int> m_ControlledIDs;

    void OnUpdateContents();

public:
    DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnClicked();
    afx_msg void OnMove(int x, int y);
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
protected:
    virtual void PreSubclassWindow();
};
