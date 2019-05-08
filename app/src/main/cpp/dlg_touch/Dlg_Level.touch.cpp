#include "stdafx.h"
#include "Dlg_Level.h"
#include "Dlg_SkillInfo.h"

#include "lang.h"

//////////////////////////////////////////////////////////////////////////
#ifdef PC_VERSION
const sint32 SKILL_BTN_HEIGHT = 220;
#else
const sint32 SKILL_BTN_HEIGHT = 160;
#endif
class iLevelDlgBtn : public iButton
{
public:
	iLevelDlgBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, SECONDARY_SKILLS ss, SECSKILL_LEVEL sl)
	: iButton(pViewMgr, pCmdHandler, rect, uid, Visible | Enabled), m_ss(ss), m_sl(sl) {}

	static void ComposeLevelBtn(iDib& surf, iRect rc, uint32 state, SECONDARY_SKILLS ss, SECSKILL_LEVEL sl)
	{	
		/*gGfxMgr.BlitTile(PDGG(BKTILE),surf,rc);
		surf.Darken25Rect(rc);
		ButtonFrame(surf,rc,state);
		*/
		
		iRect trc = AlignRect(iSize(80, 80), rc, AlignTop);		
		sint16 sid = PDGG(BTN_FRAME_TREASURE_DELUXE);
		gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(PDGG(BTN_FRAME_TREASURE_DELUXE), trc, false, AlignCenter));
		sid = PDGG(SEC_SKILLS)+ss;
		gGfxMgr.Blit(sid, surf, GetRealSpritePos(sid, trc, false, AlignCenter));

		rc.y += 80;		
		rc.y += gTextComposer.TextBoxOut(btnfc_normal, surf, iFormat(_T("%s %s"), gTextMgr[TRID_HSKILL_LEVEL_BASIC+sl], gTextMgr[TRID_HSKILL_ESTATES+ss]), rc);
		
		rc.y += gTextComposer.TextBoxOut(btnfc_small, surf, iFormat(_T("%s"), gTextMgr[TRID_HSKILL_ESTATES_BAS_DESC + (ss*SSLVL_COUNT) + sl]), rc);

		if(state & Pressed)
		{
			gGfxMgr.Blit(PDGG(BTN_FRAME_TREASURE_DELUXE_PRESSED), gApp.Surface(), GetRealSpritePos(PDGG(BTN_FRAME_TREASURE_DELUXE), trc, false, AlignCenter));
		}
				
	}

	void OnCompose()
	{
		ComposeLevelBtn(gApp.Surface(), GetScrRect(), GetButtonState(), m_ss, m_sl);
	}

	SECONDARY_SKILLS	m_ss;
	SECSKILL_LEVEL		m_sl;
};
//////////////////////////////////////////////////////////////////////////
class iSkillInfoBtn : public iButton, public IViewCmdHandler
{
public:
	iSkillInfoBtn(iViewMgr* pViewMgr, const iRect& rect, SECONDARY_SKILLS ss, SECSKILL_LEVEL sl, PLAYER_ID pid)
	: iButton(pViewMgr, this, rect, 200, Visible | Enabled), m_ss(ss), m_sl(sl), m_pid(pid) {}

	void OnCompose()
	{
		iRect orc(GetScrRect());
		gApp.Surface().Darken25Rect(orc);
		ButtonFrame(gApp.Surface(),orc,GetButtonState());
		BlitIcon(gApp.Surface(), PDGG(BTN_INFO), orc);
		orc.InflateRect(1);
		FrameRoundRect(gApp.Surface(), orc, cColor_Black);
	}

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
	{
		{
			iSecSkillInfoDlg dlg(m_pMgr, m_ss, m_sl, m_pid);
			dlg.DoModal();
		}
	}


	PLAYER_ID			m_pid;
	SECONDARY_SKILLS	m_ss;
	SECSKILL_LEVEL		m_sl;
};


//////////////////////////////////////////////////////////////////////////
iDlg_NewLevel::iDlg_NewLevel(iViewMgr* pViewMgr, iHero* pHero, const iNewLevelInfo& linfo)
: iBaseGameDlg(pViewMgr), m_pHero(pHero), m_linfo(linfo), m_bNochoice(false), m_selSecSkill(linfo.secSkill[0])
{

}

