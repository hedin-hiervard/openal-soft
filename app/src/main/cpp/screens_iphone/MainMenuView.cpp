////////////////////////////////////////////////////////////////////////
// MainMenu.cpp
//////////////////////////////////////////////////////////////////////////
// this file contains new main menu code (for iPhone)
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//  iMainMenuDlg
//      iMainMenuView
//          iMainMenuBtn
//          iMainMenuTab_Settings
//              iMainMenuTab
//          iMainMenuTab_Credits
//              iMainMenuTab
//      iNewSingleMenuView
//          iMainMenuTab_DownloadMap
//          iMainMenuTab_NewGame
//          iMainMenuTab_LoadGame
//      iNewMultiMainMenuView
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "MainMenuView.h"
#include "Ctl_ScenProps.h"
#include "Dlg_ScenList.h"
#include "Ctl_Save.h"
#include "Ctl_ScenList.h"
#include "Ctl_Settings.h"
// #include "NewMapShop.h"
// #include "Ctl_MapShop.h"
#include "helpers.h"

#include "iosdk/iosdk.h"

using namespace iosdk;
using namespace fileaccessor;

namespace UI_IPHONE
{

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif
//////////////////////////////////////////////////////////////////////////
// iNewMenuBottomBtn (this is button at bottom of workspace (image + label under image))
//////////////////////////////////////////////////////////////////////////

class iNewMenuBottomBtn : public iButton
{
public:
    iNewMenuBottomBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 sid, sint16 presssid, sint16 tid, uint32 state = Visible|Enabled)
    : iButton(pViewMgr, pCmdHandler, rect, uid, state), m_sid(sid), m_pressedsid(presssid), m_high(false), m_tid(tid)
    { }

    void OnBtnDown() const { }

    inline void SetTextTip(const iStringT &tip) { m_tip = tip; Invalidate(); };
    inline void SetTextBadge(const iStringT &badge) { m_badge = badge; Invalidate(); };

    inline void SetHighlighted(bool high) { m_high = high; Invalidate(); }

    void OnCompose()
    {
        iRect rect = GetScrRect();
        iSize dim = gGfxMgr.Dimension(m_sid);
        iRect dst = AlignRect(dim, rect, AlignTop);


        if (!m_high && !(m_state & Pressed))
        {
            if(m_sid != -1)
            {
                gGfxMgr.Blit(m_sid, gApp.Surface(), dst);
            }
        }
        else
        {
            if (m_pressedsid != -1)
            {
                gGfxMgr.Blit(m_pressedsid, gApp.Surface(), dst);
            }
        }
        iTextComposer::FontConfig fc_l(newmenufc_low_text);
        iTextComposer::FontConfig fc_h(newmenufc_med_text);

        fc_l.cmpProps.faceColor = RGB16(0, 0, 0);
        fc_h.cmpProps.faceColor = RGB16(0, 0, 0);

        uint16 h_lo = gTextComposer.GetCharHeight(fc_l.fontSize);
        uint16 h_hi = gTextComposer.GetCharHeight(fc_h.fontSize);

        iRect txt_rc(rect.x, rect.y + rect.h - h_lo - h_hi - 4, rect.w, h_lo + h_hi + 4);

        //gApp.Surface().FrameRect(txt_rc, cColor_Black);

		if (!m_badge.Empty()) {
			ComposeBadge(gApp.Surface(), rect.TopRight() - iPoint(50, 5), m_badge);
		}

        //gApp.Surface().FrameRect(rect, cColor_Black);

    }

    inline void SetSprite(sint16 sid, sint16 pressedSid) { m_sid = sid; m_pressedsid = pressedSid; Invalidate(); }

private:
    sint16 m_sid, m_tid, m_pressedsid;
    bool m_high;
    iStringT m_tip, m_badge;
};

//////////////////////////////////////////////////////////////////////////
// iMenuBtn (big buttons at the left of menu)
//////////////////////////////////////////////////////////////////////////


class iMainMenuView::iMenuBtn : public iButton, public IViewCmdHandler
{
public:
    iMenuBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 rid, uint32 uid)
    : iButton(pViewMgr, this, rect, uid, (Visible|Enabled)), m_rid(rid), m_active(false), m_pCmdHandler(pCmdHandler),
    m_count(0)
	{}

    void SetActive(bool b ) { m_active = b;};
	inline void SetCount(uint32 count) { m_count = count; }

private:
    void OnCompose();
    void ComposeBkg(bool pressed);

    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        if (cmd == CCI_BTNCLICK || cmd == CCI_BTNDBLCLICK)
        {
            m_pCmdHandler->iCMDH_ControlCommand(pView, cmd, 0);
        }
    }

private:
    bool                m_active;
    uint32              m_rid; // id of text resource
    IViewCmdHandler*    m_pCmdHandler;
	uint32				m_count; // the badge with number on button
};


void iMainMenuView::iMenuBtn::ComposeBkg(bool pressed)
{
    iDib& surf = gApp.Surface();
    bool bDecs = false;
    iRect rect = GetScrRect();
    //    rect.InflateRect(8, 8, 8, 8);

    uint32 toffs = (bDecs)?12:10;
    if (rect.IsEmpty()) return;

    if(rect.w <= 24 || rect.h <= 24) return; // too few space

    // tile background
    iRect bkRect(rect);
#ifndef PC_VERSION
    bkRect.DeflateRect(15, 15, 14, 14);
    bkRect -= iPoint(1,1);
#else
	bkRect.DeflateRect(5);
#endif
    if (!pressed)
    {

        iSize sz = gGfxMgr.Dimension(PDGG(NMENU_BTN_NU));

        //gGfxMgr.BlitTile(PDGG(NMENU_BTN_NBKG), surf, bkRect);
        //		surf.FillRect(bkRect, RGB16(242, 230, 197));
        surf.FillRect(bkRect, RGB16(231, 214, 189), 131);
        /*		iRect trc;
         trc = iRect(rect.x + 14, rect.y, rect.w - 28, 14);					surf.FillRect(trc, RGB16(132, 131, 130));
         trc = iRect(rect.x + 14, rect.y + rect.h - 14, rect.w - 28, 12);	surf.FillRect(trc, RGB16(132, 131, 130));
         trc = iRect(rect.x, rect.y + 14, 14, rect.h - 28);					surf.FillRect(trc, RGB16(132, 131, 130));
         trc = iRect(rect.x + rect.w - 14, rect.y + 14, 12, rect.h - 28);	surf.FillRect(trc, RGB16(132, 131, 130));
         */
        // top/bottom tiles
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_NU), surf, iRect(rect.x + 14, rect.y, rect.w - 14 * 2-1, 14));
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_ND), surf, iRect(rect.x + 14, rect.y2() - 14,rect.w - 14 * 2-1, 14));

        // left/right tiles
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_NL), surf, iRect(rect.x, rect.y + 14, 14, rect.h - 14 * 2-1));
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_NR), surf, iRect(rect.x2() - 14, rect.y + 14, 14, rect.h - 14 * 2-1));

        // corners
        gGfxMgr.Blit(PDGG(NMENU_BTN_NLU), surf, iPoint(rect.x, rect.y));
        gGfxMgr.Blit(PDGG(NMENU_BTN_NRU), surf, iPoint(rect.x2() - 14,rect.y));
        gGfxMgr.Blit(PDGG(NMENU_BTN_NLD), surf, iPoint(rect.x, rect.y2() - 14));
        gGfxMgr.Blit(PDGG(NMENU_BTN_NRD), surf, iPoint(rect.x2() - 14,rect.y2() - 14));
    }
    else
    {
		surf.FillRect(bkRect, RGB16(242, 230, 197));
		iRect trc;
		trc = iRect(rect.x + 14, rect.y, rect.w - 28, 14);					surf.FillRect(trc, RGB16(242, 230, 197));
		trc = iRect(rect.x + 14, rect.y + rect.h - 14, rect.w - 28, 12);	surf.FillRect(trc, RGB16(242, 230, 197));
		trc = iRect(rect.x, rect.y + 14, 14, rect.h - 28);					surf.FillRect(trc, RGB16(242, 230, 197));
		trc = iRect(rect.x + rect.w - 14, rect.y + 14, 12, rect.h - 28);	surf.FillRect(trc, RGB16(242, 230, 197));

        // top/bottom tiles
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_PU), surf, iRect(rect.x + 14, rect.y, rect.w - 14 * 2, 14));
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_PD), surf, iRect(rect.x + 14, rect.y2() - 14,rect.w - 14 * 2, 14));

        // left/right tiles
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_PL), surf, iRect(rect.x, rect.y + 14, 14, rect.h - 14 * 2));
        gGfxMgr.BlitTile(PDGG(NMENU_BTN_PR), surf, iRect(rect.x2() - 14, rect.y + 14, 14, rect.h - 14 * 2));

        // corners
        gGfxMgr.Blit(PDGG(NMENU_BTN_PLU), surf, iPoint(rect.x, rect.y));
        gGfxMgr.Blit(PDGG(NMENU_BTN_PRU), surf, iPoint(rect.x2() - 14,rect.y));
        gGfxMgr.Blit(PDGG(NMENU_BTN_PLD), surf, iPoint(rect.x, rect.y2() - 14));
        gGfxMgr.Blit(PDGG(NMENU_BTN_PRD), surf, iPoint(rect.x2() - 14,rect.y2() - 14));
    }

}

