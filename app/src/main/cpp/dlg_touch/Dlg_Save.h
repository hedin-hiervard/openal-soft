#ifndef PHEROES_GAME_DLG_SAVE_H_
#define PHEROES_GAME_DLG_SAVE_H_

#include "FileAccessor/FileAccessor.h"

#include "Ctl_Save.h"

/////////////////////////////////////////////////////////////////////////
class iSaveDlg : public iBaseGameDlg
{
public:
	iSaveDlg(iViewMgr* pViewMgr, bool bSave);
	~iSaveDlg();
    static void GetSaveFileName(fileaccessor::RelativeFilePath& path, uint32 slot);
    bool SelFile(fileaccessor::RelativeFilePath& path) const;
    const iMapInfo& SelScenario() const;
	iSaveGameView* SaveGameView() { return( m_ctrl ); }
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
    iSaveGameView *m_ctrl;
    bool		m_bSave;
};



#endif //PHEROES_GAME_DLG_SAVE_H_
