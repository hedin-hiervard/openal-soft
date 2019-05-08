#include "stdafx.h"
#include "ArtifactList.h"
#include "ArtifactList.stylus.h"
#include "Dlg_ArtInfo.h"

const sint32 ARIFACT_DRAG_DELTA = 5;
const uint32 STYL_SCROLL_BTN_SIZE = 15;

/*
 *	DragItem
 */
iArtDragDropItem::iDragItem::iDragItem(iArtDragDropItem* pSender, sint32 fromCell,const iArtCell& artCell, const iPoint& pos, const iSize& siz)
: m_pSender(pSender), m_fromCell(fromCell), m_artCell(artCell), m_pos(pos), m_siz(siz), m_pReceiver(NULL), m_toCell(-1)
{}

void iArtDragDropItem::iDragItem::ComposeDragGlyph()
{
	iRect rc(m_pos.x-(sint32)(m_siz.w/2),m_pos.y-(sint32)(m_siz.h/2),m_siz.w,m_siz.h);
	ComposeArtifactCell(rc, m_artCell.artId, 0, false);
	if (!m_pReceiver || m_toCell == -1) gApp.Surface().FillRect(rc,cColor_Red,32);
	else gApp.Surface().FillRect(rc,cColor_Green,32);
}


/*
 *	iArtDragDropItem
 */
iArtDragDropItem::iArtDragDropItem(iSimpleArray<iArtDragDropItem*>& competitors, const iSize& cellSiz, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iBaseCtrl(pViewMgr,pCmdHandler, rect, GENERIC_VIEWPORT, uid, Visible|Enabled)
, m_cellSiz(cellSiz)
, m_Competitors(competitors)
, m_pDragItem(NULL), m_pDropItem(NULL)
{
	m_Competitors.Add(this);
}

void iArtDragDropItem::BeginDrag(sint32 fromCell, const iArtCell& artCell, const iPoint& pos)
{
	check(!m_pDragItem);
	m_pDragItem = new iDragItem(this, fromCell, artCell, pos, m_cellSiz);
	m_pMgr->SetDragGlyph(m_pDragItem);
	Drag(pos);
	for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
		m_Competitors[xx]->Highlight(m_pDragItem);
	}	
}

void iArtDragDropItem::EndDrag()
{
	check(m_pDragItem);
	if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->Drop();
	m_pMgr->SetDragGlyph(NULL);
	delete m_pDragItem;
	m_pDragItem = NULL;
	for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
		m_Competitors[xx]->Unhighlight();
	}	
	Invalidate();
}

void iArtDragDropItem::Drag(const iPoint& pos)
{
	check(m_pDragItem);
	m_pDragItem->m_pos = pos;
	for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
		if (m_Competitors[xx]->GetScrRect().PtInRect(pos)) {
			if (m_pDragItem->m_pReceiver != m_Competitors[xx]) {
				if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->LeaveDrop();
				m_Competitors[xx]->EnterDrop(m_pDragItem);
			}
			if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->DragDrop();
			Invalidate();
			return;
		}
	}
	if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->LeaveDrop();
	Invalidate();
}



iArtItemCtrl_stylus::iArtItemCtrl_stylus(iSimpleArray<iArtDragDropItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 uid)
: iArtItemCtrl(), iArtDragDropItem(competitors, cellSiz, pViewMgr, pCmdHandler, iRect(), uid) 
{}

void iArtItemCtrl_stylus::OnCompose()
{
	iRect rc = GetScrRect();
	ComposeArtifactCell(rc, (m_pArtCell)?m_pArtCell->artId:0xFFFF, (m_bPressed)?ABF_PRESSED:0, false, true);
	if (m_pDragItem) gApp.Surface().FrameRect(rc,cColor_Blue,64);
	if (m_bHighlighted) gApp.Surface().FrameRect(rc,cColor_Yello,64);
}

void iArtItemCtrl_stylus::Unhighlight() 
{
	m_bHighlighted = false;
	Invalidate();
}


void iArtItemCtrl_stylus::OnMouseDown(const iPoint& pos, MouseId mID)
{
	if (!m_pArtCell || m_pArtCell->artId == INVART) return;
	m_bPressed = true;
	m_dragAnchor = pos;
	m_bClickMode = true;
	Invalidate();
}

void iArtItemCtrl_stylus::OnMouseUp(const iPoint& pos, MouseId mID)
{
	if (m_pDragItem) {
		EndDrag();
	}
	m_bPressed = false;
	Invalidate();
}

void iArtItemCtrl_stylus::OnMouseClick(const iPoint& pos, MouseId mID)
{
	if (m_bClickMode) {
		iDlg_ArtInfo dlg(m_pMgr, m_pArtCell->artId, m_pOwner);
		dlg.DoModal();
		m_bClickMode = false;
	}
}