void ComposeBadge(iDib& dib, const iPoint& pt, const iStringT& txt)
{
    return; // disabled
	iSize sz = gGfxMgr.Dimension(PDGG(NMENU_BADGE));
	iRect trc = iRect(pt, sz);
	gGfxMgr.Blit(PDGG(NMENU_BADGE), dib, GetRealSpritePos(PDGG(NMENU_BADGE), trc, false, AlignCenter));
	iRect txtrc = trc + iPoint(5, 0);
	iTextComposer::FontConfig fc(badgefc);
	if(txt.Length() > 2)
		fc.fontSize--;
	gTextComposer.TextOut(fc, dib, txtrc, txt, txtrc, AlignCenter);
}

void iMainMenuView::iMenuBtn::OnCompose()
{
    bool show_pressed = false;
    iRect rc = GetScrRect();

    iTextComposer::FontConfig fc(dlgfc_plain);
    iRect orc(rc);
    fc.fontSize = newmenufc_grt_text.fontSize;
    fc.cmpProps.decor = iDibFont::DecNone;

    if (m_state & Pressed)
        show_pressed = true;

    if (m_active)
        show_pressed = !show_pressed;

    if (show_pressed)
    {
        ComposeBkg(true);
        fc.cmpProps.faceColor = RGB16(0,0,0);
    }
    else
    {
        ComposeBkg(false);
        fc.cmpProps.faceColor = RGB16(255, 237, 205);//RGB16(240, 228, 192);//RGB16(232, 224, 184);
    }
    gTextComposer.TextOut(fc,gApp.Surface(),orc,gTextMgr[m_rid],orc,AlignCenter);

	if(m_count > 0) {
		iStringT txt = iFormat(_T("%d"), m_count);
		iSize sz = gGfxMgr.Dimension(PDGG(NMENU_BADGE));
		ComposeBadge(gApp.Surface(), orc.TopRight() - iPoint(sz.w * 2 / 3, sz.h / 2 - 5), txt);
	}
}


//////////////////////////////////////////////////////////////////////////
// iMenuTab
//////////////////////////////////////////////////////////////////////////

class iMainMenuView::iMenuTab: public iView, public IViewCmdHandler
{
public:
    iMenuTab(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, (Visible|Enabled)), m_pCmdHandler(pCmdHandler)  {}

    virtual void OnActivate() { };
    virtual void OnDeactivate() { };
    virtual void SetActive(bool b ) { m_active = b;};
    virtual bool Process(fix32 t) { return true; };
	virtual bool BackAction() =0;
	virtual void OnShow() {}

protected:
    virtual void OnCompose()
    {
        iRect rc = GetScrRect();
        // CotullaTODO: frame debug
        //gApp.Surface().FrameRect(rc, cColor_Black);
        ComposeBkg(0, RGB16(216, 210, 192));
    }

	virtual void AfterCompose()
	{

	}

    virtual void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
    }

protected:
    void ComposeBkg(uint32 h_size, uint32 clr_top);
    void ComposeFrame();

    bool m_active;
    IViewCmdHandler*    m_pCmdHandler;
};



void iMainMenuView::iMenuTab::ComposeBkg(uint32 h_size, uint32 clr)
{
    iDib& surf = gApp.Surface();
    bool bDecs = false;
    iRect rect = GetScrRect();
    //  rect.InflateRect(8, 8, 8, 8);

    if (rect.IsEmpty()) return;

    if(rect.w <= 36 || rect.h <= 36) return; // too few space

	uint32 clr_top = RGB16(216, 210, 192);

    // tile background
    iRect bkRect(rect);
	bkRect.DeflateRect(5);
	surf.FillRect(bkRect, clr);
	iRect trc;
	trc = iRect(rect.x + 11, rect.y + 2, rect.w - 22, 20);
	surf.FillRect(trc, h_size ? clr_top : clr);
	trc = iRect(rect.x + 2, rect.y + 11, 12, rect.h - 22);				surf.FillRect(trc, clr);

	trc = iRect(rect.x + 14, rect.y + rect.h - 14, rect.w - 28, 12);	surf.FillRect(trc, clr);
	trc = iRect(rect.x + rect.w - 14, rect.y + 14, 12, rect.h - 28);	surf.FillRect(trc, clr);

    if (h_size)
    {
        iRect bkRect2(rect);
		//uint32 clr_bot = RGB16(132, 131, 130);
		//bkRect2.DeflateRect(5);
        bkRect2.DeflateRect(2, 18, 2, h_size + 22);
        surf.FillRect(bkRect2, clr_top);
    }


	// top/bottom tiles
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_NU), surf, iRect(rect.x + 18, rect.y, rect.w - 18 * 2, 18));
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_ND), surf, iRect(rect.x + 18, rect.y2() - 18,rect.w - 18 * 2, 18));

    // left/right tiles
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_NL), surf, iRect(rect.x, rect.y + 18, 18, rect.h - 18 * 2));
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_NR), surf, iRect(rect.x2() - 18, rect.y + 18, 18, rect.h - 18 * 2));

    // separator
    if (h_size)
    {
        sint32 y_pos = rect.y2() - 18 - 19 - h_size;
        gGfxMgr.BlitTile(PDGG(NMENU_WSP_SEP), surf, iRect(rect.x + 18, y_pos, rect.w - 18 * 2, 18));
        gGfxMgr.Blit(PDGG(NMENU_WSP_NLL), surf, iPoint(rect.x, y_pos));
        gGfxMgr.Blit(PDGG(NMENU_WSP_NRR), surf, iPoint(rect.x2() - 18, y_pos));
    }

    // corners
	if(h_size) {
		gGfxMgr.Blit(PDGG(NMENU_WSP_GLU), surf, iPoint(rect.x, rect.y));
		gGfxMgr.Blit(PDGG(NMENU_WSP_GRU), surf, iPoint(rect.x2() - 18,rect.y));
		gGfxMgr.Blit(PDGG(NMENU_WSP_NLD), surf, iPoint(rect.x, rect.y2() - 18));
		gGfxMgr.Blit(PDGG(NMENU_WSP_NRD), surf, iPoint(rect.x2() - 18,rect.y2() - 18));
	} else {
		gGfxMgr.Blit(PDGG(NMENU_WSP_NLU), surf, iPoint(rect.x, rect.y));
		gGfxMgr.Blit(PDGG(NMENU_WSP_NRU), surf, iPoint(rect.x2() - 18,rect.y));
		gGfxMgr.Blit(PDGG(NMENU_WSP_NLD), surf, iPoint(rect.x, rect.y2() - 18));
		gGfxMgr.Blit(PDGG(NMENU_WSP_NRD), surf, iPoint(rect.x2() - 18,rect.y2() - 18));
	}

}


void iMainMenuView::iMenuTab::ComposeFrame()
{
    iDib& surf = gApp.Surface();
    bool bDecs = false;
    iRect rect = GetScrRect();
	//  rect.InflateRect(8, 8, 8, 8);

    if (rect.IsEmpty()) return;

    if(rect.w <= 36 || rect.h <= 36) return; // too few space

	uint32 clr_top = RGB16(216, 210, 192);

	// top/bottom tiles
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_NU), surf, iRect(rect.x + 18, rect.y, rect.w - 18 * 2, 18));
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_ND), surf, iRect(rect.x + 18, rect.y2() - 18,rect.w - 18 * 2, 18));

    // left/right tiles
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_NL), surf, iRect(rect.x, rect.y + 18, 18, rect.h - 18 * 2));
    gGfxMgr.BlitTile(PDGG(NMENU_WSP_NR), surf, iRect(rect.x2() - 18, rect.y + 18, 18, rect.h - 18 * 2));

    // corners
	gGfxMgr.Blit(PDGG(NMENU_WSP_NLU), surf, iPoint(rect.x, rect.y));
	gGfxMgr.Blit(PDGG(NMENU_WSP_NRU), surf, iPoint(rect.x2() - 18,rect.y));
	gGfxMgr.Blit(PDGG(NMENU_WSP_NLD), surf, iPoint(rect.x, rect.y2() - 18));
	gGfxMgr.Blit(PDGG(NMENU_WSP_NRD), surf, iPoint(rect.x2() - 18,rect.y2() - 18));
}



//////////////////////////////////////////////////////////////////////////
// iMenuTab_SingleNew
//////////////////////////////////////////////////////////////////////////
// two stages: select map and select game settings
// as well this dialog used for hot seat, flag m_hotseat used for this task
//////////////////////////////////////////////////////////////////////////

class iMainMenuView::iMenuTab_SingleNew: public iMainMenuView::iMenuTab
{
public:
    iMenuTab_SingleNew(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 param)
    : iMenuTab(pViewMgr, pCmdHandler, rect, uid), m_ctrl1(NULL), m_ctrl2(NULL), m_second_stage(false)
    {
        iRect clRect = iRect(0, 0, m_Rect.w - 1, m_Rect.h);

        clRect.h -= 12;

        m_hotseat = param ? true : false;

        //clRect.DeflateRect(4, 0, 4, 0);
        AddChild(m_ctrl1 = new iScenListView(m_pMgr, this, clRect, 109, m_hotseat ? SCL_HOTSEAT : SCL_SINGLE));
        AddChild(m_ctrl2 = new iScenPropsView(m_pMgr, this, clRect, 109));

        //        iRect btnRc(18, m_Rect.h - 118, 100, 100);
        iRect btnRc(7, m_Rect.h - 60, m_Rect.w / 4 - 2, 60);

        //      sint32 off = (m_Rect.w - 2 * 18 - 4 * 100) / 3;

        /*
         iRect btnRc(18, m_Rect.h - 18 - 80, 100, 80);
         sint32 off = (m_Rect.w - 2 * 18 - 4 * 100) / 3;
         */


		btnRc.x += btnRc.w * 3;

		AddChild(new iNewMenuBottomBtn(m_pMgr, this, btnRc, 205, PDGG(NMENU_BBTN_NPLAY),
									   PDGG(NMENU_BBTN_PPLAY), TRID_MENU_START, Enabled));


        m_ctrl2->SetVisible(false);

		UpdateHighlights();

        m_map_desc = _T("");
    }

private:
	bool Process(fix32 t)
	{
		return m_ctrl1->Process(t);
	}

