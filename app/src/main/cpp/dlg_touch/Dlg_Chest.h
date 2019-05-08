#ifndef PHEROES_GAME_DLG_CHEST_H_
#define PHEROES_GAME_DLG_CHEST_H_

class iChestDlgBtn;
/*
 *	
 */
class iDlg_Chest : public iBaseGameDlg
{
public:
	enum Selection {
		Gold = 100,
		Experience
	};
public:
	iDlg_Chest(iViewMgr* pViewMgr, iHero* pHero, iMapItem_Chest* pChest);

	inline iHero* Hero() { return m_pHero; }
	inline sint32 GoldCount() const	{ return m_cntGold; }
	inline sint32 ExpCount() const { return m_cntExp; }
	inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif
private:
	iHero*			m_pHero;
	sint32			m_cntExp;
	sint32			m_cntGold;
    iChestDlgBtn*   m_pExpBtn;
    iChestDlgBtn*   m_pGoldBtn;
};



#endif //PHEROES_GAME_DLG_CHEST_H_
