#include "stdafx.h"
#include "Dlg_Rewards.h"
#include "Dlg_ArtInfo.h"
#include "Dlg_CreatInfo.h"
#include "Dlg_MLInfo.h"
#ifndef PC_VERSION
#include "helpers.h"
#endif

const sint32 BTNS_DIST[10][2] = {
	{ 1,1 },
	{ 2,1 },
	{ 3,1 },
	{ 4,1 },
	{ 3,2 },
	{ 3,2 },
	{ 4,2 },
	{ 4,2 },
	{ 5,2 },
	{ 5,2 }
};

//////////////////////////////////////////////////////////////////////////
struct iBtnEntry {
	iBtnEntry(SpriteId _sid, const iStringT& _text, const sint32 xoff) : sid(_sid), text(_text), xoffset(xoff) {}
	SpriteId	sid;
	iStringT	text;
	sint32		xoffset;
};

//////////////////////////////////////////////////////////////////////////
inline iBtnEntry GetRewardBtnEntry(const iRewardItem& reward)
{
	switch(reward.m_type) {
	case RIT_MINERAL:
		return iBtnEntry(PDGG(RES_ICONS) + reward.m_fParam, FormatNumber(reward.m_sParam,true), 25);
		break;
	case RIT_EXPERIENCE:
		return iBtnEntry(PDGG(DGLYPH_EXP), FormatNumber(reward.m_sParam,true), 30);
		break;
	case RIT_MANAPTS:
		return iBtnEntry(PDGG(DGLYPH_MANA), FormatNumber(reward.m_sParam, true), 30);
		break;
	case RIT_TRAVELPTS:
		return iBtnEntry(PDGG(DGLYPH_MOVES), FormatNumber(reward.m_sParam,true), 18);
		break;
	case RIT_MORALE:
		{
			SpriteId sid = PDGG(DGLYPH_MORALE);
			if (reward.m_sParam < 0) sid += 2;
			else if (reward.m_sParam == 0) sid += 1;
			return iBtnEntry( sid, FormatNumber(reward.m_sParam,true), 15);
		}
		break;
	case RIT_LUCK:
		{
			SpriteId sid = PDGG(DGLYPH_LUCK);
			if (reward.m_sParam < 0) sid += 2;
			else if (reward.m_sParam == 0) sid += 1;
			return iBtnEntry(sid, FormatNumber(reward.m_sParam, true), 15);
		}
		break;
	case RIT_FURTSKILL:
		return iBtnEntry(PDGG(DGLYPH_PRSKILLS) + (reward.m_fParam - FSK_ATTACK), FormatNumber(reward.m_sParam,true), 25);
		break;
	case RIT_SECSKILL:
		return iBtnEntry(PDGG(SEC_SKILLS) + (reward.m_fParam - SECSK_ESTATES), FormatNumber(reward.m_sParam, true), 25);
		break;
	
	case RIT_ARTIFACT:
		return iBtnEntry(gGame.ItemMgr().m_artMgr[reward.m_fParam].Sprite(), (reward.m_sParam != 0 && reward.m_sParam != 1) ? iFormat(_T("%d"), reward.m_sParam) : _T(""), 25);
		break;
	case RIT_MAGICSPELL:
		return iBtnEntry(PDGG(SPELL_ICONS) + reward.m_fParam, gTextMgr[gGame.ItemMgr().m_spellMgr.Spell((MAGIC_SPELL)reward.m_fParam)->NameKey()], 15);
		break;
	case RIT_CREATGROUP:
		return iBtnEntry(PDGG(MINIMON) + reward.m_fParam, FormatNumber(reward.m_sParam), 35);
		break;
	default:
		check(0);
	}
	return iBtnEntry(PDGG(BTN_CLOSE), _T("Invalid Entry"), 0);
}

