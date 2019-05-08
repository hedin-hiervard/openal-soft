//////////////////////////////////////////////////////////////////////////
// Ctl_ScenProps.cpp
//////////////////////////////////////////////////////////////////////////
// Cotulla: moved code to separate control
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Ctl_ScenProps.h"
#include "screens_ipad/MainMenuView.h"
#include "screens_iphone/MainMenuView.h"
//#include "xxc/xxc.security.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class iDifLvlTab : public iTabbedSwitch
{
public:
    iDifLvlTab(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 state)
        : iTabbedSwitch(pViewMgr, pCmdHandler, rect, DFC_COUNT, uid, state) {}

    void ComposeTabItem(uint32 idx, uint32 itemState, const iRect& rect)
    {
        iRect orc = rect;
        SpriteId icn = PDGG(NMENU_DIFFS) + idx;
        if (itemState & iButton::Selected) icn += 5;
        BlitIcon(gApp.Surface(), icn, orc);
		
		iPoint pnt(orc.point());
		pnt.y += orc.size().h;
		iTextComposer::FontConfig fc(FS_PT_10, iDibFont::ComposeProps(RGB16(0,0,0), cColor_Black, iDibFont::DecNone));
		
		gTextComposer.TextOut(fc, gApp.Surface(), pnt, gTextMgr[TRID_DIFFICULTY_LEVEL+idx+1], iRect(pnt.x, pnt.y, orc.size().w, 10), AlignCenter);
    }

};

//////////////////////////////////////////////////////////////////////////
iScenPropsView::iScenPropsView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler)
{
    iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);
    sint32 yp = clRect.y;

    m_scProps.m_Difficulty = DFC_NORMAL;

    if(!gUseIpadUI)
        yp += 85;
    else
        yp += 135;

	
    // 44x44 each picture
    AddChild(m_difLevel = new iDifLvlTab(m_pMgr, this, iRect((clRect.w - 330) / 2, yp, 330, 38), 301, (Visible|Enabled)));
    if (m_scProps.m_Difficulty == DFC_UNDEFINED) m_scProps.m_Difficulty = DFC_NORMAL;
    m_difLevel->SetCurrentTab(m_scProps.m_Difficulty);
    if(!gUseIpadUI)
        yp += m_difLevel->GetSize().h + 10;
    else
        yp += 45;

    uint32 cnt = 7;
    sint32 btnsw = cnt * 60 + (cnt - 1) * 3;
    sint32 sy = yp;
    sint32 sx = clRect.x + (clRect.w/2 - btnsw/2);
    bool bIsHuman = false;
    for (uint32 xx=0; xx<cnt; ++xx) 
    {
		iNationBtn* pNatBtn = new iNationBtn(m_pMgr, iRect( iPoint(sx, sy), iIphoneBtn::sm_defSize), PID_RED, CTLT_TOWER, 150+xx, false);
        AddChild(pNatBtn);
        m_btnNations.Add(pNatBtn);
		iPlayerBtn* pPlBtn = new iPlayerBtn(m_pMgr, this, iRect( iPoint(sx, sy + iIphoneBtn::sm_defSize.h), iIphoneBtn::sm_defSize), PID_RED, PT_HUMAN, 200+xx, (Visible|Enabled));
        AddChild(pPlBtn);
        m_btnPlayers.Add(pPlBtn);
		sx += iIphoneBtn::sm_defSize.w * 1.5;
    }

    UpdateControls();

}


void iScenPropsView::SetData(iMapInfo& scProps)
{   
    uint32 xx;
    iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);
    
    m_scProps = scProps;    

    if (m_scProps.m_Difficulty == DFC_UNDEFINED) m_scProps.m_Difficulty = DFC_NORMAL;
    m_difLevel->SetCurrentTab(m_scProps.m_Difficulty);

    sint32 sy = 0;
#ifdef PC_VERSION
    sint32 yp = clRect.y;
    yp += 200;
    sy = yp;
