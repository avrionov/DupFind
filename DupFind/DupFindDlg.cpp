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
// DupFindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DupFind.h"
#include "DupFindDlg.h"
#include "md5\md5.h"
#include "crc32\crc32static.h"
#include "Settings.h"
#include "SysImageList.h"
#include <Windowsx.h>
#include <Limits.h>
#include "afxpriv.h"
#include "AboutDlg.h"

#include "FileDigest.h"

using namespace LayoutFlags;

LONG gStopFlag;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ULONGLONG gMin = 0;
ULONGLONG gMax = 0;	

static UINT BASED_CODE indicators[] =
{
	ID_INDICATOR_FOLDER,
	ID_INDICATOR_FILES,
	ID_INDICATOR_DUPS,
	ID_INDICATOR_DUPSIZE,
	ID_INDICATOR_TIME
};


// CDupFindDlg dialog

int GetFileIcon (const TCHAR *file_name) {	
	SHFILEINFO    sfi;
	SHGetFileInfo(file_name, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_TYPENAME );	
	return sfi.iIcon;
}


CDupFindDlg::CDupFindDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDupFindDlg::IDD, pParent)
	, m_Folder(_T(""))
	, m_Status(_T(""))
	, m_Min(0)
	, m_Max(0)
	,m_DropTarget(&m_Folders)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Folder = gSettings.m_Folder;
	m_nSortCol  = 0;
	m_bAsc = true;
	gStopFlag = 0;
	m_Percent = -1;
}

void DDX_TextWithFormat(CDataExchange* pDX, int nIDC, LPCTSTR lpszFormat, UINT nIDPrompt, ...)
{
	va_list pData;
	va_start(pData, nIDPrompt);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	ASSERT( hWndCtrl != NULL );

	TCHAR szT[64];
	if (pDX->m_bSaveAndValidate)
	{
		void* pResult;

		pResult = va_arg( pData, void* );
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, _countof(szT));
		if (_stscanf(szT, lpszFormat, pResult) != 1)
		{			
//			pDX->Fail();        // throws exception
		}
	}
	else
	{
		_vstprintf(szT, lpszFormat, pData);
		// does not support floating point numbers - see dlgfloat.cpp
		AfxSetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
}

void DDX_TextExt(CDataExchange* pDX, int nIDC, ULONGLONG& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		value = 0;
		DDX_TextWithFormat(pDX, nIDC, _T("%I64u"), AFX_IDP_PARSE_UINT, &value);
	}
	else
		DDX_TextWithFormat(pDX, nIDC, _T("%I64u"), AFX_IDP_PARSE_UINT, value);
}

void CDupFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILE, m_Folder);	
	DDX_Control(pDX, IDC_DUPLIST, m_DupList);
	DDX_Control(pDX, IDC_SELECTFOLDERS, m_SelectFolderBtn);
	DDX_Control(pDX, IDC_SELECT, m_SelectBtn);

	DDX_Control(pDX, IDC_FOLDERS, m_Folders);
	DDX_Control(pDX, IDC_DELETEFOLDER, m_Delete);
	DDX_Control(pDX, IDC_FILE, m_EditFile);
	DDX_Control(pDX, IDC_STATIC_FOLDERS, m_FoldersSep);
	DDX_Control(pDX, IDC_STATIC_FOLDER, m_AddFoldersSep);
	DDX_Control(pDX, IDC_STATIC_SELECT, m_SelectSep);
	DDX_Control(pDX, IDC_STATIC_SEARCH, m_SearchSep);
	
	if (!pDX->m_bSaveAndValidate)
	{
		if (!m_Min)
			DDX_Text(pDX, IDC_EDIT_MIN, m_NoNumber);
		else
			DDX_TextExt(pDX, IDC_EDIT_MIN, m_Min);

		if (!m_Max)
			DDX_Text(pDX, IDC_EDIT_MAX, m_NoNumber);
		else
			DDX_TextExt(pDX, IDC_EDIT_MAX, m_Max);
	}
	else
	{
		DDX_TextExt(pDX, IDC_EDIT_MIN, m_Min);
		DDX_TextExt(pDX, IDC_EDIT_MAX, m_Max);
	}
	
}

BEGIN_MESSAGE_MAP(CDupFindDlg, CResizableDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SCAN, OnBnClickedScan)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DELETEALL, OnBnClickedDeleteall)	
	ON_MESSAGE (WM_FD_PROGRESS, OnProgress)
	ON_MESSAGE (WM_FD_CALC, OnCalc)
	ON_MESSAGE (WM_FD_END, OnEndScan)
	ON_MESSAGE (WM_FD_END, OnStartScan)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_DUPLIST, OnLvnGetdispinfoDuplist)
	ON_BN_DOUBLECLICKED(IDC_SELECTFOLDERS, OnBnDoubleclickedSelectfolders)
	ON_BN_DOUBLECLICKED(IDC_SELECT, OnBnDoubleclickedSelect)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_DESELECT, OnBnClickedDeselect)
	ON_NOTIFY(NM_CLICK, IDC_DUPLIST, OnNMClickDupList)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FOLDERS, OnLvnItemchangedFolders)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickFolders)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_DELETEFOLDER, OnBnClickedDeletefolder)
	ON_BN_CLICKED(IDC_WILDCARDS, &CDupFindDlg::OnBnClickedWildcards)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDupFindDlg message handlers

