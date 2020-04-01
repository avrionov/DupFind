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
// SysImageList.h: interface for the CSysImageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSIMAGELIST_H__26EC6EC0_2DFF_457C_9AF6_8E60606DBB0E__INCLUDED_)
#define AFX_SYSIMAGELIST_H__26EC6EC0_2DFF_457C_9AF6_8E60606DBB0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSysImageList  
{
public:
	CSysImageList();
	virtual ~CSysImageList();
	HIMAGELIST m_hSystemSmallImageList;
	CImageList m_ImageList;

};

extern CSysImageList gSysImageList;

#endif // !defined(AFX_SYSIMAGELIST_H__26EC6EC0_2DFF_457C_9AF6_8E60606DBB0E__INCLUDED_)
