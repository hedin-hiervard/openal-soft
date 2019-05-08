#include "stdafx.h"
#include "Dlg_WitchHut.h"

#include "lang.h"

iWitchHutDlg::iWitchHutDlg(iViewMgr* pViewMgr, SECONDARY_SKILLS skill, PLAYER_ID pid)
: iBaseGameDlg(pViewMgr), m_skill(skill)
{}

void iWitchHutDlg::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	iRect clRect = ClientRect();
//	AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	AddChild(m_pTweetBtn = new iImgBtn(m_pMgr, this, AlignRect(sz, clRect, AlignBottom) , 100, PDGG(TWEET), PDGG(TWEET_PRESSED)));
#endif
}
#ifdef PC_VERSION
void iWitchHutDlg::UpdateSize(){

    iRect clRect = ClientRect();
#if 0
    iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
    m_pTweetBtn->SetRect( AlignRect(sz, clRect, AlignBottom) );
#endif
}
#endif
void iWitchHutDlg::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.w = iCLAMP<sint32>(0, gApp.Surface().GetWidth()/2, gTextComposer.GetTextSize(gTextMgr[TRID_OBJ_WITCHHUT_MSG1], dlgfc_plain).w);
	rc.x += 25;
	rc.y += iBaseGameDlg::GetTopOffset();

	// Title
	rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[TRID_OBJ_WITCHHUT], rc) + 5;

	// Message
	iStringT msg = iFormat(gTextMgr[TRID_OBJ_WITCHHUT_MSG1], gTextMgr[TRID_HSKILL_ESTATES+m_skill]);
	rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), msg, rc) + 10;

	// Icon
	iRect orc(rc.x + (rc.w/2 - 18), rc.y, 36, 30);
	gApp.Surface().Darken25Rect(orc);
	ButtonFrame(gApp.Surface(),orc,0);
	BlitIcon(gApp.Surface(), PDGG(SEC_SKILLS) + m_skill, orc);
	orc.InflateRect(1);
	FrameRoundRect(gApp.Surface(), orc, cColor_Black);
	rc.y += 35;
}

iSize iWitchHutDlg::ClientSize() const
{
	
	sint32 w = iCLAMP<sint32>(0, gApp.Surface().GetWidth()/2, gTextComposer.GetTextSize(gTextMgr[TRID_OBJ_WITCHHUT_MSG1], dlgfc_plain).w);
	sint32 h = iBaseGameDlg::GetTopOffset();

	// Title
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_OBJ_WITCHHUT], w, dlgfc_hdr).h + 5;

	// Message
	iStringT msg = iFormat(gTextMgr[TRID_OBJ_WITCHHUT_MSG1], gTextMgr[TRID_HSKILL_ESTATES+m_skill]);
	h += gTextComposer.GetTextBoxSize(msg, w, dlgfc_plain).h + 10;

	// Icon
	h += 35;
	
	// tweet this
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	h += sz.h;
#endif
	return iSize(w + 50, h);
}

void iWitchHutDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 100) {
		
		//Twitts must be in English		
#if defined( OS_MACOS )	
		iStringT msg = iFormat(gTextMgr.English(TRID_OBJ_WITCHHUT_MSG1), gTextMgr.English(TRID_HSKILL_ESTATES+m_skill));
//		gTwitter.Tweet(msg);
		m_pTweetBtn->SetVisible(false);
#endif		
		return;
	}
	EndDialog(uid);
}


void iWitchHutDlg::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {
	EndDialog(0);
}
