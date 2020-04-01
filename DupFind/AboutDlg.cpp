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
#include "DupFind.h"
#include "AboutDlg.h"



CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	m_Version = EMPTYSTR;
	m_Time = EMPTYSTR;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_VERSION, m_Version);
	DDX_Control(pDX, IDC_HARMONDALE, m_HyperLink);
	DDX_Text(pDX, IDC_TIME, m_Time);
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

char *data = __TIMESTAMP__;


BOOL CAboutDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_Version = "1.0";
  m_Time = data;
  UpdateData (FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}