BOOL CDupFindDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect rc;
	m_Folders.ModifyStyle(0, WS_CLIPCHILDREN);
	m_Folders.GetClientRect (rc);
	m_Folders.SetImageList(&gSysImageList.m_ImageList, LVSIL_SMALL);
	m_Folders.SetExtendedStyle (LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT  );
	m_Folders.InsertColumn (0, _T("Folder"), LVCFMT_LEFT, rc.Width());

	for (unsigned int i = 0; i < gSettings.m_Folders.size(); i++)
	{
		m_Folders.InsertItem(i, gSettings.m_Folders[i].m_Folder, GetFileIcon(gSettings.m_Folders[i].m_Folder));
	}

	
	
	m_DupList.ModifyStyle(0, WS_CLIPCHILDREN);
	m_DupList.GetClientRect (rc);
	int width = (rc.Width () - 120) / 2;
	
	m_ImageList.Create (IDR_CHECKS, 17, 5, RGB(255,255, 255));
	m_DupList.SetImageList (&m_ImageList, LVSIL_SMALL);

	m_DupList.InsertColumn (0, _T("Original"), LVCFMT_LEFT, width);
	m_DupList.InsertColumn (1, _T("Duplicate"), LVCFMT_LEFT, width);
	m_DupList.InsertColumn (2, _T("Size"), LVCFMT_LEFT, 120);
	//m_DupList.SetExtendedStyle (LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES );
	m_DupList.SetExtendedStyle (LVS_EX_GRIDLINES  | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES );
	
//	m_DupList.EnableSortIcon   (TRUE);
//	m_DupList.ColorSortColumn  (FALSE);
//	m_DupList.SetSortColumn    (1);

	m_bar.Create(this); //We create the status bar

	m_bar.SetIndicators(indicators, 5); //Set the number of panes 
	m_bar.SetPaneInfo(0,ID_INDICATOR_TIME,SBPS_STRETCH ,0);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_TIME);
	ClearStatusBar ();

	m_layout.initialize(m_hWnd, true);
	m_layout.mapControl(IDC_FOLDERS, ANCHOR_LEFTRIGHT);
	m_layout.mapControl(IDC_SELECTFOLDERS, ANCHOR_BOTTOMLEFT);
	m_layout.mapControl(IDC_DESELECT, ANCHOR_BOTTOMLEFT);
	m_layout.mapControl(IDC_SELECT, ANCHOR_BOTTOMLEFT);
	m_layout.mapControl(IDC_DELETE, ANCHOR_BOTTOMLEFT);
	m_layout.mapControl(IDC_WILDCARDS, ANCHOR_BOTTOMLEFT);
	m_layout.mapControl(IDC_DUPLIST, ANCHOR_ALL);
	m_layout.mapControl(IDC_FILE, ANCHOR_LEFTRIGHT);
	m_layout.mapControl(IDC_STATIC_FOLDER, ANCHOR_LEFTRIGHT);
	m_layout.mapControl(IDC_ADD, ANCHOR_TOPRIGHT);
	m_layout.mapControl(IDC_BROWSE, ANCHOR_TOPRIGHT);
	m_layout.mapControl(IDC_STATIC_FOLDERS, ANCHOR_LEFTRIGHT);
	m_layout.mapControl(IDC_STATIC_SEARCH, ANCHOR_LEFTRIGHT);
	m_layout.mapControl(IDC_STATIC_SELECT, ANCHOR_BOTTOMLEFTRIGHT);
	m_layout.mapControl(IDC_DELETEFOLDER, ANCHOR_TOPRIGHT);
	m_layout.mapControl(IDC_DELETEALL, ANCHOR_TOPRIGHT);
	EnableDupButtons ();
	EnableSaveRestore (_T("DupFind"));
	m_EditFile.SetMode (_MODE_FS_ONLY_DIR_);

	m_DropTarget.Register (this);

	m_SelectBtn.SetMenu(IDR_SELECT_MENU);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDupFindDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDupFindDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CResizableDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDupFindDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void SureBackSlash (CString &cs)
{
	if (cs.IsEmpty ())
		return;

	if (cs[cs.GetLength()-1] != '\\')
		cs += '\\';

}

