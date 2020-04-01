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
#ifndef __INOUT_H__

#pragma once

class InOut 
{
public:
	InOut (const TCHAR *funcName,...)
	{
		 va_list argptr;

		va_start(argptr, funcName);
		
		TCHAR *ptr = m_FuncName.GetBuffer (2560);

		_vsntprintf (ptr, 2550, funcName, argptr);
		
		m_FuncName.ReleaseBuffer ();

		TRACE(_T("-> %s\n"), (LPCTSTR)m_FuncName);
		m_TimeOut = m_timeIn = GetTickCount ();
	}

	~InOut ()
	{
		m_TimeOut = GetTickCount ();
		
		double span = (m_TimeOut  - m_timeIn) / 1000.0f;

		if (span != 0.0)
		{
			//TRACE("*****************************\n");
			TRACE(_T("<- %s %6.3lf \n"), (LPCTSTR)m_FuncName, span);
			//TRACE("*****************************\n");
		}
			
		else
			TRACE(_T("<- %s \n"), (LPCTSTR)m_FuncName);
	}
	
protected:
	CString m_FuncName;
	DWORD m_timeIn;
	DWORD m_TimeOut;
};

class TimeStamp 
{
public:
	
	void Start () 
	{
		m_TimeOut = m_timeIn = GetTickCount ();
		//m_TimeOut = m_timeIn = CTime::GetCurrentTime ();
	}

	void End ()
	{
		m_TimeOut = GetTickCount ();
//		m_TimeOut = CTime::GetCurrentTime ();

		
	}
	
	CString getTime ()
	{
		DWORD span = (m_TimeOut  - m_timeIn);
		int hours = span / 3600000L;
		int minutes = (span / 60000) % 60;
		int seconds = (span / 1000) % 60;
		int mlsec = span  % 1000;
		
		CString time;
		time.Format(_T("%02d:%02d:%02d.%d"),hours, minutes,seconds, mlsec);
		return time;

		/*CTimeSpan span;
		span = m_TimeOut - m_timeIn;
		CString time = span.Format("%H:%M:%S");
		return time;*/
	}

protected:

	/*CTime m_timeIn;
	CTime m_TimeOut;*/
	DWORD m_timeIn;
	DWORD m_TimeOut;
};


#endif //__INOUT_H__