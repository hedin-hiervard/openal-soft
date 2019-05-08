#include "stdafx.h"
#include "CastleView.h"
#include "CastleView.stylus.h"
#include "Ctl_CnstPage.h"
#include "Ctl_CnstPage.stylus.h"

/* Castle constructions listbox */
iCnstListBox_stylus::iCnstListBox_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
iCnstListBox(pViewMgr, pCmdHandler, rect, uid)
{}

uint32 iCnstListBox_stylus::LBItemHeight() const
{ return 16; }	


void iCnstListBox_stylus::ComposeLBBackground(const iRect& rect)
{
	iRect rc = rect;
	gApp.Surface().Darken25Rect(rc);
}


void iCnstListBox_stylus::ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc)
{
	iTextComposer::FontConfig fc(dlgfc_plain);
	iRect rc=irc;
	ButtonFrame(gApp.Surface(),rc,iButton::Pressed);
	rc.DeflateRect(1);
	if (bSel) {
		gGfxMgr.BlitTile(PDGG_CTILE, gApp.Surface(),rc);
		ButtonFrame(gApp.Surface(),rc,0);
	}

	if (!m_pCastle) return;
	rc.DeflateRect(1);
	iCtlCnst::State state = m_pCastle->Cnsts()[iIdx]->GetState();
	if (state == iCtlCnst::Built) {
		fc.cmpProps.faceColor = RGB16(255,192,0);
		BlitIcon(gApp.Surface(),PDGG_CNST_STATE_GLYPH+0, iRect(rc.x2()-14,rc.y,14,rc.h));
	} else if (state == iCtlCnst::Available) {
		fc.cmpProps.faceColor = RGB16(128,255,64);
	} else if (state == iCtlCnst::NotEnoughRes) {
		fc.cmpProps.faceColor = RGB16(255,128,64);
		BlitIcon(gApp.Surface(),PDGG_CNST_STATE_GLYPH+2, iRect(rc.x2()-14,rc.y,14,rc.h));
	} else if (state == iCtlCnst::Unavailable) {
		fc.cmpProps.faceColor = RGB16(255,64,64);
		BlitIcon(gApp.Surface(),PDGG_CNST_STATE_GLYPH+1, iRect(rc.x2()-14,rc.y,14,rc.h));
	}

	gTextComposer.TextOut(fc,gApp.Surface(),rc,m_pCastle->Cnsts()[iIdx]->Name(),rc,AlignLeft);
}

/* Castle construction description view */
iCnstDescView_stylus::iCnstDescView_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect):
iCnstDescView(pViewMgr, pCmdHandler, rect)
{
	AddChild(pBuildBtn = new iTextButton(pViewMgr, pCmdHandler, iRect(rect.x2()-90,rect.y2()-DEF_BTN_HEIGHT-5,85,DEF_BTN_HEIGHT), TRID_BTN_BUILD, 113));
	pBuildBtn->SetVisible(false);
}


void iCnstDescView_stylus::OnCompose()
{
	iTextComposer::FontConfig fc(dlgfc_plain);
	iRect rc = GetScrRect();

	gApp.Surface().Darken25Rect(rc);
	ButtonFrame(gApp.Surface(),rc,iButton::Pressed);

	iCnstDescView::OnCompose();
}

/* Castle constructions tab */
iCnstTab_stylus::iCnstTab_stylus(iViewMgr* pViewMgr, iCastleView* pCastleView):
iCnstTab(pViewMgr, pCastleView, iRect(0, 37, gApp.Surface().GetWidth(), gApp.Surface().GetHeight() - 58))
{
	AddChild(pDescView = new iCnstDescView_stylus(pViewMgr, this, iRect()));
	AddChild(pListBox = new iCnstListBox_stylus(pViewMgr, this, iRect(), 111));
	AddChild(pScrBar = new iPHScrollBar(pViewMgr, NULL, iRect(), 112));
	pListBox->SetScrollBar(pScrBar);	
	UpdateSize();
}

void iCnstTab_stylus::UpdateSize()
{
	uint32 height = gApp.Surface().GetHeight() - 66;
	uint32 left = (uint32)((gApp.Surface().GetWidth() - 28) * 0.58), 
		right = (uint32)((gApp.Surface().GetWidth() - 28) * 0.42);

	pDescView->SetRect(iRect(3,3,left, height));
	pDescView->UpdateSize();
	pDescView->Invalidate();

	pListBox->SetRect(iRect(left + 7,3,right, height));
	pListBox->UpdateSize();
	pListBox->Invalidate();

	pScrBar->SetRect(iRect(left + right + 7,4,15,height - 2));
	pScrBar->UpdateSize();
	pScrBar->Invalidate();
}

