#include "stdafx.h"
#include "ArtifactList.h"
#include "ArtifactList.touch.h"
#include "Dlg_ArtInfo.h"

/*
*	iArtClickItem
*/
iArtClickItem::iArtClickItem(iSimpleArray<iArtClickItem*>& competitors, const iSize& cellSiz, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iBaseCtrl(pViewMgr,pCmdHandler, rect, GENERIC_VIEWPORT, uid, Visible|Enabled)
, m_cellSiz(cellSiz)
, m_Competitors(competitors)
, m_selectedItem(-1)
{
	m_Competitors.Add(this);
}


void ComposeArtifactCell(const iRect& rect, uint16 artId, uint32 flags)
{
	iRect rc(rect);	
	
	sint16 sid = PDGG(NEWHERO_ARTIFACTS_CELL);
	gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, rc, false, AlignCenter));

	if (artId != INVART) BlitIcon(gApp.Surface(), gGame.ItemMgr().m_artMgr[artId].Sprite(), rect);
	if(flags & ABF_PRESSED) {
		sint16 sid = PDGG(BTN_FRAME_PRESSED);
		gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, rc, false, AlignCenter));
	}
}

iArtItemCtrl_touch::iArtItemCtrl_touch(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 uid)
: iArtClickItem(competitors, cellSiz, pViewMgr, pCmdHandler, iRect(pos, cellSiz) , uid)
{}

void iArtItemCtrl_touch::OnCompose()
{
	iRect rc = GetScrRect();
	//gApp.Surface().FrameRect(rc, 0);
	ComposeArtifactCell(rc, (m_pArtCell)?m_pArtCell->artId:0xFFFF, (m_selectedItem != -1 || m_bHighlighted)?ABF_PRESSED:0);	
}

void iArtItemCtrl_touch::Unhighlight() 
{
	m_bHighlighted = false;
	Invalidate();
}


void iArtItemCtrl_touch::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
}

void iArtItemCtrl_touch::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
}

void iArtItemCtrl_touch::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
		if(m_Competitors[xx]->m_selectedItem != -1) {
			sint32 fcell = m_Competitors[xx]->m_selectedItem;
			if (m_Competitors[xx] == this) {
				// (same cell) do nothing
				m_selectedItem = -1;
				for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
					m_Competitors[yy]->Unhighlight();
				Invalidate();
				return;
			} else {
				if(gGame.ItemMgr().m_artMgr[m_Competitors[xx]->ArtId()].Assign() == m_pArtCell->AssignType() && 
					m_pOwner->Artifacts().CanAttach(m_Competitors[xx]->ArtId())) {

						if (m_pArtCell->artId != INVART) 
							Replace(0, m_Competitors[xx]->Replace(fcell, *m_pArtCell));
						else 
							Replace(0, m_Competitors[xx]->Remove(fcell));
				}
			}
			m_selectedItem = -1;
			m_Competitors[xx]->m_selectedItem = -1;
			for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
				m_Competitors[yy]->Unhighlight();
			Invalidate();
			return;	
		}
	}

	if(m_selectedItem != -1) {
		m_selectedItem = -1;
		for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
			m_Competitors[yy]->Unhighlight();
	}
	else if(m_pArtCell->artId != INVART) {
		m_selectedItem = 0;
		for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx)
			m_Competitors[xx]->Highlight(this, m_pArtCell->artId);
	}
	Invalidate();
}


void iArtItemCtrl_touch::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(m_pArtCell->artId == INVART) return;
	iDlg_ArtInfo dlg(m_pMgr, m_pArtCell->artId, m_pOwner);
	dlg.DoModal();
	m_selectedItem = -1;
	for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
		m_Competitors[yy]->Unhighlight();

	Invalidate();
}

void iArtItemCtrl_touch::Highlight(iArtClickItem* pSource, uint16 artId) 
{
	m_bHighlighted = ( gGame.ItemMgr().m_artMgr[artId].Assign() == m_pArtCell->AssignType() && 
		m_pOwner->Artifacts().CanAttach(artId));
	Invalidate();
}

inline iRect BackPackListMetrics_Touch(const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags)
{
	if(flags & iArtBackPackCtrl::Horizontal)
		return iRect(pos.x, pos.y, (cellSiz.w+1)*cellsCount-1 + 52,cellSiz.h);
	else
		return iRect(pos.x, pos.y, cellSiz.w,cellSiz.h * cellsCount + cellsCount + 1 + 31);
}


