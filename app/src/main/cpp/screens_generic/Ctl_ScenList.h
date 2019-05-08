#ifndef PHEROES_GAME_CTL_SCENLIST_H_
#define PHEROES_GAME_CTL_SCENLIST_H_

//////////////////////////////////////////////////////////////////////////


/*
*	Scenario List dialog
*/

#define SCL_SINGLE   (1 << 1)
#define SCL_MULTI    (1 << 2)
#define SCL_TUTORIAL (1 << 3)
#define SCL_HOTSEAT  (1 << 4)



class iScenListBox;

class iScenListView: public iView, public IViewCmdHandler
{
public:
    enum ShowFilter 
    {
        ShowFilter_ALL = 0,
        ShowFilter_ADV,
        ShowFilter_ACT,
        ShowFilter_MULTI
    };

    enum SortBy {
        Size = 0,
        RevSize,        
        Name,
        RevName,
        Players,        
        RevPlayers,
        Author,
        RevAuthor
    };

    iScenListView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 filter) ;
    ~iScenListView();
    inline const iMapInfo& SelScen() const { return *m_scList[m_selScen]; }
    
    void LoadData();
    void SetShowFilter(ShowFilter sf) { m_ShowFilter = sf; LoadData(); };
	ShowFilter GetShowFilter() { return m_ShowFilter; }
    uint32 GetMapCount(ShowFilter sf);
	virtual bool Process(fix32 t);

private:
    
    void CheckHeaderButton(uint32 id);
    void SortScenarios(SortBy sort_by);
    void OnCompose();
    void SendResult(uint32 res);
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
    virtual bool KeyDown(sint32 key);
    virtual bool KeyUp(sint32 key);    
private:
    fix32		m_scrollCounter;
    uint32		m_scrollMask;
    iScenListBox* m_pLB;
    sint32		m_selScen;
    iScenList	m_scList;
    uint32		m_filter;
    IViewCmdHandler*  m_pCmdHandler; 
    static SortBy      m_sort;
    static ShowFilter  m_ShowFilter;
};


#endif //PHEROES_GAME_CTL_SCENLIST_H_
