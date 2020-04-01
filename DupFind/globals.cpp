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
#include "globals.h"
#include "ExplorerXP.h"
#include "MainFrm.h"
#include "ExplorerXPDoc.h"
#include "ExplorerXPView.h"
#include "SysImageList.h"
#include "DirSize.h"   
#include "ThreadPool.h"
#include "SplitPath.h"
#include "Filter.h"
#include "shlwapi.h"
#include "Options.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDirSize dirs;
CThreadPool gPool;

CStatusBar *gpStatusBar;

UINT CF_PASTESUCCEEDED;
UINT CF_PERFORMEDDROPEFFECT;
UINT CF_PREFERREDDROPEFFECT;
UINT CF_LOGICALPERFORMEDDROPEFFECT;
UINT CF_FILENAME;

const TCHAR *CONST_MYCOMPUTER =_T("My Computer");
const TCHAR *CONST_RECYCLEBIN =_T("Recycle Bin");

extern CExplorerXPApp theApp;

CFilterMan gFilterMan;

typedef std::map <CString, SHFILEINFO, std::less <CString> > CShellInfoMap;
typedef CShellInfoMap::iterator icon_it;
typedef CShellInfoMap::value_type iconmap_value;

CShellInfoMap gIconMap;

typedef std::map <CString, CString, std::less <CString> > CFileInfoMap;
typedef CFileInfoMap::iterator filetype_it;	
typedef CFileInfoMap::value_type filetype_value;

CFileInfoMap  gFileTypeMap;

CString csMyComputer;
int		gnMyComputer;

UINT g_uCustomClipbrdFormat = RegisterClipboardFormat ( _T("HARMONDALE_EXPLORER_XP_A6B726FB_793C_4ca1_81B7_C09F9A55EF86") );

void SureBackSlash (CString &cs)
{
  if (cs.IsEmpty ())
	  return;

  if (cs[cs.GetLength()-1] != '\\')
	  cs += '\\';

}

void SureBackSlashEx (CString &cs) 
{
	if (cs.CompareNoCase(CONST_MYCOMPUTER) == 0)
		return;
	
	if (cs.CompareNoCase(CONST_RECYCLEBIN) == 0)
		return;

	SureBackSlash(cs);
}


bool IsDots (const TCHAR *dir_name)
{
	if (dir_name[0] == _T('.'))
		if (dir_name[1] == _T('\0') || (dir_name[1] == _T('.') && dir_name[2] == _T('\0')))		
			return true;
	
	return false;
}

CExplorerXPView *GetCurrentView ()
{
	CFrameWnd *pWnd = (CFrameWnd *) AfxGetMainWnd ();
	
	if (!pWnd)
		return NULL;

	CFrameWnd *pFrame = pWnd->GetActiveFrame ();
	if (!pFrame)
		return NULL;
	
	CExplorerXPView *pView = (CExplorerXPView *)pFrame->GetActiveView ();

	return pView;

}


void SetRoot (const TCHAR *root)
{
	CExplorerXPView *pView = GetCurrentView ();

	if (pView)
	{		
		pView->Fill (root);		
		//pView->SetFocus ();
	}
	else
		OpenFolder (root);	
}

CImageList *GetSysImageList ()
{
	return & gSysImageList.m_ImageList;
}


int GetFileIcon (const TCHAR *file_name)
{	
	icon_it it= gIconMap.find ( file_name);

	if (it != gIconMap.end ())
		return it->second.iIcon;
	
	SHFILEINFO    sfi;

    SHGetFileInfo(file_name, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_TYPENAME );
	gIconMap.insert (iconmap_value(file_name, sfi));
	return sfi.iIcon;
}

const TCHAR * GetFileType (const TCHAR *parent_folder, const TCHAR *file_name)
{
	CString path = parent_folder;
	path += file_name;

	return GetFileType (path);
}

const TCHAR * GetFileType (const TCHAR *file_name)
{	
    CSplitPath path (file_name);
    const TCHAR *ext = path.GetExt();
    if (!ext[0])
    {
        if (IsDirectory(file_name))
            return _T("File Folder");
        else
            return _T("File");
    }
    
    filetype_it it= gFileTypeMap.find (ext);
    if (it != gFileTypeMap.end())
        return it->second;

    static SHFILEINFO    sfi;

    SHGetFileInfo(file_name, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_TYPENAME );
    if (!sfi.szTypeName[0]) 
    {
        if (ext[0])
        {
            sfi.szTypeName[0]= 0;
            _tcsncat (sfi.szTypeName, ext + 1, 70);
            _tcsupr  (sfi.szTypeName);
            _tcsncat (sfi.szTypeName, _T(" File"), 79);
        }
        else
            _tcscpy(sfi.szTypeName, _T("File"));        
    }

    gIconMap.insert (iconmap_value(file_name, sfi));
    gFileTypeMap.insert(filetype_value(ext, sfi.szTypeName));

	return sfi.szTypeName;
}

