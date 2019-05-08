#ifndef PHEROES_GAME_HERO_SPELLS_STYLUS_H_
#define PHEROES_GAME_HERO_SPELLS_STYLUS_H_
#pragma once

class iSpellBookTab_stylus: public iSpellBookTab
{
public:
	iSpellBookTab_stylus(iViewMgr* pViewMgr);		
	void OnHeroChanged();
	inline iSpellListView* iSpellBookTab_stylus::SpellList() { return m_pSpellListView; }
private:	
	iSpellListView*	m_pSpellListView;
};
#endif //PHEROES_GAME_HERO_SPELLS_STYLUS_H_