//////////////////////////////////////////////////////////////////////////
void ShowRewardDlg(const iRewardItem& reward, PLAYER_ID pid)
{	
	switch(reward.m_type) {
	case RIT_MINERAL:
		{
			iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_MINERAL_GOLD + reward.m_fParam], gTextMgr[TRID_INFO_MINERALS], PDGG(RES_MINI_ICONS) + reward.m_fParam, pid);
			dlg.DoModal();
		}
		break;
	case RIT_EXPERIENCE:
		{
			iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_EXPERIENCE], gTextMgr[TRID_INFO_EXPERIENCE], PDGG(DGLYPH_EXP), pid);
			dlg.DoModal();
		}
		break;
	case RIT_MANAPTS:
		{
			iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_MANA], gTextMgr[TRID_INFO_MANA], PDGG(DGLYPH_MANA), pid);
			dlg.DoModal();
		}
		break;
	case RIT_TRAVELPTS:
		{
			iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_TRAVELPTS], gTextMgr[TRID_INFO_TRAVELPTS], PDGG(DGLYPH_MOVES), pid);
			dlg.DoModal();
		}
		break;
	case RIT_MORALE:
		{
			iMLInfoDlg dlg(&gApp.ViewMgr(), false, reward.m_sParam, pid);
			dlg.DoModal();
		}
		break;
	case RIT_LUCK:
		{
			iMLInfoDlg dlg(&gApp.ViewMgr(), true, reward.m_sParam, pid);
			dlg.DoModal();
		}
		break;
	case RIT_FURTSKILL:
		{
			iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_SKILL_ATTACK + (reward.m_fParam - FSK_ATTACK)], gTextMgr[TRID_INFO_ATTACK + (reward.m_fParam - FSK_ATTACK)], PDGG(DGLYPH_PRSKILLS) + (reward.m_fParam - FSK_ATTACK), pid);
			dlg.DoModal();
		}
		break;
	case RIT_ARTIFACT:
		{
			iDlg_ArtInfo dlg(&gApp.ViewMgr(), reward.m_fParam, NULL);
			dlg.DoModal();
		}
		break;
	case RIT_MAGICSPELL:
		{
			iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_SPNAME_MAGICARROW + reward.m_fParam], gTextMgr[TRID_SPDESC_MAGICARROW_NON + reward.m_fParam*MSL_COUNT], PDGG(SPELL_ICONS)+reward.m_fParam, pid);
			dlg.DoModal();
		}
		break;
	case RIT_CREATGROUP:
		{
			iCreatGroup group((CREATURE_TYPE)reward.m_fParam, reward.m_sParam);
			iCreatBonusList cbl;
			// fake army owner
			iArmyOwner fake;
			fake.ResetArmy();
			fake.AddGroupToArmy((CREATURE_TYPE)reward.m_fParam, reward.m_sParam);
			iDlg_CreatInfo dlg(&gApp.ViewMgr(), pid, &fake, 0, iFurtSkills(), false, 0, cbl);
			dlg.DoModal();
		}
		break;
	default:
		check(0);
	}
}
//////////////////////////////////////////////////////////////////////////
class iRewardBtn : public iButton, public IViewCmdHandler
{
public:
	iRewardBtn(iViewMgr* pViewMgr, const iRect& rect, uint32 uid, const iRewardItem& reward, iDlg_Rewards* par)
	: iButton(pViewMgr, this, rect, uid, Visible | Enabled), m_reward(reward), m_btnEntry(GetRewardBtnEntry(reward)), m_dlg(par) {}

	void OnCompose()
	{
#ifdef PC_VERSION
		iRect orc = GetScrRect();
		
		iRect icorc = AlignRect(iSize(GetSize().w / 2, GetSize().h), orc, AlignLeft);
		BlitIcon(gApp.Surface(), m_btnEntry.sid, AlignRect(iSize(50, 50), icorc, AlignRight), 255);
		if (!m_btnEntry.text.Empty()) {			
			iRect torc = AlignRect(iSize(GetSize().w / 2, GetSize().h), orc, AlignRight);
			gTextComposer.TextOut(rewardqfc ,gApp.Surface(), torc, m_btnEntry.text, torc, AlignLeft);
		}
#else
        iRect rc = GetScrRect();
        //		gApp.Surface().FrameRect(rc, cColor_White);
		iSize sz1 = gGfxMgr.ObjRect(m_btnEntry.sid).size();
		iSize sz2 = gTextComposer.GetTextSize(m_btnEntry.text, rewardqfc);
		
		rc = AlignRect(iSize(sz1.w + sz2.w + 10, iMAX<sint32>(sz1.h, sz2.h)), rc, AlignCenter);
		
		iRect irc = AlignRect(sz1, rc, AlignLeft);
        //		gApp.Surface().FrameRect(irc, cColor_Red);
		gGfxMgr.Blit(m_btnEntry.sid, gApp.Surface(), GetRealSpritePos(m_btnEntry.sid, irc, false, AlignCenter));
		iRect trc = AlignRect(sz2, rc, AlignRight);
        //		gApp.Surface().FrameRect(trc, cColor_Blue);
		if (!m_btnEntry.text.Empty()) {			
			gTextComposer.TextOut(rewardqfc, gApp.Surface(), trc, m_btnEntry.text);
		}
#endif
	}

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
	{
	//	uint32 uid = pView->GetUID();		
		m_dlg->EndDialog(0);
		//{
			//ShowRewardDlg(m_reward, PID_NEUTRAL);
		//}
	}

#ifndef PC_VERSION
    static iSize GetSize(iBtnEntry e) 
	{
		iSize isz = gGfxMgr.ObjRect(e.sid).size();
		iSize tsz = gTextComposer.GetTextSize(e.text, rewardqfc);
		return iSize(isz.w + 30 + tsz.w, 50);
	}
	
	iSize GetSize() const {
		return GetSize(m_btnEntry);
	}
#endif
    
	const iRewardItem&	m_reward;
	iBtnEntry			m_btnEntry;
	iDlg_Rewards*        m_dlg;
};

iDlg_Rewards::iDlg_Rewards(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, iHero* pHero, const iRewardsCtr& rewards)
: iBaseGameDlg(pViewMgr), m_title(title), m_text(text), m_pHero(pHero), m_rewards(rewards)
{
	check(rewards.GetSize() > 0 && rewards.GetSize() <= 8);
	
}

