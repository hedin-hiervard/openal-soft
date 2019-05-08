#include "stdafx.h"
#include "CastleView.h"
#include "CastleView.stylus.h"
#include "OverlandView.h"
#include "OverlandView.stylus.h"

#include "Ctl_DockPage.h"
#include "Ctl_CnstPage.h"
#include "Ctl_RecrPage.h"
#include "Ctl_MGuildPage.h"
#include "Ctl_MarketPage.h"
#include "Ctl_TavernPage.h"

#include "Ctl_CnstPage.stylus.h"
#include "Ctl_RecrPage.stylus.h"
#include "Ctl_MarketPage.stylus.h"
#include "Ctl_DockPage.stylus.h"

iCastleView_stylus::iCastleView_stylus(): iCastleView()
{	
	AddChild(new iInfoBar(&gApp.ViewMgr(), this, iRect(0,0,m_Rect.w,15))); // 5	


	AddChild(m_pChilds[0] = new iCnstTab_stylus(&gApp.ViewMgr(), this)); // 0
	AddChild(m_pChilds[1] = new iRecrTab_stylus(&gApp.ViewMgr()));
	AddChild(m_pChilds[2] = new iMGuildTab(&gApp.ViewMgr(), false));
	AddChild(m_pChilds[3] = new iMarketTab_stylus(&gApp.ViewMgr(), false));
	AddChild(m_pChilds[4] = new iTavernTab(&gApp.ViewMgr())); // 4	
	for (uint32 xx=0; xx<CVT_COUNT; ++xx) m_pChilds[xx]->SetCastle(m_pCastle);
	ShowPage(CVT_CONSTRUCT);
}

bool iCastleView_stylus::OnKeyDown(sint32 key)
{
	if (key == KEY_UP || key == KEY_DOWN) {
		if ( m_curTab == CVT_CONSTRUCT) {
			iCnstTab* pCnstTab = (iCnstTab*)m_pChilds[CVT_CONSTRUCT];
			if (key == KEY_UP) pCnstTab->CnstList()->SelPrev();
			else if (key == KEY_DOWN) pCnstTab->CnstList()->SelNext();
		}
	} else if (key == KEY_LEFT){
		PrevTab();
	} else if (key == KEY_RIGHT){
		NextTab();
	} else {
		return false;
	}
	return true;
}

void iCastleView_stylus::OnCompose()
{
	gApp.Surface().FillRect(GetScrRect(),RGB16(32,64,128));
}

