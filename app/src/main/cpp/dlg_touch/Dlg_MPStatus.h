#ifndef PHEROES_GAME_DLG_MPSTATUS_H_
#define PHEROES_GAME_DLG_MPSTATUS_H_
class iMPStatusDlg: public iTextDlg {
public:
	iMPStatusDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid, iTextComposer::FontConfig fc_title = dlgfc_hdr, iTextComposer::FontConfig fc_text = dlgfc_plain);
	void Show(bool cancelBtn = false);
	void Hide();
	void SetPercent(sint32 percent);
	void OnCompose();
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
	void OnCreateDlg();
protected:
	iSize ClientSize() const;
private:
	sint32 m_percent;
	iButton *m_pCancelBtn;
};
#endif //PHEROES_GAME_DLG_MPSTATUS_H_
