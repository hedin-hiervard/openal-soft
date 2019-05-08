//
// MapShop.h
//

#ifndef _GAME_MAPSHOP_H_
#define _GAME_MAPSHOP_H_


/*
ResOK = 0,
ResFailed,
ResCancelled
*/

#define DELIVER_PURCHASE 0
#define DELIVER_UPDATE	 1
#define DELIVER_RESTORE	 2

class iMapShop 
{
public:    
    enum BuyResult
    {
        BResOK = 0,
        BResFailed,
        BResWorking,
        BResCancelled
    }; 

    enum ListResult
    {
        LResOK = 0,
        LResNoConnection,
        LResNoMoney,
        LResLoading,
        LResError
    };

    struct iMapBuyInfo
    {
        iStringT m_PID;
        iStringT m_Name;
        iStringT m_Desc;
        iStringT m_Author;
        uint32 m_Size;
		uint32 m_HumanPlayers;
        uint32 m_TotalPlayers;
        uint32 m_Rating;        
        uint32 m_Type;
        double m_numberCost;
        iStringT m_Cost;
        bool	m_HotNew;
		bool	m_bUpdating;
		bool	m_bIsPack;
		uint32 m_lastUpdate;
		bool	m_bFree;
    };        

    typedef iSimpleArray<iMapBuyInfo> iMapBuyList;

    iMapShop () {m_money = 5; m_buy_counter = 0; m_list_counter = 0; prcnt = -1.0; };
    void Init();    
    ListResult List(iMapBuyList& lst, uint32 mask, bool bUpdatesOnly);
    BuyResult Buy(iStringT pid);
	uint32 GetCount(uint32 mask, bool bUpdatesOnly);
	void RestorePurchases();
    void SetProgress( float pr ) { prcnt = pr; }

	float GetProgress()
#ifndef OS_APPLE
	{return 1.0;}
#else
	;
#endif
	
    ListResult StateRestorePurchases()
#ifndef OS_APPLE
	{return LResOK;}
#else
		;
#endif
protected:
    iMapBuyList m_list;
    uint32 m_money;
    uint32 m_list_counter;
    uint32 m_buy_counter;
    float prcnt;
};


extern iMapShop gMapShop;

#endif // _GAME_MAPSHOP_H_
// END OF FILE