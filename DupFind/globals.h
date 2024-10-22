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
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#pragma once

#include "FileInfo.h"
#include "InOut.h"

//#defines 
// used for drag & drop   and drag scroll
#define RECT_BORDER	10

// threads
#define CYCLER_THREAD 100


// common messages

#define WM_EXP_SYNC           (WM_USER + 500) // syncronize the views
#define WM_EXP_UPDATE         (WM_USER + 501) // folder was deleted/copied/cut e.t
#define WM_EXP_CLIPCHANGED    (WM_USER + 502) // clipboard was changed
#define WM_EXP_RECYCLEUPDATE  (WM_USER + 503) // clipboard was changed
#define WM_EXP_FILEREMOVED    (WM_USER + 504) // file was removed - do something faster than re-reading all ?
#define WM_EXP_FILERENAMED    (WM_USER + 505) // just change the name
#define WM_EXP_FILEADDED      (WM_USER + 506) // hmm
#define WM_EXP_OPTIONS        (WM_USER + 507) // Options has changed
#define WM_EXP_OPENFROMSHELL  (WM_USER + 508) // shell context menu	
#define WM_EXP_HARDUPDATE	  (WM_USER + 509) // reload all folders;
#define WM_EXP_SETSTATE	      (WM_USER + 510) // set state - for folders only

#define WM_REN_VALUECNAGED    (WM_USER + 1000) // send from multi_rename children to the parent

#define FORWARD  1
#define BACKWARD 2
#define MAIN_VIEW	0
#define ADDRESS_BAR 1
#define MAIN_FOLDER 2


void TabFrom (int idView, int idDirection);

typedef struct tagRename {
	const TCHAR* _oldName;
	const TCHAR* _newName;
} RENAMEFILESTRUCT;

// shell notifications defined somewhere in the future SDKs ????

#define WM_SHELLNOTIFY						(WM_USER + 5)
#define WM_SHELLNOTIFYRBINDIR				(WM_USER + 6)

//function tracing macros

#ifdef _DEBUG
#define TRFUN InOut inOut
#endif

#ifndef _DEBUG
#define TRFUN(name) 
#endif

class CExplorerXPView;

void SetRoot (const TCHAR *root);
CImageList *GetSysImageList ();
int GetFileIcon (const TCHAR *file_name);
const TCHAR * GetFileType (const TCHAR *file_name);


//const TCHAR *filesize_to_str (ULONGLONG filesize);
//const TCHAR *filesize_to_str2 (ULONGLONG filesize);

CString size_to_string (ULONGLONG size);

//void GetFolder (std::vector <WIN32_FIND_DATA> &array, const TCHAR *path);
void GetFolder (std::vector <CFileInfo> &array, const TCHAR *path);
bool GetDirSize (const TCHAR *parent_folder, const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime);
bool GetDirSizeLight (const TCHAR *parent_folder, const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime);
bool GetDirSizeLight (const TCHAR *folder, ULONGLONG &size, ULONGLONG &sizeondisk, FILETIME &fTime);

const TCHAR * GetFileType (const TCHAR *parent_folder, const TCHAR *file_name);

void SureBackSlash (CString &cs);
void SureBackSlashEx (CString &cs);
void RegisterWindow (HWND hWnd);
void RegisterWindow (CExplorerXPView *pView);
void UnRegisterWindow (CExplorerXPView *pView);	

void StartSizeThread ();
void StopSizeThread ();

bool IsExist (const TCHAR *file);
bool IsDirectory (const TCHAR *file);
bool IsDots (const TCHAR *dir_name);
bool IsSubFolder (const TCHAR *folder1, const TCHAR *folder2);


CExplorerXPView *GetCurrentView ();

extern CStatusBar *gpStatusBar;
extern CString csMyComputer;
extern int	   gnMyComputer;

class CDriveInfo
{
	public:
	CDriveInfo () : m_nType(0), m_dwMediaID(0), m_TotalSize (0), m_FreeSpace(0),m_nImage(-1) {};
	CString m_Name;
	CString m_Type;
	CString m_Path;
	UINT	m_nType;
	DWORD   m_dwMediaID;