typedef std::vector <CString> StringAr;
typedef std::vector <FileDigest> DigestAr;
typedef std::list <FileDigest> DigestList;
typedef std::map <CString, FileDigest, std::less <CString> > CDigestMap;
typedef CDigestMap::value_type CDigestMapValue;
typedef CDigestMap::iterator CDigestIt;

StringAr files;
DigestList digests;
CDigestMap digest_map;
CDigestMap complete_digests;


int dirCount = 0;
//void MDFile (const TCHAR *filename, unsigned char *digest);
bool MDFileMM (const TCHAR *filename, unsigned char *digest, DWORD sampleSize);

TCHAR currentFolder [4096];
size_t calculated = 0;
size_t scanned    = 0;

bool CalcDigest (FileDigest &dg, CDigestMap& digest_map, DWORD size) 
{
	CDigestIt it = digest_map.find( dg.fileName);
	if (it != digest_map.end())
	{
		if (CompareFileTime (&it->second.lastModifed, &dg.lastModifed) == 0)
		{
			dg = it->second;
		//	dg.print ();
			return true;
		}
		digest_map.erase(it);
	}

	if (MDFileMM(dg.fileName, dg.acDigest, size)) 
	{
		digest_map.insert(CDigestMapValue(dg.fileName, dg));
		//dg.print();
		return true;
	}
	
	return false;
}

bool CalcDigest (FileDigest &dg) 
{
	CDigestIt it = digest_map.find( dg.fileName);
	if (it != digest_map.end())
	{
		if (CompareFileTime (&it->second.lastModifed, &dg.lastModifed) == 0)
		{
			dg = it->second;
		//	dg.print ();
			return true;
		}
		digest_map.erase(it);
	}

	if (MDFileMM(dg.fileName, dg.acDigest, 1024)) 
	{
		digest_map.insert(CDigestMapValue(dg.fileName, dg));
		//dg.print();
		return true;
	}
	
	return false;
}

void scan (const TCHAR * path, CWnd *pWnd)
{
	CFileFind ff;
	BOOL bFound = TRUE;
	
	_tcscpy(currentFolder, path);
	scanned = digests.size ();
	pWnd->SendMessage(WM_FD_PROGRESS,0,0);

	CString mask = path;
	SureBackSlash (mask);
	mask += "\\*.*";

	if( !ff.FindFile(mask) )
	{
		ff.Close();
		return;
	}

	int count = 0;

	while(bFound)
	{
		if (InterlockedCompareExchange (&gStopFlag, 1,1) ==1)
			return;

		bFound = ff.FindNextFile();

		if((!ff.IsDots()) && (!ff.IsDirectory())) {			
			FileDigest dg;
			dg.fileName = ff.GetFilePath();
			dg.size = ff.GetLength();
			ff.GetLastWriteTime(&dg.lastModifed);

			if (dg.size > 1024) {
				CalcDigest (dg);

				if (InterlockedCompareExchange (&gStopFlag, 1,1) ==1)
					return;
				
				digests.push_back(dg);
				count ++;
				if ((count % 5) == 0) {
					scanned = digests.size ();
					pWnd->SendMessage(WM_FD_PROGRESS,0,0);
				}

			}			
    } else {
      dirCount++;
    }
			

		if((!ff.IsDots()) && (ff.IsDirectory())) {
			CString strFile = ff.GetFilePath();				
			scan (strFile, pWnd);
			if (InterlockedCompareExchange (&gStopFlag, 1,1) ==1)
				return;
		}

	}
}

void scan_fast (const TCHAR * path, CWnd *pWnd)
{
	CFileFind ff;
	BOOL bFound = TRUE;

	//_tcscpy(currentFolder, path);
	//scanned = digests.size ();
	pWnd->SendMessage(WM_FD_PROGRESS,0,0);

	CString mask = path;
	SureBackSlash (mask);
	mask += "\\*.*";

	if( !ff.FindFile(mask) )
	{
		ff.Close();
		return;
	}

	int count = 0;

	while(bFound)
	{
		if (InterlockedCompareExchange (&gStopFlag, 1,1) ==1)
			return;

		bFound = ff.FindNextFile();

		if((!ff.IsDots()) && (!ff.IsDirectory())) {	
			
			FileDigest dg;
			dg.size = ff.GetLength();

			if ( (dg.size > gMin) && (dg.size < gMax)) 
			{
				dg.fileName = ff.GetFilePath();		
				dg.size = ff.GetLength();
				ff.GetLastWriteTime(&dg.lastModifed);

				digests.push_back(dg);
				count ++;
				/*if ((count % 10) == 0)
				{
					scanned = digests.size ();
					pWnd->SendMessage(WM_FD_PROGRESS,0,0);
				}*/
			}			
		}  else {
			dirCount++;
		}

		if((!ff.IsDots()) && (ff.IsDirectory())) {
			CString strFile = ff.GetFilePath();				
			scan_fast (strFile, pWnd);
			if (InterlockedCompareExchange (&gStopFlag, 1,1) ==1)
				return;
		}
	}
}

