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
// DupFindDlg.h : header file
//

#pragma once

#include "inout.h"
#include "afxcmn.h"

#include "menubutton.h"
#include "afxwin.h"
#include "LayoutManager.h"
#include "ACEdit\ACEdit.h"
#include "ListControlMem.h"
#include "WildCardSelect.h"
#include "MyDropTarget.h"
#include "FileOperation.h"

#include "Separators\StaticSeparator.h"

// CDupFindDlg dialog
#define WM_FD_BEGIN		(WM_APP + 5)
#define WM_FD_PROGRESS  (WM_APP + 6)
#define WM_FD_END       (WM_APP + 7)
#define WM_FD_CALC		(WM_APP + 8)

#include "ResizableLib\ResizableDialog.h"

typedef std::map <CString, int, std::less <CString> > CStringMap;
typedef CStringMap::value_type CStrMapValue;
typedef CStringMap::iterator CStrIt;


class CDupFindDlg : public CResizableDialog
{
// Construction
public:
	CDupFindDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DUPFIND_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
	void GetSelection (CSelRowArray& ar, CStringMap& map);

public:
	afx_msg void OnBnClickedScan();	
	void AddFolder (const TCHAR *folder);
    void FilldupList ();
	void EnableButtons (int *ids, int count, BOOL enable);
	void EnableDupButtons ();
	void OnStartEnable ();
	void OnEndDisable ();
	void EnableControl (int id, bool enable);
	void EnableDeleteFolder ();
	void SetTitlePercent (int percent);
	CLayoutManager m_layout;
	
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDeleteall();	

	afx_msg LRESULT OnStartScan(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnProgress(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnEndScan(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnCalc(WPARAM wP, LPARAM lP);
	afx_msg void OnLvnGetdispinfoDuplist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnDoubleclickedSelectfolders();
	afx_msg void OnBnDoubleclickedSelect();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedDeselect();
	afx_msg void OnNMClickDupList(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnLvnItemchangedFolders(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickFolders(NMHDR *pNMHDR, LRESULT *pResult);


	void SelectItem (int item, int subitem, bool bSelect);

	//CEvent m_StopEvent;
	int m_nSortCol;
	bool m_bAsc;
	CButton m_Delete;	
	afx_msg void OnBnClickedStop();
	CACEdit m_EditFile;
	afx_msg void OnBnClickedDeletefolder();
	ULONGLONG m_Min;
	ULONGLONG m_Max;
	CString   m_NoNumber;
	CStaticSeparator m_FoldersSep;
	CStaticSeparator m_AddFoldersSep;
	CStaticSeparator m_SelectSep;
	CStaticSeparator m_SearchSep;
	CMyDropTarget	m_DropTarget;
	void ClearStatusBar ();
	CString m_Folder;
	CString m_Status;
	CStatusBar m_bar;
	TimeStamp m_Stamp;
	int m_Percent;
	CListControlMem m_DupList;	
	CListControlMem m_Folders;
	CImageList m_ImageList;
	CMenuButton m_SelectFolderBtn;
	CMenuButton m_SelectBtn;

public:
	afx_msg void OnBnClickedWildcards();
	afx_msg void OnClose();
};
