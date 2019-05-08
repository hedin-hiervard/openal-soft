#include "stdafx.h"
#include "HeroView.h"
#include "HeroView.stylus.h"
#include "SpellListView.h"
#include "Hero_Spells.h"
#include "Hero_Spells.stylus.h"

iSpellBookTab_stylus::iSpellBookTab_stylus(iViewMgr* pViewMgr) : iSpellBookTab(pViewMgr)
{
	iRect clRect(5,8, m_Rect.w-10, m_Rect.h-16);
	AddChild(m_pSpellListView = new iSpellListView(pViewMgr, NULL, clRect, 100, NULL, false));
	m_pSpellListView->SetMask(SPT_MASK_ALL);
}


void iSpellBookTab_stylus::OnHeroChanged() 
{
	m_pSpellListView->SetOwner(m_pHero);
}