class CompFiles
{
public:
	CString file1, file2;
	ULONGLONG size;
	FILETIME ft1, ft2;

	bool operator < (const CompFiles & cp)
	{
		/*int ret = file1.CompareNoCase(cp.file1);
		if (ret == 0)
			return file2.CompareNoCase(cp.file2) >= 0;
		else
			return ret <= 0;*/
		return size < cp.size;
	}
};

struct sort_by_size
{
	sort_by_size(bool ascending)
		:_ascending (ascending) {};

	bool operator() (const CompFiles &f1, const CompFiles &f2) 
	{
		if (_ascending)
			return f1.size < f2.size;
		else
			return f2.size < f1.size;
	}

	private:
		bool _ascending;
};

struct sort_by_second_file 
{
	sort_by_second_file(bool ascending)
		:_ascending (ascending) {}

	bool operator () (const CompFiles &f1, const CompFiles &f2) 
	{	
		if (_ascending)
		{
			int ret = f1.file2.CompareNoCase(f2.file2);

			if (ret == 0)
				return f1.file1.CompareNoCase(f2.file1) >= 0;
			else
				return ret <= 0;
		} 
		else
		{
			int ret = f2.file2.CompareNoCase(f1.file2);

			if (ret == 0)
				return f2.file1.CompareNoCase(f1.file1) >= 0;
			else
				return ret <= 0;
		}		
	}

	private:
		bool _ascending;
};

struct sort_by_first_file 
{
	sort_by_first_file(bool ascending)
		:_ascending (ascending) {}

		bool operator () (const CompFiles &f1, const CompFiles &f2) 
		{	
			if (_ascending)
			{
				int ret = f1.file1.CompareNoCase(f2.file1);

				if (ret == 0)
					return f1.file2.CompareNoCase(f2.file2) >= 0;
				else
					return ret <= 0;
			} 
			else
			{
				int ret = f2.file1.CompareNoCase(f1.file1);

				if (ret == 0)
					return f2.file2.CompareNoCase(f1.file2) >= 0;
				else
					return ret <= 0;
			}		
		}

private:
	bool _ascending;
};

typedef std::vector <CompFiles> DupArr;

DupArr dups;
ULONGLONG size = 0;

CStringMap dupfolders;

#define _SPLIT_MAX_PATH 4096

TCHAR * StripFName (const TCHAR * full_path)
{
	static TCHAR path_buffer	[_SPLIT_MAX_PATH];
	_tcsncpy (path_buffer, full_path, _SPLIT_MAX_PATH);
	PathRemoveFileSpec (path_buffer);
	return path_buffer;
}

void addfolder (CString& filename) 
{
	CString folder = StripFName (filename);
	CStrIt it =  dupfolders.find (folder);
	if (it != dupfolders.end ())
		it->second++;
	else
		dupfolders.insert(CStrMapValue (folder, 1));			
}

bool compareFullFiles (FileDigest& f1, FileDigest& f2) 
{
	FileDigest f1x = f1;

	if (!CalcDigest(f1x, complete_digests, 0))
		return false;

	FileDigest f2x = f2;

	if (!CalcDigest(f2x, complete_digests, 0))
		return false;

	return f1x == f2x;
}

void calcDups ()
{
	dups.clear ();
	dupfolders.clear();

	//int i =0;
	DigestList::iterator i = digests.begin();

	while (i !=  digests.end ())
	{
		//int j = i+1;
		DigestList::iterator j = i;
		j++;

		while (j != digests.end () && (*i == *j))
		{
			if (compareFullFiles(*i, *j))
			{
				CompFiles cmp;
				cmp.file1 = i->fileName;
				cmp.file2 = j->fileName;
				cmp.ft1 = i->lastModifed;
				cmp.ft2 = j->lastModifed;

				cmp.size = i->size;
				size += i->size;

				dups.push_back (cmp);

				addfolder(cmp.file1);
				addfolder(cmp.file2);
			}

			j++;
		}

		if (i == j)
			j++;

		i = j;
	}	
}

bool comp_size_digest (FileDigest &f1, FileDigest &f2)
{
	return f1.size < f2.size;
}


