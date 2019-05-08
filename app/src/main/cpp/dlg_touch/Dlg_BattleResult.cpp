#include "stdafx.h"
#include "Dlg_BattleResult.h"

#include "lang.h"

iDlg_BattleResult::iDlg_BattleResult(iViewMgr* pViewMgr, iBattleEngine& engine, iBattleResult br)
: iBaseGameDlg(pViewMgr), m_engine(engine), m_br(br)
{
	iBattleMember::Side is = engine.GetBattleInfo().InteractorSide();
	// title and text
	if ((br.m_result == BR_ASSAULTER_WIN && is == iBattleMember::Assaulter) || (br.m_result == BR_DEFENDER_WIN && is == iBattleMember::Defender)) {
		switch (br.m_defCause) {
		case DC_DEFEAT: m_Title = gTextMgr[TRID_MSG_BAT_VICTORY]; break;
		case DC_RETREAT: m_Title = gTextMgr[TRID_MSG_BAT_ENEMY_FLEES]; break;
		case DC_SURRENDER: m_Title = gTextMgr[TRID_MSG_BAT_ENEMY_SURREND]; break;
		default: check(0);
		}
#ifdef PC_VERSION
		switch (br.m_defCause) {
			case DC_DEFEAT: m_TwitterTitle = gTextMgr.English(TRID_MSG_BAT_VICTORY); break;
			case DC_RETREAT: m_TwitterTitle = gTextMgr.English(TRID_MSG_BAT_ENEMY_FLEES); break;
			case DC_SURRENDER: m_TwitterTitle = gTextMgr.English(TRID_MSG_BAT_ENEMY_SURREND); break;
			default: check(0);
		}
#endif
		iBattleMember::Side ws = is;
		//const iBattleArmy& wa = (ws == iBattleMember::Assaulter)?engine.AArmy():engine.DArmy();

		if (const iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(engine.GetBattleInfo().Member(ws))){
			const iHero* pHero = pHeroMember->GetHero();
			m_Text.Setf(gTextMgr[TRID_MSG_BAT_RECEXP],pHero->Name().CStr(),br.m_experience);
#ifdef PC_VERSION			          
			m_TwitterText.Setf(gTextMgr.English(TRID_MSG_BAT_RECEXP),pHero->EnglishName().CStr(),br.m_experience);
#endif
		}

	} else {
		iBattleMember::Side ds = is;
		//const iBattleArmy& da = (ds == iBattleMember::Assaulter)?engine.AArmy():engine.DArmy();

		if (const iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(engine.GetBattleInfo().Member(ds))){
			const iHero* pHero = pHeroMember->GetHero();
			if (br.m_defCause == DC_DEFEAT) m_Text.Setf(gTextMgr[TRID_MSG_BAT_HERO_DEFEAT],pHero->Name().CStr());
			else if (br.m_defCause == DC_RETREAT) m_Text.Setf(gTextMgr[TRID_MSG_BAT_FLEES],pHero->Name().CStr());
			else if (br.m_defCause == DC_SURRENDER) m_Text.Setf(gTextMgr[TRID_MSG_BAT_SURREND],pHero->Name().CStr());
		} else {
			check(br.m_defCause == DC_DEFEAT);
			m_Text = gTextMgr[TRID_MSG_BAT_FORCES_DEFEAT];
		}
#ifdef PC_VERSION		
		if (const iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(engine.GetBattleInfo().Member(ds))){
			const iHero* pHero = pHeroMember->GetHero();            
			if (br.m_defCause == DC_DEFEAT)	m_TwitterText.Setf(gTextMgr.English(TRID_MSG_BAT_HERO_DEFEAT),pHero->EnglishName().CStr());
			else if (br.m_defCause == DC_RETREAT) m_TwitterText.Setf(gTextMgr.English(TRID_MSG_BAT_FLEES),pHero->EnglishName().CStr());
			else if (br.m_defCause == DC_SURRENDER) m_TwitterText.Setf(gTextMgr.English(TRID_MSG_BAT_SURREND),pHero->EnglishName().CStr());
		} else {
			check(br.m_defCause == DC_DEFEAT);
			m_TwitterText = gTextMgr.English(TRID_MSG_BAT_FORCES_DEFEAT);
		}
#endif
	}

	//const iArmy& aArmy = engine.GetBattleInfo().m_pAssaulter->Army();
	for (uint32 aidx=0; aidx<engine.AArmy().Count(); ++aidx) {
		if (engine.AArmy()[aidx]->Casualties()) {
			m_aCas.Add(engine.AArmy()[aidx]->Type(), engine.AArmy()[aidx]->Casualties());
		}
	}

	//const iArmy& dArmy = engine.GetBattleInfo().m_pDefender->Army();
	for (uint32 didx=0; didx<engine.DArmy().Count(); ++didx) {
		if (engine.DArmy()[didx]->Casualties()) {
			m_dCas.Add(engine.DArmy()[didx]->Type(), engine.DArmy()[didx]->Casualties());
		}
	}
}

