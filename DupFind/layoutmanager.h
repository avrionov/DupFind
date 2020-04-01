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
#ifndef __LAYOUTMANAGER_H__
#define __LAYOUTMANAGER_H__

#pragma warning( disable : 4786 )  

#include <map>


class CDeferPos
{
public:
	CDeferPos(size_t nWindows = 1)
    {
        m_hdwp = BeginDeferWindowPos((int)nWindows);
    }
	~CDeferPos()
    {
        EndDeferWindowPos(m_hdwp);
    }

	BOOL MoveWindow(HWND hWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint)
    {
        UINT uFlags = SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER;
        if (!bRepaint)
            uFlags |= SWP_NOREDRAW;
        return SetWindowPos(hWnd, 0, x, y, nWidth, nHeight, uFlags);
    }

	BOOL SetWindowPos(HWND hWnd, HWND hWndAfter, int x, int y, int nWidth,
		int nHeight, UINT uFlags)
    {
        if (m_hdwp != 0)
        {
            m_hdwp = DeferWindowPos(m_hdwp, hWnd, hWndAfter, x, y, nWidth, nHeight,
                uFlags);
        }
        return m_hdwp != 0;
    }

#ifdef	_MFC_VER
	BOOL MoveWindow(CWnd* pWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint)
    {
        return MoveWindow(pWnd->GetSafeHwnd(), x, y, nWidth, nHeight, bRepaint);
    }

	BOOL SetWindowPos(CWnd* pWnd, CWnd* pWndAfter, int x, int y, int nWidth,
		int nHeight, UINT uFlags)
    {
        return SetWindowPos(pWnd->GetSafeHwnd(), pWndAfter->GetSafeHwnd(), x, y, nWidth,
            nHeight, uFlags);
    }
#endif

private:
	HDWP m_hdwp;
};

//==================================================================================================
// Layout manager
//==================================================================================================

// Flags for anchoring or docking the control to the window
// Should be called with mapControl();

namespace LayoutFlags
{
    const int NONE                =       0x0000;                     // No position mapping

    const int DOCK_TOP            =       0x0001;                     // Docked to window top
    const int DOCK_BOTTOM         =       0x0002;                     // Docked to window bottom
    const int DOCK_LEFT           =       0x0004;                     // Docked to window left
    const int DOCK_RIGHT          =       0x0008;                     // Docked to window right
    const int DOCK_ALL            =       0x000F;                     // Docked to all sides of window

    const int DOCK_TOPLEFT        =       DOCK_TOP | DOCK_LEFT;       // Docked to top and left
    const int DOCK_TOPRIGHT       =       DOCK_TOP | DOCK_RIGHT;      // Docked to top and right
    const int DOCK_BOTTOMLEFT     =       DOCK_BOTTOM | DOCK_LEFT;    // Docked to bottom and left
    const int DOCK_BOTTOMRIGHT    =       DOCK_BOTTOM | DOCK_RIGHT;   // Docked to bottom and right
    const int DOCK_LEFTRIGHT      =       DOCK_LEFT | DOCK_RIGHT;     // Docked to left and right
    const int DOCK_TOPBOTTOM      =       DOCK_TOP | DOCK_BOTTOM;     // Docked to top and bottom

    const int DOCK_TOPBOTTOMLEFT  =       DOCK_TOP | DOCK_BOTTOM | DOCK_LEFT;   // Docked to top and bottom and left
    const int DOCK_TOPBOTTOMRIGHT =       DOCK_TOP | DOCK_BOTTOM | DOCK_RIGHT;  // Docked to top and bottom and right
    const int DOCK_TOPLEFTRIGHT   =       DOCK_TOP | DOCK_LEFT | DOCK_RIGHT;    // Docked to top and left and right
    const int DOCK_BOTTOMLEFTRIGHT=       DOCK_BOTTOM | DOCK_LEFT | DOCK_RIGHT; // Docked to bottom and left and right


    const int ANCHOR_TOP          =       0x0010;                     // Distance between top of child and top of window will be constant
    const int ANCHOR_BOTTOM       =       0x0020;                     // Distance between bottom of child and bottom of window will be constant
    const int ANCHOR_LEFT         =       0x0040;                     // Distance between left of child and left of window will be constant
    const int ANCHOR_RIGHT        =       0x0080;                     // Distance between right of child and right of window will be constant
    const int ANCHOR_ALL          =       0x00F0;                     // All edges of child will remain at constant distance with all edges of window    
    
