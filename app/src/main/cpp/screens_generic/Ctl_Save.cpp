//
// Ctl_Save.cpp
//

#include "stdafx.h"
#include "Dlg_Save.h"
#include "helpers.h"
//#include "xxc/xxc.file.h"
#include "Ctl_Save.h"
#include "FileAccessor/FileAccessor.h"

using namespace fileaccessor;

// CotullaTODO: copy of button code, from ScenList, move it somewhere else later
class iSortBtn2 : public iButton
{
public:
    iSortBtn2(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, sint16 sid, sint16 pressedSid, sint16 tid, uint32 state = Visible|Enabled, Alignment align = AlignTopLeft)\
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

/*
*
*/
static void EnumSaveGames(bool bSave, iSaveSlots& saveSlots)
{
	// skip the autosave & quicksave if saving
    for (uint32 xx = bSave ? 1 : 0; xx < SAVE_GAME_SLOTS - (bSave ? 1 : 0); ++xx)
    {
        auto fpath = RelativeFilePath((gSavePath + iFormat(_T("save%02d.phsd"),xx)).CStr(), FileLocationType::FLT_DOCUMENT);
#ifdef HMM_COMPOVERSION
        iFilePtr pFile(xxc::OpenXFile(fname.CStr(),HMM_COMPOCODE));
#else
        iFilePtr pFile(OpenWin32File(fpath));
#endif
        if (pFile)
        {
            iMapInfo* pMapInfo = new iMapInfo;
            pMapInfo->m_bNewGame = false;
            pMapInfo->m_FileName = fpath;
            uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc));
            if (fourcc == GMAP_FILE_HDR_KEY && pMapInfo->ReadMapInfo(pFile.get()))
            {
				iSaveSlot slot;
				slot.mapInfo = pMapInfo;
				slot.idx = xx;
                saveSlots.Add(slot);
                continue;
            }
            delete pMapInfo;
        }
        else
        {
            // file is missing, end of saves
			// disabled, Hedin, 15.05.2011
            // break;
        }
    }

	if (bSave) {
		iSaveSlot sl;
		sl.mapInfo = NULL;
		sl.idx = 0;

		if(saveSlots.GetSize() > 0)
			saveSlots.InsertBefore(0, sl);
		else
			saveSlots.Add(sl);
	}
}

static bool GetNextFileName(iStringT& fname)
{
    fname = _T("");
	// do not select quicksave & autosave
    for (uint32 xx = 1; xx < SAVE_GAME_SLOTS - 1; ++xx)
    {
        iStringT fn = gSavePath + iFormat(_T("save%02d.phsd"),xx);
        if (!iFile::Exists(fn))
        {
            fname = fn;
            return true;
        }
    }
	return false;
}