#endif
    uint32 cnt = m_scProps.TotalPlayers();	
    sint32 btnsw = cnt * 50 + (cnt - 1) * 10;
    PLAYER_TYPE tp;
    sint32 sx = clRect.x + (clRect.w / 2 - btnsw / 2);
    bool bIsHuman = false;
    for (xx = 0; xx < cnt; ++xx)
    {
        m_btnNations[xx]->SetData(m_scProps.m_Players[xx].m_Id, m_scProps.m_Players[xx].m_Nation, m_scProps.m_Players[xx].m_Nation != CTLT_RANDOM);
#ifndef PC_VERSION
        sy = m_btnNations[xx]->GetPos().y;
#endif
        m_btnNations[xx]->SetPos(iPoint(sx, sy));
		m_btnNations[xx]->SetVisible(true);
        switch (m_scProps.m_Players[xx].m_TypeMask) 
        {
        case PTM_HUMAN_ONLY:
            tp = PT_HUMAN;
            break;
        case PTM_COMPUTER_ONLY:
            tp = PT_COMPUTER;
            break;
        default:	
            tp = bIsHuman ? PT_COMPUTER : PT_HUMAN;				
            break;
        }
        if (tp == PT_HUMAN) bIsHuman = true;
        iPlayerBtn* pPlBtn = m_btnPlayers[xx];
        pPlBtn->SetPos(iPoint(sx,sy + iIphoneBtn::sm_defSize.h));
        pPlBtn->SetData(m_scProps.m_Players[xx].m_Id, tp);
		pPlBtn->SetVisible(true);
        pPlBtn->SetEnabled((m_scProps.m_Players[xx].m_TypeMask == PTM_HUMAN_OR_COMPUTER));
        sx += 60;
    }

    for ( ; xx < 7; ++xx)
    {
        m_btnPlayers[xx]->SetVisible(false);
        m_btnPlayers[xx]->SetEnabled(false);
        m_btnNations[xx]->SetVisible(false);        
        m_btnNations[xx]->SetEnabled(false);
    }
    Invalidate();
    UpdateControls();
}


iStringT iScenPropsView::GetDfcString(DIFFICULTY_LEVEL dl)
{
    return iFormat(_T("#FCCC%s: #FFF0%s"), gTextMgr[TRID_DIFFICULTY_LEVEL], gTextMgr[TRID_DIFF_EASY+dl]);
}

void iScenPropsView::UpdateControls()
{
    bool bHasHuman = false;
    for (uint32 xx=0; xx<m_scProps.m_Players.GetSize(); ++xx) 
    {
        if (m_btnPlayers[xx]->PlayerType() == PT_HUMAN) 
        {
            bHasHuman = true;
            break;
        }
    }
}

void ComposeMapHeaderBkgnd(iDib& dib, const iRect& irc, bool bEven, bool bSel)
{
    if (!bSel)
    {
        if (bEven)
            dib.FillRect(irc, RGB16(196, 184, 163));//RGB16(132, 131, 130)); //RGB16(160, 152, 136));
        else
            dib.FillRect(irc, RGB16(165, 155, 133));// RGB16(96, 96, 96)); //RGB16(176, 168, 152));
    }
    else
    {
        dib.FillRect(irc, RGB16(242, 230, 198));
    }
}