    const int ANCHOR_TOPLEFT      =       ANCHOR_TOP | ANCHOR_LEFT;   // Distance between top and left of child and top and left of window will be constant
    const int ANCHOR_TOPRIGHT     =       ANCHOR_TOP | ANCHOR_RIGHT;  // Distance between top and right of child and top and right of window will be constant
    const int ANCHOR_BOTTOMLEFT   =       ANCHOR_BOTTOM | ANCHOR_LEFT;  // Distance between bottom and left of child and bottom and left of window will be constant
    const int ANCHOR_BOTTOMRIGHT  =       ANCHOR_BOTTOM | ANCHOR_RIGHT; // Distance between bottom and right of child and bottom and right of window will be constant
    const int ANCHOR_LEFTRIGHT    =       ANCHOR_LEFT | ANCHOR_RIGHT; // Distance between left and right of child and left and right of window will be constant
    const int ANCHOR_TOPBOTTOM    =       ANCHOR_TOP | ANCHOR_BOTTOM; // Distance between top and bottom of child and top and bottom of window will be constant

    const int ANCHOR_TOPBOTTOMLEFT  =     ANCHOR_TOP | ANCHOR_BOTTOM | ANCHOR_LEFT;     // ANCHORed at top and bottom and left
    const int ANCHOR_TOPBOTTOMRIGHT =     ANCHOR_TOP | ANCHOR_BOTTOM | ANCHOR_RIGHT;    // ANCHORed at top and bottom and right
    const int ANCHOR_TOPLEFTRIGHT   =     ANCHOR_TOP | ANCHOR_LEFT | ANCHOR_RIGHT;      // ANCHORed at top and left and right
    const int ANCHOR_BOTTOMLEFTRIGHT=     ANCHOR_BOTTOM | ANCHOR_LEFT | ANCHOR_RIGHT;   // ANCHORed at bottom and left and right

    const int LEFT                =       0x0100;                     // Edge tracking
    const int RIGHT               =       0x0200;                     // Edge tracking
    const int TOP                 =       0x0400;                     // Edge tracking
    const int BOTTOM              =       0x0800;                     // Edge tracking
    const int ALL                 =       0x0F00;                     // Edge tracking

};

using namespace LayoutFlags;

class CLayoutManager
{
public:
    typedef struct FRECT {  // floating-point RECT structure
        double  top;
        double  left;
        double  right;
        double  bottom;
    };

	void eq (RECT &rc, FRECT &frc)
	{
		rc.top = (LONG)frc.top;
		rc.left = (LONG)frc.left;
		rc.right = (LONG)frc.right;
		rc.bottom = (LONG)frc.bottom;
	};


    typedef struct FSIZE {  // floating-point SIZE structure
        double  cx;
        double  cy;
    };

    typedef struct TCtrlEntry {
        unsigned int    nCtrlID;    // dialog-control id of the window
        unsigned int    nFlags;     // docking/anchoring flags for this control
        FRECT           rect;       // actual client-rectangle of the control
    };

    typedef std::map<HWND, TCtrlEntry> TCtrlMap;
    typedef TCtrlMap::iterator TCtrlMapIter;
    typedef std::pair<HWND, TCtrlEntry> TCtrlMapPair;

protected:

    BOOL            bInitialized_m;         // class and control-information is inited
    RECT            rcOld_m;                // previous rect of the parent window
    RECT            rcNew_m;                // current window-rect of the parent window
    RECT            rcClient_m;             // current client-rect of the parent window
    RECT            rcClientOld_m;          // previous client-rect of the parent window
    unsigned int    uiSizedBorders_m;       // Flags for borders that have been sized
    SIZE            szDelta_m;              // delta of the size-change
    HWND            hParent_m;              // handle of the window that contains the controls
    TCtrlMap        ctrlMap_m;                 // control-map
    TCtrlMapIter    ctrlMapIter_m;             // control-map iterator
    unsigned int    nDefaultFlags_m;        // flags for "default" controls

public:
    CLayoutManager()
    {
        clear ();               
    }

    ~CLayoutManager()
    {  }

