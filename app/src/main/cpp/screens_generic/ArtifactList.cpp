#include "stdafx.h"
#include "ArtifactList.h"
#include "Dlg_ArtInfo.h"

//
void ComposeArtifactCell(const iRect& rect, uint16 artId, uint32 flags, bool touch, bool bFrameRect)
{
	iRect rc(rect);
	rc.InflateRect(1);
	if(bFrameRect)
	{
		gApp.Surface().FrameRect(rc,cColor_Black);

	/*
	SpriteId bsid;
	if (artId == INVART) bsid = PDGG(ART_CELLS);
	else if (flags & ABF_PRESSED) bsid = PDGG(ART_CELLS) + 1;
	else bsid = PDGG(ART_CELLS) + 2;
	Tile3x3Sprite(gApp.Surface(), rect, bsid);*/

		gApp.Surface().FillRect(rect,(artId == INVART)?RGB16(64,64,64):RGB16(128,128,0),64);
		if(!touch)
			ButtonFrame(gApp.Surface(),rect, (flags & ABF_PRESSED)?iButton::Pressed:0);
		else
			ButtonFrame(gApp.Surface(),rect, 0);
	}

	if(touch && (flags & ABF_PRESSED)) {
		//rc.DeflateRect(3);
		//gApp.Surface().FrameRect(rc,cColor_Yello,64);	
		gApp.Surface().Darken50Rect(rc);
	}
	if (artId != INVART) BlitIcon(gApp.Surface(), gGame.ItemMgr().m_artMgr[artId].Sprite(), rect);
}


/*
 *	iArtItemCtrl
 */
iArtItemCtrl::iArtItemCtrl():
m_pOwner(NULL)
, m_pArtCell(NULL)
, m_bPressed(false)
, m_bClickMode(false)
, m_bHighlighted(false)
{
}

iArtCell iArtItemCtrl::Remove(sint32 cell)
{
	iArtCell result = *m_pArtCell;
	if (result.artId != INVART) gGame.ItemMgr().m_artMgr[result.artId].Dettach(m_pOwner);
	m_pArtCell->Reset();
#ifdef MULTIPLAYER	
	gMPMgr.OnHeroUpdate(m_pOwner);
#endif

	return result;
}

iArtCell iArtItemCtrl::Replace(sint32 cell, const iArtCell& newArt)
{
	iArtCell result = *m_pArtCell;
	if (result.artId != INVART) gGame.ItemMgr().m_artMgr[result.artId].Dettach(m_pOwner);
	if (newArt.artId != INVART) gGame.ItemMgr().m_artMgr[newArt.artId].Attach(m_pOwner);

	m_pArtCell->artId = newArt.artId;
#ifdef MULTIPLAYER	
	gMPMgr.OnHeroUpdate(m_pOwner);
#endif

	return result;
}


/*
 *	iArtBackPackCtrl
 */


iArtBackPackCtrl::iArtBackPackCtrl(const iSize& cellSiz, uint32 cellsCount, uint32 flags):
m_flags(flags)
, m_cellSize(cellSiz)
, m_cellsCount(cellsCount)
, m_pBackPack(NULL)
, m_listOffset(0)
, m_dCell(-1)
, m_bClickMode(false)
, m_pOwner(NULL)
{
}

void iArtBackPackCtrl::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == 101) ListPageUp();
	else if (uid == 102) ListPageDown();
}

void iArtBackPackCtrl::ListPageUp()
{
	if (!m_pBackPack || !m_pBackPack->GetSize()) {
		m_listOffset = 0;
		return;
	}
	
	if (m_pBackPack->GetSize() > m_cellsCount) {
		if (m_listOffset == 0) SetListOffset(m_pBackPack->GetSize() - 1);
		else SetListOffset(m_listOffset - 1);
	}
}

