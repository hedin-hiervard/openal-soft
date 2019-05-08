#include "stdafx.h"
#include "Dlg_BattleHero.h"

iDlg_BattleHero::iDlg_BattleHero(iViewMgr* pViewMgr, iHero& hero, bool bActiveMember, uint32 flags)
: iBaseGameDlg(pViewMgr), m_hero(hero), m_flags(flags), m_bActiveMember(bActiveMember)
{}

void iDlg_BattleHero::OnCreateDlg()
{
	iRect clRect = ClientRect();

	if(m_bActiveMember) {
		sint32 bw1 = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_RETREAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_SURRENDER], btnfc_normal).w) + 30;
		sint32 bw2 = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_AUTO_COMBAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_MENU_EXITGAME], btnfc_normal).w) + 30;
		sint32 bw = iMAX(bw1, bw2);
		iSize sz = iSize(bw, DEF_BTN_HEIGHT);
#ifndef ROYAL_BOUNTY
		if(!m_hero.InShip()) {
//			AddChild(m_pBtnSurrender = new iTextButton(m_pMgr,this,AlignChild(sz, AlignBottomLeft),TRID_SURRENDER, Surrender, (m_flags&CanSurrender)?(Visible|Enabled):(Visible) ));		
			AddChild(m_pBtnretreat = new iTextButton(m_pMgr,this,AlignChild(sz, AlignBottom) 
#ifndef PC_VERSION
                                                     + iPoint(sz.w -3, 0)
#else
                                                     + iPoint(sz.w+5, 0)
#endif
                                                     ,TRID_RETREAT, Retreat, (m_flags&CanRetreat)?(Visible|Enabled):(Visible)));	
		}