    void OnShow()
    {
        Analytics::sharedInstance()->TrackEvent("main_menu", "entered_screen", m_hotseat ? "hotseat" : "single game");
    }


	virtual bool BackAction() {
		if(IsStageTwo()) {
			StageOne();
			return true;
		} else
			return false;
	}

    void OnCompose()
    {
        iMenuTab::OnCompose();
        //iRect rc = GetScrRect();
        // CotullaTODO: frame debug
        ComposeBkg(
				   0
				   , RGB16(242, 230, 197));


    }

    void FillTipsWithMapCounts()
    {
        uint32 adv_maps, act_maps, hot_maps, all_maps;

        iNewMenuBottomBtn *vADV = (iNewMenuBottomBtn*)GetChildById(200);
        iNewMenuBottomBtn *vACT = (iNewMenuBottomBtn*)GetChildById(201);
        iNewMenuBottomBtn *vALL = (iNewMenuBottomBtn*)GetChildById(202);
        iNewMenuBottomBtn *vHOT = (iNewMenuBottomBtn*)GetChildById(203);

        adv_maps = m_ctrl1->GetMapCount(iScenListView::ShowFilter_ADV);
        act_maps = m_ctrl1->GetMapCount(iScenListView::ShowFilter_ACT);
        hot_maps = m_ctrl1->GetMapCount(iScenListView::ShowFilter_MULTI);
        all_maps = m_ctrl1->GetMapCount(iScenListView::ShowFilter_ALL);

        if (vADV) vADV->SetTextTip(iFormat(_T("%d"), adv_maps));
        if (vACT) vACT->SetTextTip(iFormat(_T("%d"), act_maps));
        if (vHOT) vHOT->SetTextTip(iFormat(_T("%d"), hot_maps));
        if (vALL) {
			vALL->SetTextTip(iFormat(_T("%d"), all_maps));
//			uint32 unpl = gMapEnum.GetUnplayedNum();
//			if(unpl > 0)
//				vALL->SetTextBadge(iFormat(_T("%d"), unpl));
//			else
//				vALL->SetTextBadge(_T(""));
		}
    }

    void OnActivate()
    {
        gMapEnum.Update();
        if (m_ctrl1)
        {
            // SHL: NOT NEEDED m_ctrl1->SetShowFilter(iScenListView::ShowFilter_ALL);
            m_ctrl1->LoadData();
            FillTipsWithMapCounts();
        }

        if (IsStageTwo())
            StageOne();
    }

    void StageTwo()
    {
        m_scProps = m_ctrl1->SelScen();
        m_map_desc = m_scProps.MapDescription();

        m_ctrl2->SetData(m_scProps);
        m_ctrl1->SetVisible(false);
        m_ctrl2->SetVisible(true);

        SetVisibleById(200, false);
        SetVisibleById(201, false);
        SetVisibleById(202, false);
        SetVisibleById(203, false);
        SetVisibleById(204,

					   // no back button for iphone version
					   false
					   );
        SetVisibleById(205, true);

        m_second_stage = true;

        m_ctrl2->SetData(m_scProps);
        Invalidate();
    }

    void StageOne()
    {
        m_ctrl2->SetVisible(false);
        m_ctrl1->SetVisible(true);

        SetVisibleById(200, true);
        SetVisibleById(201, true);
        SetVisibleById(202, true);
        SetVisibleById(203, true);
        SetVisibleById(204, false);
        SetVisibleById(205, false);

        m_second_stage = false;
        Invalidate();
    }

    bool IsStageTwo()
    {
        return m_second_stage;
    }

    void SendResult(uint32 res)
    {
        if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, (uint32)res);
    }


    void SetHighlightBtn(uint32 uid, bool f)
    {
        iNewMenuBottomBtn *v = (iNewMenuBottomBtn*)GetChildById(uid);
        if (v) v->SetHighlighted(f);
    }

	void UpdateHighlights() {
		SetHighlightBtn(200, m_ctrl1->GetShowFilter() == iScenListView::ShowFilter_ADV);
		SetHighlightBtn(201, m_ctrl1->GetShowFilter() == iScenListView::ShowFilter_ACT);
		SetHighlightBtn(202, m_ctrl1->GetShowFilter() == iScenListView::ShowFilter_ALL);
		SetHighlightBtn(203, m_ctrl1->GetShowFilter() == iScenListView::ShowFilter_MULTI);
	}

    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();

        if (!IsStageTwo())
        {
            if (uid == 200)
            {
                m_ctrl1->SetShowFilter(iScenListView::ShowFilter_ADV);
                UpdateHighlights();
            }
            else if (uid == 202)
            {
                m_ctrl1->SetShowFilter(iScenListView::ShowFilter_ALL);
                UpdateHighlights();
            }
            else if (uid == 203)
            {
                m_ctrl1->SetShowFilter(iScenListView::ShowFilter_MULTI);
                UpdateHighlights();
            }
            else if (uid == 201)
            {
                m_ctrl1->SetShowFilter(iScenListView::ShowFilter_ACT);
				UpdateHighlights();
            }
        }

        if (uid == 204)
        {
            // back button
            StageOne();
        }
        else if (uid == 205)
        {
            // start button
            // time to start new game
			//m_ctrl2->StartGame();
            m_scProps = m_ctrl2->ScenProps();
            m_scProps.ReorderPlayers();
			gGame.ExitGame(false);
            gGame.StartNewGame(m_scProps, true, true);
            //	SetVisible(false);
            SendResult(ResContinueNoHide);
        }
        else if (uid == 109)
        {
            if (param == DRC_OK)
            {
                if (IsStageTwo())
                {
                    // time to start new game
                    m_scProps = m_ctrl2->ScenProps();
                    m_scProps.ReorderPlayers();
					gGame.ExitGame(false);
                    gGame.StartNewGame(m_scProps, true, true);
					//SetVisible(false);
                    SendResult(ResContinueNoHide);
                }
                else
                {
                    // go to stage two
                    StageTwo();
                }
            }
            else if (param == DRC_CANCEL)
            {
                if (IsStageTwo())
                {   // go back to select scenario
                    StageOne();
                }
                else
                {
                    // this will hide current workspace
                    SendResult(ResSingle);
                }
            }
            else if (cmd == CCI_EDITCHANGED)
            {
                m_map_desc = m_scProps.MapDescription();
                Invalidate();
            }

        }

    }

private:
    iMapInfo        m_scProps;
    iPHLabel*       m_mapname;
    iScenListView  *m_ctrl1;
    iScenPropsView *m_ctrl2;
    bool            m_second_stage;
    bool            m_hotseat;
    iStringT        m_map_desc;
};



//////////////////////////////////////////////////////////////////////////
// iMenuTab_Credits
//////////////////////////////////////////////////////////////////////////

