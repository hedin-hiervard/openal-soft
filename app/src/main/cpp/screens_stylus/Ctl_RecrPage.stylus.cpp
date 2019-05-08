#include "stdafx.h"
#include "CastleView.h"
#include "Ctl_RecrPage.h"
#include "Ctl_RecrPage.stylus.h"

#include "ArmyList.stylus.h"

const sint32 RECR_BTNSX = 106;
const sint32 RECR_BTNSY = 53;

void iRecrButton_stylus::OnCompose()
{
	iRect rc = GetScrRect();
	if (!m_pDwel) gApp.Surface().Darken25Rect(rc);
	ButtonFrame(gApp.Surface(), rc, GetButtonState());

	iRect icRect(rc.x+1,rc.y+11,40,38);
	if (m_pDwel) {				
		// Creature icon
		SpriteId sid = PDGG_MINIMON + m_pDwel->CrType();
		icRect = AlignRect(gGfxMgr.Dimension(sid), icRect, AlignCenter);
		gGfxMgr.Blit(sid,gApp.Surface(),icRect.point()-gGfxMgr.Anchor(sid));

		// Name
		gTextComposer.TextOut(gApp.Surface(), rc.point(), iStringT(_T("#FFF6")) + gTextMgr[m_pDwel->CrType()*3+TRID_CREATURE_PEASANT_F2],iRect(rc.x+1,rc.y+1,102,12),AlignCenter);

		iRect orc(rc.x+41, rc.y+13, rc.w-41, 24);

		/*
		gTextComposer.TextOut(gApp.Surface(), orc.point(), iFormat(_T("#S2%s%d#S1#FCA6 / %d"), ((m_pDwel->CrCount())?_T("#FFF0"):_T("#FCA6")),m_pDwel->CrCount(),m_pDwel->Growth()),orc,AlignCenter);
		orc.y+=24;
		*/

		// Available
		gTextComposer.TextOut(gApp.Surface(), orc.point(), iFormat(_T("#S0#FB95%s: #S1%s%d"), gTextMgr[TRID_AVAILABLE], ((m_pDwel->CrCount())?_T("#FFF0"):_T("#FCA6")),m_pDwel->CrCount()),orc,AlignTop);
		orc.y += 11;
		// Growth
		gTextComposer.TextOut(gApp.Surface(), orc.point(), iFormat(_T("#S0#FB95%s: #S1#FCA6%d"), gTextMgr[TRID_GROWTH], m_pDwel->Growth()),orc,AlignTop);
		orc.y += 13;

		// Cost
		gTextComposer.TextOut(gApp.Surface(), orc.point(), iStringT(_T("#S1#FFED")) + MineralSet2Text(CREAT_DESC[m_pDwel->CrType()].cost),orc,AlignTop);

		// Skills
		/*
		iTextComposer::FontConfig fc(FS_MEDIUM, RGB16(192,192,128));
		iPoint op = rc.point() + iPoint(41,16);
		gGfxMgr.Blit(PDGG_ICN_PSKILLS,gApp.Surface(),op + iPoint(0,0) - gGfxMgr.Anchor(PDGG_ICN_PSKILLS));
		gTextComposer.TextOut(fc, gApp.Surface(), op + iPoint(0,0) + iPoint(12,-2), iFormat(_T("%d"),CREAT_DESC[m_pDwel->CrType()].attack));
		gGfxMgr.Blit(PDGG_ICN_PSKILLS+1,gApp.Surface(),op + iPoint(0,12) - gGfxMgr.Anchor(PDGG_ICN_PSKILLS+1));
		gTextComposer.TextOut(fc, gApp.Surface(), op + iPoint(0,12) + iPoint(12,-2), iFormat(_T("%d"),CREAT_DESC[m_pDwel->CrType()].defence));
		gGfxMgr.Blit(PDGG_ICN_SPEED,gApp.Surface(),op + iPoint(0,24) - gGfxMgr.Anchor(PDGG_ICN_SPEED));
		gTextComposer.TextOut(fc, gApp.Surface(), op + iPoint(0,24) + iPoint(12,-2), iFormat(_T("%d"),CREAT_DESC[m_pDwel->CrType()].speed));
		gGfxMgr.Blit(PDGG_ICN_DAMAGE,gApp.Surface(),op + iPoint(26,0) - gGfxMgr.Anchor(PDGG_ICN_DAMAGE));
		gTextComposer.TextOut(fc, gApp.Surface(), op + iPoint(26,0) + iPoint(12,-2), iFormat(_T("%d-%d"),CREAT_DESC[m_pDwel->CrType()].damage_min,CREAT_DESC[m_pDwel->CrType()].damage_max));
		gGfxMgr.Blit(PDGG_ICN_LIFES,gApp.Surface(),op + iPoint(26,12) - gGfxMgr.Anchor(PDGG_ICN_LIFES));
		gTextComposer.TextOut(fc, gApp.Surface(), op + iPoint(26,12) + iPoint(12,-2), iFormat(_T("%d"),CREAT_DESC[m_pDwel->CrType()].hits));
		gGfxMgr.Blit(PDGG_ICN_GROWTH,gApp.Surface(),op + iPoint(26,24) - gGfxMgr.Anchor(PDGG_ICN_GROWTH));
		gTextComposer.TextOut(fc, gApp.Surface(), op + iPoint(26,24) + iPoint(12,-2), iFormat(_T("%d"),m_pDwel->Growth()));
		*/
	}
	rc.InflateRect(1);
	gApp.Surface().FrameRect(rc, cColor_Black);
}


