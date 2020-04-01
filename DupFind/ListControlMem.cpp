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
// ListControlMem.cpp : implementation file
//

#include "stdafx.h"
#include "ListControlMem.h"
#include "MemDC.h"



// CListControlMem

IMPLEMENT_DYNAMIC(CListControlMem, CListCtrl)
CListControlMem::CListControlMem()
{
}

CListControlMem::~CListControlMem()
{
}


BEGIN_MESSAGE_MAP(CListControlMem, CListCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()	
END_MESSAGE_MAP()



// CListControlMem message handlers


void CListControlMem::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	GetClientRect(m_rectClient);

	CHeaderCtrl* pHC;
	pHC = GetHeaderCtrl();
	if (pHC != NULL && pHC->IsWindowVisible())
	{
		CRect rectHeader;
		pHC->GetItemRect( 0, &rectHeader );
		if (rectHeader.bottom > 0)
			m_rectClient.top += rectHeader.bottom;
	}

  CMemDC_Exp memDC(&dc, &m_rectClient);

	if (IsWindowEnabled ())	// Let the window do its default painting...
		CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
	else 
		memDC.FillSolidRect(m_rectClient, ::GetSysColor (COLOR_3DFACE));
}

BOOL CListControlMem::OnEraseBkgnd(CDC* pDC)
{
	UNUSED_ALWAYS(pDC);
	
	return TRUE;
}