const iStringT CREDITS[] =
{
    _T("*PK2: Remastered"),
    _T(""),
    _T("*Programmers:"),
    _T(""),
    _T("Hedin"),
    _T("Cotulla"),
    _T("Andrey Shilov (ZOOM)"),
    _T("Alexander Nazarov (Nazavrik)"),
    _T("Sergei Chistov (Polovinel)"),
    _T(""),
    _T("*Artists:"),
    _T(""),
    _T("Eugene Ivanov (Jim)"),
    _T("Danny Flexner"),
    _T("Stanislav (Mr.Flasher)"),
    _T("Dmitry Koshelev (variag)"),
    _T("Kevin Brock (tekpir8)"),
    _T(""),
    _T("Dmitry Zaitsev"),
    _T("Alek Revyako"),
    _T("Mihail Chilikin"),
    _T("Oleg Kirshev"),
    _T(""),
    _T("*Castle Graphics:"),
    _T("Anton Baecker (improvisator)"),
    _T("Vladimir Gulevskiy (GRAFuS)"),
    _T(""),
    _T(""),
    _T("*Guest artists:"),
    _T(""),
    _T("Simon Butler"),
    _T("Ian Schlaepfer"),
    _T("Paul Noble"),
    _T("Michael Elwin Setiadi"),
    _T("Ivan Ivanov"),
    _T(""),
    _T("*Music:"),
    _T(""),
    _T("Kevin MacLeod"),
    _T("www.incompetech.com"),
    _T(""),
    _T("*Testing"),
    _T(""),
    _T("Daniil Levashov (dizabled)"),
    _T("Victor Chirea (Victorash)"),
    _T("Dmitry Freeman (Fuzzy)"),
    _T("David Santos"),
    _T("Yaron Karasik"),
    _T("Richard Hallas"),
    _T("Lynn Kozak"),
    _T("Peter Pyrkov"),
    _T("Michael Sughroue"),
    _T("Draco Lockhard"),
    _T(""),
    _T(""),
    _T(""),
    _T("*Palm Kingdoms 1"),
    _T(""),
    _T("*Programmers:"),
    _T(""),
    _T("Hedin"),
    _T("Anton Stuk (SiGMan)"),
    _T("Cotulla"),
    _T(""),
    _T("*Artists:"),
    _T(""),
    _T("Eugene Ivanov (Jim)"),
    _T("Danny Flexner"),
    _T("Stanislav (Mr.Flasher)"),
    _T("Dmitry Zaitsev"),
    _T("Alek Revyako"),
    _T("Mihail Chilikin"),
    _T("Oleg Kirshev"),
    _T("Vladimir Gulevskiy (GRAFuS)"),
    _T("Kevin Brock (tekpir8)"),
    _T(""),
    _T("*Guest artists:"),
    _T(""),
    _T("Simon Butler"),
    _T("Ian Schlaepfer"),
    _T("Paul Noble"),
    _T("Michael Elwin Setiadi"),
    _T("Ivan Ivanov"),
    _T(""),
    _T("*Lead Tester:"),
    _T(""),
    _T("Victor Chirea (Victorash)"),
    _T(""),
    _T("*Testers:"),
    _T(""),
    _T("Victor Chirea (victorash)"),
    _T("Yaroslaw Artemiev (Yaros)"),
    _T("Stanislav (Mr.Flasher)"),
    _T("Andrew Busygin"),
    _T("Sergey Indiukov (DukereD)"),
    _T("Eugene Ivanov (Jim)"),
    _T("Obi-Van (Stas)"),
    _T("Ivan Alekhin (gsvano)"),
    _T("BaRaKuDa (Alexander)"),
    _T("Artem Stragnik (Starmi)"),
    _T("Chibis"),
    _T("Mihail Loktionov (Matador)"),
    _T("Alexey Muratov (Till)"),
    _T("Alexander Muranov (SAS)"),
    _T("Tsukanov Nikita (keks-n)"),
    _T("Lain"),
    _T("leshekb"),
    _T(""),
    _T("*Localizations:"),
    _T(""),
    _T("Slovak: feriancr"),
    _T("French: Fhal"),
    _T("Czech: Georgee "),
    _T("Bronislav Lavecky (Broono)"),
    _T("German: Dirk Schaefer (Devender)"),
    _T("Simon Hackfort (Spawn)"),
    _T("Wuff"),
    _T(""),
    _T("*Map designers:"),
    _T(""),
    _T("Roman Tersky (Mongol)"),
    _T("Ruslan Pantyushin (Lain)"),
    _T("Victor Chirea (Victorash)"),
    _T("Eugene Ivanov (Jim)"),
    _T("Obi-Van (Stas)"),
    _T("Artem Stragnik (Starmi)"),
    _T("Mihail Loktionov (Matador)"),
    _T("Draco Lockhard"),
    _T("Vladimir Shevelev (redvova)"),
    _T("Kevin Brock (tekpir8)"),
    _T("Yaroslaw Artemiev (Yaros)"),
    _T("Dmitry Koshelev (variag)"),
    _T(""),
    _T("*WEB-design & mastering:"),
    _T(""),
    _T("Hedin"),
    _T("Eugene Ivanov (Jim)"),
    _T(""),
    _T("*Manual and Help file:"),
    _T(""),
    _T("Victor Chirea (victorash)"),
    _T("Andy Young (67L48)"),
    _T("Sergey Indiukov"),
    _T(""),
    _T("*Alpha version supporters:"),
    _T(""),
    _T("Andrey Glinsky"),
    _T("Fjodor Dzjuba"),
    _T("Justin Jones"),
    _T("Chibis"),
    _T("Ralph Sanders"),
    _T("Chris Bygrave"),
    _T("David Zuravel"),
    _T("Artem Stragnik (Starmi)"),
    _T("Valdemar"),
    _T("RS"),
    _T("Sergey Indiukov (DukereD)"),
    _T("Tom Fitzpatrick"),
    _T("Roman Klyachkivsky"),
    _T("Olivier Binda"),
    _T("Emanuel Mayer"),
    _T("Konstantin Ilyin"),
    _T("Paul Cook"),
    _T("Anton Rozen"),
    _T("Petr Cermak"),
    _T("Norry Messina"),
    _T("Cary Fleming"),
    _T("Ljusalfheim (4PDA)"),
    _T("Marc Ruder"),
    _T("Michael Thomas"),
    _T("Jean-Paul Lacombe"),
    _T("estena.ru"),
    _T("Maksim Frolov"),
    _T("Miroslav Dvoracek"),
    _T("Evgeny Kremer"),
    _T("GrulezZ"),
    _T("LSDsl"),
    _T("Sergey Chalykh"),
    _T("Anton Stuk"),
    _T("Dmitry Sinchilin"),
    _T("Aleksander Larionov"),
    _T("Lain "),
    _T("Dead mummy"),
    _T(""),
    _T("*Special thanx to:"),
    _T(""),
    _T("Robert Tarasov"),
    _T("Jon Van Caneghem"),
    _T("Dmitry Lekhno (Krondor)"),
    _T("~")
};

class iMainMenuView::iMenuTab_Credits: public iMainMenuView::iMenuTab
{
public:
    iMenuTab_Credits(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iMenuTab(pViewMgr, pCmdHandler, rect, uid), m_totallines(0), m_curline(0),
    m_draw_start(0), m_draw_count(0), m_alpha(0), m_tcounter(0)
    {
        iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);
        sint32 yp = clRect.y + 100;

        //AddChild(new iPHLabel(m_pMgr, iRect(clRect.x, yp, clRect.w, 15), iStringT(_T("Credits")), AlignTop, dlgfc_topic));

        m_credits.RemoveAll();
        int i = 0;
        while(CREDITS[i].Length() == 0 || CREDITS[i].CStr()[0] != '~')
        {
            m_credits.Add(CREDITS[i]);
            i++;
        }
        const iSimpleArray<iStringT>& crtext = m_credits;
        m_totallines = crtext.GetSize();
    }

	virtual bool BackAction() { return false;	}


    ~iMenuTab_Credits()
    {
        m_pMgr->CleanupTimers(this);
    }

    void OnActivate()
    {
        m_pMgr->SetTimer(100, 1, this);
    }

    void OnShow()
    {
        Analytics::sharedInstance()->TrackEvent("main_menu", "entered_screen", "credits");
    }


	void OnDeactivate()
	{
		m_pMgr->CleanupTimers(this);
	}

protected:

    void OnCompose()
    {
        iMenuTab::OnCompose();

        iRect rc = GetScrRect();
        rc.DeflateRect(18);

        const iSimpleArray<iStringT>& crtext = m_credits;
        uint32 ypos = 0;

        iTextComposer::FontConfig fc(newmenufc_credits);
        fc.cmpProps.decor = iDibFont::DecBorder;
        fc.cmpProps.faceColor = RGB16(0,0,0);
        fc.cmpProps.alpha = m_alpha;

        ypos += (m_Rect.h - m_draw_count * 25 - gTextComposer.GetFontHeight(fc.fontSize)) / 3;

		fc.fontSize = FS_PT_24;
		gTextComposer.TextOut(fc, gApp.Surface(), iPoint(),  m_title, iRect(rc.x,rc.y + ypos, rc.w, 14), AlignTop);

		ypos += gTextComposer.GetFontHeight(fc.fontSize);

		fc.fontSize = FS_PT_16;

        for (uint i = 0; i < m_draw_count; i++)
        {
            OutputDebugString(crtext[m_draw_start + i].CStr());
            OutputDebugString(L"\r\n");
            gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), crtext[m_draw_start + i], iRect(rc.x,rc.y + ypos, rc.w, 14), AlignTop);
            ypos += 25;
        }

    }


    void CalcCurrentGroup()
    {
        uint32 i;
        const iSimpleArray<iStringT>& crtext = m_credits;

        m_curline = m_draw_start + m_draw_count;
        if (m_curline >= m_totallines)
            m_curline = 0;

		iStringT curline = crtext[m_curline];
        if (curline.Find(iStringT(_T("*"))) != iStringT::NOT_FOUND)
        {
            // save title string
			iStringT str = crtext[m_curline];
			//str.Replace(_T("*"), _T(""));
            m_title = str;
        }

        // calc number of lines to show (start of group detected by # in string)
        for (i = m_curline + 1; i < m_totallines; i++)
        {
            if (crtext[i].Find(iStringT(_T("*"))) != iStringT::NOT_FOUND)
            {
                break;
            }
            if (i - m_curline > 15)
                break;
        }

        m_draw_start = m_curline + 1;
        m_draw_count = i - m_curline - 1;
    }


    void OnTimer(uint32 tid)
    {
        if (tid != 1) return;
        if (!m_active) return;

        if (m_tcounter == 0)
        {
            CalcCurrentGroup();
        }

        m_tcounter++;
        if (m_tcounter < 20)  // fade in (20 * 10 = 200ms)
        {
            m_alpha += 13;
            if (m_alpha > 255) m_alpha = 255;
            Invalidate();
            m_pMgr->SetTimer(20, 1, this);
            return;
        }
        else if (m_tcounter < 100)  // const show ((100 - 20) * 10 = 800ms)
        {
            m_alpha = 255;
            m_pMgr->SetTimer(20, 1, this);
            return;
        }
		else if (m_tcounter < 120)  // fade out (20 * 10 = 200ms)
		{
			m_alpha -= 13;
			if (m_alpha < 0) m_alpha = 0;
			Invalidate();
			m_pMgr->SetTimer(20, 1, this);
			return;
		}
        else // more than 1 sec - next group
        {
            m_tcounter = 0;
            m_alpha = 0;
        }

        Invalidate();
        m_pMgr->SetTimer(20, 1, this);
    }

