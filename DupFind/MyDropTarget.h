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
//////////////////////////////////////////////////////////////////////
//
// MyDropTarget.h: interface for the CMyDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDROPTARGET_H__04E739F2_5474_4A14_97CB_BDF035E0B8B2__INCLUDED_)
#define AFX_MYDROPTARGET_H__04E739F2_5474_4A14_97CB_BDF035E0B8B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// CMyDropTarget implements a drop target for the MultiFiler dialog.
// I was also messing around with the IDropTargetHelper stuff in 
// Win2K, which lets the shell draw the nifty shaded drag image when
// you drag into the dialog.  If you're using 9x or NT 4, that stuff
// is disabled.
 
struct IDropTargetHelper;   // forward reference, in case the latest PSDK isn't installed.

class CDupFindDlg;

class CMyDropTarget : public COleDropTarget  
{
public:
	BOOL Register (CDupFindDlg* pWnd);
	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

	void OnDragLeave ( CWnd* pWnd );

    CMyDropTarget ( CListCtrl* pList );
	virtual ~CMyDropTarget();

protected:
    CListCtrl*         m_pList;
    IDropTargetHelper* m_piDropHelper;
    bool               m_bUseDnDHelper;
	CDupFindDlg*	   m_pDupFindDlg;				
    BOOL ReadHdropData ( COleDataObject* pDataObject );
};

#endif // !defined(AFX_MYDROPTARGET_H__04E739F2_5474_4A14_97CB_BDF035E0B8B2__INCLUDED_)
