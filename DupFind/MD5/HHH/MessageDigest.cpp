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

//MessageDigest.cpp

#include "stdafx.h"
#include "MessageDigest.h"
#include "DoubleBuffering.h"

#include <exception>
#include <fstream>
#include <strstream>

using namespace std;

//Digesting a Full File
void IMessageDigest::DigestFile(CString const& rostrFileIn, char* pcDigest)
{
	//Is the User's responsability to ensure that pcDigest is appropriately allocated
	//Open Input File
	ifstream in(rostrFileIn, ios::binary);
	if(!in)
	{
		ostrstream ostr;
		ostr << "FileDigest ERROR: in IMessageDigest::DigestFile(): Cannot open File " << rostrFileIn << "!" << ends;
		string ostrMsg = ostr.str();
		ostr.freeze(false);
		throw runtime_error(ostrMsg);
	}
	//Resetting first
	Reset();
	//Reading from file
	char szLargeBuff[BUFF_LEN+1] = {0};
	char szBuff[DATA_LEN+1] = {0};
	CDoubleBuffering oDoubleBuffering(in, szLargeBuff, BUFF_LEN, DATA_LEN);
	int iRead;
	while((iRead=oDoubleBuffering.GetData(szBuff)) > 0)
		AddData(szBuff, iRead);
	in.close();
	//Final Step
	FinalDigest(pcDigest);
}