private:
    iStringT m_title;
    uint32 m_totallines;
    uint32 m_curline;
    uint32 m_draw_start;
    uint32 m_draw_count;
    sint32 m_alpha;
    uint32 m_tcounter;
    iSimpleArray<iStringT> m_credits;
};


//////////////////////////////////////////////////////////////////////////
// iMenuTab_DownloadMaps
//////////////////////////////////////////////////////////////////////////

void ComposeMapPrice(iDib& dib, const iRect& irc, const iStringT& cost, bool bIsUpdating, bool bIsHotNew)
{
	iTextComposer::FontConfig fc_m(newmenufc_grt_text);

	fc_m.cmpProps.faceColor = RGB16(0, 0, 0);

	iStringT c = cost;

	if(bIsUpdating)
		c = iStringT(gTextMgr[TRID_SHOP_UPDATE]);
	/*else if(bIsHotNew)
     c = iStringT(gTextMgr[TRID_SHOP_HOT_NEW]) + _T("   ") + cost;
     */
	iSize sz = gTextComposer.GetTextBoxSize(c, irc.w, fc_m);
	gTextComposer.TextBoxOut(fc_m, dib, c, AlignRect(sz, irc, AlignCenter));
}

// class iMainMenuView::iMenuTab_DownloadMaps: public iMainMenuView::iMenuTab
// {
// public:
// 	enum ShopTabState {
// 		STS_BLOCKED = 0,
// 		STS_LIST,
// 		STS_DESC
// 	};
// public:
//     iMenuTab_DownloadMaps(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
//     : iMenuTab(pViewMgr, pCmdHandler, rect, uid), isPurch(false)
//     {
//         iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);

//         clRect.h -= 10;

//         AddChild(m_ctrl = new iMapShopView(m_pMgr, this, clRect, 109));

//         iRect btnRc(7, m_Rect.h - 80, m_Rect.w / 5 - 2, 70);
//         //sint32 off = (m_Rect.w - 2 * 18 - 4 * 100) / 4;

//         //        iSize sz = gGfxMgr.Dimension(PDGG(NMENU_BBTN_NBACK));
//         //        iRect rc = iRect(-sz.w, m_Rect.h/2 - sz.h, sz.w, sz.h);

//         //        AddChild(m_pBtnBack = new iImgBtn(m_pMgr, this, rc, 400, PDGG(NMENU_BBTN_NBACK), PDGG(NMENU_BBTN_PBACK)));
//         /*

//          iRect btnRc(18, m_Rect.h - 18 - 80, 100, 80);
//          sint32 off = (m_Rect.w - 2 * 18 - 4 * 100) / 3;
//          */

// 		btnRc.x += btnRc.w * 4;
// 		AddChild(m_pBtnBuy = new iNewMenuBottomBtn(m_pMgr, this, btnRc, 206, PDGG(NMENU_BBTN_NUPDATES),
//                                                    PDGG(NMENU_BBTN_PUPDATES), TRID_MENU_PURCHASE));

// 		SetState(STS_LIST);
// 		UpdateHighlights();
//     }

// 	virtual bool BackAction() {
// 		if(m_state == STS_DESC) {
// 			SetState(STS_LIST);
// 			m_ctrl->LoadData();
// 			Invalidate();
// 			return true;
// 		} else
// 			return false;
// 	}

//     void OnShow()
//     {
//         Analytics::sharedInstance()->TrackEvent("main_menu", "entered_screen", "download maps");
//     }


//     bool Process(fix32 t)
//     {
//         bool bShowList = m_ctrl->Process(t);

//         if( bShowList ){
//             ((iMainMenuView*)m_pCmdHandler)->m_pBtnBack->SetVisible(true);

//         }
//         return bShowList;
//     }

// private:

//     void OnCompose()
//     {
//         iMenuTab::OnCompose();
//         ComposeBkg(0, RGB16(242, 230, 197));

// 		iTextComposer::FontConfig fc_l(newmenufc_high_text);
//         fc_l.cmpProps.faceColor = RGB16(0, 0, 0);

// //		if(m_ctrl->CurMap()) {
// //			iRect rc = GetScrRect();
// //			iRect txt_rc = AlignRect(iSize(rc.w - 140, 150), rc, AlignBottom);
// //			ComposeMapPrice(gApp.Surface(), txt_rc, m_ctrl->CurMap()->m_Cost, m_ctrl->CurMap()->m_bUpdating, m_ctrl->CurMap()->m_HotNew);
// //		}
//     }

// 	void SetState(ShopTabState sts) {
// 		m_state = sts;
// 		switch(sts) {
// 			case STS_BLOCKED:
// 				m_pBtnBuy->SetVisible(false);
//                 ((iMainMenuView*)m_pCmdHandler)->m_pBtnBack->SetVisible(false);
//                 //m_pBtnBack->SetVisible(false);
// 				break;
// 			case STS_LIST:
// 				m_pBtnBuy->SetVisible(false);
//                 ((iMainMenuView*)m_pCmdHandler)->m_pBtnBack->SetVisible(true);
//                 //m_pBtnBack->SetVisible(true);
// 				break;
// 			case STS_DESC:

// 				m_pBtnBuy->SetVisible(true);
//                 ((iMainMenuView*)m_pCmdHandler)->m_pBtnBack->SetVisible(true);
//                 //m_pBtnBack->SetVisible(true);
// 				break;
// 		}
// 	}

//     void FillTipsWithMapCounts()
//     {
//         uint32 adv_maps, act_maps, hot_maps, all_maps, updates;

//         iNewMenuBottomBtn *vADV = (iNewMenuBottomBtn*)GetChildById(200);
//         iNewMenuBottomBtn *vACT = (iNewMenuBottomBtn*)GetChildById(201);
//         iNewMenuBottomBtn *vALL = (iNewMenuBottomBtn*)GetChildById(202);
//         iNewMenuBottomBtn *vHOT = (iNewMenuBottomBtn*)GetChildById(203);
// 		iNewMenuBottomBtn *vUPD = (iNewMenuBottomBtn*)GetChildById(204);

//         adv_maps = m_ctrl->GetMapCount(MapShop::ShowFilter_ADV);
//         act_maps = m_ctrl->GetMapCount(MapShop::ShowFilter_ACT);
//         hot_maps = m_ctrl->GetMapCount(MapShop::ShowFilter_MULTI);
//         all_maps = m_ctrl->GetMapCount(MapShop::ShowFilter_ALL);
// 		updates = m_ctrl->GetMapCount(MapShop::ShowFilter_UPD);

//         if (vADV) vADV->SetTextTip(iFormat(_T("%d %s"), adv_maps, gTextMgr[TRID_MENU_MAPS]));
//         if (vACT) vACT->SetTextTip(iFormat(_T("%d %s"), act_maps, gTextMgr[TRID_MENU_MAPS]));
//         if (vHOT) vHOT->SetTextTip(iFormat(_T("%d %s"), hot_maps, gTextMgr[TRID_MENU_MAPS]));
//         if (vALL) {
// 			vALL->SetTextTip(iFormat(_T("%d %s"), all_maps, gTextMgr[TRID_MENU_MAPS]));
// 			uint32 newmaps = m_ctrl->GetNewMapCount();
// 			if(newmaps > 0)
// 				vALL->SetTextBadge(iFormat(_T("%d"), newmaps));
// 			else
// 				vALL->SetTextBadge(_T(""));

// 		}
// 		if (vUPD) {
// 			vUPD->SetTextTip(iFormat(_T("%d %s"), updates, gTextMgr[TRID_MENU_MAPS]));
// 			if(updates > 0)
// 				vUPD->SetTextBadge(iFormat(_T("%d"), updates));
// 			else
// 				vUPD->SetTextBadge(_T(""));
// 		}
//     }

//     void OnActivate()
//     {
//         if (m_ctrl) m_ctrl->LoadData(), FillTipsWithMapCounts();
//     }

//     void SetHighlightBtn(uint32 uid, bool f)
//     {
//         iNewMenuBottomBtn *v = (iNewMenuBottomBtn*)GetChildById(uid);
//         if (v) v->SetHighlighted(f);
//     }


// 	void UpdateHighlights() {
// 		SetHighlightBtn(200, m_ctrl->GetShowFilter() == MapShop::ShowFilter_ADV);
// 		SetHighlightBtn(201, m_ctrl->GetShowFilter() == MapShop::ShowFilter_ACT);
// 		SetHighlightBtn(202, m_ctrl->GetShowFilter() == MapShop::ShowFilter_ALL);
// 		SetHighlightBtn(203, m_ctrl->GetShowFilter() == MapShop::ShowFilter_MULTI);
// 		SetHighlightBtn(204, m_ctrl->GetShowFilter() == MapShop::ShowFilter_UPD);
// 	}

//     void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
//     {
//         uint32 uid = pView->GetUID();

