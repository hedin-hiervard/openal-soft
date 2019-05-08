#include "stdafx.h"
#include "CastleView.h"
#include "Ctl_MarketPage.h"
#include "Ctl_MarketPage.stylus.h"

void iMineralBtn_stylus::OnCompose()
{
	iRect rc = GetScrRect();
	ButtonFrame(gApp.Surface(),rc,GetButtonState());
	if (m_bSelected) gApp.Surface().FillRect(rc, RGB16(255,192,0), 64);
	gApp.Surface().FrameRect(iRect(rc.x-1,rc.y-1,rc.w+2,rc.h+2),cColor_Black);

	BlitIcon(gApp.Surface(), PDGG_RES_BIG_ICONS+m_mt, iRect(rc.x,rc.y,rc.w,rc.h-13));
	rc.DeflateRect(0,rc.h-13,0,0);

	iTextComposer::FontConfig fontcfg(FS_MEDIUM,RGB16(192,192,192)) ;
	if (!m_quant.IsValid()) gTextComposer.TextOut( fontcfg, gApp.Surface(),rc.point(), gTextMgr[TRID_NA], rc, AlignCenter);
	else if (m_quant.num == 0 || m_quant.num >= m_quant.denum) gTextComposer.TextOut( fontcfg, gApp.Surface(),rc.point(), FormatNumber(m_quant.num), rc, AlignCenter);
	else gTextComposer.TextOut(fontcfg, gApp.Surface(),rc.point(), iFormat(_T("%d : %d"), m_quant.num, m_quant.denum), rc, AlignCenter);
}

iMarketView_stylus::iMarketView_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, const iSize& cellSiz, bool is_harbor):
iMarketView(pViewMgr, pCmdHandler, rect, uid, cellSiz, is_harbor)
{
	sint32 ox = rect.w/4 - cellSiz.w*3/2;
	sint32 oy = 14;
	sint32 bw = cellSiz.w;
	sint32 bh = cellSiz.h;
	sint32 b = 1;

	// Left side minerals
	AddChild(m_leftButtons[2] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*0,(bh+b)*0+oy,bw,bh), 203, MINERAL_WOOD));
	AddChild(m_leftButtons[3] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*1,(bh+b)*0+oy,bw,bh), 204, MINERAL_MERCURY));
	AddChild(m_leftButtons[1] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*2,(bh+b)*0+oy,bw,bh), 202, MINERAL_ORE));
	AddChild(m_leftButtons[6] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*0,(bh+b)*1+oy,bw,bh), 207, MINERAL_SULFUR));
	AddChild(m_leftButtons[5] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*1,(bh+b)*1+oy,bw,bh), 206, MINERAL_CRYSTAL));
	AddChild(m_leftButtons[4] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*2,(bh+b)*1+oy,bw,bh), 205, MINERAL_GEMS));
	AddChild(m_leftButtons[0] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*0,(bh+b)*2+oy,bw,bh), 201, MINERAL_GOLD));
	ox += rect.w/2;

	// Right side minerals
	AddChild(m_rightButtons[2] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*0,(bh+b)*0+oy,bw,bh), 213, MINERAL_WOOD));
	AddChild(m_rightButtons[3] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*1,(bh+b)*0+oy,bw,bh), 214, MINERAL_MERCURY));
	AddChild(m_rightButtons[1] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*2,(bh+b)*0+oy,bw,bh), 212, MINERAL_ORE));
	AddChild(m_rightButtons[6] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*0,(bh+b)*1+oy,bw,bh), 217, MINERAL_SULFUR));
	AddChild(m_rightButtons[5] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*1,(bh+b)*1+oy,bw,bh), 216, MINERAL_CRYSTAL));
	AddChild(m_rightButtons[4] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*2,(bh+b)*1+oy,bw,bh), 215, MINERAL_GEMS));
	AddChild(m_rightButtons[0] = new iMineralBtn_stylus(pViewMgr, this, iRect(ox+(bw+b)*2,(bh+b)*2+oy,bw,bh), 211, MINERAL_GOLD));

	// Slider
	AddChild(m_pSlider = new iPHScrollBar(m_pMgr, this, iRect(rect.w/2-(bw*2), oy + bh * 3,bw*4,15), 100, iScrollBar::Horizontal));

	// Max and Trade buttons
	AddChild(m_pMaxBtn = new iDlgIconButton(m_pMgr, this, iRect(rect.w/2-55, oy + bh * 3 + 20, 50, DEF_BTN_HEIGHT),PDGG_BTN_MAX, 101));
	AddChild(m_pTradeBtn = new iTextButton(m_pMgr, this, iRect(rect.w/2+5, oy + bh * 3 + 20, 50, DEF_BTN_HEIGHT), TRID_TRADE, 102));
}

