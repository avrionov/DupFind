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
#include "FileDigest.h"


//Function to convert a Hex string of length 2 to an unsigned char
bool Hex2Char(char const* szHex, unsigned char& rch)
{
  if (*szHex >= '0' && *szHex <= '9')
    rch = *szHex - '0';
  else if (*szHex >= 'A' && *szHex <= 'F')
    rch = *szHex - 55; //-'A' + 10
  else
    //Is not really a Hex string
    return false;
  szHex++;
  if (*szHex >= '0' && *szHex <= '9')
    (rch <<= 4) += *szHex - '0';
  else if (*szHex >= 'A' && *szHex <= 'F')
    (rch <<= 4) += *szHex - 55; //-'A' + 10;
  else
    //Is not really a Hex string
    return false;
  return true;
}


void Char2Hex(unsigned char ch, char* szHex)
{
  static unsigned char saucHex[] = "0123456789ABCDEF";
  szHex[0] = saucHex[ch >> 4];
  szHex[1] = saucHex[ch & 0xF];
  szHex[2] = 0;
}

//Function to convert binary string to hex string
void Binary2Hex(unsigned char const* pucBinStr, int iBinSize, char* pszHexStr)
{
  int i;
  char szHex[3];
  unsigned char const* pucBinStr1 = pucBinStr;
  *pszHexStr = 0;
  for (i = 0; i<iBinSize; i++, pucBinStr1++)
  {
    Char2Hex(*pucBinStr1, szHex);
    strcat(pszHexStr, szHex);
  }
}

void FileDigest::print() {  
    char szHex[256] = { 0 };
    Binary2Hex(reinterpret_cast<unsigned char*> (acDigest), 16, szHex);
    TRACE(_T("%s - "), (LPCTSTR)fileName);
    TRACE("%s\n", szHex); 
}