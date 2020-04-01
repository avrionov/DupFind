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

#include <rsettings.h>

class CStrOptions : public CRegSettings
{
public:
	CString m_Folder;
	BOOL m_bChecked;
	BEGIN_REG_MAP(CStrOptions)
		REG_ITEM(m_Folder, "")				
		//REG_ITEM(m_bChecked, TRUE)
	END_REG_MAP()

};

class CSettings: public CRegSettings
{
public:	
	std::vector<CStrOptions> m_Folders;
	CString m_Folder;

	BEGIN_REG_MAP(CSettings)		
		REG_ITEM_VECTOR(m_Folders)
		REG_ITEM(m_Folder,"")
	END_REG_MAP()
};


extern CSettings gSettings;