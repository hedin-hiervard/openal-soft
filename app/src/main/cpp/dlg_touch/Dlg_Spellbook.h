#ifndef PHEROES_GAME_DLG_SPELLBOOK_H_
#define PHEROES_GAME_DLG_SPELLBOOK_H_

class iSpellBookItem: public iButton
{
public:
	iSpellBookItem(iViewMgr *pViewMgr, IViewCmdHandler* pHandler, iRect &rect, uint32 uid, iHero* pOwner);
	inline void SetSpell(MAGIC_SPELL spell) { m_spell = spell; Invalidate(); }
	void OnCompose();
	inline MAGIC_SPELL Spell() const { return m_spell; }
	inline void SetOwner(iHero *owner) { m_pOwner = owner; }
private:
	MAGIC_SPELL m_spell;
	iHero*		m_pOwner;
};

class iSpellBookView: public iView, public IViewCmdHandler
{
public:
	iSpellBookView(iViewMgr* pViewMgr, IViewCmdHandler* cmdHandler, const iRect& rect, uint32 uid, iHero* owner, uint32 typeMask, 
					SPELL_SET_MODES ssm, uint32 startingPage, bool bLandOnly);
	void UpdateSpells();
	void UpdateSize();
	void NextPage();
	void PrevPage();
	void BuildSpellList();
	
	void OnSwipeLeft(sint32 length);
	void OnSwipeRight(sint32 length);
	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void SetOwner(iHero *owner);
	virtual inline bool AlwaysReceiveMouse() { return true; }
public:
	static iSize sm_defSize;
private:
	void OnCompose();
	void SetHero( iHero* pHero );
private:
	static const iRect sm_area_air, sm_area_fire, sm_area_water, sm_area_earth, sm_area_all,
		sm_area_prev, sm_area_next;
	iHero*			m_pOwner;
	iSortArray<uint16>	m_spells;
	iSpellBookItem*	m_items[4];
	uint32			m_page;
	bool			m_bGestureComplete;
	IViewCmdHandler* m_pCmdHandler;
	uint32			m_typeMask;
	SPELL_SET_MODES m_ssm;	
	bool			m_bLandOnly;
	iButton				*m_pPrevPageBtn;  ///< The pointer previous page button
	iButton				*m_pNextPageBtn;  ///< The pointer next page button
};

class iDlg_Spellbook : public iDialog, public IViewCmdHandler
{
public:
	iDlg_Spellbook(iViewMgr* pViewMgr, iHero* pOwner, uint32 typeMask, bool bLandOnly);
	MAGIC_SPELL SelSpell() const;
	virtual inline bool AlwaysReceiveMouse() { return true; }
#if defined( PC_VERSION )	
	void OnClickNo() { EndDialog(DRC_CANCEL); }
	void OnClickYes() { EndDialog(DRC_CANCEL); }
	void OnDown() { EndDialog(DRC_CANCEL); }
#endif
private:
	void OnCreateDlg();
	virtual iSize GetDlgMetrics() const;
	iSize ClientSize() const;
	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	inline bool CanCastSelSpell() const	{ return m_selItem != MSP_INVALID && m_pOwner->ManaPts() >= iBaseSpell::GetSpellCost(m_selItem, m_pOwner); }
	void OnCompose();
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
private:
	iSpellBookView* m_spellbook;
	iHero*			m_pOwner;
	MAGIC_SPELL		m_selItem;
};

#endif //PHEROES_GAME_DLG_SPELLBOOK_H_