//         if (uid == 200)
//         {
//             m_ctrl->SetShowFilter(MapShop::ShowFilter_ADV);
// 			UpdateHighlights();
//         }
//         else if (uid == 202)
//         {
//             m_ctrl->SetShowFilter(MapShop::ShowFilter_ALL);
//             UpdateHighlights();
//         }
//         else if (uid == 203)
//         {
//             m_ctrl->SetShowFilter(MapShop::ShowFilter_MULTI);
//             UpdateHighlights();
//         }
//         else if (uid == 201)
//         {
//             m_ctrl->SetShowFilter(MapShop::ShowFilter_ACT);
//             UpdateHighlights();
//         }
// 		else if (uid == 204)
// 		{
// 			m_ctrl->SetShowFilter(MapShop::ShowFilter_UPD);
// 			UpdateHighlights();
// 		}

// 		else if(uid == 109) {
// 			if (cmd == CCI_EDITCHANGED)
// 			{
// 				// called on map count update
// 				FillTipsWithMapCounts();
// 				SetState(STS_LIST);
// 				Invalidate();
// 			} else if(cmd == CCI_LBSELECTED)
// 			{
//                 if( param == 1 ){

//                     isPurch = true;
//                     SetState(STS_BLOCKED);
//                 }
//                 else if( param == 2 ){

//                     m_ctrl->LoadData();
//                     SetState(STS_LIST);
//                 }
//                 else
//                     SetState(STS_DESC);
// 			}
// 		} else if(uid == 205) {
// 			m_ctrl->LoadData();
// 			SetState(STS_LIST);
// 		} else if(uid == 206) {
// 			m_ctrl->DoBuy();
// 			SetState(STS_BLOCKED);
// 		}

//     }

//     iMapShopView *m_ctrl;
// 	iButton* m_pBtnBuy;
// 	ShopTabState m_state;
//     bool isPurch;
// };

//////////////////////////////////////////////////////////////////////////
// iMenuTab_SingleLoad
//////////////////////////////////////////////////////////////////////////

class iMainMenuView::iMenuTab_SingleLoad: public iMainMenuView::iMenuTab
{
public:
    iMenuTab_SingleLoad(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iMenuTab(pViewMgr, pCmdHandler, rect, uid)
    {
        iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);
		clRect.h -= 25;
        AddChild(m_ctrl = new iSaveGameView(m_pMgr, this, clRect, 109, false) );

        iRect btnRc(m_Rect.w - 18 - 100, m_Rect.h - 118, 100, 100);
        m_map_desc = _T("");
    }

private:
	bool Process(fix32 t)
	{
		return m_ctrl->Process(t);
	}

    void OnActivate()
    {
        // reload saved games list on activate tab
        if (m_ctrl) m_ctrl->LoadData();
    }

	virtual bool BackAction() {
		return false;
	}



    void OnCompose()
    {
        iMenuTab::OnCompose();

		ComposeBkg(
				   0,
				   RGB16(242, 230, 197));


        iTextComposer::FontConfig fc_l(newmenufc_high_text);
        fc_l.cmpProps.faceColor = RGB16(0, 0, 0);

        iRect rc = GetScrRect();
        iRect txt_rc(rc.x + 20, rc.y + rc.h - 18 - 100, rc.w - 140, 100);
        //gApp.Surface().FrameRect(txt_rc, cColor_Black);
        //       gTextComposer.TextBoxOut(fc_l, gApp.Surface(), m_map_desc, txt_rc);
    }

	void AfterCompose()
    {
		ComposeFrame();
    }


    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        // event can come from bottom-btn or from iSaveGameView control
        if (uid == 200 || uid == 109)
        {
            if (cmd == CCI_BTNCLICK)
            {
                if (m_ctrl->IsGoodSelection())
                {
                    iQuestDlg dlg(&gApp.ViewMgr(), _T(""), gTextMgr[TRID_CONFIRM_RELOAD], PID_NEUTRAL);
                    if(gGame.Started() && dlg.DoModal() != DRC_YES)
                    {
                        return;
                    }
                    Result rs;
                    // load button
                    iMapInfo scenProps = m_ctrl->SelScenario();
                    scenProps.ReorderPlayers();
                    gGame.ExitGame(false);
                    gGame.StartNewGame(scenProps, false, false);
					SetVisible(false);
                    rs = ResContinueNoHide;

                    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, rs);
                } else if(m_ctrl->GetAutoSave()) {
					Result rs;
					iMapInfo scenProps = *m_ctrl->GetAutoSave();
					scenProps.ReorderPlayers();
					gGame.ExitGame(false);
					gGame.StartNewGame(scenProps, false, false);
					SetVisible(false);
					rs = ResContinueNoHide;

					if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, rs);
				}
            }
            else if (cmd == CCI_EDITCHANGED)
            {
                const iMapInfo &scenProps = m_ctrl->SelScenario();
                m_map_desc = scenProps.MapDescription();
                Invalidate();
            }
        }
    }

    iSaveGameView *m_ctrl;
    iStringT m_map_desc;
};

//////////////////////////////////////////////////////////////////////////
// iMenuTab_SingleSave
//////////////////////////////////////////////////////////////////////////

class iMainMenuView::iMenuTab_SingleSave: public iMainMenuView::iMenuTab
{
public:
    iMenuTab_SingleSave(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iMenuTab(pViewMgr, pCmdHandler, rect, uid)
    {
        iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);

		clRect.h -= 25;
        AddChild(m_ctrl = new iSaveGameView(m_pMgr, this, clRect, 109, true) );

    }

	virtual bool BackAction() {
		return false;
	}


private:
	bool Process(fix32 t)
	{
		return m_ctrl->Process(t);
	}

    void OnActivate()
    {
        // reload saved games list on activate tab
        if (m_ctrl) m_ctrl->LoadData();
    }

    void OnCompose()
    {
        iMenuTab::OnCompose();
        ComposeBkg(
				   0,
				   RGB16(242, 230, 197));

        iTextComposer::FontConfig fc_l(newmenufc_high_text);
        fc_l.cmpProps.faceColor = RGB16(0, 0, 0);

        iRect rc = GetScrRect();
        iRect txt_rc(rc.x + 20, rc.y + rc.h - 18 - 100, rc.w - 140, 100);
        //gApp.Surface().FrameRect(txt_rc, cColor_Black);
        //gTextComposer.TextBoxOut(fc_l, gApp.Surface(), m_map_desc, txt_rc);
    }


    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        // event can come from bottom-btn or from iSaveGameView control
        if (uid == 200 || uid == 109)
        {
            if (cmd == CCI_BTNCLICK)
            {
                if (m_ctrl->IsGoodSelection())
                {
                    Result rs;
                    RelativeFilePath fname;
                    if(m_ctrl->SelFile(fname)) {
                        SaveGameFile(fname, gGame.Map());
					} else {
						iTextDlg dlg(&gApp.ViewMgr(), _T("Unable to save"), _T("No more space: choose existing slot!"), PID_NEUTRAL);
						dlg.DoModal();
					}
                    rs = ResContinue;
                    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, rs);
                } else {
					Result rs;
					gGame.Autosave();
					rs = ResContinue;
                    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, rs);
				}
            }
        }
    }

    iSaveGameView *m_ctrl;
};

//////////////////////////////////////////////////////////////////////////
// iMenuTab_SingleTutorial
//////////////////////////////////////////////////////////////////////////
// two stages: select map and select game settings
//////////////////////////////////////////////////////////////////////////

class iMainMenuView::iMenuTab_SingleTutorial: public iMainMenuView::iMenuTab
{
public:
    iMenuTab_SingleTutorial(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iMenuTab(pViewMgr, pCmdHandler, rect, uid), m_ctrl1(NULL), m_ctrl2(NULL), m_second_stage(false)
    {
        iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);

        clRect.h -= 129;
        //clRect.DeflateRect(4, 0, 4, 0);
        AddChild(m_ctrl1 = new iScenListView(m_pMgr, this, clRect, 109, SCL_TUTORIAL));
        AddChild(m_ctrl2 = new iScenPropsView(m_pMgr, this, clRect, 109));

        iRect btnRc(18, m_Rect.h - 118, 100, 100);
        sint32 off = (m_Rect.w - 2 * 18 - 4 * 100) / 3;

        AddChild(new iNewMenuBottomBtn(m_pMgr, this, btnRc, 200, PDGG(NMENU_BBTN_NADV),
                                       PDGG(NMENU_BBTN_PADV), TRID_MENU_MAPS1, Visible|Enabled));
        AddChild(new iNewMenuBottomBtn(m_pMgr, this, btnRc, 204, PDGG(NMENU_BBTN_NBACK),
                                       PDGG(NMENU_BBTN_PBACK), TRID_MENU_BACK, Enabled));
        btnRc.x += 100 + off;

        AddChild(new iNewMenuBottomBtn(m_pMgr, this, btnRc, 201, PDGG(NMENU_BBTN_NHNS),
                                       PDGG(NMENU_BBTN_PHNS), TRID_MENU_MAPS2, Visible|Enabled));
        btnRc.x = m_Rect.w - 18 - 100;

        AddChild(new iNewMenuBottomBtn(m_pMgr, this, btnRc, 202, PDGG(NMENU_BBTN_NALLMAPS),
                                       PDGG(NMENU_BBTN_PALLMAPS), TRID_MENU_MAPS4));
        AddChild(new iNewMenuBottomBtn(m_pMgr, this, btnRc, 205, PDGG(NMENU_BBTN_NPLAY),
                                       PDGG(NMENU_BBTN_PPLAY), TRID_MENU_START, Enabled));
        btnRc.x -= 100 + off;

        AddChild(new iNewMenuBottomBtn(m_pMgr, this, btnRc, 203, PDGG(NMENU_BBTN_NMULT),
                                       PDGG(NMENU_BBTN_PMULT), TRID_MENU_MAPS3));

        m_ctrl2->SetVisible(false);

        if (gMapEnum.GetTutorialsNum() == 1)
        {
            StageTwo();
        }
    }

