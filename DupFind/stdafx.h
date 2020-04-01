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

#ifndef __STDAFX_H__
#define __STDAFX_H__

#pragma once


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define WINVER		0x0501
//#define _AFX_NO_OCC_SUPPORT

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma warning (disable : 4786)
#pragma warning (disable : 4100)

#include <afxwin.h>
#include <afxcmn.h>
#include <afxcview.h>
#include <afxtempl.h>
#include <afxext.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <afxole.h>

#include <algorithm>
#include <vector>
#include <list>
#include <map>

#define SLASH    _T('\\')
#define EMPTYSTR _T("")

//#include "spdlog/spdlog.h"

#endif // __STDAFX_H__

//#endif // _AFX_NO_AFXCMN_SUPPORT