/*
*	Scenario list box
*/
class iSaveListBox : public iListBoxEx
{
public:
    iSaveListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, const iSaveSlots& saveSlots)
        : iListBoxEx(pViewMgr, pCmdHandler, rect, gApp.Surface(), uid), m_saveSlots(saveSlots)
    {
        // fill lines
        RefillColors();
    }

    inline uint32 LBItemHeight() const { return 75; }
    inline uint32 LBItemsCount() const { return m_saveSlots.GetSize(); }

    void RefillColors()
    {
        m_clrdata.RemoveAll();
        for (uint32 i = 0; i < m_saveSlots.GetSize(); i++)
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

        iTextComposer::FontConfig fc_l(newmenufc_low_text);
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
				dib.FillRect(rc,
#ifdef PC_VERSION
                             RGB16(196, 184, 163)
#else
                             RGB16(196, 184, 163)
#endif
                             ); //RGB16(160, 152, 136));
			else
				dib.FillRect(rc,
#ifdef PC_VERSION
                             RGB16(165, 155, 133)
#else
                             RGB16(165, 155, 133)
#endif
                             ); //RGB16(176, 168, 152));
		}
		else
		{
			fc_b.cmpProps.faceColor = RGB16(0, 0, 0);
			dib.FillRect(rc, RGB16(242, 230, 198));
		}

       /* if (!bSel)
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
        }*/

 //       rc = irc;

        if (m_saveSlots[iIdx].mapInfo)
        {
            // Size/PlayCount Name Date DateGame


            // 1. Size
            rc.DeflateRect(5,0,0,0);
            gTextComposer.TextOut(fc_h, dib,rc,gTextMgr[TRID_SHORT_MAPSIZ_SMALL+m_saveSlots[iIdx].mapInfo->m_Size],
                    iRect(rc.x + 5,rc.y, 45,rc.h), AlignCenter);
            // 2. Players count
            gTextComposer.TextOut(fc_l, dib,rc,iFormat(_T("%d/%d"),m_saveSlots[iIdx].mapInfo->HumanPlayers(),
                m_saveSlots[iIdx].mapInfo->TotalPlayers()),iRect(rc.x + 5,rc.y,45,rc.h), AlignBottom);

            rc.DeflateRect(50,0,0,0);

            // 3. Map name
			iStringT name;
			if(m_saveSlots[iIdx].idx ==  0)
				name = iFormat(_T("[A] %s"), m_saveSlots[iIdx].mapInfo->MapName().CStr());
			else if(m_saveSlots[iIdx].idx == SAVE_GAME_SLOTS - 1)
				name = iFormat(_T("[Q] %s"), m_saveSlots[iIdx].mapInfo->MapName().CStr());
			else
				name = iFormat(_T("[%d] %s"), m_saveSlots[iIdx].idx, m_saveSlots[iIdx].mapInfo->MapName().CStr());

            gTextComposer.TextOut(fc_b, dib,rc, name, iRect(rc.x,rc.y,300,rc.h),AlignLeft);
#ifdef PC_VERSION
            rc.DeflateRect(300,0,0,0);
#else
            rc = AlignRect(iSize(140, rc.h), rc, AlignRight);
#endif
            // 4. Date
            uint16 ch = gTextComposer.GetCharHeight(fc_m.fontSize);
            iRect rt(rc.x, rc.y + (rc.h - 2 * ch - 8) / 2, 70, 2 * ch + 8);

            gTextComposer.TextOut(fc_m, dib, rt, FormatDateDM(m_saveSlots[iIdx].mapInfo->m_saveTime, false),
                rt, AlignTop);
            gTextComposer.TextOut(fc_m, dib, rt, FormatTime(m_saveSlots[iIdx].mapInfo->m_saveTime),
                rt, AlignBottom);
            rc.DeflateRect(70,0,0,0);

            // 5. GameDate
            sint32 days = m_saveSlots[iIdx].mapInfo->m_curDay - 1;
            gTextComposer.TextOut(fc_m, dib, rc, iFormat(_T("%s%d%s%d%s%d"), gTextMgr[TRID_MONTH_S],
                    days / 28 + 1, gTextMgr[TRID_WEEK_S], (days % 28) / 7 + 1,gTextMgr[TRID_DAY_S],days % 7 + 1),
                    iRect(rc.x,rc.y,80,rc.h), AlignLeft);
        }
        else
        {
            iTextComposer::FontConfig fc_m(newmenufc_med_text);
            fc_m.cmpProps.faceColor = RGB16(0, 0, 0);
            gTextComposer.TextOut(fc_m, dib,rc,gTextMgr[TRID_EMPTY],rc,AlignCenter);
        }
    }

private:
    const iSaveSlots&	m_saveSlots;
    iSimpleArray<bool>  m_clrdata;
};