	virtual bool BackAction() {
		return false;
	}


private:
    void OnCompose()
    {
        iMenuTab::OnCompose();
        //iRect rc = GetScrRect();
        // CotullaTODO: frame debug
        ComposeBkg(100, RGB16(242, 230, 197));
        //gApp.Surface().FrameRect(rc, cColor_Black);
    }

    void OnActivate()
    {
        if (gMapEnum.GetTutorialsNum() == 1)
        {
            StageTwo();
            return;
        }

        if (IsStageTwo())
        {
            StageOne();
        }
    }

    void StageTwo()
    {
        if (gMapEnum.GetTutorialsNum() == 1)
        {
            // we have only one map, find it
            iScenList sl;

            //gMapEnum.Filter(sl, iMapInfo::MTTutorial);
            // check(sl.GetSize() == 1);
            m_scProps = *sl[0];
        }
        else
        {
            m_scProps = m_ctrl1->SelScen();
        }

        m_ctrl2->SetData(m_scProps);
        m_ctrl1->SetVisible(false);
        m_ctrl2->SetVisible(true);

        SetVisibleById(200, false);
        SetVisibleById(201, false);
        SetVisibleById(202, false);
        SetVisibleById(203, false);
        SetVisibleById(204, true);
        SetVisibleById(205, true);

        m_second_stage = true;

        m_ctrl2->SetData(m_scProps);
        Invalidate();
    }

    void StageOne()
    {
        m_ctrl2->SetVisible(false);
        m_ctrl1->SetVisible(true);

        SetVisibleById(200, true);
        SetVisibleById(201, true);
        SetVisibleById(202, true);
        SetVisibleById(203, true);
        SetVisibleById(204, false);
        SetVisibleById(205, false);

        m_second_stage = false;
        Invalidate();
    }

    bool IsStageTwo()
    {
        return m_second_stage;
    }

    void SendResult(uint32 res)
    {
        if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, (uint32)res);
    }


    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if (uid == 204)
        {
            // back button
            StageOne();
        }
        else if (uid == 205)
        {
            // start button
            // time to start new game
            m_scProps = m_ctrl2->ScenProps();
            m_scProps.ReorderPlayers();
			gGame.ExitGame(false);
            gGame.StartNewGame(m_scProps, true, true);
			//SetVisible(false);
			SendResult(ResContinueNoHide);
        }
        else if (uid == 109)
        {
            if (param == DRC_OK)
            {
                if (IsStageTwo())
                {
                    // time to start new game
                    m_scProps = m_ctrl2->ScenProps();
                    m_scProps.ReorderPlayers();
					gGame.ExitGame(false);
                    gGame.StartNewGame(m_scProps, true, true);
					//SetVisible(false);
                    SendResult(ResContinueNoHide);
                }
                else
                {
                    // go to stage two
                    StageTwo();
                }
            }
            else if (param == DRC_CANCEL)
            {
                if (IsStageTwo())
                {   // go back to select scenario
                    StageOne();
                }
                else
                {
                    // this will hide current workspace
                    SendResult(ResSingle);
                }
            }

        }
    }

private:
    iMapInfo        m_scProps;
    iPHLabel*       m_mapname;
    iScenListView  *m_ctrl1;
    iScenPropsView *m_ctrl2;
    bool            m_second_stage;
};


//////////////////////////////////////////////////////////////////////////
// iMenuTab_Settings
//////////////////////////////////////////////////////////////////////////

class iMainMenuView::iMenuTab_Settings: public iMainMenuView::iMenuTab
{
public:
    iMenuTab_Settings(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iMenuTab(pViewMgr, pCmdHandler, rect, uid)
    {
        iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);
        clRect.DeflateRect(18);
        AddChild(m_ctrl = new iSettingsView(m_pMgr, this, clRect, 109));
    }
	virtual bool BackAction() {
		return false;
	}

private:
	virtual void OnCompose()
    {
        iRect rc = GetScrRect();
        ComposeBkg(0, RGB16(246, 233, 197));
	}
	void OnActivate()
	{
		m_ctrl->OnActivate();
	}

    void OnShow()
    {
        Analytics::sharedInstance()->TrackEvent("main_menu", "entered_screen", "settings");
    }

    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        // CotullaTODO: not used, remove?
    }

    iSettingsView *m_ctrl;
};



//////////////////////////////////////////////////////////////////////////
// iMainMenuView (one view used for main, single and multi menus)
//////////////////////////////////////////////////////////////////////////


static uint32 s_buttons_trid_main[5] =
{
    TRID_MENU_SINGLEPLAYER,
    TRID_MENU_MULTIPLAYER,
    TRID_MENU_TUTORIAL,
    TRID_MENU_CREDITS,
    TRID_MENU_EXITGAME,
};


static uint32 s_buttons_trid_single[5] =
{
    TRID_MENU_NEWSCENARIO,
    TRID_MENU_LOADGAME,
    TRID_MENU_TUTORIAL,
    TRID_MENU_GAMESETTINGS,
    TRID_MENU_BACK
};

static uint32 s_buttons_trid_ingame[5] =
{
    TRID_MENU_EXITTOMAINMENU,
    TRID_MENU_SAVEGAME,
    TRID_MENU_LOADGAME,
	TRID_MENU_GAMESETTINGS,
    TRID_MENU_BACK
};

static uint32 s_buttons_trid_multi[5] =
{
    TRID_MENU_ONLINE,
    TRID_MENU_MULTIPLAYER,
    TRID_MENU_HELP,
    TRID_MENU_GAMESETTINGS,
    TRID_MENU_BACK
};

static uint32 s_buttons_trid_singlemap[5] =
{
    TRID_MENU_NEWGAME,
    TRID_MENU_LOADGAME,
    TRID_MENU_GAMESETTINGS,
    TRID_MENU_TUTORIAL,
    TRID_MENU_CREDITS,
};


static uint32 s_buttons_trid_singlemap_in_game[5] =
{
    TRID_MENU_EXITTOMAINMENU,
	TRID_MENU_SAVEGAME,
    TRID_MENU_LOADGAME,
    TRID_MENU_GAMESETTINGS,
    TRID_MENU_TUTORIAL
};


// tab entry can be NULL, if result have not ResTab
#define DEF_TAB_ENTRY(id, classname)    \
pBtn = new (classname)(m_pMgr, this, iRect(sx,sy,w,h), (id)); \
AddChild(pBtn); \
m_tabs.Add(pBtn);   \
m_results.Add(ResTab);

#define DEF_TAB_ENTRY_PARAM(id, classname, param)    \
pBtn = new (classname)(m_pMgr, this, iRect(sx,sy,w,h), (id), (param)); \
AddChild(pBtn); \
m_tabs.Add(pBtn);   \
m_results.Add(ResTab);

#define DEF_RES_ENTRY(res)    \
m_tabs.Add(NULL);   \
m_results.Add((res));



void iMainMenuView::FillControlsForType()
{
    iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);
    uint32 sx, sy;
    uint32* p_buttons_trid;
    uint n_buttons_count;
    iMenuTab* pBtn;

    if (m_type == TypeMain)
    {
        p_buttons_trid = s_buttons_trid_main;
        n_buttons_count = ARRAYSIZE(s_buttons_trid_main);
    }
    else if (m_type == TypeSingle)
    {
        p_buttons_trid = s_buttons_trid_single;
        n_buttons_count = ARRAYSIZE(s_buttons_trid_single);
    }
    else if (m_type == TypeInGame)
    {
        p_buttons_trid = s_buttons_trid_ingame;
        n_buttons_count = ARRAYSIZE(s_buttons_trid_ingame);
    }
    else if (m_type == TypeMulti)
    {
        p_buttons_trid = s_buttons_trid_multi;
        n_buttons_count = ARRAYSIZE(s_buttons_trid_multi);
    }
	else if (m_type == TypeSingleMap)
    {
        p_buttons_trid = s_buttons_trid_singlemap;
        n_buttons_count = ARRAYSIZE(s_buttons_trid_singlemap);
    }
	else if (m_type == TypeSingleMapInGame)
    {
        p_buttons_trid = s_buttons_trid_singlemap_in_game;
        n_buttons_count = ARRAYSIZE(s_buttons_trid_singlemap_in_game);
    }

	iSize sz = gGfxMgr.Dimension(PDGG(NMENU_BBTN_NBACK));
	iRect rc = AlignRect(sz, m_Rect, AlignCenter);
    rc.x = 21;
    rc.x += m_Rect.w / 2;
    AddChild(m_pBtnBack = new iImgBtn(m_pMgr, this, rc, 400, PDGG(NMENU_BBTN_NBACK), PDGG(NMENU_BBTN_PBACK)));


	//const sint32 w = 930, h = 400;
	sint32 w = 400, h = 300;

    sx = 21;
    sy = (m_Rect.h - h) / 2;

#if defined(ROYAL_BOUNTY)
	sy += 60;