void ComposeMapHeader(iDib& dib, const iRect& irc, const iStringT& size, const iStringT& humanplayers, const iStringT& totalplayers, const iStringT& name, const iStringT& author, const iStringT& cost, bool bIsUpdating, bool bIsHotNew, bool bEven, bool bSel, bool bBackground)
{
    iRect rc = irc;
    iTextComposer::FontConfig fc_l(newmenufc_low_text);
    iTextComposer::FontConfig fc_h(newmenufc_grt_text);
    iTextComposer::FontConfig fc_m(newmenufc_med_text);
    iTextComposer::FontConfig fc_b(newmenufc_big_text);
    
    fc_l.cmpProps.faceColor = RGB16(0, 0, 0);
    fc_h.cmpProps.faceColor = RGB16(0, 0, 0);
    fc_m.cmpProps.faceColor = RGB16(0, 0, 0);
    if(bBackground)
        fc_b.cmpProps.faceColor = RGB16(248, 248, 224);
    else
        fc_b.cmpProps.faceColor = RGB16(0, 0, 0);
    
    
    if(bBackground)
        ComposeMapHeaderBkgnd(dib, irc, bEven, bSel);
    
    if(bBackground && bSel)
        fc_b.cmpProps.faceColor = RGB16(0, 0, 0);
    
    // 1. Size
    rc.DeflateRect(5,0,0,0);
    gTextComposer.TextOut(fc_h, dib,rc, size,
                          iRect(rc.x + 5 + (size.Length() > 2 ? - 15 : 0),rc.y, 45,rc.h), AlignCenter);
    // 2. Players count
    if(!humanplayers.Empty() && !totalplayers.Empty())
        gTextComposer.TextOut(fc_l, dib,rc,iFormat(_T("%s/%s"), humanplayers.CStr(), totalplayers.CStr()),iRect(rc.x + 5,rc.y,45,rc.h), AlignBottom);
    
    rc.DeflateRect(50,0,0,0);
    
    // 3. Map name
    uint32 namelen = gTextComposer.GetTextSize(name, fc_b).w;
    gTextComposer.TextOut(fc_b, dib,rc, name,iRect(rc.x,rc.y,300,rc.h), AlignLeft);
    rc.DeflateRect(0,0,20,0);
    
    // 4. Author/Cost
    iStringT c = cost;
    
    if(bIsUpdating)
        c = iStringT(gTextMgr[TRID_SHOP_UPDATE]);
    
    if(bIsHotNew) {
        if(gUseIpadUI)
            UI_IPAD::ComposeBadge(dib, iPoint(rc.x + namelen, rc.y), _T("NEW"));
        else
            UI_IPHONE::ComposeBadge(dib, iPoint(rc.x + namelen, rc.y), _T("NEW"));
        //		c = iStringT(gTextMgr[TRID_SHOP_HOT_NEW]) + _T("   ") + cost;
    }
    
    gTextComposer.TextOut(fc_m, dib,rc, c, rc, AlignRight);
    gTextComposer.TextOut(fc_l, dib,rc, author,iRect(rc.x,rc.y,45,rc.h), AlignBottomLeft);
}

void iScenPropsView::OnCompose()
{
	sint32 w = gTextComposer.GetTextSize(m_scProps.MapName(), newmenufc_big_text).w;
	w += 50;
    iRect rc = AlignRect(iSize(w, 75), GetScrRect(), AlignTop);
    iStringT h = iFormat(_T("%d"), m_scProps.HumanPlayers());
    iStringT t = iFormat(_T("%d"),  m_scProps.TotalPlayers());

    
	ComposeMapHeader(gApp.Surface(), rc, gTextMgr[TRID_SHORT_MAPSIZ_SMALL + m_scProps.m_Size], h, t, m_scProps.MapName(), m_scProps.m_Author, _T(""), false, false, false, false, false);
   /* iTextComposer::FontConfig fc_l(newmenufc_low_text);
    iTextComposer::FontConfig fc_h(newmenufc_grt_text);
    iTextComposer::FontConfig fc_m(newmenufc_med_text);
    iTextComposer::FontConfig fc_b(newmenufc_big_text);
    iTextComposer::FontConfig fc_ll(newmenufc_lrg_text);    

    fc_l.cmpProps.faceColor = RGB16(0, 0, 0);
    fc_h.cmpProps.faceColor = RGB16(0, 0, 0);
    fc_m.cmpProps.faceColor = RGB16(0, 0, 0);
    fc_ll.cmpProps.faceColor = RGB16(0, 0, 0);
    fc_b.cmpProps.faceColor = RGB16(2, 248, 224);

    
    uint32 dx = gTextComposer.GetTextSize(m_scProps.MapName(), fc_ll).w + 45;
    uint32 x = (rc.w - dx) / 2;



    iRect rt1(rc.x + x, rc.y + 22, 100, 60); // map size, players count
    iRect rt2(rc.x + x+ 50 + 5, rc.y + 20, 100, 40); // map name
    iRect rt3(rc.x + x + 50 + 5, rc.y + 20 + 40, 100, 30); // map author

  /*  gApp.Surface().FrameRect(rt1, cColor_Black);
    gApp.Surface().FrameRect(rt2, cColor_Black);
    gApp.Surface().FrameRect(rt3, cColor_Black);

    // 1. Size
    gTextComposer.TextOut(fc_h, gApp.Surface(), rt1, gTextMgr[TRID_SHORT_MAPSIZ_SMALL + m_scProps.m_Size],
        iRect(rt1.x + 5,rt1.y, 45, rt1.h), AlignCenter);
    // 2. Players count
    gTextComposer.TextOut(fc_l, gApp.Surface(), rt1, iFormat(_T("%d/%d"), m_scProps.HumanPlayers(),
        m_scProps.TotalPlayers()),iRect(rt1.x + 5,rt1.y,45,rt1.h), AlignBottom);

    // 3. Map name
    gTextComposer.TextOut(fc_ll, gApp.Surface(), rt2, m_scProps.MapName(), rt2, AlignTopLeft);

    // 4. Author
    iStringT str = m_scProps.m_Author;
    uint32 cur_ii = 3;  // CotullaTODO: get real rating of this map

    str += _T(" ");
    for (uint32 ii = 0; ii < 5; ii++)
    {
        str.Addf(_T("#I%04X"), cur_ii > ii ? PDGG(NMENU_STAR_N) : PDGG(NMENU_STAR_P));       
    }
    gTextComposer.TextOut(fc_m, gApp.Surface(), rt3, str, rt3, AlignTopLeft);
   */
}