    void clear()
    {
        ctrlMap_m.clear();
        hParent_m = NULL;
        ::SetRect(&rcOld_m, 0, 0, 0, 0);
        ::SetRect(&rcNew_m, 0, 0, 0, 0);
        ::SetRect(&rcClient_m, 0, 0, 0, 0);
        ::SetRect(&rcClientOld_m, 0, 0, 0, 0);
        uiSizedBorders_m = 0;
        szDelta_m.cx = 0;
        szDelta_m.cy = 0;
        nDefaultFlags_m = NONE;
        bInitialized_m = FALSE;
        
    }

    // Call this function after initialize() to set the flags for each control
    bool mapControl(unsigned int control_id, unsigned int layout_flag)
    {
        //ASSERT(hParent_m != NULL);
        if (bInitialized_m == FALSE || hParent_m == NULL) return false;

        HWND hCtrl = ::GetDlgItem(hParent_m, control_id);

        if (hCtrl == NULL) return false;

        switch (layout_flag & DOCK_ALL)
        {
            case (NONE):
                break;
            case (DOCK_ALL):
                layout_flag = layout_flag & DOCK_ALL;
                break;
            case (DOCK_LEFT):
                layout_flag = layout_flag | ANCHOR_LEFT;
                break;
            case (DOCK_RIGHT):
                layout_flag = layout_flag | ANCHOR_RIGHT;
                break;
            case (DOCK_TOP):
                layout_flag = layout_flag | ANCHOR_TOP;
                break;
            case (DOCK_BOTTOM):
                layout_flag = layout_flag | ANCHOR_BOTTOM;
                break;
            case (DOCK_TOPLEFT):
                layout_flag = layout_flag | ANCHOR_TOPLEFT;
                break;
            case (DOCK_TOPRIGHT):
                layout_flag = layout_flag | ANCHOR_TOPRIGHT;
                break;
            case (DOCK_BOTTOMLEFT):
                layout_flag = layout_flag | ANCHOR_BOTTOMLEFT;
                break;
            case (DOCK_BOTTOMRIGHT):
                layout_flag = layout_flag | ANCHOR_BOTTOMRIGHT;
                break;
            case (DOCK_TOPBOTTOM):
                layout_flag = layout_flag | ANCHOR_TOPBOTTOM;
                break;
            case (DOCK_LEFTRIGHT):
                layout_flag = layout_flag | ANCHOR_LEFTRIGHT;
                break;
            case (DOCK_TOPBOTTOMLEFT):
                layout_flag = layout_flag | ANCHOR_TOPBOTTOMLEFT;
                break;
            case (DOCK_TOPBOTTOMRIGHT):
                layout_flag = layout_flag | ANCHOR_TOPBOTTOMRIGHT;
                break;
            case (DOCK_TOPLEFTRIGHT):
                layout_flag = layout_flag | ANCHOR_TOPLEFTRIGHT;
                break;
            case (DOCK_BOTTOMLEFTRIGHT):
                layout_flag = layout_flag | ANCHOR_BOTTOMLEFTRIGHT;
                break;
            default:
                break;
        }

        // Modify the flags of the control
        ctrlMap_m[hCtrl].nCtrlID = control_id;
        ctrlMap_m[hCtrl].nFlags  = layout_flag;

        return false;
    }

    // Check if all the controls have been initialized
    BOOL isInitialized(void) 
    {
        return(bInitialized_m);
    }

    // SetFrect helper function
    void SetFRect(FRECT &Rect, double left, double top, double right, double bottom)
    {
        Rect.left = left;
        Rect.top = top;
        Rect.right = right;
        Rect.bottom = bottom;
    };

    void setDefaultFlag(unsigned int defaultFlags)
    {
        nDefaultFlags_m = defaultFlags;
    }

    // Screen to client helper function
    BOOL ScreenToClient(HWND hParent, RECT &rect)
    {
        POINT topleft, bottomright;

        topleft.x = rect.left;
        topleft.y = rect.top;
        bottomright.x = rect.right;
        bottomright.y = rect.bottom;
        ::ScreenToClient(hParent, &topleft);
        ::ScreenToClient(hParent, &bottomright);
        rect.left = topleft.x;
        rect.top = topleft.y;
        rect.right = bottomright.x;
        rect.bottom = bottomright.y;

        return TRUE;
    }