void StartSizeThread ()
{
	if (gPool.IsThreadWorking (CYCLER_THREAD))
		return;

	if (!IsNotInFOP())
		return;

	if (!dirs.IsStackEmpty ())
	{
		dirs.SortStack();
		gPool.StartThread (CYCLER_THREAD, &dirs, &CDirSize::Cycler);
	//	dirs.Init ();	
	}
}

void StopSizeThread ()
{
	dirs.ClearStack();	
}

bool GetDirSize (const TCHAR *parent_folder, const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime)
{	
	return dirs.GetSize (parent_folder, folder, size, sizeondisk, fTime);	
}

bool GetDirSizeLight (const TCHAR *parent_folder, const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime)
{
	return dirs.GetSizeLight (parent_folder, folder,size, sizeondisk, fTime);	
}

bool GetDirSizeLight (const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime)
{
	return dirs.GetSizeLight (folder,size, sizeondisk, fTime);
}

void RegisterWindow (CExplorerXPView *pView)
{
	dirs.AddWindow (pView);	
}

void UnRegisterWindow (CExplorerXPView *pView)	
{
	dirs.DeleteWindow (pView);
}
void RegisterWindow (HWND hWnd)
{
	dirs.AddWindow (hWnd);	
}

bool gbSync = true;

void SyncUI (const TCHAR *path)
{	
	if (!gbSync)
		return;

	CMainFrame *pFrame = ((CMainFrame*)AfxGetMainWnd());
	pFrame->m_wndDlgBar.SetFolder (path);
	m_DriveTree.Find (path);
}

void DisableSync ()
{
	gbSync = false;
}

void EnableSync ()
{
	gbSync = true;
}

