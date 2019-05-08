//
// Ctl_ScenList.cpp
//
#include "stdafx.h"
#include "ScopePtr.h"
#include "Dlg_ScenList.h"
//#include "xxc/xxc.file.h"
#include "serialize.h"
#include "gxl.inc.h"
#include "Ctl_ScenList.h"


//////////////////////////////////////////////////////////////////////////

class iSortBtn : public iButton
{
public:
    iSortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, sint16 sid, sint16 pressedSid, sint16 tid, uint32 state = Visible|Enabled, Alignment align = AlignTopLeft)\
        : iButton(pViewMgr, pCmdHandler, rect, uid, state), m_sid(sid), m_align(align), m_pressedSid(pressedSid), m_high(false), m_tid(tid)
    {}

    void OnBtnDown() const { }

    inline void SetHighlighted(bool high) { m_high = high; Invalidate(); }
    inline void SetAlign(Alignment align) { m_align = align; Invalidate(); }

    void OnCompose()
    {
        iRect rect = GetScrRect();
        iSize dim = gGfxMgr.Dimension(m_sid);
        iRect dst = AlignRect(dim, rect, m_align);

        if(!m_high && !(m_state & Pressed)) {
            if(m_sid != -1) {
                gGfxMgr.Blit(m_sid, gApp.Surface(), dst);
            }
        } else {
            if(m_pressedSid != -1) {
                gGfxMgr.Blit(m_pressedSid, gApp.Surface(), dst);
            } else if(m_sid != -1) {
                gGfxMgr.Blit(m_sid, gApp.Surface(), dst);
                if(GetScrRect() == iIphoneBtn::sm_defSize)
                    gGfxMgr.Blit(PDGG(BTN_FRAME), gApp.Surface(), dst);
            }
        }		
        iTextComposer::FontConfig fc(newmenufc_med_text);
        fc.cmpProps.faceColor = RGB16(0, 0, 0);
        gTextComposer.TextOut(fc,gApp.Surface(),rect,gTextMgr[m_tid],rect,AlignCenter, iPoint());
    }

    inline void SetSprite(sint16 sid, sint16 pressedSid) { m_sid = sid; m_pressedSid = pressedSid; Invalidate(); }

private:
    sint16 m_sid, m_pressedSid, m_tid;
    Alignment m_align;
    bool m_high;
};



void ComposeMapHeader(iDib& dib, const iRect& irc, const iStringT& size, const iStringT& humanplayers, const iStringT& totalplayers, const iStringT& name, const iStringT& author, const iStringT& cost, bool bIsUpdating, bool bIsHotNew, bool bEven, bool bSel, bool bBackground);

/*
*	Scenario list box
*/
class iScenListBox : public iListBoxEx
{
public:
    iScenListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, const iScenList& scList)
        : iListBoxEx(pViewMgr, pCmdHandler, rect, gApp.Surface(), uid), m_scList(scList)
    {
        // fill lines
        RefillColors();
    }

    inline uint32 LBItemHeight() const { return 70; }
    inline uint32 LBItemsCount() const { return m_scList.GetSize(); }

    void RefillColors()
    {
        m_clrdata.RemoveAll();
        for (uint32 i = 0; i < m_scList.GetSize(); i++)
        {
            m_clrdata.Add((i % 2) ? 1 : 0);
        }        
    }