#endif
		AddChild(m_pBtnAutocombat = new iTextButton(m_pMgr,this,AlignChild(sz, AlignBottom) 
#ifndef PC_VERSION
                                                    - iPoint(sz.w - 3, 0)
#else
                                                    - iPoint(sz.w+5, 0)
#endif
                                                    ,TRID_AUTO_COMBAT, AutoBattle));
		AddChild(m_pBtnExitgame = new iTextButton(m_pMgr,this,AlignChild(sz, AlignBottom),TRID_MENU_EXITGAME, MainMenu));
	}
}
#ifdef PC_VERSION
void iDlg_BattleHero::UpdateSize(){

    Center();
    
    if(m_bActiveMember) {
		sint32 bw1 = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_RETREAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_SURRENDER], btnfc_normal).w) + 30;
		sint32 bw2 = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_AUTO_COMBAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_MENU_EXITGAME], btnfc_normal).w) + 30;
		sint32 bw = iMAX(bw1, bw2);
		iSize sz = iSize(bw, DEF_BTN_HEIGHT);
		if(!m_hero.InShip()) {
		//	m_pBtnSurrender->SetRect( AlignChild(sz, AlignBottomLeft) );	
			m_pBtnretreat->SetRect( AlignChild(sz, AlignBottom) );
		}
		m_pBtnAutocombat->SetRect( AlignChild(sz, AlignBottom) - iPoint(sz.w + 5, 0) );
        m_pBtnExitgame->SetRect( AlignChild(sz, AlignBottom) + iPoint(sz.w + 5, 0) );
	}
}
#endif
void iDlg_BattleHero::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

	// Portrait
	iRect prc = AlignRect(iSize(55, 55), rc, AlignTop);
	ComposeHeroAvatar(gApp.Surface(), prc, &m_hero);
	rc.y += prc.h + 10; 

	// Name
	iStringT txt;
	rc.y += ComposeHeroTitleName(gApp.Surface(), rc, &m_hero);

	iTextComposer::FontConfig fch(dlgfc_stopic.fontSize, RGB16(255,192,128));
	iTextComposer::FontConfig fcv(dlgfc_stopic.fontSize, RGB16(255,255,0));	
	
	sint32 height = gGfxMgr.Dimension(PDGG(DGLYPH_PRSKILLS) + 0).h + 5;
	iRect tmp, trc;

	for(uint32 xx=0; xx<7; xx++) {
		sint32 width;
		sint16 sid;
		iStringT txt;
		switch(xx) {
			case 0:
			case 1:
			case 2:
			case 3:
				width = gGfxMgr.Dimension(PDGG(DGLYPH_PRSKILLS) + xx).w;
				sid = PDGG(DGLYPH_PRSKILLS) + xx;
				txt = FormatNumber(m_hero.FurtSkill((FURTHER_SKILLS)(FSK_ATTACK + xx)));

				tmp = iRect(rc. x + rc.w * xx/ 7, rc.y, rc.w/ 7, height);
				trc = AlignRect(gGfxMgr.Dimension(sid), tmp, AlignTop);
				gGfxMgr.Blit(sid, gApp.Surface(), trc);
				gTextComposer.TextOut(fcv, gApp.Surface(),iPoint(), txt,
					iRect(trc.point() + gGfxMgr.Anchor(sid) + iPoint(0, height), iSize(width, 20)), AlignTop);			
				break;
			case 4:
				width = 52;			
				txt = FormatNumber(m_hero.FurtSkill((FURTHER_SKILLS)(FSK_MORALE)));
				tmp = iRect(rc. x + rc.w * xx/ 7, rc.y, rc.w/ 7, height);
				trc = AlignRect(iSize(width, height), tmp, AlignTop);
				ComposeMoraleGlyph(gApp.Surface(), trc, m_hero.FurtSkill(FSK_MORALE), false);
				gTextComposer.TextOut(fcv, gApp.Surface(),iPoint(), txt,
					iRect(trc.point() + iPoint(0, height), iSize(width, 20)), AlignTop);			

				break;
			case 5:
				width = 52;			
				txt = FormatNumber(m_hero.FurtSkill((FURTHER_SKILLS)(FSK_LUCK)));
				tmp = iRect(rc. x + rc.w * xx/ 7, rc.y, rc.w/ 7, height);
				trc = AlignRect(iSize(width, height), tmp, AlignTop);
				ComposeLuckGlyph(gApp.Surface(), trc, m_hero.FurtSkill(FSK_LUCK), false);
				gTextComposer.TextOut(fcv, gApp.Surface(),iPoint(), txt,
					iRect(trc.point() + iPoint(0, height), iSize(width, 20)), AlignTop);			

				break;
				
			case 6:
				width = gGfxMgr.Dimension(PDGG(DGLYPH_MANA)).w;			
				txt = iFormat(_T("%d/%d"),m_hero.ManaPts(),m_hero.MaxManaPts());
				tmp = iRect(rc. x + rc.w * xx/ 7, rc.y, rc.w/ 7, height);
				trc = AlignRect(iSize(width, height), tmp, AlignTop);
				gGfxMgr.Blit(PDGG(DGLYPH_MANA), gApp.Surface(), trc);
				gTextComposer.TextOut(fcv, gApp.Surface(),iPoint(), txt,
					iRect(trc.point() + iPoint(0, height), iSize(width, 20)), AlignTop);			

				break;


				break;
		}		
	}			
}

iSize iDlg_BattleHero::ClientSize() const
{
	sint32 bw1 = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_RETREAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_SURRENDER], btnfc_normal).w) + 30;
	sint32 bw2 = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_AUTO_COMBAT], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_MENU_EXITGAME], btnfc_normal).w) + 30;
	sint32 bw = iMAX(bw1, bw2);

	sint32 w = iCLAMP<sint32>(iBaseGameDlg::GetDefaultWidth(), gApp.Surface().GetWidth(), (bw + 5) * 4);
	sint32 h = iBaseGameDlg::GetTopOffset();

	// portrait
	h += 55 + 10;

	// Name
	iStringT txt;
	txt.Addf(_T("#S2#FEEE%s#S2#FAFF %s #S2#FEEE%s %d"),gTextMgr[TRID_HERO_TYPE_KNIGHT+m_hero.Type()],m_hero.Name().CStr(),gTextMgr[TRID_LEVEL],m_hero.Level());	
	h += gTextComposer.GetTextBoxSize(txt, w, dlgfc_topic).h;
	
	// params
	h += 100; //gTextComposer.GetFontHeight(dlgfc_stopic.fontSize) * 5;

	// buttons
	if(m_bActiveMember)
		h += DEF_BTN_HEIGHT;
	return iSize(w,h);
}

void iDlg_BattleHero::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	//uint32 uid = pView->GetUID();
	{
		EndDialog(pView->GetUID());
	}
}

void iDlg_BattleHero::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	EndDialog(0);
}

