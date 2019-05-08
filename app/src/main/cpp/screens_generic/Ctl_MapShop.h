//
// Ctl_MapShop.h
// 

#ifndef PHEROES_GAME_CTL_MAPSHOP_H_
#define PHEROES_GAME_CTL_MAPSHOP_H_

//////////////////////////////////////////////////////////////////////////

#include <string>

class iShopListBox;

class iMapShopView: public iView, public IViewCmdHandler
{
public:
public:
    
    iMapShopView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid) ;
    ~iMapShopView();

    
    bool Process(fix32 t);
    void LoadData();
    void SetShowFilter(MapShop::ShowFilter sf) { m_ShowFilter = sf; LoadData(); };
	MapShop::ShowFilter GetShowFilter() { return m_ShowFilter; }
    uint32 GetMapCount(MapShop::ShowFilter sf);
	uint32 GetNewMapCount();
	void DoBuy();	
	//const iMapShop::iMapBuyInfo* CurMap() const;

private:
    enum SortBy 
    {
        Size = 0,
        RevSize,
        Name,
        RevName,
        Author, 
        RevAuthor, 
        Rating,
        RevRating,
		Price,
        RevPrice

    };

    enum State
    {
        StateIdle = 0,
        StateLoad,
        StateLoading,
        StateBuy,
        StateBuying,        
        StateNoMoney,
        StateNoConnection,
        StateLoadError,
        StateBuyError,
		StateDesc,
        StateRestorePurchases,
        StateLoadingRedeem
    };
    
    void UpdateMapCount();
    void CheckHeaderButton(uint32 id);
    void SortScenarios(SortBy sort_by);
    void OnCompose();
    void AfterCompose();
    void SendResult(uint32 res);
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
	void SetLBVisible(bool bVisible);
private:
    iShopListBox* m_pLB;
    std::string    m_MapToBuy;
    sint32      m_selScen;
    //iMapShop::iMapBuyList   m_List;
    uint32      m_filter;
	bool		m_bUpdatesOnly;
    static SortBy      m_sort;
    IViewCmdHandler*  m_pCmdHandler; 
    State       m_state;
    static MapShop::ShowFilter  m_ShowFilter;
//    uint32      m_CountADV, m_CountACT, m_CountALL, m_CountMULTI, m_CountUPD;
	iButton* m_pBtnSort1;
	iButton* m_pBtnSort2;
	iButton* m_pBtnSort3;
	iButton* m_pBtnSort4;
};


#endif //PHEROES_GAME_CTL_MAPSHOP_H_