    // Call back function for EnumChildWindows
    static BOOL CALLBACK EnumChildProcX(HWND hWnd, LPARAM lParam)
    {
        CLayoutManager *playoutmgr = (CLayoutManager *)lParam;
        //ASSERT(playoutmgr != NULL);
        if (playoutmgr == NULL) return FALSE;

        return playoutmgr->EnumChildProcXHelper(hWnd);
    }

    // EnumChildProcX Helper. Inserts the child controls into the map
    BOOL EnumChildProcXHelper(HWND hWnd)
    {
        TCtrlEntry          *pEntry = NULL;
        TCtrlMapIter iter;
        RECT rcCtrl;

        // Check if child belongs to our window
        if (::GetParent(hWnd) != hParent_m) return TRUE;

        // Do not add if already present
        if (ctrlMap_m.find(hWnd) != ctrlMap_m.end()) return TRUE;

        ctrlMap_m[hWnd].nCtrlID = ::GetDlgCtrlID(hWnd);
        ctrlMap_m[hWnd].nFlags = nDefaultFlags_m;

        // Gather information about child windows wrt parent window
        if (!::GetWindowRect(hWnd, &rcCtrl)) return FALSE;
        if (!ScreenToClient(hParent_m, rcCtrl)) return FALSE;
        
        ctrlMap_m[hWnd].rect.left   = rcCtrl.left;
        ctrlMap_m[hWnd].rect.top    = rcCtrl.top;
        ctrlMap_m[hWnd].rect.right  = rcCtrl.right;
        ctrlMap_m[hWnd].rect.bottom = rcCtrl.bottom;
        return TRUE;
    }

    // Initialize the map. Should be called before calling mapControl()
    bool initialize(HWND hWnd, BOOL bFindEdge = false)
    {
        // Parent handle
        hParent_m = hWnd;
        //ASSERT(hParent_m != NULL);
        if (hParent_m == NULL) return false;

        TCtrlMapIter iter;
        RECT    rcMaxBR;

        ::SetRect(&rcMaxBR, 0, 0, 0, 0);

        // Window, client rect of parent
        ::GetWindowRect(hParent_m, &rcOld_m);
        ::CopyRect(&rcNew_m, &rcOld_m);
        ::GetClientRect(hParent_m, &rcClientOld_m);
        ::CopyRect(&rcClient_m, &rcClientOld_m);

        // Populate child control map
        ctrlMap_m.clear();
        if (!::EnumChildWindows(hParent_m, EnumChildProcX, LPARAM(this)))  
            return false;

        
        // if bFindEdge = true, find the bottom-right-most edge to get
        // the initial size of the window
        if (bFindEdge == TRUE)
        {
            for (iter = ctrlMap_m.begin(); iter != ctrlMap_m.end(); iter++) 
            {

                if (iter->second.rect.right > rcMaxBR.right) 
                    rcMaxBR.right = (LONG)iter->second.rect.right;

                if (iter->second.rect.bottom > rcMaxBR.bottom) 
                    rcMaxBR.bottom = (LONG)iter->second.rect.bottom;

            };

            rcOld_m.right = rcOld_m.left+rcMaxBR.right;
            rcOld_m.bottom = rcOld_m.top+rcMaxBR.bottom;
            rcClient_m.right = rcClient_m.left+(rcOld_m.right-rcOld_m.left);
            rcClient_m.bottom = rcClient_m.top+(rcOld_m.bottom-rcOld_m.top); 
        }

        bInitialized_m = TRUE;
        return true;
    }

    void preMapping()
    {
        //ASSERT(hParent_m != NULL);
        if (hParent_m == NULL) return;

        ::GetWindowRect(hParent_m, &rcNew_m);

        // Check which borders were changed
        uiSizedBorders_m = 0;

        if (rcNew_m.left != rcOld_m.left) uiSizedBorders_m |= LEFT;
        if (rcNew_m.top != rcOld_m.top) uiSizedBorders_m |= TOP;
        if (rcNew_m.right != rcOld_m.right) uiSizedBorders_m |= RIGHT;
        if (rcNew_m.bottom != rcOld_m.bottom) uiSizedBorders_m |= BOTTOM;

        // calculate new client-rect
        ::GetClientRect(hParent_m, &rcClient_m);

        // calculate deltas
        szDelta_m.cx = (rcClient_m.right - rcClient_m.left) - (rcClientOld_m.right - rcClientOld_m.left);
        szDelta_m.cy = (rcClient_m.bottom - rcClient_m.top) - (rcClientOld_m.bottom - rcClientOld_m.top);

    }