bool IsExist (const TCHAR *file)
{
	if (GetFileAttributes(file) == INVALID_FILE_ATTRIBUTES)
		return false;
	return true;
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


void RegisterShellClipboardFormats ()
{
    CF_PERFORMEDDROPEFFECT = RegisterClipboardFormat (CFSTR_PERFORMEDDROPEFFECT);
    CF_PREFERREDDROPEFFECT = RegisterClipboardFormat (CFSTR_PREFERREDDROPEFFECT);
    CF_LOGICALPERFORMEDDROPEFFECT = RegisterClipboardFormat (CFSTR_LOGICALPERFORMEDDROPEFFECT);    
	CF_FILENAME = RegisterClipboardFormat (CFSTR_FILENAME);
}


bool ReadDropEffect (COleDataObject *pObject, DROPEFFECT &dropEffect)
{
	if (pObject->IsDataAvailable (CF_PREFERREDDROPEFFECT))
	{
		HANDLE hGlobalDropEffect = pObject->GetGlobalData (CF_PREFERREDDROPEFFECT);
        if (hGlobalDropEffect!= NULL)
        {
		    DWORD *lpGlobalDropEffect = (DWORD*)GlobalLock(hGlobalDropEffect);		
            if (lpGlobalDropEffect)
		        dropEffect = (DROPEFFECT)*lpGlobalDropEffect;		
		    GlobalUnlock (hGlobalDropEffect);
            return true;	
        }		
	}
	return false;
}

//#define ID_COPY                         32781
//#define ID_DELETE                       32782
//#define ID_UNDO                         32783
//#define ID_MOVE                         32784
//#define ID_PASTE                        32790
//#define ID_EDIT_SELECTALL               32791
//#define ID_EDIT_INVERTSELECTION         32792
//#define ID_CUT                          32793
//#define ID_RENAME                       32814


bool ReadHDropData (COleDataObject *pObject, CSelRowArray &ar, CString &source_folder)
{
	HGLOBAL     hg;
	HDROP       hdrop;
	UINT        uNumFiles;
	TCHAR       szNextFile [MAX_PATH];
	CString    csTemp;

	TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
	
	source_folder.Empty ();

    // Get the HDROP data from the data object.
	
    hg = pObject->GetGlobalData ( CF_HDROP );
    
    if ( NULL == hg )        
        return false;

    hdrop = (HDROP) GlobalLock ( hg );

    if ( NULL == hdrop )            
        return false;

	uNumFiles = DragQueryFile ( hdrop, -1, NULL, 0 );
	
	TRACE("Drag NumFiles %d", uNumFiles);

    for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
        {		
			if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
				{				   
					//TRACE (_T("Cut File --> %s\n"), szNextFile);
					_tsplitpath( szNextFile, drive, dir, fname, ext ); 

					if (source_folder.IsEmpty ())
					{
						source_folder = drive;
						source_folder += dir;
						SureBackSlash (source_folder);
					}
				
					csTemp = szNextFile;

					//csTemp.MakeLower ();
					ar.push_back (CSelectRow (csTemp, _T("xoxo"), 1));					
				}
        }  

	return true;
}

CString XFormatNumber(LPCTSTR lpszNumber, int nFracDigits)
{
	ASSERT(lpszNumber);
	ASSERT(lpszNumber[0] != 0);

	CString str = lpszNumber;

	NUMBERFMT nf;
	memset(&nf, 0, sizeof(nf));


	_TCHAR szBuffer[10];
	_TCHAR szDecSep[10];
	_TCHAR szThousandsSep[10];

	// set decimal separator string from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecSep, sizeof(szDecSep)-1);
	nf.lpDecimalSep = szDecSep;

	// set thousand separator string from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szThousandsSep, sizeof(szThousandsSep)-1);
	nf.lpThousandSep = szThousandsSep;

	// set leading zeroes from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILZERO, szBuffer, sizeof(szBuffer)-1);
	nf.LeadingZero = _ttoi(szBuffer);

	// set grouping of digits from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, szBuffer, sizeof(szBuffer)-1);
	nf.Grouping = _ttoi(szBuffer);

	// set negative number mode from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, szBuffer, sizeof(szBuffer)-1);
	nf.NegativeOrder = _ttoi(szBuffer);

	int dp = nFracDigits;

	if (dp == -1)
	{
		// set number of fractional digits from input string
		CString strDecSep = szDecSep;
		dp = str.Find(szDecSep);
		dp = (dp == -1) ? 0 : str.GetLength() - dp - strDecSep.GetLength();
	}
	nf.NumDigits = dp;

	int nSize = (_tcslen(lpszNumber) * 3) + 100;
	_TCHAR *pszFormattedNumber = new _TCHAR [nSize];
	ASSERT(pszFormattedNumber);

	if (GetNumberFormat(LOCALE_USER_DEFAULT, 
		0, 
		lpszNumber, 
		&nf, 
		pszFormattedNumber, 
		nSize-1))
	{
		str = pszFormattedNumber;
	}

	if (pszFormattedNumber)
		delete [] pszFormattedNumber;

	return str;
}

CString size_to_string (ULONGLONG size)
{
	TCHAR buf[128];
	switch (gOptions.m_SizeIn)
	{
		case SIZE_IN_BYTES:
		{
			int k = 1;	
			int max_chars = 127;
			memset (buf, _T(' '), max_chars-1);
			buf[max_chars-1]=0;

			if (size)
				while (size)
				{		
					buf[max_chars - k-1] = (TCHAR) (size % 10) + _T('0');
					size /= 10;	
					k++;
				}
			else
			{	
				buf[max_chars - k-1] = 	_T('0');
				k++;
			}

			//return (LPCTSTR) & buf[max_chars - k];
			return XFormatNumber ((LPCTSTR) & buf[max_chars - k], -1);
		} break;

		case SIZE_IN_KBYTES:
			StrFormatKBSize (size, buf, 127);
			break;
		default:
			StrFormatByteSize64 (size, buf, 127);
			break;
	}	
	return buf;
}


CString GetDrive (const TCHAR *path)
{
	TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];

	_tsplitpath( path, drive, dir, fname, ext ); 

	return CString (drive);
}

HICON GetSpecialFolderSmallIcon (int nFolder)
{
    ITEMIDLIST*   pidl_folder;

    if (SHGetSpecialFolderLocation (NULL, nFolder, &pidl_folder) != NOERROR )
        return NULL;


    SHFILEINFO     sfi;
    
	if (!SHGetFileInfo ((LPCTSTR)pidl_folder, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON  ))
        return NULL;

    CImageList *pImgList = GetSysImageList();
    int x, y;

    ImageList_GetIconSize (pImgList->m_hImageList, &x, &y);

    return pImgList->ExtractIcon (sfi.iIcon);
}