void iDlg_BattleResult::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	iRect clRect = ClientRect();
	//AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	AddChild(m_pTweetBtn = new iImgBtn(m_pMgr, this, AlignRect(sz, clRect, AlignBottom) , 100, PDGG(TWEET), PDGG(TWEET_PRESSED)));
#endif
}
#ifdef PC_VERSION
void iDlg_BattleResult::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
    m_pTweetBtn->SetRect( AlignRect(sz, clRect, AlignBottom) );
#endif
}
#endif
void iDlg_BattleResult::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

	// title
	if (!m_Title.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), m_Title, rc);		
	}

	// text
	if (!m_Text.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_splain,gApp.Surface(),m_Text, rc);
	}

	// Battlefield casualties
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), gTextMgr[TRID_BAT_CASUALTIES], rc);
	
	// Attacker
	rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), gTextMgr[TRID_BAT_ATTACKER], rc);
	
	if (m_aCas.Count()) {		
		sint32 xpos = rc.x + (sint32)(rc.w/2) - (sint32)((m_aCas.Count()* CreatureCellSize().w)/2);
		for (uint32 xx=0; xx<m_aCas.Count(); ++xx) {
			ComposeCreatureCell(gApp.Surface(), iRect(iPoint(xpos,rc.y), CreatureCellSize()), m_aCas[xx].Type(), m_aCas[xx].Count(), VL_EXPERT);
			xpos += CreatureCellSize().w;
		}
		rc.y += CreatureCellSize().h;
	} else {
		rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), gTextMgr[TRID_NONE], rc);
	}
	rc.y += CreatureCellSize().h;

	// Defender
	rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), gTextMgr[TRID_BAT_DEFENDER], rc);	
	if (m_dCas.Count()) {
		sint32 xpos = rc.x + (sint32)(rc.w/2) - (sint32)((m_dCas.Count()*CreatureCellSize().w)/2);
		for (uint32 xx=0; xx<m_dCas.Count(); ++xx) {
			ComposeCreatureCell(gApp.Surface(), iRect(iPoint(xpos,rc.y), CreatureCellSize()), m_dCas[xx].Type(), m_dCas[xx].Count(), VL_EXPERT);
			xpos += CreatureCellSize().w;
		}
		rc.y += CreatureCellSize().h;
	} else {
		rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), gTextMgr[TRID_NONE], rc);
	}	
}

iSize iDlg_BattleResult::ClientSize() const
{
	sint32 w = iCLAMP<sint32>(iMAX<sint32>(m_aCas.Count(),m_dCas.Count())*30, gApp.Surface().GetWidth(), iBaseGameDlg::GetDefaultWidth());
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	if (!m_Title.Empty()) {
		h += gTextComposer.GetTextBoxSize(m_Title, w, dlgfc_hdr).h;		
	}

	// text
	if (!m_Text.Empty()) {
		h += gTextComposer.GetTextBoxSize(m_Text, w, dlgfc_splain).h;
	}

	// Battlefield casualties
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_BAT_CASUALTIES], w, dlgfc_topic).h;

	// Attacker
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_BAT_ATTACKER], w, dlgfc_splain).h;
	if (m_aCas.Count())
		h += CreatureCellSize().h;
	else
		h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_NONE], w, dlgfc_splain).h;

	h += CreatureCellSize().h;
	
	// Defender
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_BAT_DEFENDER], w, dlgfc_splain).h;
	if (m_dCas.Count())
		h += CreatureCellSize().h;
	else
		h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_NONE], w, dlgfc_splain).h;
	
    if(gUseIpadUI)
        h += 10;
    
#if 0
	// tweet btn
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	h += sz.h;
#endif
	return iSize(w, h);
}

void iDlg_BattleResult::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 100) {
#if defined( OS_MACOS )
//		gTwitter.Tweet(m_TwitterTitle + _T(" ") + m_TwitterText);
		m_pTweetBtn->SetVisible(false);
#endif
	} else
		EndDialog(pView->GetUID());
}

void iDlg_BattleResult::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {
	EndDialog(0);
}
