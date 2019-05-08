#ifndef PHEROES_GAME_DLG_CHOOSEBATTLEGROUP_H_
#define PHEROES_GAME_DLG_CHOOSEBATTLEGROUP_H_

class iDlg_ChooseBattleGroup:	public iBaseGameDlg
{
public:
	iDlg_ChooseBattleGroup(iViewMgr* pViewMgr, iSimpleArray<iBattleGroup *> list, iSimpleArray<iStringT> texts, PLAYER_ID pid, uint32* idx);
	~iDlg_ChooseBattleGroup(void) {}	
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void DoCompose(const iRect& rect);

	iSize ClientSize() const;

	void OnCreateDlg();
private:
	class iBattleGroupBtn : public iButton
	{
		public:
			iBattleGroupBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, uint32 uid, iBattleGroup *grp, iStringT text);			
		private:
			void OnCompose();			
	private:			
			iBattleGroup *m_group;
			iStringT m_text;			
	};
#ifdef PC_VERSION	
    void UpdateSize();
#endif
	iSimpleArray<iBattleGroup *> m_list;
	iSimpleArray<iStringT> m_texts;	
	uint32 *m_idx;	

	
	iSimpleArray<iBattleGroupBtn*> m_buttons;
    iButton* m_pBtnCancel;
};
#endif //PHEROES_GAME_DLG_CHOOSEBATTLEGROUP_H_
