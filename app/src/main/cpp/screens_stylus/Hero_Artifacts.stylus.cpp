#include "stdafx.h"
#include "HeroView.h"
#include "HeroView.stylus.h"
#include "ArtifactList.h"
#include "ArtifactList.stylus.h"
#include "Hero_Artifacts.stylus.h"

const sint32 doX = 65;
const sint32 doY = 5;
const sint32 acW = 30;
const sint32 acH = 28;

//////////////////////////////////////////////////////////////////////////
const sint32 HERO_DOLL_CELLS[AC_COUNT][2] = {
	{74, -2}, {74,29}, {74,60}, {-6,23}, {109,91}, {-6,54}, {142,91}, {109,122}, {76,125}, {76,162}, {27,65}, {27,96}, {27,127}, {27,158}
};

//////////////////////////////////////////////////////////////////////////
iArtifactsTab_stylus::iArtifactsTab_stylus(iViewMgr* pViewMgr) : iHeroViewChild(pViewMgr) 
{
	iRect rc = GetScrRect();
	iPoint offset(gApp.Surface().GetWidth() / 2 - 135, 0);
	m_pArtBackPackCtrl = new iArtBackPackCtrl_stylus(m_Competitors, pViewMgr, this, offset, iSize(acW,acH), 5, 0/*iArtBackPackCtrl::Horizontal*/, 101);
	AddChild((iArtBackPackCtrl_stylus*)m_pArtBackPackCtrl);
	for (uint32 xx=0; xx<AC_COUNT; ++xx) {
		m_pDollItems[xx] = new iArtItemCtrl_stylus(m_Competitors, pViewMgr, this, offset + iPoint(HERO_DOLL_CELLS[xx][0]+doX,HERO_DOLL_CELLS[xx][1]+doY), iSize(acW,acH), 110+xx);
		AddChild((iArtItemCtrl_stylus*)m_pDollItems[xx]);
	}
}

void iArtifactsTab_stylus::OnHeroChanged() 
{
	m_pArtBackPackCtrl->SetBackPack(&m_pHero->Artifacts().BackPack(), m_pHero);
	for (uint32 xx=0; xx<AC_COUNT; ++xx) {
		m_pDollItems[xx]->SetOwner(m_pHero);
		m_pDollItems[xx]->SetArtCell(&m_pHero->Artifacts().DressedItem((HERO_ART_CELL)xx));
	}
	Invalidate();
}

void iArtifactsTab_stylus::ComposeArtCell(const iRect& rc, uint16 artId)
{
	gApp.Surface().FillRect(rc,RGB16(255,192,64),64);
	ButtonFrame(gApp.Surface(),rc, 0);
	if (artId != 0xFFFF) BlitIcon(gApp.Surface(),gGame.ItemMgr().m_artMgr[artId].Sprite(),rc);
}

void iArtifactsTab_stylus::OnCompose()
{
	iPoint offset(gApp.Surface().GetWidth() / 2 - 135, 0);
	iHeroViewChild::OnCompose();
	iRect rc = GetScrRect();

	// Draw Hero doll
	iPoint hdPos(rc.x+doX,rc.y+doY);
	hdPos += offset;
	gGfxMgr.Blit(PDGG_HDOLL_WIRE,gApp.Surface(),hdPos);
	gGfxMgr.BlitEffect(PDGG_HDOLL_BODY,gApp.Surface(),hdPos,iGfxManager::EfxTransparent);
	gGfxMgr.BlitEffect(PDGG_HDOLL_CAPE,gApp.Surface(),hdPos,iGfxManager::EfxTransparent);
	gGfxMgr.BlitEffect(PDGG_HDOLL_CAPE,gApp.Surface(),hdPos,iGfxManager::EfxTransparent);

	/*
	iRect bpRect(rc.x+3,rc.y+3,acW,17);
	bpRect.y += 17; bpRect.h=acH;
	for (uint32 xx=0; xx<5; ++xx){
	ComposeArtCell(bpRect, 0xFFFF);
	bpRect.y += acH+1;
	}
	bpRect.h=17;

	// Compose doll's cells
	for (uint32 xx=0; xx<m_dollCells.GetSize(); ++xx) {
	ComposeArtCell(m_dollCells[xx],m_pHero->Artifacts().DressedItem((HERO_ART_CELL)xx).artId);
	}
	*/
}

void iArtifactsTab_stylus::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	//uint32 uid = pView->GetUID();
}

sint32 iArtifactsTab_stylus::CellByPos(const iPoint& pos)
{
	sint32 result = -1;
	// find in doll's cells
	for (uint32 xx=0; xx<m_dollCells.GetSize(); ++xx) {
		if (m_dollCells[xx].PtInRect(pos)) return xx;
	}

	return result;
}

void iArtifactsTab_stylus::OnMouseDown(const iPoint& pos, MouseId mID) 
{
}

void iArtifactsTab_stylus::OnMouseUp(const iPoint& pos) 
{
}


