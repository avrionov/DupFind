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
#include "CmdLineParser.h"

const TCHAR CCmdLineParser::m_sDelimeters[] = _T("-/");
const TCHAR CCmdLineParser::m_sQuotes[] = _T("\"");	// Can be _T("\"\'"),  for instance
const TCHAR CCmdLineParser::m_sValueSep[] = _T(" :"); // Space MUST be in set

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdLineParser::CCmdLineParser(LPCTSTR sCmdLine, bool bCaseSensitive)
: m_bCaseSensitive(bCaseSensitive)
{
	if(sCmdLine) {
		Parse(sCmdLine);
	}
}

CCmdLineParser::~CCmdLineParser()
{
	m_ValsMap.clear();
}

bool CCmdLineParser::Parse(LPCTSTR sCmdLine) {
	if(!sCmdLine) return false;
	
	m_sCmdLine = sCmdLine;
	m_ValsMap.clear();
	const CCmdLineParser_String sEmpty;

	int nArgs = 0;
	LPCTSTR sCurrent = sCmdLine;
	while(true) {
		// /Key:"arg"
		if(_tcslen(sCurrent) == 0) { break; } // No data left
		LPCTSTR sArg = _tcspbrk(sCurrent, m_sDelimeters);
		if(!sArg) break; // No delimeters found
		sArg =  _tcsinc(sArg);
		// Key:"arg"
		if(_tcslen(sArg) == 0) break; // String ends with delimeter
		LPCTSTR sVal = _tcspbrk(sArg, m_sValueSep);
		if(sVal == NULL) { //Key ends command line
			CCmdLineParser_String csKey(sArg);
			if(!m_bCaseSensitive) {
				csKey.MakeLower();
			}
			m_ValsMap.insert(CValsMap::value_type(csKey, sEmpty));
			break;
		} else if(sVal[0] == _T(' ') || _tcslen(sVal) == 1 ) { // Key with no value or cmdline ends with /Key:
			CCmdLineParser_String csKey(sArg, sVal - sArg);
			if(!csKey.IsEmpty()) { // Prevent /: case
				if(!m_bCaseSensitive) {
					csKey.MakeLower();
				}
				m_ValsMap.insert(CValsMap::value_type(csKey, sEmpty));
			}
			sCurrent = _tcsinc(sVal);
			continue;
		} else { // Key with value
			CCmdLineParser_String csKey(sArg, sVal - sArg);
			if(!m_bCaseSensitive) {
				csKey.MakeLower();
			}

			sVal = _tcsinc(sVal);
			// "arg"
			LPCTSTR sQuote = _tcspbrk(sVal, m_sQuotes), sEndQuote(NULL);
			if(sQuote == sVal) { // Quoted String
				sQuote = _tcsinc(sVal);
				sEndQuote = _tcspbrk(sQuote, m_sQuotes);
			} else {
				sQuote = sVal;
				sEndQuote = _tcschr(sQuote, _T(' '));
			}

			if(sEndQuote == NULL) { // No end quotes or terminating space, take rest of string
				CCmdLineParser_String csVal(sQuote);
				if(!csKey.IsEmpty()) { // Prevent /:val case
					m_ValsMap.insert(CValsMap::value_type(csKey, csVal));
				}
				break;
			} else { // End quote or space present
				if(!csKey.IsEmpty()) {	// Prevent /:"val" case
					CCmdLineParser_String csVal(sQuote, sEndQuote - sQuote);
					m_ValsMap.insert(CValsMap::value_type(csKey, csVal));
				}
				sCurrent = _tcsinc(sEndQuote);
				continue;
			}
		}

	}
	
	return (nArgs > 0);
}

CCmdLineParser::CValsMap::const_iterator CCmdLineParser::findKey(LPCTSTR sKey) const {
	CCmdLineParser_String s(sKey);
	if(!m_bCaseSensitive) {
		s.MakeLower();
	}
	return m_ValsMap.find(s);
}
// TRUE if "Key" present in command line
bool CCmdLineParser::HasKey(LPCTSTR sKey) const {
	CValsMap::const_iterator it = findKey(sKey);
	if(it == m_ValsMap.end()) return false;
	return true;
}

// Is "key" present in command line and have some value
bool CCmdLineParser::HasVal(LPCTSTR sKey) const {
	CValsMap::const_iterator it = findKey(sKey);
	if(it == m_ValsMap.end()) return false;
	if(it->second.IsEmpty()) return false;
	return true;
}
// Returns value if value was found or NULL otherwise
LPCTSTR CCmdLineParser::GetVal(LPCTSTR sKey) const {
	CValsMap::const_iterator it = findKey(sKey);
	if(it == m_ValsMap.end()) return false;
	return LPCTSTR(it->second);
}
// Returns true if value was found
bool CCmdLineParser::GetVal(LPCTSTR sKey, CCmdLineParser_String& sValue) const {
	CValsMap::const_iterator it = findKey(sKey);
	if(it == m_ValsMap.end()) return false;
	sValue = it->second;
	return true;
}

CCmdLineParser::POSITION CCmdLineParser::getFirst() const {
	return m_ValsMap.begin();
}
CCmdLineParser::POSITION CCmdLineParser::getNext(POSITION& pos, CCmdLineParser_String& sKey, CCmdLineParser_String& sValue) const {
	if(isLast(pos)) {
		sKey.Empty();
		return pos;
	} else {
		sKey = pos->first;
		sValue = pos->second;
		pos ++;
		return pos;
	}
}
// just helper ;)
bool CCmdLineParser::isLast(POSITION& pos) const {
	return (pos == m_ValsMap.end());
}
