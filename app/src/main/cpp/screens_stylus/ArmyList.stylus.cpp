#include "stdafx.h"
#include "Dlg_SplitCreatures.h"
#include "Dlg_CreatInfo.h"

#include "ArmyList.stylus.h"

const sint32 DRAG_DELTA = 5;

/*
 *	DragItem
 */
iArmyListEx_stylus::iDragItem::iDragItem(iArmyListEx_stylus* pSender, sint8 fromCell, bool bSplit, const iCreatGroup& cGroup, const iPoint& pos, const iSize& siz, bool bCanDismiss)
: m_pSender(pSender), m_fromCell(fromCell), m_bSplit(bSplit), m_cGroup(cGroup), m_pos(pos), m_siz(siz), m_bCanDismiss(bCanDismiss), m_pReceiver(NULL), m_toCell(-1)
{}

void iArmyListEx_stylus::iDragItem::ComposeDragGlyph()
{
	iRect rc(m_pos.x-(sint32)(m_siz.w/2),m_pos.y-(sint32)(m_siz.h/2),m_siz.w,m_siz.h);
	ComposeCreatureCell(gApp.Surface(),rc,m_cGroup.Type(),m_cGroup.Count(),VL_EXPERT);
	if (!m_pReceiver || m_toCell == -1) gApp.Surface().FillRect(rc,cColor_Red,32);
	else gApp.Surface().FillRect(rc,cColor_Green,32);
}


void iArmyListEx_stylus::OnCompose()
{
	iRect rc = GetScrRect();

	
	iRect cMetrix(rc.x,rc.y,m_itemW,rc.h);
	for (sint32 xx=0; xx<7; ++xx) {
		gApp.Surface().FrameRect(iRect(cMetrix.x-1,cMetrix.y-1,cMetrix.w+2,cMetrix.h+2), cColor_Black);
		if (m_pOwner) ComposeCreatureButton(gApp.Surface(), cMetrix, m_pOwner->Army().At(xx).Type(), m_pOwner->Army().At(xx).Count(), VL_EXPERT, m_dCell == xx, (m_pDragItem && xx == m_pDragItem->m_fromCell), (m_pDropItem && xx == m_pDropItem->m_toCell));
		else ComposeCreatureButton(gApp.Surface(), cMetrix, CREAT_UNKNOWN, 0, VL_EXPERT, m_dCell == xx, false,false);
		cMetrix.x += (cMetrix.w+1);
	}
}

void iArmyListEx_stylus::OnMouseUp(const iPoint& pos, MouseId mID)
{
	if (m_pDragItem) {
		EndDrag();
	}
}


void iArmyListEx_stylus::OnMouseClick(const iPoint& pos, MouseId mID)
{
	if (m_pDragItem) return;
	if (m_pOwner && m_dCell != -1 && Pos2Cell(pos) == m_dCell) {
		uint32 nt = GetTickCount();
		sint32 nv = m_dCell;
		m_dCell = -1;
		Invalidate();
		if ( m_lastClick && nt > m_lastClick && (nt-m_lastClick) < 500) {
			m_lastClick = 0;
			m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNDBLCLICK,nv);
			if (m_pOwner->Army().At(nv).Type() != CREAT_UNKNOWN) {
				iFurtSkills fs;
				iHero* pHero = m_pOwner->GetArmyOwnerHero();
				if (pHero) 
					fs = pHero->GetFullFurtSkills();
				else 
					fs.Reset();
				iCreatBonusList cbl;
				if(pHero)
					pHero->Artifacts().HasCreatBonus(m_pOwner->Army().At(nv).Type(), cbl);
				iDlg_CreatInfo dlg(m_pMgr, PID_NEUTRAL, m_pOwner, nv, fs, m_bCanDismiss || m_pOwner->Army().GroupCount()>1, m_pOwner->Army().MoraleModifier(), cbl);
				dlg.DoModal();
			}
		} else {
			m_lastClick = nt;
			m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,nv);
		}
	}
}

void iArmyListEx_stylus::OnMouseTrack(const iPoint& pos, MouseId mID)
{
	if (m_pDragItem) {
		Drag(pos);
	} else if (m_pOwner && pos.GetSqDelta(m_dragAnchor) > DRAG_DELTA && m_dCell != -1 && m_pOwner->Army().At(m_dCell).IsValid()) {
		BeginDrag(m_dCell, m_pOwner->Army().At(m_dCell), pos);
		m_dCell = -1;
	}
}

void iArmyListEx_stylus::OnMouseDown(const iPoint& pos, MouseId mID)
{
	if (!m_pOwner) return;
	sint8 ncell = Pos2Cell(pos);
	if (!m_pOwner->Army().At(ncell).IsValid()) return;
	m_dCell = ncell;
	m_dragAnchor = pos;
	Invalidate();
}


void iArmyListEx_stylus::BeginDrag(sint8 fromCell, const iCreatGroup& cGroup, const iPoint& pos)
{
	check(!m_pDragItem);
	m_pDragItem = new iDragItem(this, fromCell, (m_bSplit || (m_pSplitBtn && m_pSplitBtn->IsChecked())), cGroup, pos, iSize(m_itemW,m_Rect.h), m_bCanDismiss || m_pOwner->Army().GroupCount() > 1);
	m_pMgr->SetDragGlyph(m_pDragItem);
	Drag(pos);
}
void iArmyListEx_stylus::EndDrag()
{
	check(m_pDragItem);
	check(m_pOwner);
	m_pMgr->SetDragGlyph(NULL);
	if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->Drop();
	delete m_pDragItem;
	m_pDragItem = NULL;
	if (m_pSplitBtn) m_pSplitBtn->SetChecked(false);
	Invalidate();
}