private:
    void ComposeLBBackground(const iRect& rect)
    {
        iRect rc = rect;        
        //gApp.Surface().Darken25Rect(rc);
    }

    //void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc)
    void ExComposeLBItem(iDib &dib, sint32 iIdx, bool bSel, const iRect& irc)    
    {
        iRect rc=irc;
//		void ComposeMapHeader(iDib& dib, const iRect& irc, MAP_SIZE size, uint32 humanplayers, uint32 totalplayers, iStringT author, bool bIsPack, bIsUpdating, bIsHotNew, bool bEven, bool bSel, bool bBackground)
//		ComposeMapHeader(iDib& dib, const iRect& irc, MAP_SIZE size, uint32 humanplayers, uint32 totalplayers, const iStringT& name, const iStringT& author, bool bIsPack, bool bIsUpdating, bool bIsHotNew, bool bEven, bool bSel, bool bBackground);
        iStringT h = iFormat(_T("%d"), m_scList[iIdx]->HumanPlayers());
        iStringT t = iFormat(_T("%d"), m_scList[iIdx]->TotalPlayers());
        
        ComposeMapHeader(dib, rc, gTextMgr[TRID_SHORT_MAPSIZ_SMALL + m_scList[iIdx]->m_Size], h, t, m_scList[iIdx]->MapName(), m_scList[iIdx]->m_Author, _T(""), false, gMapEnum.GetPlayed(m_scList[iIdx]->m_Name[GLNG_ENGLISH]) == 0, m_clrdata[iIdx], bSel, true);
		
	/*	iTextComposer::FontConfig fc_l(newmenufc_low_text);
        iTextComposer::FontConfig fc_h(newmenufc_grt_text);
        iTextComposer::FontConfig fc_m(newmenufc_med_text);
        iTextComposer::FontConfig fc_b(newmenufc_big_text);

        fc_l.cmpProps.faceColor = RGB16(0, 0, 0);
        fc_h.cmpProps.faceColor = RGB16(0, 0, 0);
        fc_m.cmpProps.faceColor = RGB16(0, 0, 0);
        fc_b.cmpProps.faceColor = RGB16(248, 248, 224);

		bool bClr = m_clrdata[iIdx];
		if (!bSel)
		{
			if (bClr)
				dib.FillRect(irc, RGB16(132, 131, 130)); //RGB16(160, 152, 136));
			else
				dib.FillRect(irc, RGB16(96, 96, 96)); //RGB16(176, 168, 152));        
		}
		else
		{
			fc_b.cmpProps.faceColor = RGB16(0, 0, 0);
			dib.FillRect(irc, RGB16(242, 230, 198));            
		}
		
        if (!bSel)
        {
            if (m_clrdata[iIdx])
                dib.FillRect(rc, RGB16(160, 152, 136));
            else
                dib.FillRect(rc, RGB16(176, 168, 152));        
        }
        else
        {
            dib.FillRect(rc, RGB16(242, 230, 198));            
            fc_b.cmpProps.faceColor = RGB16(0, 0, 0);
        }
*/
        // 1. Size
  /*      rc.DeflateRect(5,0,0,0);
        gTextComposer.TextOut(fc_h, dib,rc,gTextMgr[TRID_SHORT_MAPSIZ_SMALL + m_scList[iIdx]->m_Size],
            iRect(rc.x + 5,rc.y, 45,rc.h), AlignCenter);
        // 2. Players count
        gTextComposer.TextOut(fc_l, dib,rc,iFormat(_T("%d/%d"),m_scList[iIdx]->HumanPlayers(),
            m_scList[iIdx]->TotalPlayers()),iRect(rc.x + 5,rc.y,45,rc.h), AlignBottom);

        rc.DeflateRect(50,0,0,0);

        // 3. Map name
        gTextComposer.TextOut(fc_b, dib,rc,m_scList[iIdx]->MapName(),iRect(rc.x,rc.y,300,rc.h),AlignLeft);
        rc.DeflateRect(0,0,20,0);

        // 4. Author
        gTextComposer.TextOut(fc_m, dib,rc,m_scList[iIdx]->m_Author,iRect(rc.x,rc.y,rc.w,rc.h), AlignRight);
*/
    }

private:    
    const iScenList&	m_scList;
    iSimpleArray<bool>  m_clrdata;
};

/*
*	Scenario List dialog
*/

iScenListView::SortBy iScenListView::m_sort = iScenListView::Name;
iScenListView::ShowFilter iScenListView::m_ShowFilter = iScenListView::ShowFilter_ALL;

iScenListView::iScenListView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 filter) 
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), 
  m_pCmdHandler(pCmdHandler), m_pLB(NULL), m_filter(filter), m_selScen(-1)
{
    iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);

    // Listbox header
    AddChild(new iSortBtn(m_pMgr, this, iRect(0, 0, 129, 47), 500, 
            PDGG(NMENU_SORT_NL), PDGG(NMENU_SORT_PL), TRID_MENU_SORTING1, Visible | Enabled));    
    AddChild(new iSortBtn(m_pMgr, this, iRect(128, 0, 128, 47), 501, 
            PDGG(NMENU_SORT_NM), PDGG(NMENU_SORT_PM), TRID_MENU_SORTING2,Visible | Enabled));
    if(gUseIpadUI)
        AddChild(new iSortBtn(m_pMgr, this, iRect(clRect.w - 129 - 128 + 1, 0, 128, 47), 502,
                              PDGG(NMENU_SORT_NM), PDGG(NMENU_SORT_PM), TRID_MENU_SORTING3,Visible | Enabled));

    AddChild(new iSortBtn(m_pMgr, this, iRect(clRect.w - 129, 0, 129, 47), 503, 
            PDGG(NMENU_SORT_NR), PDGG(NMENU_SORT_PR), TRID_MENU_SORTING6,Visible | Enabled));
    // Listbox
    m_pLB = new iScenListBox(m_pMgr, this, iRect(3, 47, clRect.w - 3 * 2, clRect.h - 47), 100, m_scList);
    AddChild(m_pLB);

    // we fill data only on panel activate

}

