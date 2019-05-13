#include "stdafx.h"
#include "Dlg_Chest.h"

class iChestDlgBtn : public iButton
{
public:
	iChestDlgBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, SpriteId sid, const iStringT& text)
	: iButton(pViewMgr, pCmdHandler, rect, uid, Visible | Enabled), m_sid(sid), m_text(text) {}

	void OnCompose()
	{
		iRect rc = GetScrRect();
		//gApp.Surface().FillRect(rc,cColor_Black);
		//rc.DeflateRect(1);
		//gGfxMgr.BlitTile(PDGG(CTILE),gApp.Surface(),rc);
		//ButtonFrame(gApp.Surface(),rc,GetButtonState());
		//ComposeDlgButton(gApp.Surface(), rc, GetButtonState());
		
		//ComposeDlgBkgndPanel(gApp.Surface(), rc, !(m_state & Pressed));		
		gGfxMgr.Blit(PDGG(BTN_FRAME_TREASURE_DELUXE), gApp.Surface(), rc);
		iPoint offs = iPoint();		
		iSize sz = gGfxMgr.ObjRect(m_sid).size();
		iTextComposer::FontConfig fc = rewardqfc;
		fc.cmpProps.faceColor = RGB16(142, 138, 72);
		BlitIcon(gApp.Surface(), m_sid, iRect(GetRealSpritePos(m_sid, rc, false, AlignTop), sz) + iPoint(0, 20), 255, offs);
		iRect trc = AlignRect(iSize(m_Rect.w, gTextComposer.GetFontHeight(fc.fontSize)), rc, AlignBottom) - iPoint(0, 15);
		gTextComposer.TextBoxOut(fc, gApp.Surface(), m_text, trc + offs);
	
		if(m_state & Pressed)
			gGfxMgr.Blit(PDGG(BTN_FRAME_TREASURE_DELUXE_PRESSED), gApp.Surface(), rc);

	}

	SpriteId	m_sid;
	iStringT	m_text;
};

//////////////////////////////////////////////////////////////////////////
iDlg_Chest::iDlg_Chest(iViewMgr* pViewMgr, iHero* pHero, iMapItem_Chest* pChest)
: iBaseGameDlg(pViewMgr), m_pHero(pHero)
{ 
	m_cntExp = pHero->ConvExpPts(pChest->ExpPtCount());
	m_cntGold = pChest->ExpPtCount() + (pChest->ExpPtCount()>>2);
}

void iDlg_Chest::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(iDlg_Chest::Gold); 
		return;
	}		
#endif

	iRect clRect = ClientRect();
	iRect tmprc = AlignRect(iSize(m_Rect.w / 2 - 15, 60 + 30), m_Rect, AlignBottomLeft) - iPoint(0, 50);
	AddChild(m_pGoldBtn = new iChestDlgBtn(m_pMgr, this, AlignRect(iSize(79, 79), tmprc, AlignRight) - m_Rect, Gold,PDGG(DGLYPH_GOLD), FormatNumber(m_cntGold)));
	tmprc = AlignRect(iSize(m_Rect.w / 2 - 15, 60 + 30), m_Rect, AlignBottomRight) - iPoint(0, 50);
	AddChild(m_pExpBtn = new iChestDlgBtn(m_pMgr, this, AlignRect(iSize(79, 79), tmprc, AlignLeft) - m_Rect, Experience,PDGG(DGLYPH_EXP), FormatNumber(m_cntExp)));
}
#ifdef PC_VERSION
void iDlg_Chest::UpdateSize(){
    
    Center();
    
    iRect tmprc = AlignRect(iSize(m_Rect.w / 2 - 15, 60 + 30), m_Rect, AlignBottomLeft);
    m_pGoldBtn->SetRect( AlignRect(iSize(60, 60), tmprc, AlignRight) - m_Rect );
    tmprc = AlignRect(iSize(m_Rect.w / 2 - 15, 60 + 30), m_Rect, AlignBottomRight);
    m_pExpBtn->SetRect( AlignRect(iSize(60, 60), tmprc, AlignLeft) - m_Rect );    

}
#endif
void iDlg_Chest::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);	
	rc.y += iBaseGameDlg::GetTopOffset();
	gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(),rc.point(),gTextMgr[TRID_MAPRES_CHEST], iRect(rc.x,rc.y,rc.w,15),AlignCenter);
	rc.y+=gTextComposer.GetFontHeight(dlgfc_hdr.fontSize);;
	rc.y+=gTextComposer.TextBoxOut(dlgfc_splain,gApp.Surface(),gTextMgr[TRID_MAPRES_CHEST_MSG], rc);
	gTextComposer.TextBoxOut(dlgfc_stopic, gApp.Surface(),gTextMgr[TRID_CHOOSE_QUESTION], rc);
    
    rc = iRect(clRect.x, m_pExpBtn->GetScrRect().y2(), clRect.w, 100);
    const iMineralSet& ms = m_pHero->Owner()->Minerals();
    iSize mineralsize = GetMineralSetComposeSize(ms, false, true, true);
    
    sint32 posx =
#if defined(OS_IPHONE) || defined(OS_ANDROID)
        10;
#else
        rc.x - 25; 
#endif
    
    rc = iRect(posx, m_pExpBtn->GetScrRect().y2(), m_Rect.w, 100);
    
    iRect trc = AlignRect(mineralsize, rc, AlignTop);    
#if defined(PC_VERSION)
    ComposeMineralSet(ms, ms, trc, false, false, false, true, true);
#endif
    if(!gUseIpadUI)
    {
        ComposeMineralSet(ms, ms, trc, false, false, false, true, true);
    }
}

iSize iDlg_Chest::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();
    
	// title
	h += gTextComposer.GetFontHeight(dlgfc_hdr.fontSize);;
	// Message
	h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_MAPRES_CHEST_MSG],w,dlgfc_splain).h;
	// Question
	h += gTextComposer.GetFontHeight(dlgfc_stopic.fontSize);
	// buttons
	h += 80 + 15;

    h += 50; // resources
    
	return iSize(w,h);
}

void iDlg_Chest::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	{
		EndDialog(pView->GetUID());
	}
}

