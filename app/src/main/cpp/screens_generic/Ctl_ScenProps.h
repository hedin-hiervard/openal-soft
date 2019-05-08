#ifndef PHEROES_GAME_CTL_SCENPROPS_H_
#define PHEROES_GAME_CTL_SCENPROPS_H_

/*
*	Scenario List dialog
*/
class iPlayerBtn : public iButton
{
public:
    iPlayerBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, PLAYER_ID pid, PLAYER_TYPE pt, uint32 uid, uint32 state = Visible|Enabled)
        : iButton(pViewMgr, pCmdHandler, rect, uid, state), m_pid(pid), m_pt(pt) {}

    void SetData(PLAYER_ID pid, PLAYER_TYPE pt)
    {
        m_pid = pid; m_pt = pt;
        Invalidate();
    }

    void OnCompose();

    inline PLAYER_TYPE PlayerType() const { return m_pt; }
    inline PLAYER_TYPE& PlayerType() { return m_pt; }
    inline PLAYER_TYPE TogglePlayerType() { return (m_pt==PT_HUMAN)?m_pt=PT_COMPUTER:m_pt=PT_HUMAN; }

private:
    PLAYER_ID	m_pid;
    PLAYER_TYPE	m_pt;
};
//////////////////////////////////////////////////////////////////////////
class iNationBtn : public iButton, public IViewCmdHandler
{
public:
    iNationBtn(iViewMgr* pViewMgr, const iRect& rect, PLAYER_ID pid, CTL_TYPE nt, uint32 uid, bool bFixed)
        : iButton(pViewMgr, this, rect, uid, (bFixed)?(Visible):(Visible|Enabled)), m_pid(pid), m_nt(nt), m_bFixed(bFixed) {}

    void SetData(PLAYER_ID pid, CTL_TYPE nt, bool bFixed)
    {
        m_pid = pid;
        m_nt = nt;
        m_bFixed = bFixed;
        Invalidate();
    }

    inline CTL_TYPE PlayerNation() const { return m_nt; }
    inline void SetNation(const CTL_TYPE nt) { m_nt = nt; }

private:
    void OnCompose();

    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        if (!m_bFixed && (cmd == CCI_BTNCLICK || cmd == CCI_BTNDBLCLICK)) {
            if (m_nt == CTLT_COUNT-1) m_nt = (CTL_TYPE)0;
            else m_nt = (CTL_TYPE)(m_nt+1);
        }
    }


private:
    bool		m_bFixed;
    PLAYER_ID	m_pid;
    CTL_TYPE	m_nt;
};

class iDifLvlTab;
class iScenPropsView : public iView, public IViewCmdHandler
{
public:
    iScenPropsView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
    //inline const iMapInfo& ScenProps() const { return m_scProps; }
    const iMapInfo& ScenProps();    
    void SetData(iMapInfo& scProps);
private:
    void OnCompose();    
    void UpdateControls();
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
    static iStringT GetDfcString(DIFFICULTY_LEVEL dl);

private:
    iSimpleArray<iPlayerBtn*>	m_btnPlayers;
    iSimpleArray<iNationBtn*>	m_btnNations;
    iDifLvlTab*					m_difLevel;
    iMapInfo					m_scProps;
    IViewCmdHandler*            m_pCmdHandler; 
};


#endif //PHEROES_GAME_CTL_SCENPROPS_H_