iScenListView::~iScenListView()
{
}

void iScenListView::CheckHeaderButton(uint32 id)
{
    iSortBtn *p1, *p2, *p3, *p4;

    p1 = (iSortBtn*)GetChildById(500);
    p2 = (iSortBtn*)GetChildById(501);
    p3 = (iSortBtn*)GetChildById(502);
    p4 = (iSortBtn*)GetChildById(503);

    p1->SetHighlighted(id == 500);
	if(p2)
		p2->SetHighlighted(id == 501);
    if(p3)
		p3->SetHighlighted(id == 502);
    p4->SetHighlighted(id == 503);
}

bool iScenListView::Process(fix32 t)
{
	// touch interface has list box reaction to finger
	m_pLB->Process(t);
	/*if(m_scrollMask != 0) {
        m_scrollCounter += t;
        if(m_scrollCounter >= fix32(0.1f)) {
            if(m_scrollMask & (0x01)) {
                m_selScen = iCLAMP<sint32>(0, m_scList.GetSize()-1, m_selScen-1);
                m_pLB->SetCurSel(m_selScen, true);
            } else if(m_scrollMask & (0x02)) {
                m_selScen = iCLAMP<sint32>(0, m_scList.GetSize()-1, m_selScen+1);
                m_pLB->SetCurSel(m_selScen, true);
            }
            m_scrollCounter = fix32::zero;
        }
    }*/
    return false;
}

bool iScenListView::KeyDown(sint32 key)
{
    if(key == KEY_UP || key == KEY_DOWN) {
        m_scrollMask |= 0x01 << (key - KEY_UP);
        m_scrollCounter = fix32(1.0f);
    }	
    return false;
}

bool iScenListView::KeyUp(sint32 key)
{
    m_scrollMask ^= 0x01 << (key - KEY_UP);

    if(key == KEY_ENTER) {
        if (m_selScen != -1 && m_scList[m_selScen]->Supported()) SendResult(DRC_OK); 
    }
    return false;
}

void iScenListView::SortScenarios(SortBy sort_by)
{
    iSortArray<iMapInfo*> sort;
    sint32 idx;
    uint32 uid = 0;

    for (uint32 xx=0; xx<m_scList.GetSize(); ++xx)
    {
        if (sort_by == Size) idx = m_scList[xx]->m_Size;
        else if (sort_by == RevSize) idx = 010000000 - m_scList[xx]->m_Size;
        else if (sort_by == Players) idx = m_scList[xx]->m_Players.GetSize();
        else if (sort_by == RevPlayers) idx = 010000000 - m_scList[xx]->m_Players.GetSize();
        else if (sort_by == Name) idx = (m_scList[xx]->MapName()[0] << 16) | m_scList[xx]->MapName()[1];			
        else if (sort_by == RevName) idx = 010000000 - ((m_scList[xx]->MapName()[0] << 16) | m_scList[xx]->MapName()[1]);					
        else if (sort_by == Author) idx = ((m_scList[xx]->m_Author[0] << 16) | m_scList[xx]->m_Author[1]);
        else if (sort_by == RevAuthor) idx = 010000000 - ((m_scList[xx]->m_Author[0] << 16) | m_scList[xx]->m_Author[1]);
		
		
        if (!m_scList[xx]->Supported()) idx += 0x10000000;
        sort.Insert(m_scList[xx], idx);
    }
    for (uint32 xx=0; xx<m_scList.GetSize(); ++xx) m_scList[xx] = sort.Value(xx);
    if (m_pLB) m_pLB->RefillColors();

    m_sort = sort_by;

    if (sort_by == Name || sort_by == RevName) uid = 500;
    else if (sort_by == Size || sort_by == RevSize) uid = 501;
    else if (sort_by == Author || sort_by == RevAuthor) uid = 502;    
    else if (sort_by == Players || sort_by == RevPlayers) uid = 503;

    CheckHeaderButton(uid);
    Invalidate();
}


