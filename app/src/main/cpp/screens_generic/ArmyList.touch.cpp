#include "stdafx.h"
#include "Dlg_SplitCreatures.touch.h"
#include "Dlg_CreatInfo.h"
#include "ArmyList.touch.h"

iRect iArmyListEx_touch::GetCellRect( uint32 idx ) const
{
	iRect rc = GetScrRect();
	iRect res = iRect(rc.x + rc.w * idx / 7, rc.y, rc.w / 7, rc.h);
	res.DeflateRect((7-idx) / 2, 3, (idx) / 3, 2);
	
	return res;
}

iRect iArmyListEx_touch::GetCellVisualRect( uint32 idx ) const
{
	iRect rc = GetScrRect();

    iRect res = iRect(rc.x + rc.w * idx / 7
#ifndef PC_VERSION
                      -1
#endif
                      , rc.y + GetVisualOffset(), rc.w / 7, rc.h / 2);
#ifndef PC_VERSION
    if(m_bGUI)
#endif
		//res.DeflateRect((7-idx) / 2, 3, (idx) / 3, 2);
		res.DeflateRect((7-idx) / 2 - 1, 3, 2, 2); //  [|     |][     ][     ][     ][     ][     ][     ]
//		res.InflateRect(0,0,4,0);
#ifndef PC_VERSION
	else
        res.DeflateRect(0, 3, 3, 0);
    //res.DeflateRect(0, 2, 3, 1);
#endif

	return res;
}

sint32 iArmyListEx_touch::GetVisualOffset() const
{
	iRect rc = GetScrRect();
	switch(m_gt) {
		case GT_TOP: 
			return rc.h / 2;		
		case GT_MIDDLE:
			return rc.h / 4;			
		case GT_BOTTOM:
			return 0;			
	}
	return 0;
}

void iArmyListEx_touch::OnCompose()
{
	iRect rc = GetScrRect();

	//gApp.Surface().FrameRect(rc, cColor_White);
	
	
	if(m_bGUI)
    	ComposeDlgBkgndGrid(gApp.Surface(), iRect(rc.x, rc.y + GetVisualOffset(), rc.w, rc.h / 2), gGame.Map().ActPlayer()->PlayerId(), false, iSize(7, 1));		
	for (sint32 xx=0; xx<7; ++xx) {
		/*iRect tmp = GetCellRect(xx);
		gApp.Surface().FillRect(tmp, xx * 100, 64);
		*/
		
		if (m_pOwner) 
			ComposeCreatureButton(gApp.Surface(), GetCellVisualRect(xx), m_pOwner->Army().At(xx).Type(), m_pOwner->Army().At(xx).Count(), VL_EXPERT, m_selectedItem == xx || m_highlighedCell == xx, false, false);
		else 
			ComposeCreatureButton(gApp.Surface(), GetCellVisualRect(xx), CREAT_UNKNOWN, 0, VL_EXPERT, m_selectedItem == xx || m_highlighedCell == xx, false, false);
		 
	}
#ifndef OS_IPHONE	
	if(!gApp.ViewMgr().HasModalDlg() && m_pOwner && m_pOwner->ChangedCell() != -1) {
		BlinkCell(m_pOwner->ChangedCell());
		m_pOwner->ResetChangedCell();
	}
#endif
}

void iArmyListEx_touch::BlinkCell(sint32 idx)
{
	AniQID q = gAniHost.AddAniObj(new iGAniObj_ChangeValDisc<sint32>(fix32(0.5f), m_highlighedCell, idx));
	gAniHost.AddAniObjToQ(new iGAniObj_ChangeValDisc<sint32>(fix32(0.5f), m_highlighedCell, -1), q);																	
	gAniHost.AddAniObjToQ(new iGAniObj_ChangeValDisc<sint32>(fix32(0.5f), m_highlighedCell, idx), q);	
	gAniHost.AddAniObjToQ(new iGAniObj_ChangeValDisc<sint32>(fix32(0.5f), m_highlighedCell, -1), q);				
 
	//gAniHost.Finish(q);
}

void iArmyListEx_touch::OnMouseEntered(const iPoint& pos, MouseId mID){

}

void iArmyListEx_touch::OnMouseLeave(const iPoint& pos, MouseId mID){

}