/*
*	Save dialog
*/
iSaveGameView::iSaveGameView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool bSave)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_bSave(bSave), m_selSlot(-1),  m_sort(RevDate), m_pCmdHandler(pCmdHandler)
{
    iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);


    // Listbox header
    AddChild(new iSortBtn2(m_pMgr, this, iRect(0, 0, 129, 47), 500,
        PDGG(NMENU_SORT_NL), PDGG(NMENU_SORT_PL), TRID_MENU_SAVELOAD1, Visible | Enabled));
    if(!gUseIpadUI)
    {
        AddChild(new iSortBtn2(m_pMgr, this, iRect(128, 0, 128, 47), 501,
                               PDGG(NMENU_SORT_NM), PDGG(NMENU_SORT_PM), TRID_MENU_SAVELOAD2,Visible | Enabled));
    }
    AddChild(new iSortBtn2(m_pMgr, this, iRect(clRect.w - 129 - 128 + 1, 0, 128, 47), 502,
        PDGG(NMENU_SORT_NM), PDGG(NMENU_SORT_PM), TRID_MENU_SAVELOAD3,Visible | Enabled));

    AddChild(new iSortBtn2(m_pMgr, this, iRect(clRect.w - 129, 0, 129, 47), 503,
        PDGG(NMENU_SORT_NR), PDGG(NMENU_SORT_PR), TRID_MENU_SAVELOAD4,Visible | Enabled));
    AddChild(m_pLB = new iSaveListBox(m_pMgr, this, iRect(3, 47, clRect.w - 3 * 2, clRect.h - 47), 100, m_saveSlots));

    // we fill data only on panel activate
}


iSaveGameView::~iSaveGameView()
{
    for (uint32 xx=0; xx<m_saveSlots.GetSize(); ++xx)
    {
        if (m_saveSlots[xx].mapInfo) delete m_saveSlots[xx].mapInfo;
    }
    m_saveSlots.RemoveAll();
}

bool iSaveGameView::Process(fix32 t)
{
	m_pLB->Process(t);
	return false;
}

void iSaveGameView::GetSaveFileName(iStringT& fname, uint32 slot)
{
    fname = gSavePath + iFormat(_T("save%02d.phsd"),slot);
}

bool iSaveGameView::SelFile(iStringT& fname) const
{
    if (m_selSlot > 0  && m_selSlot < (sint32)m_saveSlots.GetSize())
	{
        GetSaveFileName(fname, m_saveSlots[m_selSlot].idx);
		return true;
	}
    else if (m_selSlot == 0)
        return GetNextFileName(fname);
    else
        return false;
}

const iMapInfo& iSaveGameView::SelScenario() const
{
    check(m_selSlot >= 0  && m_selSlot < m_saveSlots.GetSize() && m_saveSlots[m_selSlot].mapInfo);
    return *m_saveSlots[m_selSlot].mapInfo;
}

const iMapInfo* iSaveGameView::GetAutoSave() const
{
	if(m_saveSlots.GetSize() == 0) return NULL;
    return m_saveSlots[0].mapInfo;
}

bool iSaveGameView::IsGoodSelection() const
{
    if (m_selSlot < 0 || m_selSlot >= (sint32)m_saveSlots.GetSize())
        return false;

    if (m_bSave)
        return true;
    else
        return !!m_saveSlots[m_selSlot].mapInfo;
}

