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
#ifndef _CRC32DYNAMIC_H_
#define _CRC32DYNAMIC_H_

#include "Common.h"

class CCrc32Dynamic
{
public:
	CCrc32Dynamic();
	virtual ~CCrc32Dynamic();

	void Init(void);
	void Free(void);

	DWORD StringCrc32(LPCTSTR szString, DWORD &dwCrc32) const;
	DWORD FileCrc32Streams(LPCTSTR szFilename, DWORD &dwCrc32) const;
	DWORD FileCrc32Win32(LPCTSTR szFilename, DWORD &dwCrc32) const;
	DWORD FileCrc32Filemap(LPCTSTR szFilename, DWORD &dwCrc32) const;
	DWORD FileCrc32Assembly(LPCTSTR szFilename, DWORD &dwCrc32) const;

protected:
	static bool GetFileSizeQW(const HANDLE hFile, QWORD &qwSize);
	inline void CalcCrc32(const BYTE byte, DWORD &dwCrc32) const;

	DWORD *m_pdwCrc32Table;
};

#endif