iRecrSet_stylus::iRecrSet_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, uint32 uid, iPoint offset)
: iRecrSet(pViewMgr, this, uid, iRect(0,0,RECR_BTNSX*3+2,RECR_BTNSY*2+1) + offset)
{
	for (uint32 yy=0; yy<2; ++yy) 
		for (uint32 xx=0; xx<3; ++xx)
			AddChild(m_pBtns[yy*3+xx] = new iRecrButton_stylus(pViewMgr, this, iRect(xx*(RECR_BTNSX+1),yy*(RECR_BTNSY+1),RECR_BTNSX,RECR_BTNSY), 50 + (yy*3+xx)));
}


void iRecrSet_stylus::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	check(m_pCastle);
	check(m_pBtns[pView->GetUID()-50]->Dwelling());
	iDwellCtlCnst* pDwel = m_pBtns[pView->GetUID()-50]->Dwelling();
	iCreatGroup cg(pDwel->CrType(), pDwel->CrCount());
	iDlg_Recruit dlg(m_pMgr, cg, m_pCastle->Garrison(),pDwel->Owner()->Owner(), NULL);
	if (dlg.DoModal() == DRC_OK) {
		pDwel->CrCount() = cg.Count();
	}
#ifdef MULTIPLAYER
	gMPMgr.OnCastleUpdate(m_pCastle);	
#endif

}

iRecrTab_stylus::iRecrTab_stylus(iViewMgr* pViewMgr)
: iRecrTab(pViewMgr, iRect(0, 37, gApp.Surface().GetWidth(), gApp.Surface().GetHeight() - 58))
{
	iPoint offset(iABS<sint32>(gApp.Surface().GetWidth() - 320) / 2, 
		iABS<sint32>(gApp.Surface().GetHeight() - 240) / 2);

	AddChild(pRecrSet = new iRecrSet_stylus(pViewMgr,this, 101, offset));

	AddChild(m_pCastleGlyphBtn = new iCastleGlyphBtn(pViewMgr, this, iRect(0,108,32,20) + offset,110));
	AddChild(m_pSplitBtn = new iCheckButton(pViewMgr, this, iRect(0,108+20+1,32,32) + offset, PDGG_BTN_SPLIT, 111));
	AddChild(m_pHeroPortBtn = new iHeroPortBtn(pViewMgr, this, iRect(0,162,32,20) + offset,112));
	AddChild(pCastleArmyList = new iArmyListEx_stylus(pViewMgr, this, iPoint(33,108) + offset, m_pSplitBtn, iSize(40,37), 102));
	AddChild(pHeroArmyList = new iArmyListEx_stylus(pViewMgr, this, iPoint(33,146) + offset, m_pSplitBtn, iSize(40,36), 103));

	pCastleArmyList->AddCompetitor(pHeroArmyList);
	pHeroArmyList->AddCompetitor(pCastleArmyList);
}

void iRecrTab_stylus::UpdateSize()
{
	iPoint offset(iABS<sint32>(gApp.Surface().GetWidth() - 320) / 2, 
		iABS<sint32>(gApp.Surface().GetHeight() - 240) / 2);


	pRecrSet->SetRect(iRect(0,0,RECR_BTNSX*3+2,RECR_BTNSY*2+1) + offset);
	pRecrSet->Invalidate();

	m_pCastleGlyphBtn->SetRect(iRect(0,108,32,20) + offset);
	m_pCastleGlyphBtn->UpdateSize();
	m_pCastleGlyphBtn->Invalidate();

	m_pSplitBtn->SetRect(iRect(0,108+20+1,32,32) + offset);
	m_pSplitBtn->UpdateSize();
	m_pSplitBtn->Invalidate();

	m_pHeroPortBtn->SetRect(iRect(0,162,32,20) + offset);
	m_pHeroPortBtn->UpdateSize();
	m_pHeroPortBtn->Invalidate();

	pCastleArmyList->SetPos(iPoint(33,108) + offset);
	pCastleArmyList->UpdateSize();
	pCastleArmyList->Invalidate();

	pHeroArmyList->SetPos(iPoint(33,146) + offset);
	pHeroArmyList->UpdateSize();
	pHeroArmyList->Invalidate();

}
