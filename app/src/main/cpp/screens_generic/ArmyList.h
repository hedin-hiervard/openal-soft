#ifndef PHEROES_GAME_ARMYLIST_H_
#define PHEROES_GAME_ARMYLIST_H_

/*
 *	Extendet Army List control (with drag'n'drop capabilities)
 */
class iArmyListEx : public iBaseCtrl
{
public:
	iArmyListEx(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, iCheckButton* pSplitBtn, const iSize& itemSiz, uint32 uid);
	void AddCompetitor(iArmyListEx* pCompetitor);
		
	//
	inline void SetSplitBtn(iCheckButton* pSplitBtn) { m_pSplitBtn = pSplitBtn; }
	void SetSplitMode(bool bSplit);
	void ToggleSplitMode();
	inline bool IsSplitMode() const { return m_bSplit; }
	void SetArmy(iArmyOwner* pOwner, bool bCanDismiss);
	inline const iArmy* Army() { return &m_pOwner->Army(); }
	inline iArmyOwner* ArmyOwner() { return m_pOwner; }

protected:
	virtual void OnCompose() {};
	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {};
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID) {};
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID) {};
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID) {};
	virtual void OnMouseEntered(const iPoint& pos, MouseId mID) {}
	virtual void OnMouseLeave(const iPoint& pos, MouseId mID) {}
	inline sint8 Pos2Cell(const iPoint& pos) const { return iMIN<sint8>((sint8)((pos.x-GetScrRect().x) / m_itemW),6);}

protected:
	typedef iSimpleArray<iArmyListEx*> iCompList;

	bool			m_bCanDismiss;
	bool			m_bSplit;
	sint8			m_dCell;	
	iArmyOwner*		m_pOwner;
	sint32			m_itemW;
	uint32			m_lastClick;

	iCompList		m_Competitors;
	iCheckButton*	m_pSplitBtn;
};



#endif //PHEROES_GAME_ARMYLIST_H_
