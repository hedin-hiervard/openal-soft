#ifndef PHEROES_GAME_DLG_SPLITCREATURES_TOUCH_H_
#define PHEROES_GAME_DLG_SPLITCREATURES_TOUCH_H_

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
#if defined( PC_VERSION )
	void OnClickNo() {EndDialog(DRC_CANCEL);}
	void OnClickYes();
#endif
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#endif
    void OnMouseWheelDown(sint16 line, MouseId mID);
    bool IsScrolling() {return true;}
private:
	uint32			m_flags;
	uint32			m_total;
	uint32			m_left;
	iArmyOwner		*m_own1, *m_own2;
	sint32			m_idx1, m_idx2;
	iAmountChooserGlow* m_pAmount;
    iButton*    m_pBtnOk;
    iButton*    m_pBtnCancel;
};


#endif //PHEROES_GAME_DLG_SPLITCREATURES_TOUCH_H_
