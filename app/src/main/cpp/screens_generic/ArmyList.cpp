#include "stdafx.h"
#include "Dlg_SplitCreatures.touch.h"
#include "Dlg_CreatInfo.h"



/*
 *	Extended Army List constrol (with drag'n'drop capabilities)
 */
iArmyListEx::iArmyListEx(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, iCheckButton* pSplitBtn, const iSize& itemSiz, uint32 uid)
: iBaseCtrl(pViewMgr,pCmdHandler,iRect(pos.x,pos.y,(itemSiz.w+3)*7,itemSiz.h * 2),GENERIC_VIEWPORT,uid, Visible|Enabled)
, m_itemW(itemSiz.w), m_dCell(-1), m_pOwner(NULL), m_bCanDismiss(false), m_lastClick(0), m_bSplit(false), m_pSplitBtn(pSplitBtn)
{
	AddCompetitor(this);
}

//
void iArmyListEx::AddCompetitor(iArmyListEx* pCompetitor)
{
	m_Competitors.Add(pCompetitor);
}

//
void iArmyListEx::SetSplitMode(bool bSplit)
{
	m_bSplit = bSplit;
}

void iArmyListEx::ToggleSplitMode()
{
	SetSplitMode(!m_bSplit);
}

void iArmyListEx::SetArmy(iArmyOwner* pOwner, bool bCanDismiss)
{	
	m_pOwner = pOwner;
	m_bCanDismiss = bCanDismiss;
	m_dCell = -1;
	Invalidate();
}


