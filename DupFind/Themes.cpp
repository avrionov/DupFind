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
#include "stdafx.h"

#include "Themes.h"

#define ICON_HEIGHT 16
#define ICON_WIDTH 16

BOOL Themed = FALSE;
BOOL ThemeLibLoaded = FALSE;

PFNOPENTHEMEDATA zOpenThemeData = NULL;
PFNDRAWTHEMEBACKGROUND zDrawThemeBackground = NULL;
PFNCLOSETHEMEDATA zCloseThemeData = NULL;
PFNDRAWTHEMETEXT zDrawThemeText = NULL;
PFNGETTHEMEBACKGROUNDCONTENTRECT zGetThemeBackgroundContentRect = NULL;
ISTHEMEACTIVE zIsThemeActtive;

static HMODULE hModThemes = NULL;

void InitThemes()
{
	hModThemes = LoadLibrary(_T("UXTHEME.DLL"));
	if(hModThemes)
	{
		zIsThemeActtive = (ISTHEMEACTIVE) GetProcAddress(hModThemes, "IsThemeActive");
		if (zIsThemeActtive)
			if (!zIsThemeActtive()) 
			{
				ThemeLibLoaded = FALSE;
				return;
			}
		zOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(hModThemes, "OpenThemeData");
		zDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProcAddress(hModThemes, "DrawThemeBackground");
		zCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(hModThemes, "CloseThemeData");
		zDrawThemeText = (PFNDRAWTHEMETEXT)GetProcAddress(hModThemes, "DrawThemeText");
		zGetThemeBackgroundContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProcAddress(hModThemes, "GetThemeBackgroundContentRect");

		if(zOpenThemeData 
			&& zDrawThemeBackground
			&& zCloseThemeData
			&& zDrawThemeText
			&& zGetThemeBackgroundContentRect)
		{
			ThemeLibLoaded = TRUE;			
		}
		else
		{
			FreeLibrary(hModThemes);
			hModThemes = NULL;
		}
	}
}

void FinThemes()
{
	FreeLibrary(hModThemes);
	hModThemes = NULL;
	ThemeLibLoaded = FALSE;
	Themed = FALSE;
}

void PrepareImageRect(HWND hButtonWnd, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, RECT* rpImage)
{
	RECT rBtn;

	CopyRect(rpImage, rpItem);

	GetClientRect(hButtonWnd, &rBtn);
	if (bHasTitle == FALSE)
	{
		// Center image horizontally
		LONG rpImageWidth = rpImage->right - rpImage->left;
		rpImage->left += ((rpImageWidth - (long)dwWidth)/2);
	}
	else
	{
		// Image must be placed just inside the focus rect
		LONG rpTitleWidth = rpTitle->right - rpTitle->left;
		rpTitle->right = rpTitleWidth - dwWidth - 30;
		rpTitle->left = 30;
		rpImage->left = rBtn.right - dwWidth - 30;
		// Center image vertically
		LONG rpImageHeight = rpImage->bottom - rpImage->top;
		rpImage->top += ((rpImageHeight - (long)dwHeight)/2);
	}
		
	// If button is pressed then press image also
	if (bIsPressed && !Themed)
		OffsetRect(rpImage, 1, 1);
} // End of PrepareImageRect

void DrawTheIcon(HWND hButtonWnd, HICON hOwnerDrawIcon, HDC* dc, BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, BOOL bIsDisabled)
{
	RECT	rImage;
	PrepareImageRect(hButtonWnd, bHasTitle, rpItem, rpTitle, bIsPressed, ICON_WIDTH, ICON_HEIGHT, &rImage);

	// Ole'!
	DrawState(	*dc,
				NULL,
				NULL,
				(LPARAM)hOwnerDrawIcon,
				0,
				rImage.left,
				rImage.top,
				ICON_WIDTH,
				ICON_HEIGHT, 
				(bIsDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_ICON);
} // End of DrawTheIcon