void iScenListView::OnCompose() 
{
}

void iScenListView::SendResult(uint32 res)
{
    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, (uint32)res);    
}

void iScenListView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) 
{ 
    uint32 uid = pView->GetUID();
    if (uid == DRC_OK || uid == DRC_CANCEL){
        SendResult(uid); 
    } else if (uid == 301) {
        iStringT title = m_scList[m_selScen]->MapName();
        //if (!m_scList[m_selScen]->m_Version.Empty()) title.Addf(_T(" v.%s"), m_scList[m_selScen]->m_Version.CStr());
        iStringT desc = m_scList[m_selScen]->MapDescription();
        if (!m_scList[m_selScen]->m_Author.Empty()) desc.Addf(_T("\n\n%s: %s"), gTextMgr[TRID_MAP_AUTHOR], m_scList[m_selScen]->m_Author.CStr());
        iTextDlg tdlg(m_pMgr, title, desc, PID_NEUTRAL, dlgfc_topic, dlgfc_splain);
        tdlg.DoModal();
    } 
    else if (uid == 500) // Name
    {
        SortScenarios(m_sort == Name ? RevName : Name);        
    } 
    else if (uid == 501) // Size
    {
        SortScenarios(m_sort == Size ? RevSize : Size);
    } 
    else if (uid == 502)  // Author
    {           
        SortScenarios(m_sort == Author ? RevAuthor : Author);
    } 
    else if (uid == 503)  // Played
    {
        SortScenarios(m_sort == Players ? RevPlayers : Players);
    } 
    else if (uid == 100) 
    {
        if (cmd == CCI_LBSELCHANGED) 
        {
            m_selScen = param;
            gSettings.SetEntryValue(CET_NGDPOS, param);
        } 
        else if (cmd == CCI_LBSELECTED) 
        {
            if (m_selScen != -1 && m_scList[m_selScen]->Supported()) SendResult(DRC_OK); 
        } 
    }
}

void iScenListView::LoadData()
{
    uint32 mask = 0;

    // called for list refresh (on activate of menu panel)
    m_scList.RemoveAll();

    if (m_filter == SCL_SINGLE || m_filter == SCL_HOTSEAT)
    {
        if (m_ShowFilter == ShowFilter_ALL)
            mask = GMP_GMODE_ADVENTURE | GMP_GMODE_ACTION  | GMP_GMODE_MULTIPLAYER;
        else if (m_ShowFilter == ShowFilter_ACT)
            mask = GMP_GMODE_ACTION;
        else if (m_ShowFilter == ShowFilter_ADV)
            mask = GMP_GMODE_ADVENTURE;
        else if (m_ShowFilter == ShowFilter_MULTI)
            mask = GMP_GMODE_MULTIPLAYER;
        else 
            check(0);
    }
    else if (m_filter == SCL_MULTI) 
    {
        mask = GMP_GMODE_MULTIPLAYER;
    }
    else if (m_filter == SCL_TUTORIAL)
    {
        mask = GMP_GMODE_TUTORIAL;   
    }
        
    gMapEnum.Filter(m_scList, mask, m_filter == SCL_HOTSEAT);

    SortScenarios(m_sort);
}


uint32 iScenListView::GetMapCount(ShowFilter sf)
{
    uint32 res = 0;
    uint32 mask = 0;

    if (m_filter == SCL_SINGLE || m_filter == SCL_HOTSEAT)
    {
        if (sf == ShowFilter_ALL)
            mask = GMP_GMODE_ADVENTURE | GMP_GMODE_ACTION  | GMP_GMODE_MULTIPLAYER;
        else if (sf == ShowFilter_ACT)
            mask = GMP_GMODE_ACTION;
        else if (sf == ShowFilter_ADV)
            mask = GMP_GMODE_ADVENTURE;
        else if (sf == ShowFilter_MULTI)
            mask = GMP_GMODE_MULTIPLAYER;
        else 
            check(0);
    }

    res = gMapEnum.GetCount(mask, m_filter == SCL_HOTSEAT);

    return res;
}

// END OF FILE