void iSaveGameView::SortSaves(SortBy sort_by)
{
    iSortArray<iSaveSlot> sort;
    sint32 idx;
    uint32 uid = 0;

    for (uint32 xx = 0; xx < m_saveSlots.GetSize(); ++xx)
    {
        if (!m_saveSlots[xx].mapInfo || m_saveSlots[xx].idx == 0)
        {
            idx = 0x0000000;
        }
        else
        {
            idx = 0x10000000;
            if (sort_by == Size) idx = 0x10000000 + m_saveSlots[xx].mapInfo->m_Size;
            else if (sort_by == RevSize) idx = 0x10000000 - m_saveSlots[xx].mapInfo->m_Size;
            else if (sort_by == Name) idx = 0x10000000 + (m_saveSlots[xx].mapInfo->MapName()[0] << 16) | m_saveSlots[xx].mapInfo->MapName()[1];
            else if (sort_by == RevName) idx = 0x10000000 - (m_saveSlots[xx].mapInfo->MapName()[0] << 16) | m_saveSlots[xx].mapInfo->MapName()[1];
            else if (sort_by == Progress) idx = 0x10000000 + m_saveSlots[xx].mapInfo->m_curDay;
            else if (sort_by == RevProgress) idx = 0x10000000 - m_saveSlots[xx].mapInfo->m_curDay;
            else if (sort_by == Date) idx = 0x10000000 + m_saveSlots[xx].mapInfo->m_saveTime / 8;
            else if (sort_by == RevDate) idx = 0x10000000 - m_saveSlots[xx].mapInfo->m_saveTime / 8;

            if (!m_saveSlots[xx].mapInfo->Supported()) idx += 0x10000000;

        }
        sort.Insert(m_saveSlots[xx], idx);
    }

    for (uint32 xx = 0; xx < m_saveSlots.GetSize(); ++xx) m_saveSlots[xx] = sort.Value(xx);

    if (m_pLB) m_pLB->RefillColors();
    m_sort = sort_by;

    if (sort_by == Name || sort_by == RevName) uid = 500;
    else if (sort_by == Date || sort_by == RevDate) uid = 501;
    else if (sort_by == Progress || sort_by == RevProgress) uid = 502;
    else if (sort_by == Size || sort_by == RevSize) uid = 503;

    CheckHeaderButton(uid);
    Invalidate();
}


void iSaveGameView::OnCompose()
{
}


void iSaveGameView::SendResult(uint32 res)
{
    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, (uint32)res);
}

void iSaveGameView::CheckHeaderButton(uint32 id)
{
    iSortBtn2 *p1, *p2, *p3, *p4;
    p1 = (iSortBtn2*)GetChildById(500);
    p2 = (iSortBtn2*)GetChildById(501);
    p3 = (iSortBtn2*)GetChildById(502);
    p4 = (iSortBtn2*)GetChildById(503);

    p1->SetHighlighted(id == 500);
    if( p2 )
        p2->SetHighlighted(id == 501);
    p3->SetHighlighted(id == 502);
    p4->SetHighlighted(id == 503);
}

void iSaveGameView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == 100)
    {
        if (cmd == CCI_LBSELCHANGED)
        {
            m_selSlot = param;
            if (CanSelect())
            {
                if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_EDITCHANGED, (uint32)0);
            }
        }
        else if (cmd == CCI_LBSELECTED)
        {
            if (CanSelect() && Confirmed())
            {
                SendResult(DRC_OK);
            }
        }
    }
    else if (uid == 500)
    {
        SortSaves(m_sort == Name ? RevName : Name);
    }
    else if (uid == 501)
    {
        SortSaves(m_sort == Date ? RevDate : Date);
    }
    else if (uid == 502)
    {
        SortSaves(m_sort == Progress ? RevProgress : Progress);
    }
    else if (uid == 503)
    {
        SortSaves(m_sort == Size ? RevSize : Size);
    }

}

bool iSaveGameView::Confirmed() const
{
    check(m_selSlot != -1);
    if (m_bSave && m_saveSlots[m_selSlot].mapInfo != NULL)
    {
        iQuestDlg qdlg(m_pMgr, _T(""), gTextMgr[TRID_CONFIRM_OVERWRITE], PID_NEUTRAL);
        if (qdlg.DoModal() == DRC_NO) return false;
    }
    return true;
}

void iSaveGameView::LoadData()
{
    // called for list refresh (on activate of menu panel)
	for(uint32 xx=0; xx<m_saveSlots.GetSize(); xx++)
		if(m_saveSlots[xx].mapInfo) delete m_saveSlots[xx].mapInfo;
    m_saveSlots.RemoveAll();
    EnumSaveGames(m_bSave, m_saveSlots);
    SortSaves(m_sort);
}

// END OF FILE