	ULONGLONG m_TotalSize;
	ULONGLONG m_FreeSpace;
	int m_nImage;
	const TCHAR* as_text (int column);
};

typedef std::vector <CDriveInfo> CDriveArray;

//LPITEMIDLIST Pidl_Concatenate(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
//ULONGLONG GetSizeOnDisk (const TCHAR *path, WIN32_FIND_DATA &fileinfo, ULONGLONG &size);

void GetDrives (CDriveArray &array);
void SyncUI (const TCHAR *folder);

void LoadFilters ();
void SaveFilters ();

extern UINT g_uCustomClipbrdFormat; // custom clipboard format

void DisableAcc ();
void EnableAcc ();
CString GetDrive (const TCHAR *path);
void Split (CString &path, CString &parent, CString &name);


struct CSelectRow
{
	CSelectRow (const TCHAR *path, const TCHAR *name = NULL, int row = 0)
	{
		m_Path = path;
		if (name) m_Name = name;
		m_nRow = row;
	}

	CSelectRow (int row, const TCHAR *name)
	{
		m_Name = name;
		m_nRow = row;
	}

	bool operator < (const CSelectRow & row)
	{
		return m_Name <= row.m_Name;
	}

	CString m_Path;
	CString m_Name;
	int		m_nRow;
};


typedef std::vector<CSelectRow> CSelRowArrayTmpl;

class CSelRowArray : public CSelRowArrayTmpl
{
public:	
	CString m_FocusCell;
	CString m_Path;	
};


// shell clipboard formats 
extern UINT CF_PASTESUCCEEDED;
extern UINT CF_PERFORMEDDROPEFFECT;
extern UINT CF_PREFERREDDROPEFFECT;
extern UINT CF_LOGICALPERFORMEDDROPEFFECT;
extern UINT CF_FILENAME;

void RegisterShellClipboardFormats ();

bool ReadDropEffect (COleDataObject *pObject, DROPEFFECT &dropEffect);
bool ReadHDropData (COleDataObject *pObject, CSelRowArray &ar, CString &source_folder);
HICON GetSpecialFolderSmallIcon (int nFolder);

void OpenFolder (const TCHAR *folder_name);

void DoMyComputerMenu (NMHDR* pNotifyStruct);
void DoRecycleMenu (NMHDR* pNotifyStruct);

bool CreateShortcuts (CSelRowArray &ar, const TCHAR *dest_folder);
TCHAR * AllocateNullPaths (CSelRowArray &ar, int& buf_size);
TCHAR * AllocateNullPath (const TCHAR *str, int& buf_size);
bool FileOperation (CWnd *pWnd, CSelRowArray &ar, UINT wFunc, FILEOP_FLAGS fFlags, const TCHAR *pTo);
void RenameFiles (CSelRowArray &ar, const TCHAR *path);
void LastErrorMessage ();
void CopyToClipboard (CSelRowArray &ar, DWORD effect);

extern const TCHAR *CONST_MYCOMPUTER;
extern const TCHAR *CONST_RECYCLEBIN;
void GetExePath (CString &cs);

typedef std::vector <CString> CStringAr;
typedef std::vector <int> IntAr;

COLORREF HLS_TRANSFORM (COLORREF rgb, int percent_L, int percent_S);

bool isWindowsXP();
char* GetLocalFile (const char *filename, const char *ext);

#define DELIM  _T("|*|")

int GetFolderIconEx (const TCHAR *folder);
DROPEFFECT CalcDropEffect (CSelRowArray &ar, const TCHAR * root, const TCHAR * drop_source,  DWORD dwKeyState);
void DisableSync ();
void EnableSync ();
void CopyTextToClip (CString &cs);
void AddFOPThread ();
void RemoveFOPThread ();
bool IsNotInFOP ();

bool GetLastWriteTime (const TCHAR *path, FILETIME &writetime);

#endif 