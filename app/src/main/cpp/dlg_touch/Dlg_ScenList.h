#ifndef PHEROES_GAME_DLG_SCENLIST_H_
#define PHEROES_GAME_DLG_SCENLIST_H_



#include "Ctl_ScenList.h"

/////////////////////////////////////////////////////////////////////////
class iScenListDlg : public iBaseGameDlg
{
public:
    iScenListDlg(iViewMgr* pViewMgr, uint32 filter);
    ~iScenListDlg();
    const iMapInfo& SelScen() const;
private:
    void OnCreateDlg();
    void DoCompose(const iRect& rect);
    iSize ClientSize() const;
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);    
private:
    iScenListView *m_ctrl;
    uint32         m_filter;
};


#endif //PHEROES_GAME_DLG_SCENLIST_H_