void iArmyListEx_touch::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	if(m_bInfoMode) {
		sint32 nv = Pos2Cell(pos);
		if(nv != -1)
			ShowInfo(nv);
		return;
	}

	// the algorithm follows:
	// 1. check all the competitors. if one has selected item, perform the action
	if(!m_pOwner)
		return;
	sint32 cell = Pos2Cell(pos);
	if(cell == -1) return;
	if(!m_bIsDoubleClicking)
		m_lastClickedItem = cell;
		

	for (uint32 xx=0; xx<m_Competitors.GetSize(); ++xx) {
		iArmyListEx_touch* comp = (iArmyListEx_touch*)m_Competitors[xx];
		if (comp->m_selectedItem != -1) {
			sint32 ccell = comp->m_selectedItem;
			
			// clicked the same cell as selected
			if(comp == this && cell == ccell) {
				if(m_selectedItem == cell)
					m_selectedItem = -1;
				Invalidate();
				m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selectedItem);
				return;
			}

			// clicked the army of the same type
			else if (m_pOwner->Army().At(cell).Type() == comp->m_pOwner->Army().At(ccell).Type()) {
			
				bool bSplit = m_pSplitBtn && m_pSplitBtn->IsChecked();
				if(this != comp && !comp->ArmyOwner()->CanBeEmpty() && comp->Army()->GroupCount() <= 1) bSplit = true;

				if(bSplit && comp->ArmyOwner()->Army().At(ccell).Count() <= 1) return;

				if (bSplit) {
					// split mode (show split dialog)
					uint32 flags = iDlg_SplitCreatures::Normal;
					if (this != comp){ // if transferring between different armies, special rules apply
						if (!comp->ArmyOwner()->CanBeEmpty() && comp->Army()->GroupCount() <= 1)
							flags |= iDlg_SplitCreatures::SafeG1;
						if (!m_pOwner->CanBeEmpty() && m_pOwner->Army().GroupCount() <= 1) 
							flags |= iDlg_SplitCreatures::SafeG2;
					}					
					iDlg_SplitCreatures scdlg(m_pMgr, PID_NEUTRAL, comp->ArmyOwner(), ccell, m_pOwner, cell, flags);
					scdlg.DoModal();
					m_selectedItem = -1;
					comp->m_selectedItem = -1;
					if(m_pSplitBtn)
						m_pSplitBtn->SetChecked(false);
					m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selectedItem);
					return;
				} else {					
					m_pOwner->AddArmyGroupCount(cell, comp->Army()->At(ccell).Count());
					comp->ArmyOwner()->ResetArmyGroup(ccell);
					//}
					m_selectedItem = -1;
					comp->m_selectedItem = -1;
					Invalidate();
					m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selectedItem);
					return;
				}	

			// click the army of the other type OR empty space
			} else {
				bool bSplit = m_pSplitBtn && m_pSplitBtn->IsChecked();

				// if moving the last unit of non-empty owner, always split
				if(this != comp && !comp->ArmyOwner()->CanBeEmpty() && comp->Army()->GroupCount() <= 1 && m_pOwner->Army().At(cell).Count() == 0) 
					bSplit = true;
				else {
					 // else if moving less than 1 creatures, don't split
					if(comp->ArmyOwner()->Army().At(ccell).Count() <= 1) 
						bSplit = false;
				}

				if (bSplit) {
					// show split dialog if moving >1 creatures to empty space
					if(m_pOwner->Army().At(cell).Count() == 0 && comp->Army()->At(ccell).Count() > 1) {
						uint32 flags = iDlg_SplitCreatures::Normal;
						if (this != comp && comp->Army()->GroupCount() <= 1 && !comp->ArmyOwner()->CanBeEmpty()) 
							flags |= iDlg_SplitCreatures::SafeG1;
						iDlg_SplitCreatures scdlg(m_pMgr, PID_NEUTRAL, comp->ArmyOwner(), ccell, m_pOwner, cell, flags);
						scdlg.DoModal();

						m_selectedItem = -1;
						comp->m_selectedItem = -1;
						if(m_pSplitBtn)
							m_pSplitBtn->SetChecked(false);						
					} else { // otherwise, cancel operation and deselect cells
						comp->m_selectedItem = -1;						
						m_selectedItem = -1;					
					}

					Invalidate();
					m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selectedItem);
					return;
				} else {
					// swap cells			

					// if both are not empty and swap is prohibited, select other clicked and return
					if(m_pOwner->Army().At(cell).Type() != CREAT_UNKNOWN && comp->Army()->At(ccell).Type() != CREAT_UNKNOWN && m_bSwapProhibited) {
						comp->m_selectedItem = -1;
						m_selectedItem = cell;						
						m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selectedItem);
						return;
					}
					m_pOwner->SwapArmyGroups(cell, ccell, comp->ArmyOwner());						
					m_selectedItem = -1;
					comp->m_selectedItem = -1;
					Invalidate();
					m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selectedItem);
					return;
				}
			}
		}
	}
	
	// 2. if no, select or deselect the item
	if(m_selectedItem == cell)
		m_selectedItem = -1;
	else if(m_pOwner && m_pOwner->Army().At(cell).Count() > 0)
		m_selectedItem = cell;
	Invalidate();
	m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selectedItem);
}

void iArmyListEx_touch::ShowInfo(sint32 idx)
{
	if (m_pOwner->Army().At(idx).Type() != CREAT_UNKNOWN) {
		iFurtSkills fs;
		iHero* pHero = m_pOwner->GetArmyOwnerHero();
		if (pHero) 
			fs = pHero->GetFullFurtSkills();
		else 
			fs.Reset();
		iCreatBonusList cbl;
		if(pHero)
			pHero->Artifacts().HasCreatBonus(m_pOwner->Army().At(idx).Type(), cbl);
		iDlg_CreatInfo dlg(m_pMgr, PID_NEUTRAL, m_pOwner, idx, fs, m_bCanDismiss &&  (m_pOwner->CanBeEmpty() || m_pOwner->Army().GroupCount()>1), m_pOwner->Army().MoraleModifier(), cbl);		
		dlg.DoModal();
		m_selectedItem = -1;
	}	
}

void iArmyListEx_touch::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	sint8 nv = Pos2Cell(pos);
	if(nv == -1) return;	
	if(nv != m_lastClickedItem) return;	

	if(m_pSplitBtn && m_pSplitBtn->IsChecked()) return;

	ShowInfo(nv);
	m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNDBLCLICK, 0);
}	

sint8 iArmyListEx_touch::Pos2Cell( const iPoint& pos ) const
{
	for(uint32 xx=0; xx<7; xx++)
		if(GetCellRect(xx).PtInRect(pos))
			return xx;

	return -1;
}

iRect iArmyListEx_touch::GetVisualRect()
{	
	return iRect(m_Rect.x, m_Rect.y + GetVisualOffset(), m_Rect.w, m_Rect.h - GetVisualOffset() - 1);
}



#ifdef PC_VERSION
iSize iArmyListEx_touch::sm_defCellSize = iSize(52, 36);
#else
iSize iArmyListEx_touch::sm_defCellSize = iSize(50, 36);
#endif