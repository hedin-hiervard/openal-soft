#ifndef PHEROES_GAME_DLG_SPLITCREATURES_STYLUS_H_
#define PHEROES_GAME_DLG_SPLITCREATURES_STYLUS_H_

class iDlg_SplitCreatures : public iBaseGameDlg
{
public:
	enum SplitFlags {
		Normal	= 0,
		SafeG1	= 0x1,
		SafeG2	= 0x2
	};
	
public:
	iDlg_SplitCreatures(iViewMgr* pViewMgr, PLAYER_ID pid, iArmyOwner* own1, sint32 idx1, iArmyOwner* own2, sint32 idx2, uint32 flags );

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	uint32			m_flags;
	uint32			m_total;
	uint32			m_left;
	iArmyOwner		*m_own1, *m_own2;
	sint32			m_idx1, m_idx2;
	iPHScrollBar*	m_pSlider;
};


#endif //PHEROES_GAME_DLG_SPLITCREATURES_STYLUS_H_