void remove_unique (CWnd *pWnd)
{
	digests.sort(comp_size_digest);

	DigestList tmp_list;

	//int i =0;
	DigestList::iterator i = digests.begin();
	bool bPushed = false;
	calculated = 0;

	while (i !=  digests.end ())
	{
		//int j = i+1;
		DigestList::iterator j = i;
		j++;

		if (j == digests.end ())
			goto end_scan;

		calculated++;

		bPushed = false;
		while (j != digests.end () && (j->size == i->size))
		{
			if (InterlockedCompareExchange (&gStopFlag, 1,1) ==1)
				return;

			if (CalcDigest(*j)) 
			{
				if ((calculated % 100) == 0) 
				{
					_tcscpy(currentFolder, j->fileName);
					pWnd->SendMessage(WM_FD_CALC, 0, 0);
				}

				tmp_list.push_back (*j);
				bPushed = true;
			}		
			
			j++;
			calculated++;
		}

		if (bPushed) 
		{
			if (CalcDigest(*i)) 
			{				
				tmp_list.push_back (*i);
			}			
		}
		i = j;
	}	

end_scan:

	TRACE ("Old %d New %d\n", digests.size(),  tmp_list.size());

	tmp_list.sort ();
	digests.clear();
	digests.assign(tmp_list.begin(), tmp_list.end());
}


UINT __cdecl ThreadProc(LPVOID lpParam)
{
	CWnd *pWnd = CWnd::FromHandle ((HWND)lpParam);

	for (unsigned int i = 0; i < gSettings.m_Folders.size(); i++)
		scan (gSettings.m_Folders[i].m_Folder, pWnd);

	//std::sort (digests.begin(), digests.end());
	if (!InterlockedCompareExchange (&gStopFlag, 1,1) ==1) {
		digests.sort();
		calcDups ();
//		std::sort (dups.begin(), dups.end());
	}  else  {
		digests.clear();
		dups.clear();
	}

	pWnd->SendMessage( WM_FD_END, 0, 0);
	return TRUE;
}

UINT __cdecl ThreadProcFast (LPVOID lpParam)
{
	CWnd *pWnd = CWnd::FromHandle ((HWND)lpParam);

	for (unsigned int i = 0; i < gSettings.m_Folders.size(); i++)
	{		
		scan_fast (gSettings.m_Folders[i].m_Folder, pWnd);
		scanned = digests.size ();
	}
	
	scanned = digests.size ();
	pWnd->SendMessage(WM_FD_PROGRESS,0,0);

	//std::sort (digests.begin(), digests.end());

	if (!InterlockedCompareExchange (&gStopFlag, 1,1) ==1) {
		//digests.sort();
		remove_unique(pWnd);
		if (!InterlockedCompareExchange (&gStopFlag, 1,1) ==1) {
			calcDups ();			
		} else {
			digests.clear();
			dups.clear();
		}
	}  else  {
		digests.clear();
		dups.clear();
	}

	pWnd->SendMessage( WM_FD_END, 0, 0);
	return TRUE;
}

void CDupFindDlg::OnBnClickedScan()
{
	UpdateData();
	
	gMin = m_Min * 1024;
	gMax = m_Max * 1024;

	if (gMax == 0) gMax = ULLONG_MAX;

	gStopFlag = 0;

	OnStartEnable ();

	gSettings.m_Folder = m_Folder;
	
	files.clear();
	digests.clear();
	ClearStatusBar ();
	m_DupList.DeleteAllItems();
	size = 0;
	SetTitlePercent( -1);

	m_Stamp.Start();
	dirCount = 0;
	
	AfxBeginThread (ThreadProcFast, GetSafeHwnd());
}

void CDupFindDlg::ClearStatusBar ()
{
	for (int i = 0; i < 5; i++)
		m_bar.SetPaneText(i, _T(""));

}

void CDupFindDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_layout.reMap ();
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_TIME);

	if (::IsWindow (m_Folders.m_hWnd))
	{
		CRect rc;	
		m_Folders.GetClientRect (rc);
		m_Folders.SetColumnWidth (0, rc.Width() - 1);
	}	
}