iArtBackPackCtrl_touch::iArtBackPackCtrl_touch(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags, uint32 uid)
: iArtClickItem(competitors, cellSiz, pViewMgr,pCmdHandler,BackPackListMetrics_Touch(pos, cellSiz, cellsCount, flags), uid),
iArtBackPackCtrl(cellSiz, cellsCount, flags)
, m_lastClickedItem(-1)
{
	if (flags & Horizontal) {
		AddChild(new iImgBtn(pViewMgr, this, iRect(0,0,25,37), 101, PDGG(BTN_SCROLL_ARTS_LEFT), -1));
		AddChild(new iImgBtn(pViewMgr, this, iRect(25,0,25,37), 102, PDGG(BTN_SCROLL_ARTS_RIGHT), -1));
	} else {
		AddChild(new iImgBtn(pViewMgr, this, iRect(0,m_Rect.h-30,32,15), 101, PDGG(BTN_SCROLL_ARTS_SMALL_UP), -1));
		AddChild(new iImgBtn(pViewMgr, this, iRect(0,m_Rect.h-15,32,15), 102, PDGG(BTN_SCROLL_ARTS_SMALL_DOWN), -1));
	}	
}

void iArtBackPackCtrl_touch::ListPageUp()
{
	m_selectedItem = -1;
	iArtBackPackCtrl::ListPageUp();	
}

void iArtBackPackCtrl_touch::ListPageDown()
{
	m_selectedItem = -1;
	iArtBackPackCtrl::ListPageDown();	
}

sint32 iArtBackPackCtrl_touch::CellByPos(const iPoint& pos) const
{
	sint32 val;
	if (m_flags & Horizontal) {
		val = (pos.x - GetScrRect().x - 52) / (sint32)(m_cellSize.w+1);
	} else {
		val = (pos.y - GetScrRect().y) / (sint32)(m_cellSize.h+1);
	}	

	if (val<0 || val >= (sint32)m_cellsCount) return -1;
	return val;
}

void iArtBackPackCtrl_touch::OnCompose()
{
	iRect rc = GetScrRect();
	iRect crc(rc.x, rc.y, m_cellSize.w, m_cellSize.h);

	//overall background
	if(m_flags & Horizontal) {
		crc.x += 50 + 2;
		rc.DeflateRect(52, 0, 0, 0);
		ComposeDlgBkgndGrid(gApp.Surface(), rc, gGame.Map().ActPlayer()->PlayerId(), false, iSize(m_cellsCount, 1));
	}
	//	else
	//		ComposeDlgBkgndGrid(gApp.Surface(), rc, gGame.Map().ActPlayer()->PlayerId(), false, iSize(1, m_cellsCount));


	// Compose cells
	for (uint32 xx=0; xx<m_cellsCount; ++xx) {
		sint32 bpidx = BackPackIdxByCell(xx);
		ComposeArtifactCell(crc, (bpidx==-1)?INVART:m_pBackPack->At(bpidx), ((sint32)xx==m_selectedItem)?ABF_PRESSED:0, !(m_flags & Horizontal));		
		if (m_flags & Horizontal) crc.x += m_cellSize.w + 1;
		else crc.y += m_cellSize.h + 1;
	}
}


void iArtBackPackCtrl_touch::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
}

void iArtBackPackCtrl_touch::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(!m_bIsDoubleClicking)
		m_lastClickedItem = CellByPos(pos);

	for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
		if(m_Competitors[xx]->m_selectedItem != -1) {
			sint32 frombpidx = m_Competitors[xx]->m_selectedItem;
			sint32 tobpidx = CellByPos(pos);
			if (m_Competitors[xx] == this && tobpidx != -1) {
				if (tobpidx != frombpidx && BackPackIdxByCell(tobpidx) != -1) {
					iSwap(m_pBackPack->At(BackPackIdxByCell(frombpidx)), m_pBackPack->At(BackPackIdxByCell(tobpidx)));
				}
			} else {
				if (tobpidx == -1) 
					m_pBackPack->Add(m_Competitors[xx]->Remove(frombpidx).artId);
				else {
					sint32 idx = BackPackIdxByCell(tobpidx);
					if(idx == -1)
						m_pBackPack->Add(m_Competitors[xx]->Remove(frombpidx).artId);
					else
						m_pBackPack->InsertBefore(idx, m_Competitors[xx]->Remove(frombpidx).artId);
				}
			}				
			m_selectedItem = -1;
			m_Competitors[xx]->m_selectedItem = -1;
			for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
				m_Competitors[yy]->Unhighlight();
			Invalidate();
			return;	
		}
	}

	if(m_selectedItem != -1) {
		m_selectedItem = -1;
		for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
			m_Competitors[yy]->Unhighlight();
	}
	else if(BackPackIdxByCell(CellByPos(pos)) != -1) {
		m_selectedItem = CellByPos(pos);
		for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx)
			m_Competitors[xx]->Highlight(this, ArtId());
	}
	Invalidate();	
}

void iArtBackPackCtrl_touch::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(CellByPos(pos) != m_lastClickedItem) return;

	sint32 bpIdx = BackPackIdxByCell(CellByPos(pos));
	if (bpIdx != -1) {
		iDlg_ArtInfo dlg(m_pMgr, m_pBackPack->At(bpIdx), m_pOwner);
		dlg.DoModal();
		m_selectedItem = -1;
		for (uint32 yy=0; yy<m_Competitors.GetSize(); ++yy)
			m_Competitors[yy]->Unhighlight();
		Invalidate();
	}
}

void iArtBackPackCtrl_touch::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
}
