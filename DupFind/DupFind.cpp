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
// DupFind.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DupFind.h"
#include "DupFindDlg.h"
#include "Themes.h"
#include "Settings.h"
#include "CmdLineParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CDupFindApp

BEGIN_MESSAGE_MAP(CDupFindApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDupFindApp construction

CDupFindApp::CDupFindApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDupFindApp object

CDupFindApp theApp;


// CDupFindApp initialization

BOOL CDupFindApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.

	AfxOleInit();

	InitCommonControls();
	
	CWinApp::InitInstance();
	InitThemes();
	gSettings.Load();

	CCmdLineParser cmdParser(::GetCommandLine());

  //CreateDirectory(_T("logs"), NULL);
  //auto my_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");
  //my_logger->info("Some log message");
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("ExplorerXP\\DupFind 1.0"));

	CString cmdFolder;
	
	if (cmdParser.GetVal(_T("Folder"), cmdFolder))
		gSettings.m_Folder = cmdFolder;

	CDupFindDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	
	}
	else if (nResponse == IDCANCEL)
	{
	
	}
	
	gSettings.Save();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