    void postMapping()
    {
        ::CopyRect(&rcOld_m, &rcNew_m);
        ::CopyRect(&rcClientOld_m, &rcClient_m);
    }
    
    // Call from WM_SIZE to re do the mapping 
    bool reMap()
    {
        //ASSERT(hParent_m != NULL);
        if (hParent_m == NULL) return false;

        preMapping();

        CDeferPos drawing(ctrlMap_m.size());

        TCtrlMapIter iter;
        SIZE szCtrl;
        RECT rcCtrl;
        RECT rcCtrl2;
        BOOL bChanged = FALSE;

        for (iter = ctrlMap_m.begin(); iter != ctrlMap_m.end(); iter++)
        {
            // get the size of the control
            //::CopyRect(&rcCtrl, &(iter->second.rect));
			eq (rcCtrl, iter->second.rect);

            /*rcCtrl.top = iter->second.rect.top;
            rcCtrl.left = iter->second.rect.left;
            rcCtrl.right = iter->second.rect.right;
            rcCtrl.bottom = iter->second.rect.bottom;*/
            szCtrl.cx = rcCtrl.right - rcCtrl.left;
            szCtrl.cy = rcCtrl.bottom - rcCtrl.top;
                        
            bChanged = TRUE;
            // Handle docking
            switch (iter->second.nFlags & DOCK_ALL)
            {
                case (NONE):
                    // Go to Anchoring
                    break;
                case (DOCK_ALL):
                    SetFRect(iter->second.rect, 0, 0, rcClient_m.right, rcClient_m.bottom);
                    break;
                case (DOCK_LEFT):
                    SetFRect(iter->second.rect, 0, rcCtrl.top, szCtrl.cx, rcCtrl.bottom);
                    break;
                case (DOCK_RIGHT):
                    SetFRect(iter->second.rect, rcClient_m.right - szCtrl.cx, rcCtrl.top, rcClient_m.right, rcCtrl.bottom);
                    break;
                case (DOCK_TOP):
                    SetFRect(iter->second.rect, rcCtrl.left, 0, rcCtrl.right, szCtrl.cy);
                    break;
                case (DOCK_BOTTOM):
                    SetFRect(iter->second.rect, rcCtrl.left, rcClient_m.bottom - szCtrl.cy, rcCtrl.right, rcClient_m.bottom);
                    break;
                case (DOCK_TOPLEFT):
                    SetFRect(iter->second.rect, 0, 0, szCtrl.cx, szCtrl.cy);
                    break;
                case (DOCK_TOPRIGHT):
                    SetFRect(iter->second.rect, rcClient_m.right - szCtrl.cx, 0, rcClient_m.right, szCtrl.cy);
                    break;
                case (DOCK_BOTTOMLEFT):
                    SetFRect(iter->second.rect, 0, rcClient_m.bottom - szCtrl.cy, szCtrl.cx, rcClient_m.bottom);
                    break;
                case (DOCK_BOTTOMRIGHT):
                    SetFRect(iter->second.rect, rcClient_m.right - szCtrl.cx, rcClient_m.bottom - szCtrl.cy, rcClient_m.right, rcClient_m.bottom);
                    break;
                case (DOCK_TOPBOTTOM):
                    SetFRect(iter->second.rect, rcCtrl.left, 0, rcCtrl.right, rcClient_m.bottom);
                    break;
                case (DOCK_LEFTRIGHT):
                    SetFRect(iter->second.rect, 0, rcCtrl.top, rcClient_m.right, rcCtrl.bottom);
                    break;
                case (DOCK_TOPBOTTOMLEFT):
                    SetFRect(iter->second.rect, 0, 0, szCtrl.cy, rcClient_m.bottom);
                    break;
                case (DOCK_TOPBOTTOMRIGHT):
                    SetFRect(iter->second.rect, rcClient_m.right - szCtrl.cx, 0, rcClient_m.right, rcClient_m.bottom);
                    break;
                case (DOCK_TOPLEFTRIGHT):
                    SetFRect(iter->second.rect, 0, 0, rcClient_m.right, szCtrl.cy);
                    break;
                case (DOCK_BOTTOMLEFTRIGHT):
                    SetFRect(iter->second.rect, 0, rcClient_m.bottom - szCtrl.cy, rcClient_m.right, rcClient_m.bottom);
                    break;
                default:
                    return false;
            }

            // Handle anchoring
            switch (iter->second.nFlags & ANCHOR_ALL)
            {
                case (NONE):
                    // No flags
                    bChanged = FALSE;
                    break;
                case (ANCHOR_ALL):
                    iter->second.rect.bottom += szDelta_m.cy;
                    iter->second.rect.right += szDelta_m.cx;
                    break;
                case (ANCHOR_LEFT):
                    // No need to do anything
                    break;
                case (ANCHOR_RIGHT):
                    iter->second.rect.left += szDelta_m.cx;
                    iter->second.rect.right = iter->second.rect.left + szCtrl.cx;
                    break;
                case (ANCHOR_TOP):
                    // No need to do anything
                    break;
                case (ANCHOR_BOTTOM):
                    iter->second.rect.top += szDelta_m.cy;
                    iter->second.rect.bottom = iter->second.rect.top + szCtrl.cy;
                    break;
                case (ANCHOR_TOPLEFT):
                    // No need to do anything
                    break;
                case (ANCHOR_TOPRIGHT):
                    iter->second.rect.left += szDelta_m.cx;
                    iter->second.rect.right = iter->second.rect.left + szCtrl.cx;
                    break;
                case (ANCHOR_BOTTOMLEFT):
                    iter->second.rect.top += szDelta_m.cy;
                    iter->second.rect.bottom =  iter->second.rect.top + szCtrl.cy;
                    break;
                case (ANCHOR_BOTTOMRIGHT):
                    iter->second.rect.top += szDelta_m.cy;
                    iter->second.rect.bottom =  iter->second.rect.top + szCtrl.cy;
                    iter->second.rect.left += szDelta_m.cx;
                    iter->second.rect.right = iter->second.rect.left + szCtrl.cx;
                    break;
                case (ANCHOR_TOPBOTTOM):
                    iter->second.rect.bottom += szDelta_m.cy;
                    break;
                case (ANCHOR_LEFTRIGHT):
                    iter->second.rect.right += szDelta_m.cx;
                    break;
                case (ANCHOR_TOPBOTTOMLEFT):
                    iter->second.rect.bottom += szDelta_m.cy;
                    break;
                case (ANCHOR_TOPBOTTOMRIGHT):
                    iter->second.rect.bottom += szDelta_m.cy;
                    iter->second.rect.left += szDelta_m.cx;
                    iter->second.rect.right = iter->second.rect.left + szCtrl.cx;
                    break;
                case (ANCHOR_TOPLEFTRIGHT):
                    iter->second.rect.right += szDelta_m.cx;
                    break;
                case (ANCHOR_BOTTOMLEFTRIGHT):
                    iter->second.rect.top += szDelta_m.cy;
                    iter->second.rect.bottom =  iter->second.rect.top + szCtrl.cy;
                    iter->second.rect.right += szDelta_m.cx;
                    break;
                default:
                    return false;
            }
			eq (rcCtrl2, iter->second.rect);

            /*rcCtrl2.top = iter->second.rect.top;
            rcCtrl2.left = iter->second.rect.left;
            rcCtrl2.right = iter->second.rect.right;
            rcCtrl2.bottom = iter->second.rect.bottom;*/

            if (::EqualRect(&rcCtrl, &rcCtrl2) == TRUE)
                bChanged = FALSE;
            
            // Reposition the window if size/position has changed
            if (bChanged == TRUE)
            {
                szCtrl.cx = (LONG) (iter->second.rect.right - iter->second.rect.left);
                szCtrl.cy = (LONG) (iter->second.rect.bottom - iter->second.rect.top);
                drawing.SetWindowPos(iter->first, NULL, (int)iter->second.rect.left, (int)iter->second.rect.top, (int)szCtrl.cx, (int)szCtrl.cy, SWP_NOZORDER | SWP_NOACTIVATE);
            }            
        }

        postMapping();

        return true;
    }

};




#endif