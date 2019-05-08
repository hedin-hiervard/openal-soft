#ifndef PHEROES_GAME_DLG_LEVEL_H_
#define PHEROES_GAME_DLG_LEVEL_H_

class iLevelDlgBtn;
/*
 *	
 */
class iDlg_NewLevel : public iBaseGameDlg
{
public:
	iDlg_NewLevel(iViewMgr* pViewMgr, iHero* pHero, const iNewLevelInfo& linfo);
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);
	// inlines
	inline iHero* Hero() { return m_pHero; }
	inline PRSKILL_TYPE PrimSkill() const { return m_linfo.prSkillType;}
	inline SECONDARY_SKILLS SecSkill() const { return m_selSecSkill; }
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif    
private:
	SECONDARY_SKILLS	m_selSecSkill;
	iNewLevelInfo		m_linfo;
	iHero*				m_pHero;
	iImgBtn*			m_pTweetBtn;
    iLevelDlgBtn*       m_pDlgBtnLeft;
    iLevelDlgBtn*       m_pDlgBtnRight;
	bool				m_bNochoice;
};



#endif //PHEROES_GAME_DLG_LEVEL_H_