void OpenFolder (const TCHAR *folder_name)
{
    CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd  ();

    if (pFrame->m_wndDocSelector.SelectDocument (folder_name))
        return;

    theApp.OpenDocumentFile (folder_name);
}

void DoMyComputerMenu (NMHDR* pNotifyStruct)
{	
	CMenu menu;
	CMenu *pPopup;
	
	NMTOOLBAR* pNMToolBar = ( NMTOOLBAR* )pNotifyStruct;
	
	VERIFY( menu.LoadMenu( IDR_MYCOMPUTER  ) );
	pPopup = menu.GetSubMenu( 0 );

	if ( pPopup != 0 )
		{
			CRect rc;
			::SendMessage( pNMToolBar->hdr.hwndFrom, TB_GETRECT, pNMToolBar->iItem, ( LPARAM )&rc );
			rc.top = rc.bottom;
			::ClientToScreen( pNMToolBar->hdr.hwndFrom, &rc.TopLeft() );
			
			pPopup->SetDefaultItem (ID_MYCOMPUTER);
			pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left, rc.top, AfxGetMainWnd () );
		}
}

void DoRecycleMenu (NMHDR* pNotifyStruct)
{
	CMenu menu;
	CMenu *pPopup;
	
	NMTOOLBAR* pNMToolBar = ( NMTOOLBAR* )pNotifyStruct;
	
	VERIFY( menu.LoadMenu( IDR_RECYCLE) );
	pPopup = menu.GetSubMenu( 0 );

	if ( pPopup != 0 )
		{
			CRect rc;
			::SendMessage( pNMToolBar->hdr.hwndFrom, TB_GETRECT, pNMToolBar->iItem, ( LPARAM )&rc );
			rc.top = rc.bottom;
			::ClientToScreen( pNMToolBar->hdr.hwndFrom, &rc.TopLeft() );
			
			pPopup->SetDefaultItem (ID_RECYCLEBIN);
			pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left, rc.top, AfxGetMainWnd () );
		}
}

HRESULT CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink, LPCTSTR lpszDesc) 
{ 
    HRESULT hres; 
    IShellLink* psl; 
 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl); 

    if (SUCCEEDED(hres))
	{ 
        IPersistFile* ppf; 
 
        // Set the path to the shortcut target and add the 
        // description. 
        psl->SetPath(lpszPathObj); 
		if (lpszDesc)
			psl->SetDescription(lpszDesc); 
 
       // Query IShellLink for the IPersistFile interface for saving the 
       // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
 
        if (SUCCEEDED(hres))
		{ 
            WCHAR wsz[MAX_PATH]; 
 
        // Ensure that the string is Unicode. 
#ifdef _UNICODE
			_tcscpy(wsz, lpszPathLink);
#else
          MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 
#endif
 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE); 
            ppf->Release(); 
        } 
        psl->Release(); 
    } 
    return hres; 
} 


bool CreateShortcuts (CSelRowArray &ar, const TCHAR *dest_folder)
{
	TCHAR shortcut_name[MAX_PATH];
	BOOL bMustCopy;
	
	for (unsigned int i = 0; i < ar.size (); i++)
	{
		if (SHGetNewLinkInfo (ar[i].m_Path, dest_folder, shortcut_name, &bMustCopy, SHGNLI_PREFIXNAME))		
			CreateLink (ar[i].m_Path, shortcut_name, NULL);		
	}
	return false;
}


TCHAR * AllocateNullPaths (CSelRowArray &ar, int& buf_size)
{
	buf_size = 0;

	if (ar.size () == NULL)
		return NULL;

	for (unsigned int i = 0; i < ar.size (); i++)
		buf_size += ar[i].m_Path.GetLength ()  + 1;
	
	buf_size += 1; // the last NULL;

	TCHAR *pFrom = new TCHAR [buf_size];

	TCHAR* ptr = pFrom;
	buf_size *= sizeof (TCHAR);

	memset (pFrom, 0, buf_size);

	for ( unsigned i = 0; i < ar.size (); i++)
	{
		_tcscpy (ptr, (LPCTSTR) ar[i].m_Path);
		ptr += ar[i].m_Path.GetLength ()  + 1;
	}

	
	return  pFrom;
}

