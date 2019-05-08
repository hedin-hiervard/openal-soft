#ifndef PHEROES_GAME_DLG_SCENPROPS_H_
#define PHEROES_GAME_DLG_SCENPROPS_H_

#include "Ctl_ScenProps.h"

class iScenPropsDlg : public iBaseGameDlg
{
public:
    iScenPropsDlg(iViewMgr* pViewMgr, iMapInfo& scProps);
    inline const iMapInfo& ScenProps() const;    
private:
    void OnCreateDlg();
    void DoCompose(const iRect& rect);
    iSize ClientSize() const;

    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
private:
    iMapInfo&					m_scProps;
    iScenPropsView              *m_ctrl;
};



#endif //PHEROES_GAME_DLG_SCENPROPS_H_
