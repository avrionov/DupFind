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
#ifndef __THEMES_H__

#include <uxtheme.h>
//#include <tmschema.h>

void InitThemes();
void FinThemes();
void PrepareImageRect(HWND hButtonWnd, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, RECT* rpImage);
void DrawTheIcon(HWND hButtonWnd, HICON hIcon, HDC* dc, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, BOOL bIsDisabled);

typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
							int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect);
typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, 
							int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
							DWORD dwTextFlags2, const RECT *pRect);
typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,  HDC hdc, 
							int iPartId, int iStateId,  const RECT *pBoundingRect, 
							RECT *pContentRect);

typedef BOOL (__stdcall * ISTHEMEACTIVE)(VOID);

extern PFNOPENTHEMEDATA zOpenThemeData;
extern PFNDRAWTHEMEBACKGROUND zDrawThemeBackground;
extern PFNCLOSETHEMEDATA zCloseThemeData;
extern PFNDRAWTHEMETEXT zDrawThemeText;
extern PFNGETTHEMEBACKGROUNDCONTENTRECT zGetThemeBackgroundContentRect;
extern ISTHEMEACTIVE zIsThemeActtive;

extern BOOL ThemeLibLoaded;

#define __THEMES_H__

#endif