bool IsDirectory (const TCHAR *file)
{
	DWORD attribs = GetFileAttributes(file);

	if (attribs == INVALID_FILE_ATTRIBUTES)
		return false;

	if ((attribs & FILE_ATTRIBUTE_DIRECTORY)== FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}

void CDupFindDlg::AddFolder (const TCHAR *folder)
{
	CStrOptions opt;
	opt.m_Folder = folder;
	SureBackSlash (opt.m_Folder);
	if (IsDirectory (opt.m_Folder)) 
	{
		m_Folders.InsertItem(gSettings.m_Folders.size(), opt.m_Folder, GetFileIcon(opt.m_Folder));
		gSettings.m_Folders.push_back (opt);
	}
}

void CDupFindDlg::OnBnClickedAdd()
{
	UpdateData ();
	AddFolder(m_Folder);
}

void CDupFindDlg::OnBnClickedDeleteall()
{
	m_Folders.DeleteAllItems();
	gSettings.m_Folders.clear();
	m_Delete.EnableWindow(FALSE);
}

LRESULT CDupFindDlg::OnStartScan(WPARAM wP, LPARAM lP)
{
	return NULL;
}

LRESULT CDupFindDlg::OnProgress(WPARAM wP, LPARAM lP)
{
	m_Stamp.End();
	
	m_bar.SetPaneText(0, currentFolder);

	CString tmp;
	tmp.Format(_T("Files: %d"), scanned);
	m_bar.SetPaneText(1, tmp);
	
		
	tmp = _T("Elapsed Time: ");
	tmp += m_Stamp.getTime();
	
	m_bar.SetPaneText(4, tmp);

	return NULL;
}

LRESULT CDupFindDlg::OnCalc(WPARAM wP, LPARAM lP)
{
	m_Stamp.End();

	m_bar.SetPaneText(0, currentFolder);

	CString tmp;
	tmp.Format(_T("%d / %d"), calculated , scanned );
	m_bar.SetPaneText(1, tmp);
	
	if (scanned)
	{
		int percent = ( calculated * 100 ) / scanned;
		SetTitlePercent(percent);
	}

	tmp = _T("Elapsed Time: ");
	tmp += m_Stamp.getTime();

	m_bar.SetPaneText(4, tmp);

	return NULL;
}

void CDupFindDlg::FilldupList ()
{
	TCHAR buf[1024];
	m_DupList.SetRedraw(FALSE);

	
	m_DupList.DeleteAllItems();
	m_DupList.SetItemCount(dups.size ());
	for (unsigned int i =0;  i < dups.size (); i++)
	{
		m_DupList.InsertItem (i, dups[i].file1);
		//pDlg->m_DupList.SetItemText (i, 1, it.dups[i].file2);
		m_DupList.SetItem (i, 1, LVIF_TEXT | LVIF_IMAGE, dups[i].file2, 0, 0,0,0);
		CString fmt;
		StrFormatByteSize64 (dups[i].size, buf, 1024);
		m_DupList.SetItemText (i, 2, buf);
	}

	m_DupList.SetRedraw(TRUE);
}

struct folder_count {
	CString folder;
	int	 num;
	bool operator < (const folder_count& fc)
	{
		return num > fc.num;
	}
};


LRESULT CDupFindDlg::OnEndScan(WPARAM wP, LPARAM lP)
{
	CString tmp;

	std::sort (dups.begin(), dups.end());

	m_Stamp.End();	
	
	tmp.Format(_T("Files: %d"), digests.size());
	m_bar.SetPaneText(1, tmp);

	tmp.Format(_T("Dups: %d"), dups.size());
	m_bar.SetPaneText(2, tmp);

	tmp = _T("Elapsed Time: ");
	tmp += m_Stamp.getTime();
	TCHAR buf[1024];
	StrFormatByteSize64 (size, buf, 1024);

	m_bar.SetPaneText(3, buf);

	m_bar.SetPaneText(4, tmp);
	UpdateData(FALSE);

	FilldupList ();

	CMenu menu;
	menu.CreateMenu();
	CMenu popup;
	popup.CreatePopupMenu();
	CStringMap::iterator itfolder = dupfolders.begin ();
	int pos = 1;
	std::vector <folder_count> fcs;

	while (itfolder != dupfolders.end())
	{
		folder_count fc;
		fc.folder = itfolder->first;
		fc.num = itfolder->second;
		fcs.push_back (fc);
		itfolder++;
	}

	std::sort (fcs.begin (), fcs.end ());

	for (int i = 0; i < fcs.size(); i++)
	{		
		CString menuItem ;
		menuItem.Format (_T(" [%d]"),   fcs[i].num);
		menuItem = fcs[i].folder + menuItem;
		popup.AppendMenu (MF_STRING | MF_BYPOSITION, pos++, menuItem);
	}
	
	if (popup.GetMenuItemCount())
	{
		menu.AppendMenu(MF_POPUP, (UINT) popup.m_hMenu, _T("xxx"));
		m_SelectFolderBtn.SetMenu(&menu);
		popup.Detach();		
	}

	m_bar.SetPaneText(0, _T("Done"));
	SetTitlePercent( -1);

	OnEndDisable ();
	return NULL;
}

void CDupFindDlg::OnLvnGetdispinfoDuplist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDupFindDlg::OnBnDoubleclickedSelectfolders()
{
	int index = m_SelectFolderBtn.GetCommand () -1;
	CStringMap::iterator itfolder = dupfolders.begin ();
	
	CString folder = m_SelectFolderBtn.GetTextFromIndex (index);	
	int pos = folder.ReverseFind (_T(' '));

	if (pos != -1) 
		folder.Delete (pos, folder.GetLength() - pos + 1);
						
	for (int i =0;  i < dups.size (); i++)
	{
		if (folder.CompareNoCase(StripFName (dups[i].file2)) == 0)
		{
			SelectItem (i, 1, true);			
			continue;
		}

		if (folder.CompareNoCase(StripFName (dups[i].file1)) == 0)		
			SelectItem (i, 0, true);						
		
	}	
}

void CDupFindDlg::OnBnDoubleclickedSelect() {

	int index = m_SelectBtn.GetCommand ();
			
	if (index != ID_SELECT_OLDER && index != ID_SELECT_NEWER)
		return;

	for (int i =0;  i < dups.size (); i++)
	{
		if (CompareFileTime (&dups[i].ft1, &dups[i].ft2) < 0)
		{
			if (index == ID_SELECT_OLDER)
				SelectItem (i, 0, true);
			else
				SelectItem (i, 1, true);
		} else {
			if (index == ID_SELECT_OLDER)
				SelectItem (i, 1, true);
			else
				SelectItem (i, 0, true);
		}
	}	
}

void CDupFindDlg::OnBnClickedDelete()
{
	CSelRowArray ar;
	CStringMap map;

	GetSelection (ar, map);

	if (ar.size())	
	{
		FileOperation(this, ar, FO_DELETE , FOF_ALLOWUNDO, NULL);
		OnBnClickedScan ();
	}
}

void CDupFindDlg::OnBnClickedDeselect()
{
	for (int i =0;  i < dups.size (); i++)
	{
		SelectItem (i, 0, false);
		SelectItem (i, 1, false);		
	}
}

void CDupFindDlg::OnNMClickDupList (NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE * hititem = ( NMITEMACTIVATE *)pNMHDR;

	DWORD dwpos = GetMessagePos();

	LVHITTESTINFO ht = {0};
	*pResult = 0;

	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);
	::MapWindowPoints(HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1);


	m_DupList.SubItemHitTest(&ht);

	if ((ht.flags != LVHT_ONITEMICON))
		return;

	LVITEM item;
	item.iItem = hititem->iItem;
	item.iSubItem = hititem->iSubItem;
	item.mask = LVIF_IMAGE;


	m_DupList.GetItem(&item);
	if (item.iImage == 0)
		item.iImage = 1;
	else
		if (item.iImage == 1)
			item.iImage = 0;

	m_DupList.SetItem(&item);

	*pResult = 1;

}