void iArtItemCtrl_stylus::OnMouseTrack(const iPoint& pos, MouseId mID)
{
	if (m_pDragItem) {
		Drag(pos);
	} else if (m_pArtCell && !m_pArtCell->Empty() && pos.GetSqDelta(m_dragAnchor) > ARIFACT_DRAG_DELTA) {
		m_bClickMode = false;
		BeginDrag(0, *m_pArtCell, pos);
		m_bPressed = false;
	}
}

void iArtItemCtrl_stylus::EnterDrop(iDragItem* pDragItem)
{
	check(!m_pDropItem);
	m_pDropItem = pDragItem;
	m_pDropItem->m_pReceiver = this;
	m_pDropItem->m_toCell = -1;
	Invalidate();
}

void iArtItemCtrl_stylus::LeaveDrop()
{
	check(m_pDropItem);
	m_pDropItem->m_pReceiver = NULL;
	m_pDropItem->m_toCell = -1;
	m_pDropItem = NULL;
	Invalidate();
}

bool iArtItemCtrl_stylus::Drop()
{
	if (m_pDropItem->m_toCell == -1) {
		LeaveDrop();
		Invalidate();
		return false;
	}

	if (m_pDropItem->m_pSender == m_pDropItem->m_pReceiver) {
		// (same cell) do nothing
	} else {
		if (m_pArtCell->artId != INVART) iArtItemCtrl::Replace(0, m_pDropItem->m_pSender->Replace(m_pDropItem->m_fromCell, *m_pArtCell));
		else iArtItemCtrl::Replace(0, m_pDropItem->m_pSender->Remove(m_pDropItem->m_fromCell));
	}
#ifdef MULTIPLAYER	
	gMPMgr.OnHeroUpdate(m_pOwner);
#endif

	LeaveDrop();
	Invalidate();	
	return true;
}


bool iArtItemCtrl_stylus::DragDrop() const
{
	if ( gGame.ItemMgr().m_artMgr[m_pDropItem->m_artCell.artId].Assign() == m_pArtCell->AssignType() && m_pOwner->Artifacts().CanAttach(m_pDropItem->m_artCell.artId)) m_pDropItem->m_toCell = 0;
	else m_pDropItem->m_toCell = -1;
	return true;
}

void iArtItemCtrl_stylus::Highlight(iDragItem *dr) 
{
	m_bHighlighted = ( gGame.ItemMgr().m_artMgr[dr->m_artCell.artId].Assign() == m_pArtCell->AssignType() && 
					  m_pOwner->Artifacts().CanAttach(dr->m_artCell.artId));
	Invalidate();
}


inline iRect BackPackListMetrics(const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags)
{
	uint32 w = cellSiz.w;
	uint32 h = cellSiz.h;

	if (flags & iArtBackPackCtrl_stylus::Horizontal) {
		w = STYL_SCROLL_BTN_SIZE * 2 + cellSiz.w * cellsCount + cellsCount + 1;
	} else {
		h = STYL_SCROLL_BTN_SIZE * 2 + cellSiz.h * cellsCount + cellsCount + 1;
	}
	return iRect(pos.x, pos.y, w, h);
}


iArtBackPackCtrl_stylus::iArtBackPackCtrl_stylus(iSimpleArray<iArtDragDropItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags, uint32 uid)
: iArtDragDropItem(competitors, cellSiz, pViewMgr,pCmdHandler,BackPackListMetrics(pos, cellSiz, cellsCount, flags), uid),
  iArtBackPackCtrl(cellSiz, cellsCount, flags)
{
	if (flags & Horizontal) {
		AddChild(new iIconButton(pViewMgr, this, iRect(0,0,STYL_SCROLL_BTN_SIZE, m_Rect.h),PDGG_SCRBAR_BTNS+2,101));
		AddChild(new iIconButton(pViewMgr, this, iRect(m_Rect.w-STYL_SCROLL_BTN_SIZE,0,STYL_SCROLL_BTN_SIZE, m_Rect.h),PDGG_SCRBAR_BTNS+3,102));
	} else {
		AddChild(new iIconButton(pViewMgr, this, iRect(0,0,m_Rect.w,STYL_SCROLL_BTN_SIZE),PDGG_SCRBAR_BTNS,101));
		AddChild(new iIconButton(pViewMgr, this, iRect(0,m_Rect.h-STYL_SCROLL_BTN_SIZE,m_Rect.w,STYL_SCROLL_BTN_SIZE),PDGG_SCRBAR_BTNS+1,102));
	}
}

sint32 iArtBackPackCtrl_stylus::CellByPos(const iPoint& pos) const
{
	sint32 val;
	if (m_flags & Horizontal) {
		val = (pos.x - GetScrRect().x - STYL_SCROLL_BTN_SIZE) / (sint32)(m_cellSize.w+1);
	} else {
		val = (pos.y - GetScrRect().y - STYL_SCROLL_BTN_SIZE) / (sint32)(m_cellSize.h+1);
	}

	if (val<0 || val >= (sint32)m_cellsCount) return -1;
	return val;
}