void iScenPropsView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid >= 200 && uid < (200+m_scProps.m_Players.GetSize()) && cmd == CCI_BTNCLICK) 
    {
        uint32 val = uid - 200;
        m_btnPlayers[val]->TogglePlayerType();
        UpdateControls();
    }
}

const iMapInfo& iScenPropsView::ScenProps()
{
    // update data at first from UI

    // setup difficulty
    m_scProps.m_Difficulty = (DIFFICULTY_LEVEL)m_difLevel->GetCurrentTab();

    // setup players
    for (uint32 xx = 0; xx < m_scProps.m_Players.GetSize(); ++xx) 
    {
        m_scProps.m_Players[xx].m_Type = m_btnPlayers[xx]->PlayerType();
        m_scProps.m_Players[xx].m_Nation = m_btnNations[xx]->PlayerNation();
    }

    return m_scProps;
}

void iPlayerBtn::OnCompose()
{
	iRect rc = GetScrRect();
	/*
	gApp.Surface().FrameRect(rc, cColor_Black);
	rc.DeflateRect(1);
	gApp.Surface().FillRect(rc, PLAYER_COLORS[m_pid],96);
	ButtonFrame(gApp.Surface(), rc, m_state);		
	*/

	//SpriteId sid = (m_pt == PT_HUMAN)?PDGG(ICN_PLT_HUMAN):PDGG(ICN_PLT_AI);
	SpriteId sid2 = (m_pt == PT_HUMAN) ? PDGG(ICN_PLT_HUMAN) :PDGG(ICN_PLT_AI);
	SpriteId sid = PDGG(NMENU_COLORS) + m_pid;		        
	gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, rc, false, AlignCenter));
	gGfxMgr.Blit(sid2, gApp.Surface(), GetRealSpritePos(sid2, rc, false, AlignCenter));
	if(m_state & Pressed) {
		sint16 frsid = PDGG(BTN_FRAME_PRESSED);
		gGfxMgr.Blit(frsid, gApp.Surface(), GetRealSpritePos(frsid, rc, false, AlignCenter));
	}


	/*
	if (m_pt == PT_HUMAN) gTextComposer.TextOut(dlgfc_stopic, gApp.Surface(), iPoint(), _T("Human"), rc, AlignCenter);
	else if (m_pt == PT_COMPUTER) gTextComposer.TextOut(dlgfc_splain, gApp.Surface(), iPoint(), _T("CPU"), rc, AlignCenter);
	*/

	//if (!IsEnabled()) gApp.Surface().FillRect(rc, cColor_Gray64,128);
}

void iNationBtn::OnCompose()
{
	iRect rc = GetScrRect();
	SpriteId sid = PDGG(BTN_CASTLES) + m_nt;
	gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, rc, false, AlignCenter));

	if(m_state & Pressed && !(m_bFixed)) {
		sint16 frsid = PDGG(BTN_FRAME_PRESSED);
		gGfxMgr.Blit(frsid, gApp.Surface(), GetRealSpritePos(frsid, rc, false, AlignCenter));
	}

}

// END OF FILE