void CDupFindDlg::EnableDeleteFolder ()
{
	POSITION pos = m_Folders.GetFirstSelectedItemPosition();

	if (!pos) 	
		m_Delete.EnableWindow(FALSE);
	else	
		m_Delete.EnableWindow(TRUE);	
}

void CDupFindDlg::OnLvnItemchangedFolders(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;		
	EnableDeleteFolder ();
}

void CDupFindDlg::EnableControl (int id, bool enable) 
{
	CWnd *pWnd = GetDlgItem(id);
	pWnd->EnableWindow(enable);
}

void CDupFindDlg::EnableButtons (int *ids, int count, BOOL enable) 
{
	for (int i = 0; i < count; i++) 	
		EnableControl( ids[i], enable);	
}

void CDupFindDlg::OnHdnItemclickFolders(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (phdr->iItem == m_nSortCol)
		m_bAsc = !m_bAsc;

	m_nSortCol = phdr->iItem;

	CSelRowArray ar;
	CStringMap map;

	GetSelection (ar, map);

	switch (m_nSortCol)
	{
		case 0:
			std::sort (dups.begin(), dups.end(), sort_by_first_file(m_bAsc));
			break;
		case 1:
			std::sort (dups.begin(), dups.end(), sort_by_second_file(m_bAsc));
			break;
		case 2:
			std::sort (dups.begin(), dups.end(), sort_by_size(m_bAsc));
			break;
	}
	
	FilldupList ();

	for (int i =0;  i < dups.size (); i++)
	{
		
		CString file = dups[i].file2;		
		file.MakeLower ();
		
		LVITEM item;
		item.iItem = i;		
		item.mask = LVIF_IMAGE;
		item.iImage = 1;

		CStringMap::iterator twice_it = map.find(dups[i].file1);

		
		if (twice_it != map.end ())
		{
			item.iSubItem = 0;			
			m_DupList.SetItem(&item);
			
		}		

		twice_it = map.find(dups[i].file2);

		if (twice_it != map.end ())
		{
			item.iSubItem = 1;			
			m_DupList.SetItem(&item);
			
		}	
	}	
}

void CDupFindDlg::EnableDupButtons ()
{
	int buttons [] = {IDC_SELECT, IDC_DESELECT, IDC_WILDCARDS, IDC_SELECTFOLDERS, IDC_DELETE, IDC_DUPLIST};
	int count = dups.size();
	EnableButtons (buttons, sizeof (buttons) / sizeof (int), count != 0);
	EnableControl(IDC_STOP, false);
	EnableDeleteFolder ();
}