void iArtBackPackCtrl_stylus::OnCompose()
{
	iRect rc = GetScrRect();
	iRect crc(rc.x, rc.y, m_cellSize.w, m_cellSize.h);

	// Draw frame arround up/down buttons
	for (uint32 ctrlId=0; ctrlId<m_Childs.GetSize(); ++ctrlId) {
		iRect ctrlRc = m_Childs[ctrlId]->GetScrRect();
		ctrlRc.InflateRect(1);
		gApp.Surface().FrameRect(ctrlRc,cColor_Black);
	}
	if (m_flags & Horizontal) crc.x += STYL_SCROLL_BTN_SIZE + 1;
	else crc.y += STYL_SCROLL_BTN_SIZE + 1;

	// Compose cells
	for (uint32 xx=0; xx<m_cellsCount; ++xx) {
		sint32 bpidx = BackPackIdxByCell(xx);
		ComposeArtifactCell(crc, (bpidx==-1)?INVART:m_pBackPack->At(bpidx), ((sint32)xx==m_dCell)?ABF_PRESSED:0, true/*!(m_flags & Horizontal)*/);
		if (m_flags & Horizontal) crc.x += m_cellSize.w + 1;
		else crc.y += m_cellSize.h + 1;
	}
}

void iArtBackPackCtrl_stylus::OnMouseDown(const iPoint& pos, MouseId mID)
{
	if (!m_pBackPack) return;
	sint32 ncell = CellByPos(pos);
	if (ncell == -1 || BackPackIdxByCell(ncell) == -1) return;
	m_dCell = ncell;
	m_dragAnchor = pos;
	m_bClickMode = true;
	Invalidate();
}


void iArtBackPackCtrl_stylus::OnMouseUp(const iPoint& pos, MouseId mID)
{
	if (m_pDragItem) {
		EndDrag();
	}
	m_dCell = -1;
	Invalidate();
}

void iArtBackPackCtrl_stylus::OnMouseClick(const iPoint& pos, MouseId mID)
{
	if (m_bClickMode) {
		sint32 bpIdx = BackPackIdxByCell(CellByPos(pos));
		if (bpIdx != -1) {
			iDlg_ArtInfo dlg(m_pMgr, m_pBackPack->At(bpIdx), m_pOwner);
			dlg.DoModal();
		}
		m_bClickMode = false;
	}
}


void iArtBackPackCtrl_stylus::OnMouseTrack(const iPoint& pos, MouseId mID)
{
	if (m_pDragItem) {
		Drag(pos);
	} else if (m_pBackPack && m_dCell != -1 && pos.GetSqDelta(m_dragAnchor) > ARIFACT_DRAG_DELTA) {
		sint32 bpidx = BackPackIdxByCell(m_dCell);
		if (bpidx != -1) {
			iArtCell ac(AC_UNDEFINED, m_pBackPack->At(bpidx));
			m_bClickMode = false;
			BeginDrag(m_dCell, ac, pos);
		}
		m_dCell = -1;
	}
}

void iArtBackPackCtrl_stylus::EnterDrop(iDragItem* pDragItem)
{
	check(!m_pDropItem);
	m_pDropItem = pDragItem;
	m_pDropItem->m_pReceiver = this;
	m_pDropItem->m_toCell = -1;
	Invalidate();
}

void iArtBackPackCtrl_stylus::LeaveDrop()
{
	check(m_pDropItem);
	m_pDropItem->m_pReceiver = NULL;
	m_pDropItem->m_toCell = -1;
	m_pDropItem = NULL;
	Invalidate();
}

bool iArtBackPackCtrl_stylus::Drop()
{
	if (m_pDropItem->m_toCell == -1) {
		LeaveDrop();
		Invalidate();
		return false;
	}

	sint32 tobpidx = BackPackIdxByCell(m_pDropItem->m_toCell);
	if (m_pDropItem->m_pSender == m_pDropItem->m_pReceiver && tobpidx != -1) {
		sint32 frombpidx = BackPackIdxByCell(m_pDropItem->m_fromCell);
		if (tobpidx != frombpidx) {
			iSwap(m_pBackPack->At(frombpidx), m_pBackPack->At(tobpidx));
		}
	} else {
		if (tobpidx == -1) m_pBackPack->Add(m_pDropItem->m_pSender->Remove(m_pDropItem->m_fromCell).artId);
		else m_pBackPack->InsertBefore(tobpidx, m_pDropItem->m_pSender->Remove(m_pDropItem->m_fromCell).artId);
	}
#ifdef MULTIPLAYER	
	gMPMgr.OnHeroUpdate(m_pOwner);
#endif

	LeaveDrop();
	Invalidate();
	return true;
}

bool iArtBackPackCtrl_stylus::DragDrop() const
{
	m_pDropItem->m_toCell = CellByPos(m_pDropItem->m_pos);
	return true;
}
