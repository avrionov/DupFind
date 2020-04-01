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
#ifndef __MENU_BUTTON_H__
#define __MENU_BUTTON_H__

//#include "BCMenu.h"


#pragma once

#define BCMenu CNewMenu

class CMenuButton : public CButton
{
// Construction
public:
	CMenuButton();
	int m_nIndex;
	int m_nCommand;
	DWORD m_MenuID;
	CMenu m_Menu;
	CMenu *m_pPopupMenu;
	bool m_bMouseOverButton;

// Attributes
public:

// Operations
public:    
	void SetIndex (int nIndex);
	void SetMenu (DWORD nID);
	virtual BOOL SetMenu (CMenu *pMenu);
	int GetIndex () { return m_nIndex;}
	int GetCommand () { return m_nCommand;};
	bool	InRect();
	CString GetTextFromIndex (int index);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMenuButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMenuButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMenuButton)
	afx_msg BOOL OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	void SetTextFromMenu ();
	DWORD DoSingleClick ();
	int PosFromID (DWORD nID);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //__MENU_BUTTON_H__
//
///EOF

void DDX_MenuButton (CDataExchange *pDX, int nIDC, CMenuButton &rCFEC);
