#include "stdafx.h"
#include "Dlg_ArtInfo.h"

#include "lang.h"

iDlg_ArtInfo::iDlg_ArtInfo(iViewMgr* pViewMgr, uint16 artId, const iHero* pOwner)
: iBaseGameDlg(pViewMgr), m_pOwner(pOwner), m_artId(artId), m_fcReqMsg(dlgfc_splain)
{
	SECONDARY_SKILLS reqSkill = gGame.ItemMgr().m_artMgr[artId].ReqSkill();
	uint16 reqLevel = gGame.ItemMgr().m_artMgr[artId].ReqLevel();
	if (reqLevel && reqSkill != SECSK_NONE) {
		m_reqMsg.Setf(gTextMgr[TRID_MSG_ARTREQ_LVL_SKILL], reqLevel, gTextMgr[TRID_HSKILL_ESTATES+reqSkill]);
	} else if (reqLevel) {
		m_reqMsg.Setf(gTextMgr[TRID_MSG_ARTREQ_LVL], reqLevel);
	} else if (reqSkill != SECSK_NONE) {
		m_reqMsg.Setf(gTextMgr[TRID_MSG_ARTREQ_SKILL], gTextMgr[TRID_HSKILL_ESTATES+reqSkill]);
	}

	if (pOwner && gGame.ItemMgr().m_artMgr[artId].CanAttach(pOwner)) {
		m_fcReqMsg.cmpProps.faceColor = RGB16(96,255,96);
	} else {
		m_fcReqMsg.cmpProps.faceColor = RGB16(255,96,96);
	}
}

void iDlg_ArtInfo::OnCreateDlg()
{
	iRect clRect = ClientRect();	
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));	
	AddChild(m_pTweetBtn = new iImgBtn(m_pMgr, this, AlignRect(sz, clRect, AlignBottom) , 100, PDGG(TWEET), PDGG(TWEET_PRESSED)));
#endif
}
#ifdef PC_VERSION
void iDlg_ArtInfo::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();	
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));    
    m_pTweetBtn->SetRect( AlignRect(sz, clRect, AlignBottom) );
#endif
}
#endif
void iDlg_ArtInfo::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();
	// title
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), gTextMgr[gGame.ItemMgr().m_artMgr[m_artId].NameKey(m_pOwner)], rc);
	
	// description
	rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[gGame.ItemMgr().m_artMgr[m_artId].DescKey(m_pOwner)], rc);
	// requirements
	if (!m_reqMsg.Empty()) rc.y += gTextComposer.TextBoxOut(m_fcReqMsg, gApp.Surface(), m_reqMsg, rc);
	// icon
	SpriteId sid = gGame.ItemMgr().m_artMgr[m_artId].Sprite();
	iSize ssiz = gGfxMgr.Dimension(sid);	
	rc.y += 10;
	gGfxMgr.Blit(sid,gApp.Surface(), GetRealSpritePos(sid, rc, false, AlignTop));	
}

iSize iDlg_ArtInfo::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();


	// title
	h += gTextComposer.GetTextBoxSize(gTextMgr[gGame.ItemMgr().m_artMgr[m_artId].NameKey(m_pOwner)], w, dlgfc_topic).h;

	// description
	h += gTextComposer.GetTextBoxSize(gTextMgr[gGame.ItemMgr().m_artMgr[m_artId].DescKey(m_pOwner)], w, dlgfc_plain).h;

	// requirements
	if (!m_reqMsg.Empty()) h += gTextComposer.GetTextBoxSize(m_reqMsg, w, m_fcReqMsg).h;

	// icon
	h += gGfxMgr.Dimension(gGame.ItemMgr().m_artMgr[m_artId].Sprite()).h + 10;
#if 0
	// tweet this
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));	
	h += sz.h;
#endif
	
	return iSize(w,h);
}

void iDlg_ArtInfo::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 100) {
		
		//Twitts must be in English
#if defined( OS_MACOS )		
//	gTwitter.Tweet(iStringT(gTextMgr.English(gGame.ItemMgr().m_artMgr[m_artId].NameKey(m_pOwner))) + _T(":") + iStringT(gTextMgr.English(gGame.ItemMgr().m_artMgr[m_artId].DescKey(m_pOwner))));
	m_pTweetBtn->SetVisible(false);
#endif		
		return;
	}
	
	EndDialog(pView->GetUID());
}

void iDlg_ArtInfo::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {
	EndDialog(0);
}