void CDupFindDlg::OnStartEnable ()
{
	int buttons [] = {IDC_SELECT, IDC_DESELECT, IDC_WILDCARDS, IDC_SELECTFOLDERS, IDC_DELETE, IDC_DELETEFOLDER,
			IDC_FOLDERS, IDC_FILE, IDC_BROWSE, IDC_ADD, IDC_DELETEALL, IDC_SCAN, IDC_DUPLIST, IDC_EDIT_MIN, IDC_EDIT_MAX
	};

	int count = dups.size();
	EnableControl(IDC_STOP, true);
	EnableButtons (buttons, sizeof (buttons) / sizeof (int), false);	
}

void CDupFindDlg::OnEndDisable ()
{
	EnableDupButtons ();
	int buttons [] = {IDC_FILE, IDC_BROWSE, IDC_ADD, IDC_DELETEALL, IDC_SCAN, IDC_DUPLIST, IDC_FOLDERS, IDC_EDIT_MIN, IDC_EDIT_MAX };

	int count = dups.size();	
	EnableButtons (buttons, sizeof (buttons) / sizeof (int), true);
	EnableDeleteFolder ();
}

void CDupFindDlg::OnBnClickedStop()
{
	EnableControl(IDC_STOP, false);
	InterlockedIncrement (&gStopFlag);
}

void CDupFindDlg::OnBnClickedDeletefolder()
{
	POSITION pos = m_Folders.GetFirstSelectedItemPosition();

	if (!pos) return;
	
	int nItem = m_Folders.GetNextSelectedItem(pos);
	m_Folders.DeleteItem (nItem);
	gSettings.m_Folders.erase(gSettings.m_Folders.begin() + nItem);

	if (nItem < m_Folders.GetItemCount()) 
	{
		m_Folders.SetItemState(nItem, LVIS_SELECTED ,LVIS_SELECTED );
	}
	else
	{
		if (nItem)
		{	
			nItem--;
			m_Folders.SetItemState(nItem, LVIS_SELECTED ,LVIS_SELECTED );
		}
	}	
}


void CDupFindDlg::SetTitlePercent (int percent)
{
	if (percent == m_Percent) return;

	m_Percent = percent;
	if (percent == -1) 
	{
		SetWindowText(_T("DupFind"));
	}
	else
	{
		CString tmp;
		tmp.Format(_T("%d%% - DupFind"), m_Percent);
		SetWindowText(tmp);
	}
}

void CDupFindDlg::OnBnClickedWildcards()
{
	CWildCardSelect dlg;

	if (dlg.DoModal () != IDOK)
		return;

	CString wildcards = dlg.m_WildCard;

	wildcards.MakeLower();

	for (int i =0;  i < dups.size (); i++)
	{
		
		CString file = dups[i].file2;		
		file.MakeLower ();
		
		LVITEM item;
		item.iItem = i;		
		item.mask = LVIF_IMAGE;
		item.iImage = 1;

		if (PathMatchSpec(file, wildcards))
		{			
			item.iSubItem = 1;			
			m_DupList.SetItem(&item);
			continue;
		}

		file = dups[i].file1;		
		file.MakeLower ();

		if (PathMatchSpec(file, wildcards))
		{		
			item.iSubItem = 0;			
			m_DupList.SetItem(&item);
		}				
	}	
	
}

void CDupFindDlg::GetSelection (CSelRowArray& ar, CStringMap& map) 
{
	ar.clear ();
	map.clear ();

	CStringMap::iterator twice_it;

	for (int i =0;  i < dups.size (); i++)
	{

		LVITEM item;
		item.iItem = i;
		item.iSubItem = 0;
		item.mask = LVIF_IMAGE;

		m_DupList.GetItem(&item);

		if (item.iImage == 1)
		{
			twice_it = map.find(dups[i].file1);
			if (twice_it == map.end ())
			{
				map.insert(CStrMapValue (dups[i].file1, 1));
				ar.push_back( CSelectRow (dups[i].file1));
			}			
		}

		item.iSubItem = 1;

		m_DupList.GetItem(&item);

		if (item.iImage == 1)
		{
			twice_it = map.find(dups[i].file2);
			if (twice_it == map.end ())
			{
				map.insert(CStrMapValue (dups[i].file2, 2));
				ar.push_back( CSelectRow (dups[i].file2));
			}			
		}
	}
}

void CDupFindDlg::SelectItem (int i, int subitem, bool bSelect) {
	LVITEM item;
	item.iItem = i;
	item.iSubItem = subitem;
	item.mask = LVIF_IMAGE;
	item.iImage = bSelect ? 1: 0;
	m_DupList.SetItem(&item);
}

void CDupFindDlg::OnClose()
{
	gSettings.m_Folder = m_Folder;

	CResizableDialog::OnClose();
}