void iArmyListEx_stylus::Drag(const iPoint& pos)
{
	check(m_pDragItem);
	m_pDragItem->m_pos = pos;
	for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
		if (m_Competitors[xx]->GetScrRect().PtInRect(pos)) {
			if (m_pDragItem->m_pReceiver != m_Competitors[xx]) {
				if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->LeaveDrop();
				((iArmyListEx_stylus*)m_Competitors[xx])->EnterDrop(m_pDragItem);
			}
			if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->DragDrop();
			Invalidate();
			return;
		}
	}
	if (m_pDragItem->m_pReceiver) m_pDragItem->m_pReceiver->LeaveDrop();
	Invalidate();
}

void iArmyListEx_stylus::EnterDrop(iDragItem* pDragItem)
{
	if (!m_pOwner) return;
	check(!m_pDropItem);
	m_pDropItem = pDragItem;
	m_pDropItem->m_pReceiver = this;
	m_pDropItem->m_toCell = -1;
	Invalidate();
}

void iArmyListEx_stylus::LeaveDrop()
{
	check(m_pDropItem);
	m_pDropItem->m_pReceiver = NULL;
	m_pDropItem->m_toCell = -1;
	m_pDropItem = NULL;
	Invalidate();
}

bool iArmyListEx_stylus::Drop()
{
	if (m_pDropItem->m_toCell == -1) {
		LeaveDrop();
		Invalidate();
		return false;
	}

	if (m_pDropItem->m_pSender == m_pDropItem->m_pReceiver && m_pDropItem->m_fromCell == m_pDropItem->m_toCell) {
		// (same cell) do nothing
	} else if (m_pOwner->Army().At(m_pDropItem->m_toCell).Type() == m_pDropItem->m_cGroup.Type()) {
		// (similar type of creatures)
		if (m_pDropItem->m_bSplit) {
			// split mode (show split dialog)
			uint32 flags = iDlg_SplitCreatures::Normal;
			if (m_pDropItem != m_pDragItem){
				if (!m_pDropItem->m_bCanDismiss && m_pDropItem->m_pSender->Army()->GroupCount() <=1) flags |= iDlg_SplitCreatures::SafeG1;
				if (!m_bCanDismiss && m_pOwner->Army().GroupCount() <= 1) flags |= iDlg_SplitCreatures::SafeG2;
			}
			iDlg_SplitCreatures scdlg(m_pMgr, PID_NEUTRAL, m_pDropItem->m_pSender->ArmyOwner(), m_pDropItem->m_fromCell, m_pOwner, m_pDropItem->m_toCell, flags);
			scdlg.DoModal();
		} else {
			// normal mode (merge groups)			
			m_pOwner->SetArmyGroupCount(m_pDropItem->m_toCell, m_pOwner->Army().At(m_pDropItem->m_toCell).Count() + m_pDropItem->m_cGroup.Count());
			m_pDropItem->m_pSender->ArmyOwner()->ResetArmyGroup(m_pDropItem->m_fromCell);
		}
	} else {
		if (m_pDropItem->m_bSplit) {
			// show split dialog
			uint32 flags = iDlg_SplitCreatures::Normal;
			if (m_pDropItem != m_pDragItem && !m_pDropItem->m_bCanDismiss && m_pDropItem->m_pSender->Army()->GroupCount() <= 1) flags |= iDlg_SplitCreatures::SafeG1;
			iDlg_SplitCreatures scdlg(m_pMgr, PID_NEUTRAL, m_pDropItem->m_pSender->ArmyOwner(), m_pDropItem->m_fromCell, m_pOwner, m_pDropItem->m_toCell, flags);
			scdlg.DoModal();
		} else {
			// swap cells
			m_pOwner->SwapArmyGroups(m_pDropItem->m_toCell, m_pDropItem->m_fromCell, m_pDropItem->m_pSender->m_pOwner);			
		}
	}	
	LeaveDrop();
	Invalidate();
	return true;
}

bool iArmyListEx_stylus::DragDrop() const
{
	sint8 toCell = Pos2Cell(m_pDropItem->m_pos);
	if (m_pDropItem->m_bSplit) {
		if (toCell != -1) {
			const iCreatGroup& fGroup = m_pDropItem->m_pSender->Army()->At(m_pDropItem->m_fromCell);
			const iCreatGroup& tGroup = m_pOwner->Army().At(toCell);
			if (!tGroup.IsValid() || tGroup.Type() == fGroup.Type()) m_pDropItem->m_toCell = toCell;
		}
	} else {
		if (toCell == -1 
			|| m_pDropItem->m_bCanDismiss 
			|| m_pDropItem == m_pDragItem 
			|| (m_pOwner->Army().At(toCell).IsValid() && m_pOwner->Army().At(toCell).Type() != m_pDropItem->m_pSender->Army()->At(m_pDropItem->m_fromCell).Type())) 
			m_pDropItem->m_toCell = toCell;
		else
			m_pDropItem->m_toCell = -1;
	}
	return true;
}