void iDlg_NewLevel::OnCreateDlg()
{
	iRect clRect = ClientRect();

	sint32 cnt = 0;	
	if (m_linfo.secSkill[0] != SECSK_NONE) cnt++;
	if (m_linfo.secSkill[1] != SECSK_NONE) cnt++;

	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));

	
	if (cnt == 2) {
		// Message
		clRect.y += gTextComposer.GetTextBoxSize(m_pHero->Name() + _T(" ") +iFormat(gTextMgr[TRID_MSG_REACHLEVEL],m_pHero->Level()), clRect.w, dlgfc_topic).h;

		if(m_pHero->Level() == gGame.Map().LevelCap()) 
			clRect.y += gTextComposer.GetTextBoxSize(gTextMgr[TRID_MSG_MAXLEVEL], clRect.w, dlgfc_topic).h;

		// Portrait
#ifdef PC_VERSION
		clRect.y += 55;
#else
        clRect.y += 40;
#endif
		// Primary skills
		clRect.y += gTextComposer.GetTextBoxSize(iFormat(_T(" %s +1"),gTextMgr[TRID_SKILL_ATTACK+m_linfo.prSkillType]), clRect.w, dlgfc_splain).h;

		// Question
		if (m_linfo.secSkill[0] != SECSK_NONE && m_linfo.secSkill[1] != SECSK_NONE)
			clRect.y += gTextComposer.GetTextBoxSize(gTextMgr[TRID_SECSKILL_CHOOSE_MESSAGE], clRect.w, dlgfc_stopic).h;
		else if (m_linfo.secSkill[0] != SECSK_NONE || m_linfo.secSkill[1] != SECSK_NONE)
			clRect.y += gTextComposer.GetTextBoxSize(gTextMgr[TRID_SECSKILL_LEARN_MESSAGE], clRect.w, dlgfc_stopic).h;
#ifdef PC_VERSION		
		clRect.y += iBaseGameDlg::GetTopOffset();
#endif
		iRect tmprc = AlignRect(iSize(clRect.w, SKILL_BTN_HEIGHT), m_Rect, AlignBottom) - m_Rect
#if 0
        - iPoint(0, sz.h);
#else
        - iPoint(0, 30);
#endif
		AddChild(m_pDlgBtnLeft = new iLevelDlgBtn(m_pMgr, this, AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignLeft), 101, m_linfo.secSkill[0], (SECSKILL_LEVEL)(m_pHero->SecSkills().SkillLevel(m_linfo.secSkill[0])+1)));
		//AddChild(new iSkillInfoBtn(m_pMgr, AlignRect(iSize(45, DEF_BTN_HEIGHT), clRect, AlignTopRight), m_linfo.secSkill[0], (SECSKILL_LEVEL)(m_pHero->SecSkills().SkillLevel(m_linfo.secSkill[0])+1), m_pHero->Owner()->PlayerId()));
				
		clRect.y += DEF_BTN_HEIGHT * 2;
		
		AddChild(m_pDlgBtnRight = new iLevelDlgBtn(m_pMgr,this, AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignRight),102,m_linfo.secSkill[1], (SECSKILL_LEVEL)(m_pHero->SecSkills().SkillLevel(m_linfo.secSkill[1])+1)));
		//AddChild(new iSkillInfoBtn(m_pMgr, AlignRect(iSize(45, DEF_BTN_HEIGHT), clRect, AlignTopRight), m_linfo.secSkill[1], (SECSKILL_LEVEL)(m_pHero->SecSkills().SkillLevel(m_linfo.secSkill[1])+1), m_pHero->Owner()->PlayerId()));		
	} else {
		m_bNochoice = true;
		m_selSecSkill = m_linfo.secSkill[0];
		//AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
	}
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		m_selSecSkill = m_linfo.secSkill[0]; //doesn't matter what we choose, hero will be unserialized soon after
		EndDialog(DRC_OK);
		return;
	}		
#endif
	
#if 0
	clRect = ClientRect();
    AddChild(m_pTweetBtn = new iImgBtn(m_pMgr, this, AlignRect(sz, clRect, AlignBottom) , 100, PDGG(TWEET), PDGG(TWEET_PRESSED)));
#endif
}
#ifdef PC_VERSION
void iDlg_NewLevel::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();
    
    sint32 cnt = 0;	
	if (m_linfo.secSkill[0] != SECSK_NONE) cnt++;
	if (m_linfo.secSkill[1] != SECSK_NONE) cnt++;
    
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
    
	
	if (cnt == 2) {
        
        iRect tmprc = AlignRect(iSize(clRect.w, 220), m_Rect, AlignBottom) - m_Rect 
#if 0
        - iPoint(0, sz.h)
#endif
			;
    
        m_pDlgBtnLeft->SetRect( AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignLeft) );
        m_pDlgBtnRight->SetRect( AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, AlignRight) );
        
    }
#ifdef OS_MACOS
    m_pTweetBtn->SetRect( AlignRect(sz, clRect, AlignBottom) );
#endif
}
#endif
void iDlg_NewLevel::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
//#ifdef PC_VERSION
	rc.y += iBaseGameDlg::GetTopOffset();