#endif
    for (uint32 xx = 0; xx < n_buttons_count; ++xx)
    {
        iMenuBtn* pBtn = new iMenuBtn(m_pMgr, this, iRect(sx,sy,242,55), p_buttons_trid[xx], 200 + xx);
        AddChild(pBtn);
        m_buttons.Add(pBtn);
        sy += 60;
    }

	sx += m_Rect.w / 2 + 55;
	sy = (m_Rect.h - h) / 2;

    if (m_type == TypeMain)
    {
        // DEF_TAB_ENTRY(302, iMenuTab_DownloadMaps);
        DEF_RES_ENTRY(ResSingle);
        DEF_TAB_ENTRY_PARAM(301, iMenuTab_SingleNew, 1); // iMenuTab_SingleNew
        DEF_RES_ENTRY(ResTutorial); // iMenuTab_SingleTutorial
        DEF_TAB_ENTRY(303, iMenuTab_Credits); // iMenuTab_Credits
        DEF_RES_ENTRY(ResExit); // iMenuTab_Credits
    }
    else if (m_type == TypeSingle)
    {
        DEF_TAB_ENTRY_PARAM(301, iMenuTab_SingleNew, 0); // iMenuTab_SingleNew
        DEF_TAB_ENTRY(302, iMenuTab_SingleLoad); // iMenuTab_SingleLoad
        DEF_RES_ENTRY(ResTutorial); // iMenuTab_SingleTutorial
        DEF_TAB_ENTRY(302, iMenuTab_Settings); // iMenuTab_GameSettings
        DEF_RES_ENTRY(ResMain);
    }
    else if (m_type == TypeInGame)
    {
		DEF_RES_ENTRY(ResExit);
		DEF_TAB_ENTRY(300, iMenuTab_SingleSave); // iMenuTab_SingleSave
        DEF_TAB_ENTRY(301, iMenuTab_SingleLoad); // iMenuTab_SingleLoad
		DEF_TAB_ENTRY(302, iMenuTab_Settings); // iMenuTab_GameSettings
        DEF_RES_ENTRY(ResContinue);
    }
    else if (m_type == TypeMulti)
    {
        //        DEF_TAB_ENTRY(302, iMenuTab_DownloadMaps);
        /*DEF_TAB_ENTRY(306, iMenuTab); // iMenuTab_HotSeat
         DEF_TAB_ENTRY(307, iMenuTab); // iMenuTab_MultiOnline
         DEF_TAB_ENTRY(308, iMenuTab); // iMenuTab_MultiShop
		 */
        DEF_RES_ENTRY(ResMain);
    }
    else if (m_type == TypeSingleMap)
    {
        DEF_RES_ENTRY(ResSingleMap);
		DEF_TAB_ENTRY(300, iMenuTab_SingleLoad); // iMenuTab_SingleLoad
		DEF_TAB_ENTRY(301, iMenuTab_Settings); // iMenuTab_GameSettings
        DEF_RES_ENTRY(ResTutorial); // iMenuTab_SingleTutorial
		DEF_TAB_ENTRY(302, iMenuTab_Credits); // iMenuTab_Credits
    }
	else if (m_type == TypeSingleMapInGame)
    {
        DEF_RES_ENTRY(ResExit);
		DEF_TAB_ENTRY(300, iMenuTab_SingleSave); // iMenuTab_SingleSave
		DEF_TAB_ENTRY(301, iMenuTab_SingleLoad); // iMenuTab_SingleLoad
		DEF_TAB_ENTRY(302, iMenuTab_Settings); // iMenuTab_GameSettings
        DEF_RES_ENTRY(ResTutorial); // iMenuTab_SingleTutorial
    }



    //check(m_tabs.GetSize() == n_buttons_count && m_results.GetSize() == n_buttons_count);
    //pBtn = new iMenuTab(m_pMgr, this, iRect(sx,sy, 500,550), 300 + xx);
}

#undef DEF_TAB_ENTRY
#undef DEF_RES_ENTRY



iMainMenuView::iMainMenuView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, Type tp, bool fInGame, uint32 uid)
: IMainMenuView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler), m_activebtn(-1), m_activetab(-1), m_fInGame(fInGame)
{
    m_type = tp;

    FillControlsForType();
    UpdateControls();
}


bool iMainMenuView::Process(fix32 t)
{
	//return true;
    for (uint32 xx = 0; xx < m_tabs.GetSize(); ++xx)
    {
        if (m_tabs[xx]) m_tabs[xx]->Process(t);
    }

	if((m_type == TypeMain || m_type == TypeSingle)
	   /*!m_bDoneUpdatingBadges*/) {
//		iMapShop::iMapBuyList lst;

        // m_buttons[0]->SetCount(gMapShop.GetUpdateCount());


	}
    return true;
}


void iMainMenuView::OnActivate(bool bActivate)
{
	if(!bActivate) return;

    for (uint32 xx = 0; xx < m_tabs.GetSize(); ++xx)
    {
        if (m_tabs[xx]) m_tabs[xx]->OnActivate();
    }
	m_bDoneUpdatingBadges = false;
	m_activebtn = -1;
	m_activetab = -1;
	UpdateControls();
}


void iMainMenuView::UpdateControls()
{
    for (uint32 xx = 0; xx < m_buttons.GetSize(); ++xx)
    {
        iMenuBtn* pBtn = m_buttons[xx];
        if (xx == m_activebtn)
        {
            pBtn->SetActive(true);
        }
        else
        {
            pBtn->SetActive(false);
        }
    }

	bool bScroll = false;

	for(uint32 xx=0; xx<m_tabs.GetSize(); xx++) {
		iMenuTab* pTab = m_tabs[xx];
        if (xx == m_activetab)
        {
            if (pTab)
            {
				bScroll = true;
                pTab->SetVisible(true);
                pTab->SetActive(true);
                pTab->OnActivate();
            }
        }
        else
        {
            if (pTab)
            {
                pTab->SetVisible(false);
                pTab->SetActive(false);
                pTab->OnDeactivate();
            }
        }
	}

	if(bScroll) {
		iRect rc = m_Rect;
		rc.x = -(sint32)rc.w / 2 + (m_Rect.w / 2 - 480) / 2;
		m_activebtn = -1;
		SetRect(rc);
		//gAniHost.StopAllAnimation(false);
		//gAniHost.AddAniObj(new iGAniObj_Move(fix32(10.0f), m_Rect, rc));
	}
}

bool iMainMenuView::KeyUp(sint32 key)
{
    if(key == KEY_ENTER)
    {
        //ReportRes(DRC_OK);
    }
    return false;
}


void iMainMenuView::ReportResult(uint32 res)
{
    // End dialog
    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, (uint32)res);
    //    EndDialog(res);
}

void iMainMenuView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid >= 200 && uid < 200 + m_buttons.GetSize())
    {
        sint32 newactive = uid - 200;
        if (newactive == m_activebtn) {
            m_activebtn  = -1; // hide workspace
			m_activetab = -1;
		}
        else {
            m_activebtn = newactive; // show workspace
			m_activetab = newactive;
            if(m_tabs[newactive])
                m_tabs[newactive]->OnShow();
		}

        UpdateControls();

        if (m_results[newactive] != ResTab)
            ReportResult(m_results[newactive]);

    }
    else if (uid >= 300 && uid < 300 + m_tabs.GetSize())
    {
        sint32 tab = uid - 300;
        ReportResult(param);
    } else if(uid == 400) { // scroll back btn

		if( m_activetab == -1 || m_tabs[m_activetab]->BackAction())
			return;
		else {
            if(m_activetab != -1)
                m_tabs[m_activetab]->OnDeactivate();


			iRect rc = m_Rect;
			rc.x = 0 +  (m_Rect.w / 2 - 480) / 2;
			m_activebtn = -1;
			UpdateControls();
            //	gAniHost.StopAllAnimation(false);
            //	gAniHost.AddAniObj(new iGAniObj_Move(fix32(1.0f), m_Rect, rc));
			SetRect(rc);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// iMainMenuDlg (dialog used as base for new menu)
// this dialog created from iMainMenuView::NewStart()
// and all work (start new game, change game settings) done inside workspaces
// so we just return continue or not
//////////////////////////////////////////////////////////////////////////

iNewMenuDlg::iNewMenuDlg(iViewMgr* pViewMgr)
: INewMenuDlg(pViewMgr), m_type(iMainMenuView::TypeMain), m_ctrl(NULL)
{}

iSize iNewMenuDlg::GetDlgMetrics() const
{
	return gApp.ViewMgr().Metrics();
}

void iNewMenuDlg::OnCreateDlg()
{
    //iRect clRect = ClientRect();
    iRect src = GetScrRect();
    src.w *= 2;
    src.x = (GetScrRect().w - 480) / 2;
//    src.y = (GetScrRect().h - 320) / 2;
    //clRect.h -= 20;
    //clRect.DeflateSize(50);
    AddChild(m_ctrl = new iMainMenuView(m_pMgr, this, src, m_type, m_in_game, 109) );
}

iSize iNewMenuDlg::ClientSize() const
{
    return iSize(gApp.ViewMgr().Metrics());
}

void iNewMenuDlg::DoCompose(const iRect& rect)
{
    iRect rc(rect);
}

bool iNewMenuDlg::KeyUp(sint32 key)
{
    return false;
}

bool iNewMenuDlg::Process(fix32 t)
{
    return m_ctrl->Process(t);
}

void iNewMenuDlg::OnCompose()
{
    iRect rc = GetScrRect();
    // CotullaTODO: frame debug
    //gApp.Surface().FrameRect(rc, cColor_Black);
}

void iNewMenuDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == 109)
    {
        // End dialog
		//SetVisible(false);
        EndDialog(param);
    }
}

}
// END OF FILE