void iDlg_Rewards::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	iRect clRect = ClientRect();
	
	clRect.y += iBaseGameDlg::GetTopOffset();

	sint32 w = iBaseGameDlg::GetDefaultWidth();


	if (!m_title.Empty()) {
		clRect.y += gTextComposer.GetTextBoxSize(m_title, w, dlgfc_hdr).h;		
	}
	
	if (!m_text.Empty()) {
		clRect.y += gTextComposer.GetTextBoxSize(m_text, w, dlgfc_plain).h;		
	}

	// hero avatar
	clRect.y += 65;
#ifdef PC_VERSION
	sint32 height = 50;
    
	for (uint32 xx=0; xx<m_rewards.GetSize(); ++xx) 
		if(m_rewards[xx].m_type != RIT_TELEPORT) {	
            
            iRewardBtn* pRewBtn;
            
			AddChild(pRewBtn = new iRewardBtn(m_pMgr, AlignRect(iSize(w/2, height), clRect, AlignTop), 200+xx, m_rewards[xx], this));		
            
            m_pRewards.Add( pRewBtn );
			clRect.y += height;
		}	
#else
    for (uint32 xx=0; xx<m_rewards.GetSize(); ++xx) 
		if(m_rewards[xx].m_type != RIT_TELEPORT) {						
			m_pRewards.Add(new iRewardBtn(m_pMgr, iRect(), 200+xx, m_rewards[xx], this));		
		}	
	for(uint32 xx=0; xx<m_pRewards.GetSize(); xx++) {
		AddChild(m_pRewards[xx]);
	}
	UpdateSize();
#endif
}

void iDlg_Rewards::UpdateSize(){
#ifdef PC_VERSION
    Center();
#endif
    iRect clRect = ClientRect();
	
	clRect.y += iBaseGameDlg::GetTopOffset();
    
	sint32 w = iBaseGameDlg::GetDefaultWidth();
    
    
	if (!m_title.Empty()) {
		clRect.y += gTextComposer.GetTextBoxSize(m_title, w, dlgfc_hdr).h;		
	}
	
	if (!m_text.Empty()) {
		clRect.y += gTextComposer.GetTextBoxSize(m_text, w, dlgfc_plain).h;		
	}
#ifdef PC_VERSION
	// hero avatar
	clRect.y += 65;
    
    sint32 height = 50;
	for (uint32 xx=0; xx<m_rewards.GetSize(); ++xx) 
		if(m_rewards[xx].m_type != RIT_TELEPORT) {	
           
            m_pRewards.At( xx )->SetRect( AlignRect(iSize(w/2, height), clRect, AlignTop) );
			clRect.y += height;
		}	
#else
    iRect btnrc = iRect(0, clRect.y, clRect.w, clRect.h - clRect.y);
	iSimpleArray<iSize> szs;
	for(uint32 xx=0; xx<m_pRewards.GetSize(); xx++)
	{
		iSize sz = m_pRewards[xx]->GetSize();
		szs.Add(sz);
	}
	
	for(uint32 xx=0; xx<m_pRewards.GetSize(); xx++) {
		iPoint offs = GetRectOffset(btnrc.w, xx, szs);
		m_pRewards[xx]->SetRect(iRect(btnrc.point() + offs, m_pRewards[xx]->GetSize()));
	}
#endif
}

void iDlg_Rewards::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();

	if (!m_title.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), m_title, rc);		 
	}

	if (!m_text.Empty()) {
		rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), m_text, rc);		
	}
	
#ifdef PC_VERSION	
	rc.y += 10;
	iRect prc = AlignRect(iSize(50, 50), rc, AlignTop);
	rc.y += ComposeHeroAvatar(gApp.Surface(), prc, m_pHero, false, false);
#endif
}

iSize iDlg_Rewards::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	if (!m_title.Empty()) {
		h += gTextComposer.GetTextBoxSize(m_title, w, dlgfc_hdr).h;		
	}

	// text
	if (!m_text.Empty()) {
		h += gTextComposer.GetTextBoxSize(m_text, w, dlgfc_plain).h;		
	}
#ifdef PC_VERSION
	// hero port
	h += 65;	
	
	// Icons
	h += (m_rewards.GetSize()) * 50 + 15;
#else
    iSimpleArray<iSize> szs;
	for(uint32 xx=0; xx<m_rewards.GetSize(); xx++)
	{
		iBtnEntry e = GetRewardBtnEntry(m_rewards[xx]);
		iSize sz = iRewardBtn::GetSize(e);		
		szs.Add(sz);
	}
	
	// Icons
	h += GetRectsHeight(w, szs);
#endif
	return iSize(w,h);
}

void iDlg_Rewards::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	//uint32 uid = pView->GetUID();
	//{
	//	EndDialog(DRC_OK);
	//}
}

void iDlg_Rewards::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{	
	EndDialog(DRC_OK);
}

