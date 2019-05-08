//
// MapShop.cpp
//
/// this file contains shop emulation code for test purposes -Cotulla
//

#include "stdafx.h"
#include "MapShop.h"


#define NUMBER_OF_MAPS      20


void iMapShop::Init()
{

}

static iStringT map_authors[3] =
{
    iStringT(_T("Jim")),
    iStringT(_T("Cotulla")),
    iStringT(_T("Hedin"))    
};

static wchar_t* map_names[5] =
{
    _T("Map"),
    _T("Funny"),
    _T("Difficult"),
    _T("Map"),
    _T("Sea")
};


iMapShop::ListResult iMapShop::List(iMapShop::iMapBuyList& lst, uint32 mask, bool bUpdatesOnly)
{
    static bool init_once = false;
    iMapBuyInfo mbi;


    if (!init_once)
    {
        init_once = true;
        uint32 k = 0;

        for (uint32 i = 0; i < NUMBER_OF_MAPS; i++)
        {                         
            mbi.m_Name = iFormat(_T("%s %s %s %d"), map_names[rand() % 5], map_names[rand() % 5], map_names[rand() % 5], k);    
            mbi.m_PID = iFormat(_T("PH.MAP.%d"), i);
            mbi.m_Author = map_authors[rand() % 3]; //iStringT(_T("COTULLA"));
			mbi.m_Desc = _T("Very long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long desc.");
            mbi.m_Cost = iFormat(_T("%d.%02d"), rand() % 10, rand() % 100);
            mbi.m_HotNew = (rand() % 33) & 0x3 ? true : false;
            mbi.m_HumanPlayers = rand() & 4 ? 3 : 2;
            mbi.m_TotalPlayers = rand() & 4 ? 3 : 2;
			mbi.m_Size = rand() & 1 ? 0 : 1;
            mbi.m_Rating = 4;
            mbi.m_Type = 11;
            mbi.m_bIsPack = false;

            m_list.Add(mbi);
            k++;
        }
    }

  
#ifndef OS_APPLE
    Sleep(2);
#endif

    if (m_list_counter >= 20 && m_money <= 0) 
    {
        m_list_counter = 0;
        return LResNoMoney;         
    }

    if (m_list_counter >= 20)
    {
        m_list_counter = 0;
       /* if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            return LResError;
        }
		*/
        // 5 sec after finish it
        lst = m_list;        
        return LResOK;
    }

    m_list_counter++;
    return LResLoading; //LResNoMoney;
}


iMapShop::BuyResult iMapShop::Buy(iStringT pid)
{
    uint32 idx;
        
    idx = (pid == m_list[0].m_PID) ? 0 : 1;
    if (idx > m_list.GetSize()) return BResFailed;

    /*if (m_money < m_list[idx].m_Cost)
    {
        return BResFailed;
    } */   

    // some actions 
#ifndef OS_APPLE
    Sleep(2);
#endif

    if (m_buy_counter >= 50)
    {
        // 2.5 sec afer finish it
        m_buy_counter = 0;
      /*  if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            return BResFailed;
        }
	   */
 //       m_money -= m_list[idx].m_Cost;
        m_list.RemoveAt(idx);
        return BResOK;
    }
    m_buy_counter++;
    return BResWorking;        
}


uint32 iMapShop::GetCount(uint32 mask, bool bUpdatesOnly) 
{
	return 1;
}

void iMapShop::RestorePurchases()
{
	
}

// END OF FILE