void iArtBackPackCtrl::ListPageDown()
{
	if (!m_pBackPack || !m_pBackPack->GetSize()) {
		m_listOffset = 0;
		return;
	}
	
	if (m_pBackPack->GetSize() > m_cellsCount) {
		if (m_listOffset == ((sint32)m_pBackPack->GetSize()-1)) SetListOffset(0);
		else SetListOffset(m_listOffset + 1);
	}
}

void iArtBackPackCtrl::SetListOffset(sint32 offs)
{
	if (m_pBackPack) {
		m_listOffset = iCLAMP<sint32>(0,m_pBackPack->GetSize()-1, offs);
	} else m_listOffset = 0;
}


sint32 iArtBackPackCtrl::BackPackIdxByCell(sint32 cellIdx) const
{
	if (!m_pBackPack || cellIdx == -1 || cellIdx >= (sint32)m_pBackPack->GetSize()) return -1;
	return (m_listOffset + cellIdx) % m_pBackPack->GetSize();
}



iArtCell iArtBackPackCtrl::Remove(sint32 cell)
{
	check(m_pBackPack);
	sint32 bpidx = BackPackIdxByCell(cell);
	check(bpidx != -1);
	iArtCell result(AC_UNDEFINED, m_pBackPack->At(bpidx));
	m_pBackPack->RemoveAt(bpidx);
#ifdef MULTIPLAYER	
	gMPMgr.OnHeroUpdate(m_pOwner);
#endif

	return result;
}


iArtCell iArtBackPackCtrl::Replace(sint32 cell, const iArtCell& newArt)
{
	check(m_pBackPack);
	sint32 bpidx = BackPackIdxByCell(cell);
	check(bpidx != -1);
	iArtCell result(AC_UNDEFINED, m_pBackPack->At(bpidx));
	m_pBackPack->At(bpidx) = newArt.artId;
#ifdef MULTIPLAYER	
	gMPMgr.OnHeroUpdate(m_pOwner);
#endif

	return result;
}
/*
 *	simple Artifact list control
 */
iArtListCtrl::iArtListCtrl(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iArtList& artList, const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags, uint32 uid, bool fullSize, iSize maxSize)
: iBaseCtrl(pViewMgr, pCmdHandler, iRect(), GENERIC_VIEWPORT, uid, Visible|Enabled) 
, m_flags(flags)
, m_cellSize(cellSiz)
, m_cellsCount(cellsCount)
, m_artList(artList)
, m_listOffset(0)
, m_dCell(-1)
, m_clickItem(-1)
, m_bFullSize(fullSize)
{
	if(m_bFullSize) {
		if (flags & Horizontal) {
			m_Rect = iRect(pos, iSize(cellSiz.w * cellsCount, cellSiz.h));
		} else {
			m_Rect = iRect(pos, iSize(cellSiz.w, cellSiz.h * cellsCount));
		}
	} else {	
		if (flags & Horizontal) {
			m_Rect = iRect(pos, iSize(iMIN<>(cellSiz.w * cellsCount, maxSize.w) + iIphoneBtn::sm_defSize.w * 2, iMIN<>(maxSize.h, cellSiz.h)));
			m_cellsCount = (m_Rect.w - iIphoneBtn::sm_defSize.w * 2) / cellSiz.w;

			AddChild(new iImgBtn(&gApp.ViewMgr(), this, iRect(iPoint(), iIphoneBtn::sm_defSize), 101, PDGG(BTN_SWITCH_CASTLES), PDGG(BTN_SWITCH_CASTLES) + 2));
			AddChild(new iImgBtn(&gApp.ViewMgr(), this, iRect(iPoint(m_Rect.w-iIphoneBtn::sm_defSize.w, 0), iIphoneBtn::sm_defSize), 102, PDGG(BTN_SWITCH_CASTLES) + 1, PDGG(BTN_SWITCH_CASTLES) + 3));
		} else {
			m_Rect = iRect(pos, iSize(iMIN<>(cellSiz.w, maxSize.w), iMIN<>(maxSize.h * cellsCount, cellSiz.h)));
	
			AddChild(new iImgBtn(&gApp.ViewMgr(), this, iRect(0,0,m_Rect.w, iIphoneBtn::sm_defSize.h), 101, PDGG(BTN_SWITCH_CASTLES), PDGG(BTN_SWITCH_CASTLES) + 2));
			AddChild(new iImgBtn(&gApp.ViewMgr(), this, iRect(0,m_Rect.h-iIphoneBtn::sm_defSize.h,m_Rect.w,iIphoneBtn::sm_defSize.h), 102, PDGG(BTN_SWITCH_CASTLES) + 1, PDGG(BTN_SWITCH_CASTLES) + 3));
		}	
	}
}
void iArtListCtrl::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == 101) ListPageUp();
	else if (uid == 102) ListPageDown();
}