//#endif
	// Portrait
	iRect prc = AlignRect(
#ifdef PC_VERSION
                          iSize(50, 50), 
#else
                          iSize(45,45),
#endif
                          rc, AlignTop);

	ComposeHeroAvatar(gApp.Surface(), prc, m_pHero);
#ifdef PC_VERSION
    rc.y += 65;
#else
    rc.y += 50;
#endif

	// Message
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), m_pHero->Name() + _T(" ") +iFormat(gTextMgr[TRID_MSG_REACHLEVEL],m_pHero->Level()), rc);

	// Max level reached
	if(m_pHero->Level() == gGame.Map().LevelCap())
		rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), gTextMgr[TRID_MSG_MAXLEVEL], rc);

	// Primary skills	
	rc.y += gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), iFormat(_T(" %s +1"),gTextMgr[TRID_SKILL_ATTACK+m_linfo.prSkillType]), rc);

	// Question
	if (m_linfo.secSkill[0] != SECSK_NONE && m_linfo.secSkill[1] != SECSK_NONE) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_stopic,gApp.Surface(),gTextMgr[TRID_SECSKILL_CHOOSE_MESSAGE], rc);
	} else if (m_linfo.secSkill[0] != SECSK_NONE) {		
		rc.y += gTextComposer.TextBoxOut(dlgfc_stopic,gApp.Surface(),gTextMgr[TRID_SECSKILL_LEARN_MESSAGE], rc) ;
		iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	
		iLevelDlgBtn::ComposeLevelBtn(gApp.Surface(), AlignRect(iSize(rc.w / 2, SKILL_BTN_HEIGHT), clRect, AlignBottom) /*- iPoint(0, sz.h)*/, iButton::Disabled, m_linfo.secSkill[0], (SECSKILL_LEVEL)(m_pHero->SecSkills().SkillLevel(m_linfo.secSkill[0])+1));
	}
}

iSize iDlg_NewLevel::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();	
#ifdef PC_VERSION
    sint32 h = iBaseGameDlg::GetTopOffset();
#else
    sint32 h = 0;
    
    if(m_linfo.secSkill[0] != SECSK_NONE || m_linfo.secSkill[1] != SECSK_NONE)
		return iSize(w, 320);
#endif
            
	// Message
	h += gTextComposer.GetTextBoxSize(m_pHero->Name() + _T(" ") +iFormat(gTextMgr[TRID_MSG_REACHLEVEL],m_pHero->Level()), w, dlgfc_topic).h;
	
	if(m_pHero->Level() == gGame.Map().LevelCap())
		h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_MSG_MAXLEVEL], w, dlgfc_topic).h;
	
	// Portrait
#ifdef PC_VERSION
	h += 55;
#else
    h += 45;
#endif

	// Primary skills
	h += gTextComposer.GetTextBoxSize(iFormat(_T(" %s +1"),gTextMgr[TRID_SKILL_ATTACK+m_linfo.prSkillType]), w, dlgfc_splain).h;

	// Question
	if (m_linfo.secSkill[0] != SECSK_NONE && m_linfo.secSkill[1] != SECSK_NONE)
		h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_SECSKILL_CHOOSE_MESSAGE], w, dlgfc_stopic).h;
	else if (m_linfo.secSkill[0] != SECSK_NONE || m_linfo.secSkill[1] != SECSK_NONE)
		h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_SECSKILL_LEARN_MESSAGE], w, dlgfc_stopic).h;
#ifdef PC_VERSION           
	h += iBaseGameDlg::GetTopOffset();
#endif
	// buttons
	/*if (m_linfo.secSkill[0] != SECSK_NONE && m_linfo.secSkill[1] != SECSK_NONE)
		h += 220 * 3;
		*/
	if (m_linfo.secSkill[0] != SECSK_NONE || m_linfo.secSkill[1] != SECSK_NONE)		
#ifdef PC_VERSION
		h += 220;
#else
        h += 120;
#endif
	
	// tweet btn
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	h += sz.h;
#endif
	
	return iSize(w,h);
}

void iDlg_NewLevel::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 100) {

		//Twitts must be in English
#if defined( OS_MACOS )
//		gTwitter.Tweet(m_pHero->EnglishName() + _T(" ") +iFormat(gTextMgr.English(TRID_MSG_REACHLEVEL),m_pHero->Level()) + _T("."));
		m_pTweetBtn->SetVisible(false);
#endif		
		return;
	}
	if (uid == DRC_OK) 
		m_selSecSkill = m_linfo.secSkill[0];
	else 
		m_selSecSkill = ((iLevelDlgBtn*)pView)->m_ss;

	EndDialog(DRC_OK);
}

void iDlg_NewLevel::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	if(m_bNochoice)
		EndDialog(0);
}


