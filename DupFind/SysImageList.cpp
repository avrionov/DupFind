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
// SysImageList.cpp: implementation of the CSysImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysImageList.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSysImageList gSysImageList;

CSysImageList::CSysImageList()
{
	SHFILEINFO    ssfi; 
   
	TCHAR path[2094];
	SHGetSpecialFolderPath (NULL, path, CSIDL_WINDOWS , FALSE);

    m_hSystemSmallImageList =  (HIMAGELIST)SHGetFileInfo( path, 0, &ssfi, sizeof(SHFILEINFO),  SHGFI_SYSICONINDEX | SHGFI_SMALLICON); 
    m_ImageList.Attach (m_hSystemSmallImageList);   
}

CSysImageList::~CSysImageList()
{
	m_ImageList.Detach ();
}
