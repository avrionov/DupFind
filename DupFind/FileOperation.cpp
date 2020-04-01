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
#include "StdAfx.h"
#include "fileoperation.h"

TCHAR * AllocateNullPaths (CSelRowArray &ar, int& buf_size)
{
	buf_size = 0;

	if (ar.size () == NULL)
		return NULL;

	for (unsigned int i = 0; i < ar.size (); i++)
		buf_size += ar[i].m_Path.GetLength ()  + 1;

	buf_size += 1; // the last NULL;

	TCHAR *pFrom = new TCHAR [buf_size];

	TCHAR* ptr = pFrom;
	buf_size *= sizeof (TCHAR);

	memset (pFrom, 0, buf_size);

	for ( unsigned i = 0; i < ar.size (); i++)
	{
		_tcscpy (ptr, (LPCTSTR) ar[i].m_Path);
		ptr += ar[i].m_Path.GetLength ()  + 1;
	}


	return  pFrom;
}

TCHAR * AllocateNullPath (const TCHAR *str, int& buf_size)
{
	buf_size  = (_tcsclen (str) + 2 ) * sizeof (TCHAR);

	TCHAR *pFrom = new TCHAR [buf_size];

	TCHAR* ptr = pFrom;

	memset (pFrom, 0, buf_size);

	_tcscpy (ptr, str);

	return  pFrom;
}

UINT FileOperationInternal (LPVOID info)
{

	SHFILEOPSTRUCT *fo = (SHFILEOPSTRUCT *)info;
	SHFileOperation (fo);

	delete [] (TCHAR*)fo->pFrom;

	if (fo->pTo)
		delete [] (TCHAR*)fo->pTo;

	delete fo;

	return TRUE;
}

bool FileOperation (CWnd *pWnd, CSelRowArray &ar, UINT wFunc, FILEOP_FLAGS fFlags, const TCHAR *pTo)
{	

	if (ar.size () == 0) return false;

	int size = 0;

	TCHAR *pFrom = AllocateNullPaths (ar, size);

	SHFILEOPSTRUCT *fo = new SHFILEOPSTRUCT;

	fo->hwnd = pWnd->GetSafeHwnd ();

	fo->wFunc = wFunc;
	fo->fFlags = fFlags;
	fo->pFrom = pFrom;
	if (pTo)
	{
		fo->pTo = new TCHAR [_tcsclen(pTo)+1];
		_tcscpy ((TCHAR*)fo->pTo, pTo);
	}
	else
		fo->pTo = NULL;

	fo->lpszProgressTitle = NULL;

	//int ret = SHFileOperation (&fo);
	//AfxBeginThread (FileOperationInternal, (LPVOID)fo);

	FileOperationInternal ((LPVOID)fo);
	//return ret == 0;
	return true;
}