void iArtListCtrl::ListPageUp()
{
	if (!m_artList.GetSize()) {
		m_listOffset = 0;
		return;
	}

	if (m_artList.GetSize() > m_cellsCount) {
		if (m_listOffset == 0) SetListOffset(m_artList.GetSize() - 1);
		else SetListOffset(m_listOffset - 1);
	}
}

void iArtListCtrl::ListPageDown()
{
	if (!m_artList.GetSize()) {
		m_listOffset = 0;
		return;
	}

	if (m_artList.GetSize() > m_cellsCount) {
		if (m_listOffset == ((sint32)m_artList.GetSize()-1)) SetListOffset(0);
		else SetListOffset(m_listOffset + 1);
	}
}

void iArtListCtrl::SetListOffset(sint32 offs)
{
	m_listOffset = iCLAMP<sint32>(0,m_artList.GetSize()-1, offs);
}

sint32 iArtListCtrl::CellByPos(const iPoint& pos) const
{
	sint32 val;
	if (m_flags & Horizontal) {
		val = (pos.x - GetScrRect().x - iIphoneBtn::sm_defSize.w) / (sint32)(m_cellSize.w+1);
	} else {
		val = (pos.y - GetScrRect().y - iIphoneBtn::sm_defSize.h) / (sint32)(m_cellSize.h+1);
	}

	if (val<0 || val >= (sint32)m_cellsCount) return -1;
	return val;
}

sint32 iArtListCtrl::BackPackIdxByCell(sint32 cellIdx) const
{
	if (cellIdx == -1 || cellIdx >= (sint32)m_artList.GetSize()) return -1;
	return (m_listOffset + cellIdx) % m_artList.GetSize();
}

void iArtListCtrl::OnCompose()
{
	iRect rc = GetScrRect();
	iRect crc(rc.x, rc.y, m_cellSize.w, m_cellSize.h);

	if (m_flags & Horizontal) crc.x += iIphoneBtn::sm_defSize.w + 1;
    else crc.y += iIphoneBtn::sm_defSize.h + 1;

	// Compose cells
	for (uint32 xx=0; xx<m_cellsCount; ++xx) {
		sint32 bpidx = BackPackIdxByCell(xx);
		ComposeArtifactCell(crc, (bpidx==-1)?INVART:m_artList[bpidx], ((sint32)xx==m_dCell)?ABF_PRESSED:0, false);
		if (m_flags & Horizontal) crc.x += m_cellSize.w + 1;
		else crc.y += m_cellSize.h + 1;
	}
}

void iArtListCtrl::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	sint32 ncell = CellByPos(pos);
	if (ncell == -1 || BackPackIdxByCell(ncell) == -1) return;
	m_dCell = ncell;
	Invalidate();
}

void iArtListCtrl::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	sint32 ncell = CellByPos(pos);
	if (m_dCell != -1 && m_dCell == ncell) m_clickItem = m_dCell;
	m_dCell = -1;
	Invalidate();
}

void iArtListCtrl::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if (m_clickItem != -1) {
		sint32 bpIdx = BackPackIdxByCell(m_clickItem);
		if (bpIdx != -1) {
			iDlg_ArtInfo dlg(m_pMgr, m_artList[bpIdx], NULL);
			dlg.DoModal();
		}
		m_clickItem = -1;
	}
}