void iMarketView_stylus::UpdateSize()
{
	iMarketView::UpdateSize();
	sint32 ox = m_Rect.w/4 - m_cellSiz.w*3/2;
	sint32 oy = 14;
	sint32 bw = m_cellSiz.w;
	sint32 bh = m_cellSiz.h;
	sint32 b = 1;

	m_pMaxBtn->SetRect(iRect(m_Rect.w/2-55, oy + bh * 3 + 20, 50, DEF_BTN_HEIGHT));m_pMaxBtn->UpdateSize();m_pMaxBtn->Invalidate();
	m_pTradeBtn->SetRect(iRect(m_Rect.w/2+5, oy + bh * 3 + 20, 50, DEF_BTN_HEIGHT));m_pTradeBtn->UpdateSize();m_pTradeBtn->Invalidate();
}

void iMarketView_stylus::OnCompose()
{
	iRect rc = GetScrRect();
	gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rc.point(), gTextMgr[TRID_KINGDOM_RESOURCES], iRect(rc.x+rc.w/4-m_cellSiz.w*3/2,rc.y,m_cellSiz.w*3,16), AlignTop);
	gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rc.point(), gTextMgr[TRID_AVAILABLE_FOR_TRADE], iRect(rc.x+rc.w/2 + rc.w/4-m_cellSiz.w*3/2,rc.y,m_cellSiz.w*3,16), AlignTop);

	iRect trc(rc.x+rc.w/2-m_cellSiz.w*2, rc.y + 20 + m_cellSiz.h*2, m_cellSiz.w*4, m_cellSiz.h/2+12);
	if (m_pTradeBtn->IsEnabled()) {
		MINERAL_TYPE mfrom = m_leftButtons[m_leftSel]->MineralType();
		MINERAL_TYPE mto = m_rightButtons[m_rightSel]->MineralType();
		FractionCoeff nval = MineralExchRate(mfrom, mto, m_marketsCount).GetNormalized();
		trc.DeflateRect(0, gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), gTextMgr[TRID_MSG_MARKET_OFFER], trc),0,0);
		sint32 amount = m_pSlider->CurPos();
		gTextComposer.TextOut(dlgfc_topic, gApp.Surface(), trc.point(), iFormat(gTextMgr[TRID_MSG_MARKET_SFORS], Mineral2Text(mto, nval.num * amount).CStr(), Mineral2Text(mfrom, nval.denum * amount).CStr()), trc, AlignCenter);
	} 
	else gTextComposer.TextBoxOut(dlgfc_splain, gApp.Surface(), gTextMgr[TRID_MSG_MARKET_DEF], trc);
	//gApp.Surface().FillRect(iRect(rc.x+rc.w/2-m_cellSiz.w*2, rc.y + 26 + m_cellSiz.h*2, m_cellSiz.w*4, m_cellSiz.h/2+6 ), cColor_Black);
	//gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rc.point(), gTextMgr[TRID_KINGDOM_RESOURCES], iRect(rc.x+rc.w/4-m_cellSiz.w*3/2,rc.y,m_cellSiz.w*3,16), AlignTop);
}

void iMarketView_stylus::UpdateControls()
{
	UpdateRightSide();
	bool dall = m_leftSel == -1 || m_rightSel == -1 || m_leftButtons[m_leftSel]->MineralType() == m_rightButtons[m_rightSel]->MineralType();

	if (!dall) {
		sint32 lnum = m_leftButtons[m_leftSel]->Quantity().num;
		sint32 rnum = m_rightButtons[m_rightSel]->Quantity().denum;
		m_pSlider->SetMetrics(lnum/rnum+1,1);
		if (lnum < rnum) dall = true;
	} else {
		m_pSlider->SetMetrics(0,0);
	}

	m_pTradeBtn->SetEnabled(false);
	m_pSlider->SetCurPos(0);
	m_pMaxBtn->SetEnabled(!dall);
	m_pSlider->SetEnabled(!dall);
}

void iMarketView_stylus::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if ( uid == 101 ) {
		// Max button
		m_pSlider->SetCurPos(m_pSlider->MaxPos());
		SliderPosChanged();
	}
	iMarketView::iCMDH_ControlCommand(pView, cmd, param);
}


void iMarketView_stylus::SliderPosChanged()
{
	check(m_pSlider->IsEnabled());
	m_pTradeBtn->SetEnabled(m_pSlider->CurPos() > 0);
}

iMarketTab_stylus::iMarketTab_stylus(iViewMgr* pViewMgr, bool is_harbor)
: iMarketTab(pViewMgr, is_harbor, is_harbor ? iRect(0, 15,gApp.Surface().GetWidth(), gApp.Surface().GetHeight() - 36) :
				iRect(0, 37, gApp.Surface().GetWidth(), gApp.Surface().GetHeight() - 58))
{
	AddChild( m_pMarketView = new iMarketView_stylus(pViewMgr, this, iRect(5,5,m_Rect.w - 10, m_Rect.h), 100, iSize(44,40), is_harbor));
}