TCHAR * AllocateNullPath (const TCHAR *str, int& buf_size)
{
	buf_size  = (_tcsclen (str) + 2 ) * sizeof (TCHAR);

	TCHAR *pFrom = new TCHAR [buf_size];

	TCHAR* ptr = pFrom;

	memset (pFrom, 0, buf_size);
		
	_tcscpy (ptr, str);

	return  pFrom;
}

UINT __cdecl FileOperationInternal (LPVOID info)
{
	AddFOPThread();	
	SHFILEOPSTRUCT *fo = (SHFILEOPSTRUCT *)info;
	SHFileOperation (fo);

	delete [] (TCHAR*)fo->pFrom;

	if (fo->pTo)
		delete [] (TCHAR*)fo->pTo;

	delete fo;

	RemoveFOPThread();

	return TRUE;
}

bool FileOperation (CWnd *pWnd, CSelRowArray &ar, UINT wFunc, FILEOP_FLAGS fFlags, const TCHAR *pTo)
{		
	if (ar.size () == 0) return false;

	int size = 0;

	TCHAR *pFrom = AllocateNullPaths (ar, size);
	
	SHFILEOPSTRUCT *fo = new SHFILEOPSTRUCT;
	
	fo->hwnd = pWnd->GetSafeHwnd ();

	fo->wFunc = wFunc;
	fo->fFlags = fFlags;
	fo->pFrom = pFrom;
	if (pTo)
	{
		fo->pTo = new TCHAR [_tcsclen(pTo)+1];
		_tcscpy ((TCHAR*)fo->pTo, pTo);
	}
	else
		fo->pTo = NULL;
	
	fo->lpszProgressTitle = NULL;

//	int ret = SHFileOperation (&fo);
	AfxBeginThread (FileOperationInternal, (LPVOID)fo);

	//return ret == 0;
	return true;
}


bool IsSubFolder (const TCHAR *folder1, const TCHAR *folder2)
{
	CString fld1 = folder1;
	CString fld2 = folder2;
	
	fld1.MakeLower ();
	fld2.MakeLower ();

	int pos = fld1.Find (fld2);
	if (pos == 0)
		return true;

	return false;	
}

void LastErrorMessage ()
{
	LPVOID lpMsgBuf;
	FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL 
					);
	AfxMessageBox ((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONINFORMATION);	
	LocalFree( lpMsgBuf );
}

void CopyToClipboard (CSelRowArray &ar, DWORD effect)
{

  HANDLE hGlobalDropEffect, hGlobalDrop, hGlobalFileName;
  BYTE *  lpGlobalDrop, *lpGlobalFileName;
  DWORD *lpGlobalDropEffect;
  DROPFILES df;

  TCHAR *data;
  int size = 0;

  if (ar.size () == NULL)
	  return;
  
  if (!OpenClipboard (AfxGetMainWnd ()->GetSafeHwnd ()))
      return;

  if( !EmptyClipboard() )
      return;

  hGlobalDropEffect = GlobalAlloc(GHND, sizeof (DWORD));

  if (hGlobalDropEffect != NULL )
	{
	    lpGlobalDropEffect = (DWORD*)GlobalLock(hGlobalDropEffect);
		*lpGlobalDropEffect = effect;
		GlobalUnlock(hGlobalDropEffect);

		data = AllocateNullPaths (ar, size);
		hGlobalDrop = GlobalAlloc(GHND, size + sizeof(df));

        if (hGlobalDrop != NULL) 
        {		  
		  lpGlobalDrop = (BYTE*)GlobalLock(hGlobalDrop);
          df.pFiles = sizeof (df);
#ifdef UNICODE
          df.fWide = 1;
#else
		 df.fWide = 0;	
#endif
          
          memcpy (lpGlobalDrop, &df, sizeof (df));
		
          lpGlobalDrop +=  sizeof(df);
          
          memcpy (lpGlobalDrop, data, size);

          GlobalUnlock(hGlobalDrop);
          
          hGlobalFileName = GlobalAlloc(GHND, size);
          if (hGlobalFileName != NULL)
		  {	  
            lpGlobalFileName = (BYTE*)GlobalLock(hGlobalFileName);
            memcpy (lpGlobalFileName, data, size);
            GlobalUnlock(hGlobalFileName);
          }
          // Copie dans le presse-papiers
          SetClipboardData(CF_PREFERREDDROPEFFECT, hGlobalDropEffect);
          SetClipboardData(CF_HDROP, hGlobalDrop);
          SetClipboardData(CF_FILENAME, hGlobalFileName) ; 
		  delete [] data;
        }
	}     
    CloseClipboard ();
}


