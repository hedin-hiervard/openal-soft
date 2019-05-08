#ifndef PHEROES_GAME_SPELLLISTVIEW_H_
#define PHEROES_GAME_SPELLLISTVIEW_H_

class iSpellListBox;
class iSpellListView : public iView, public IViewCmdHandler
{
public:
	iSpellListView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, iHero* pOwner, bool bShowFavorites);

	void SetOwner(iHero* pOwner);
	inline iHero* Owner() { return m_pOwner; }
	inline MAGIC_SPELL SelSpell() const { return m_curSel; }
	inline bool CanCastSelSpell() const	{ return m_curSel != MSP_INVALID && m_pOwner->ManaPts() >= iBaseSpell::GetSpellCost(m_curSel, m_pOwner); }
	inline void SetMask(uint32 typeMask) { m_typeMask = typeMask; UpdateSpellList(); }
	void SelNext();
	void SelPrev();

private:
	void UpdateSpellList();
	void OnCompose();
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	iSpellListBox*		m_pSpellListBox;
	iSpellBtn*			m_pSpellButton;
	iBarTabSwitch*		m_pSchoolSwitch;
	uint32				m_typeMask;
	MAGIC_SPELL			m_curSel;
	iHero*				m_pOwner;
	IViewCmdHandler*	m_pCmdHandler;
};


#endif //PHEROES_GAME_SPELLLISTVIEW_H_
