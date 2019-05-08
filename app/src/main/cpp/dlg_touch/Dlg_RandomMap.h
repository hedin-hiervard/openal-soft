#ifndef PHEROES_GAME_DLG_RANDOMMAP_H_
#define PHEROES_GAME_DLG_RANDOMMAP_H_
class iRandomMapDlg : public iBaseGameDlg
{
public:
	iRandomMapDlg(iViewMgr* pViewMgr);
	~iRandomMapDlg();
	inline const iRandomMapGenerator::iRandomMapParam& SelParam() const { return m_selParam; }
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
private:
	iRandomMapGenerator::iRandomMapParam m_selParam;
};

#endif //PHEROES_GAME_DLG_RANDOMMAP_H_