void LoadFilters ()
{
	gFilterMan.load ();
}


void SaveFilters ()
{
	gFilterMan.save ();
}

void GetExePath (CString &cs)
{
	TCHAR	f_name[256];
	TCHAR	drive[5];
	TCHAR	dir[256];
	TCHAR	name[50];
	TCHAR	ext[10];

	GetModuleFileName(AfxGetInstanceHandle(), f_name, sizeof(f_name));
	_tsplitpath(f_name, drive, dir, name, ext);
	_tmakepath(f_name, drive, dir, EMPTYSTR, EMPTYSTR);
	cs = f_name;
}


void TabFrom (int idView, int idDirection)
{
	CMainFrame *pWnd = (CMainFrame *) AfxGetMainWnd ();
	
	if (!pWnd) return;	

	pWnd->TabFrom (idView, idDirection);
}

bool isWindowsXP()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

	if(!GetVersionEx(&osvi)) return false; //this is bad.

	return (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		osvi.dwMajorVersion == 5 &&
		osvi.dwMinorVersion == 1);
}

char* GetLocalFile (const char *new_name, const char *new_ext)
{
	static CHAR	f_name[_SPLIT_MAX_PATH];
	CHAR	drive[_SPLIT_MAX_DRIVE];
	CHAR	dir[_SPLIT_MAX_DIR];
	CHAR	name[_SPLIT_MAX_FNAME];
	CHAR	ext[_SPLIT_MAX_EXT];

	GetModuleFileNameA(AfxGetInstanceHandle(), f_name, sizeof(f_name));
	_splitpath(f_name, drive, dir, name, ext);
	_makepath(f_name, drive, dir, new_name, new_ext);
	return f_name;
}

DROPEFFECT CalcDropEffect (CSelRowArray &ar, const TCHAR * root, const TCHAR * drop_source,  DWORD dwKeyState)
{
	DROPEFFECT dwEffect;

	if (!ar.size ())
		return DROPEFFECT_NONE;

	if ((dwKeyState & MK_ALT) == MK_ALT)
		dwEffect = DROPEFFECT_LINK; 
	else if ((dwKeyState & MK_CONTROL) == MK_CONTROL)
		dwEffect = DROPEFFECT_COPY;      
	else if ((dwKeyState & MK_SHIFT) == MK_SHIFT)
		dwEffect = DROPEFFECT_MOVE;      
	else 
	{
		CString d1 = GetDrive (root);
		CString d2 = GetDrive (drop_source);

		if (d1.CompareNoCase (d2) == 0) 
			dwEffect = DROPEFFECT_MOVE;
		else
			dwEffect = DROPEFFECT_COPY;
	}

	if (dwEffect == DROPEFFECT_MOVE)
		if (_tcsicmp (root, drop_source) == 0) 
			return  DROPEFFECT_NONE;

	if ((dwEffect == DROPEFFECT_MOVE) ||  (dwEffect == DROPEFFECT_COPY) )
		for (unsigned int i = 0; i < ar.size (); i++)
		{
			if (IsSubFolder (root, ar[i].m_Path))
				return  DROPEFFECT_NONE;
		}
		return dwEffect;
}

void CopyTextToClip (CString &cs)
{
	if (AfxGetMainWnd()->OpenClipboard ())
	{
		EmptyClipboard ();
		HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE |GMEM_DDESHARE, (cs.GetLength ()+1) * sizeof (TCHAR) );
		TCHAR *ptr = (TCHAR *)GlobalLock(hText);
		_tcscpy(ptr, cs);
		GlobalUnlock(hText);
#ifdef UNICODE
		SetClipboardData (CF_UNICODETEXT, hText);
#else
		SetClipboardData (CF_TEXT, hText);
#endif
		CloseClipboard();			
		GlobalFree(hText);
	}
}

static LONG volatile lFOPS = 0;
void AddFOPThread ()
{
	InterlockedIncrement (&lFOPS);
}

void RemoveFOPThread ()
{
	InterlockedDecrement (&lFOPS);
}

bool IsNotInFOP ()
{
	if (InterlockedCompareExchange (&lFOPS, 0, 0) == 0)
		return true;

	return false;
}