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
#ifndef _COMMON_H_
#define _COMMON_H_


// #ifndef _QWORD_DEFINED
// #define _QWORD_DEFINED
// typedef __int64 QWORD, *LPQWORD;
// #endif

#define MAKEQWORD(a, b)	\
	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#define LODWORD(l) \
	((DWORD)(l))
//#define HIDWORD(l) \
//	((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

// Read 4K of data at a time (used in the C++ streams, Win32 I/O, and assembly functions)
#define MAX_BUFFER_SIZE	4096

// Map a "view" size of 10MB (used in the filemap function)
#define MAX_VIEW_SIZE	10485760

#endif
