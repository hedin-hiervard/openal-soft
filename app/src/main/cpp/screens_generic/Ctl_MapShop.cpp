//
// Ctl_MapShop.cpp
//

#include "stdafx.h"
#include "ScopePtr.h"
//#include "xxc/xxc.file.h"
#include "serialize.h"
#include "gxl.inc.h"
// #include "NewMapShop.h"
#include "Ctl_MapShop.h"

#include "Misc_ios.h"

//////////////////////////////////////////////////////////////////////////

void ComposeMapHeader(iDib& dib, const iRect& irc, const iStringT& size, const iStringT& humanplayers, const iStringT& totalplayers, const iStringT& name, const iStringT& author, const iStringT& cost, bool bIsUpdating, bool bIsHotNew, bool bEven, bool bSel, bool bBackground);


#ifdef WIN32
iStringT UTF8TOISTRINGT(string input)
{
    return _T("");
}
#endif

class iSortBtn3 : public iButton
{
public:
    iSortBtn3(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, sint16 sid, sint16 pressedSid, sint16 tid, uint32 state = Visible|Enabled, Alignment align = AlignTopLeft)\
    : iButton(pViewMgr, pCmdHandler, rect, uid, state), m_sid(sid), m_align(align), m_pressedSid(pressedSid), m_high(false), m_tid(tid)
    {}

    void OnBtnDown() const { }

    inline void SetHighlighted(bool high) { m_high = high; Invalidate(); }
    inline void SetAlign(Alignment align) { m_align = align; Invalidate(); }

    void OnCompose()
    {
        iRect rect = GetScrRect();
        iSize dim = gGfxMgr.Dimension(m_sid);
        iRect dst = AlignRect(dim, rect, m_align);

        if(!m_high && !(m_state & Pressed)) {
            if(m_sid != -1) {
                gGfxMgr.Blit(m_sid, gApp.Surface(), dst);
            }
        } else {
            if(m_pressedSid != -1) {
                gGfxMgr.Blit(m_pressedSid, gApp.Surface(), dst);
            } else if(m_sid != -1) {
                gGfxMgr.Blit(m_sid, gApp.Surface(), dst);
                if(GetScrRect() == iIphoneBtn::sm_defSize)
                    gGfxMgr.Blit(PDGG(BTN_FRAME), gApp.Surface(), dst);
            }
        }
        iTextComposer::FontConfig fc(newmenufc_med_text);
        fc.cmpProps.faceColor = RGB16(0, 0, 0);
        gTextComposer.TextOut(fc,gApp.Surface(),rect,gTextMgr[m_tid],rect,AlignCenter, iPoint());
    }

    inline void SetSprite(sint16 sid, sint16 pressedSid) { m_sid = sid; m_pressedSid = pressedSid; Invalidate(); }

private:
    sint16 m_sid, m_pressedSid, m_tid;
    Alignment m_align;
    bool m_high;
};


void ComposeMapHeaderBkgnd(iDib& dib, const iRect& irc, bool bEven, bool bSel);

/*
 *	Scenario list box
 */
class iShopListBox : public iListBoxEx
{
public:
    iShopListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
    : iListBoxEx(pViewMgr, pCmdHandler, rect, gApp.Surface(), uid)
    {
        // fill lines
        RefillColors();
    }

    inline uint32 LBItemHeight() const { return 75; }
    inline uint32 LBItemsCount() const { return 0; }
	}

    void RefillColors()
    {
        m_clrdata.RemoveAll();
        uint32 l = gMapShop.GetProducts().size() + 1;
        for (uint32 i = 0; i < l; i++)
        {
            if (l % 2)
                m_clrdata.Add((i % 2) ? 0 : 1);
            else
                m_clrdata.Add((i % 2) ? 1 : 0);
        }
    }

private:
    void ComposeLBBackground(const iRect& rect)
    {
        iRect rc = rect;
        //gApp.Surface().Darken25Rect(rc);
    }

    void ExComposeLBItem(iDib &dib, sint32 iIdx, bool bSel, const iRect& irc)
    {
        if(iIdx == 0) {
            ComposeMapHeaderBkgnd(dib, irc, m_clrdata[iIdx], bSel);
            iTextComposer::FontConfig fc_b(newmenufc_big_text);
            fc_b.cmpProps.faceColor = RGB16(248, 248, 224);
            if(bSel)
                fc_b.cmpProps.faceColor = RGB16(0, 0, 0);
            gTextComposer.TextOut(fc_b, dib, irc.point(), gTextMgr[TRID_SHOP_RESTORE_PURCHASES], irc, AlignCenter);
        }
        else
        {
            const MapShop::ProductInfo& it = gMapShop.GetProducts()[iIdx
                                                                    - 1 // restore purchases
                                                                    ];

            iStringT cost;
            if(it.cost <= 0)
                cost = gTextMgr[TRID_SHOP_FREE];
            else
                cost = UTF8TOISTRINGT(it.formattedcost);
            iStringT h;
            if(it.min_humanPlayers > 0)
                h = iFormat(_T("%d-%d"), it.min_humanPlayers, it.max_humanPlayers);
            iStringT t;
            if(it.max_totalPlayers > 0)
                iFormat(_T("%d-%d"), it.min_totalPlayers, it.max_totalPlayers);

            iStringT size;
            if(it.max_size != -1)
                size = iFormat(_T("%s"), gTextMgr[TRID_SHORT_MAPSIZ_SMALL + it.max_size]);
            iStringT author = UTF8TOISTRINGT(it.author);
            if(author.Length() > 70)
                author = author.Left(70) + _T("...");

            ComposeMapHeader(dib, irc, size, h, t, UTF8TOISTRINGT(it.name), author, cost, it.update, false, (iIdx % 2) == 0, bSel, true);
        }
    }

private:
    iSimpleArray<bool>  m_clrdata;
};

/*
 *	Scenario List dialog
 */

iMapShopView::SortBy iMapShopView::m_sort = iMapShopView::Name;
MapShop::ShowFilter iMapShopView::m_ShowFilter = MapShop::ShowFilter_ALL;

iMapShopView::iMapShopView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled),
m_pCmdHandler(pCmdHandler), m_pLB(NULL), m_state(StateIdle), m_filter(0)
{
    iRect clRect = iRect(0, 0, m_Rect.w, m_Rect.h);

//    m_CountADV = 0, m_CountACT = 0, m_CountALL = 0, m_CountMULTI = 0, m_CountUPD = 0;

    // Listbox header
    AddChild(m_pBtnSort1 = new iSortBtn3(m_pMgr, this, iRect(0, 0, 129, 47), 500,
                                         PDGG(NMENU_SORT_NL), PDGG(NMENU_SORT_PL), TRID_MENU_SORTING1, Visible | Enabled));
    AddChild(m_pBtnSort2 = new iSortBtn3(m_pMgr, this, iRect(128, 0, 128, 47), 501,
                                         PDGG(NMENU_SORT_NM), PDGG(NMENU_SORT_PM), TRID_MENU_SORTING2,Visible | Enabled));
    AddChild(m_pBtnSort3 = new iSortBtn3(m_pMgr, this, iRect(clRect.w - 129 - 128 + 1, 0, 128, 47), 502,
                                         PDGG(NMENU_SORT_NM), PDGG(NMENU_SORT_PM), TRID_MENU_SORTING3,Visible | Enabled));
    AddChild(m_pBtnSort4 = new iSortBtn3(m_pMgr, this, iRect(clRect.w - 129, 0, 129, 47), 503,
                                         PDGG(NMENU_SORT_NR), PDGG(NMENU_SORT_PR), TRID_SHOP_PRICE,Visible | Enabled));

    // Listbox
    m_pLB = new iShopListBox(m_pMgr, this, iRect(3, 47, clRect.w - 3 * 2, clRect.h - 47), 100);
    AddChild(m_pLB);
	SetLBVisible(false);
}

void iMapShopView::SetLBVisible(bool bVisible)
{
    m_pLB->SetVisible(bVisible);
	m_pBtnSort1->SetVisible(bVisible);
	m_pBtnSort2->SetVisible(bVisible);
	m_pBtnSort3->SetVisible(bVisible);
	m_pBtnSort4->SetVisible(bVisible);
}

iMapShopView::~iMapShopView()
{
}


void iMapShopView::LoadData()
{
	m_bUpdatesOnly = false;
    if (m_ShowFilter == MapShop::ShowFilter_ALL)
        m_filter = GMP_GMODE_ADVENTURE | GMP_GMODE_ACTION  | GMP_GMODE_MULTIPLAYER;
    else if (m_ShowFilter == MapShop::ShowFilter_ACT)
        m_filter = GMP_GMODE_ACTION;
    else if (m_ShowFilter == MapShop::ShowFilter_ADV)
        m_filter = GMP_GMODE_ADVENTURE;
    else if (m_ShowFilter == MapShop::ShowFilter_MULTI)
        m_filter = GMP_GMODE_MULTIPLAYER;
    else if(m_ShowFilter == MapShop::ShowFilter_UPD)
		m_filter = MapShop::ShowFilter_UPD;
	else
        check(0);

    gMapShop.QueryAppstore();
    gMapShop.SetFilter(m_ShowFilter);
    m_state = StateLoad;
}

uint32 iMapShopView::GetNewMapCount()
{
	return gMapShop.GetUpdateCount();
}

uint32 iMapShopView::GetMapCount(MapShop::ShowFilter sf)
{
    return gMapShop.GetProductsCount(sf);
}

void iMapShopView::UpdateMapCount()
{
    // inform parent about map count update...
    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_EDITCHANGED, 0);
}


bool iMapShopView::Process(fix32 t)
{
    MapShop::ShopStatus ss = gMapShop.GetStatus();

    bool show_list = false;
    bool showBtnBack = false;

    switch (m_state)
    {
        case StateIdle:
            showBtnBack = show_list = true;
            break;

        case StateNoMoney:
        case StateNoConnection:
        case StateLoadError:
        case StateBuyError:
        case StateDesc:
            show_list = false;
            showBtnBack = true;
            break;
        case StateRestorePurchases:

            show_list = showBtnBack = false;

            switch(ss)
        {
            case MapShop::SS_READY:
                m_state = StateIdle;
                if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_LBSELECTED, 2);
                gMapEnum.Update();
                break;
            case MapShop::SS_RESTORING:
                break;
            case MapShop::SS_CANCELLED:
                m_state = StateIdle;
                break;
            case MapShop::SS_ERROR:
                m_state = StateNoConnection;
                break;
            default:
                break;
        }
            break;
        case StateBuy:
            m_state = StateBuying;
            gMapShop.InitiateBuy(m_MapToBuy);
            break;
        case StateBuying:
            switch (ss)
        {
            case MapShop::SS_READY:
                // after successful buy reload whole list
                // list box is hidden at this point
                m_state = StateLoad;
                gMapEnum.Update();
                break;
            case MapShop::SS_BUYING:
                m_state = StateBuying;
                break;
            case MapShop::SS_ERROR:
                m_state = StateBuyError;
                break;
            case MapShop::SS_CANCELLED:
                m_state = StateIdle;
                showBtnBack = show_list = true;
                break;
            default:
                break;
        }
            break;

        case StateLoad:
        case StateLoading:
            switch (ss)
        {
            case MapShop::SS_READY:
            case MapShop::SS_CANCELLED:
                // load operation done, we can get map counts now.
                // we assume information cached and can be changed only after LResOK
                UpdateMapCount();
                SortScenarios(m_sort);
                m_state = StateIdle;
                showBtnBack = show_list = true;
                break;
            case MapShop::SS_QUERYING_APPSTORE:
                m_state = StateLoading;
                break;
            case MapShop::SS_ERROR:
                m_state = StateLoadError;
                break;
            default:
                break;
        }
            break;
        case StateLoadingRedeem:
            switch(ss)
        {
            case MapShop::SS_READY:
                m_state = StateIdle;
                break;
            default:
                break;
        }
            break;
    }

	m_pLB->Process(t);
    if (show_list != m_pLB->IsVisible()) SetLBVisible(show_list);

    return showBtnBack;
}



void iMapShopView::CheckHeaderButton(uint32 id)
{
    iSortBtn3 *p1, *p2, *p3, *p4;

    p1 = (iSortBtn3*)GetChildById(500);
    p2 = (iSortBtn3*)GetChildById(501);
    p3 = (iSortBtn3*)GetChildById(502);
    p4 = (iSortBtn3*)GetChildById(503);

    p1->SetHighlighted(id == 500);
    p2->SetHighlighted(id == 501);
    p3->SetHighlighted(id == 502);
    p4->SetHighlighted(id == 503);
}


void iMapShopView::SortScenarios(SortBy sort_by)
{
    uint32 uid = 0;
    switch(sort_by)
    {
        case Size:
            gMapShop.SetListSort(MapShop::SORT_SIZE);
            break;
        case RevSize:
            gMapShop.SetListSort(MapShop::SORT_SIZE, true);
            break;
        case Name:
            gMapShop.SetListSort(MapShop::SORT_NAME);
            break;
        case RevName:
            gMapShop.SetListSort(MapShop::SORT_NAME, true);
            break;
        case Author:
            gMapShop.SetListSort(MapShop::SORT_AUTHOR);
            break;
        case RevAuthor:
            gMapShop.SetListSort(MapShop::SORT_AUTHOR, true);
            break;
        case Rating:
            gMapShop.SetListSort(MapShop::SORT_RATING);
            break;
        case RevRating:
            gMapShop.SetListSort(MapShop::SORT_RATING, true);
            break;
        case Price:
            gMapShop.SetListSort(MapShop::SORT_PRICE);
            break;
        case RevPrice:
            gMapShop.SetListSort(MapShop::SORT_PRICE, true);
            break;
    }
    m_sort = sort_by;

    if (sort_by == Name || sort_by == RevName) uid = 500;
    else if (sort_by == Size || sort_by == RevSize) uid = 501;
    else if (sort_by == Author || sort_by == RevAuthor) uid = 502;
    else if (sort_by == Price || sort_by == RevPrice) uid = 503;

    CheckHeaderButton(uid);
    Invalidate();
}


void iMapShopView::OnCompose()
{
    if (m_state == StateIdle)
    {
        if(gMapShop.GetProducts().size() == 0 && gSettings.GetLanguage() == GLNG_RUSSIAN)
        {
            iRect rc = GetScrRect();
            iTextComposer::FontConfig fc_b(newmenufc_big_text);
            fc_b.cmpProps.faceColor = RGB16(0, 0, 0);

            iStringT str(_T("Все куплено"));
            iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
            gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter));
        }
        return;
    }
    iRect rc = GetScrRect();

    iTextComposer::FontConfig fc_b(newmenufc_big_text);
    fc_b.cmpProps.faceColor = RGB16(0, 0, 0);

    if (m_state == StateLoading)
    {
        iStringT str(gTextMgr[TRID_SHOP_UPDATING_LIST]);
		iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
        gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter));
    }
    else if (m_state == StateBuying || m_state == StateLoadingRedeem)
    {
		iStringT str(gTextMgr[TRID_SHOP_PROCESSING_PURCHASE]);
		iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
        gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter) + iPoint(0,-70));

        int pr = gMapShop.GetDownloadProgress();

        if( pr > 0 ){

            iStringT prtxt = iFormat(_T("%d "), pr);
            prtxt += _T("\%");
            sz = gTextComposer.GetTextBoxSize(prtxt, rc.w - 10, fc_b);
            gTextComposer.TextBoxOut(fc_b, gApp.Surface(), prtxt, AlignRect(sz, rc, AlignCenter));
        }
    }
    else if (m_state == StateNoConnection)
    {
        iStringT str(gTextMgr[TRID_SHOP_PURCHASE_NO_NETWORK]);
		iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
        gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter));
    }
    else if (m_state == StateNoMoney)
    {
        iStringT str(gTextMgr[TRID_SHOP_PURCHASES_DISABLED]);
		iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
        gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter));
    }
    else if (m_state == StateBuyError)
    {
        iStringT str(gTextMgr[TRID_SHOP_PURCHASE_FAILED]);
		iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
        gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter));
    }
    else if (m_state == StateLoadError)
    {
        iStringT str(gTextMgr[TRID_SHOP_UPDATING_LIST_FAILED]);
		iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
        gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter));
    } else if(m_state == StateDesc)
	{
		if(m_selScen == -1) return;

        iRect rr = GetScrRect();

		iRect rc = AlignRect(iSize(m_pLB->GetSize().w
#if defined(OS_IPHONE) || defined(OS_ANDROID)
                                   -50
#endif
                                   , m_pLB->LBItemHeight()), GetScrRect(), AlignTop) + iPoint(0, 20);
        //		void ComposeMapHeader(iDib& dib, const iRect& irc, MAP_SIZE size, uint32 humanplayers, uint32 totalplayers, const iStringT& name, const iStringT& author, bool bIsPack, bool bIsUpdating, bool bIsHotNew, bool bEven, bool bSel, bool bBackground);
		const MapShop::ProductInfo& it = gMapShop.GetProducts()[m_selScen];

		sint32 w = gTextComposer.GetTextSize(UTF8TOISTRINGT(it.name), newmenufc_big_text).w;
#if defined(OS_IPHONE) || defined(OS_ANDROID)
		w += 100;
#else
        w += 50;
#endif
        //      gApp.Surface().FrameRect(rc, cColor_Red);
		iRect trc = AlignRect(iSize(w, 75), rc, AlignTop);
        //        gApp.Surface().FrameRect(trc, cColor_Blue);
#ifdef PC_VERSION
        trc -= iPoint(40, 0);
#endif
        iStringT h;
        if(it.min_humanPlayers > 0)
            h = iFormat(_T("%d-%d"), it.min_humanPlayers, it.max_humanPlayers);
        iStringT t;
        if(it.min_totalPlayers > 0)
            h = iFormat(_T("%d-%d"), it.min_totalPlayers, it.max_totalPlayers);

        iStringT sizes;
        if(it.min_size != -1)
        {
            if(it.min_size != it.max_size)
                sizes = iFormat(_T("%s-%s"), gTextMgr[TRID_SHORT_MAPSIZ_SMALL + it.min_size], gTextMgr[TRID_SHORT_MAPSIZ_SMALL + it.max_size]);
            else
                sizes = iFormat(_T("%s"), gTextMgr[TRID_SHORT_MAPSIZ_SMALL + it.max_size]);
        }

        iStringT author = UTF8TOISTRINGT(it.author);
        if(author.Length() > 50)
            author = author.Left(50) + _T("...");

        trc.InflateSize(150, 0);
		ComposeMapHeader(gApp.Surface(), trc, sizes, h, t, UTF8TOISTRINGT(it.name), author, _T(""), it.update, false, false, false, false);

		sint32 offs = 45;
        if(!gUseIpadUI)
            offs = 0;

		iRect txtrc = iRect(rc.BottomLeft() + offs, GetScrRect().BottomRight() - 45);
        //		txtrc.DeflateRect(m_pLB->GetSize().w / 4, 0);
		iTextComposer::FontConfig fc_l(newmenufc_high_text);
        fc_l.cmpProps.faceColor = RGB16(0, 0, 0);


		gTextComposer.TextBoxOut(fc_l, gApp.Surface(), UTF8TOISTRINGT(gMapShop.GetProducts()[m_selScen].desc), txtrc);
	}
    else if(m_state == StateRestorePurchases){

        iStringT str(gTextMgr[TRID_SHOP_PROCESSING_PURCHASE]);
		iSize sz = gTextComposer.GetTextBoxSize(str, rc.w - 10, fc_b);
        gTextComposer.TextBoxOut(fc_b, gApp.Surface(), str, AlignRect(sz, rc, AlignCenter) + iPoint(0,-70));


        int pr = gMapShop.GetDownloadProgress();

        if( pr > 0 ){
            iStringT prtxt = iFormat(_T("%d%d"), pr);
            sz = gTextComposer.GetTextBoxSize(prtxt, rc.w - 10, fc_b);
            gTextComposer.TextBoxOut(fc_b, gApp.Surface(), prtxt, AlignRect(sz, rc, AlignCenter));
        }
    }
}


void iMapShopView::AfterCompose()
{
    // CotullaTODO: move this to list box code?
    /*if  (m_pLB && m_pLB->IsVisible())
     {
     // draw list box shadow
     iPoint pt;
     iRect &lrc = m_pLB->GetScrRect();

     pt = lrc.TopLeft();
     gGfxMgr.Blit(PDGG(NMENU_LIST_SHDT), gApp.Surface(), pt);

     pt = lrc.BottomLeft();
     pt.MoveY(-46);
     gGfxMgr.Blit(PDGG(NMENU_LIST_SHDB), gApp.Surface(), pt);
     }*/

}

void iMapShopView::SendResult(uint32 res)
{
    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, (uint32)res);
}

void iMapShopView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();

    // disable all actions if something in the processing
    if (m_state != StateIdle)
    {
        return;
    }

    if (uid == DRC_OK || uid == DRC_CANCEL)
    {
        SendResult(uid);
    }
    else if (uid == 500) // Name
    {
        SortScenarios(m_sort == Name ? RevName : Name);
    }
    else if (uid == 501) // Size
    {
        SortScenarios(m_sort == Size ? RevSize : Size);
    }
    else if (uid == 502)  // Author
    {
        SortScenarios(m_sort == Author ? RevAuthor : Author);
    }
    else if (uid == 503)  // Price
    {
        SortScenarios(m_sort == Price ? RevPrice : Price);
    }
    else if (uid == 100)
    {
        if (cmd == CCI_LBSELCHANGED)
        {
			if (param >= 1 && param <= (sint32)gMapShop.GetProducts().size()+1) {
				m_MapToBuy = gMapShop.GetProducts()[param-1].pid;
				m_selScen = param - 1;
                }
                else {
                    m_MapToBuy= "";
                    m_selScen = -1;
                }
            }
            else if (cmd == CCI_LBSELECTED)
            {
                //m_MapToBuy = m_List[param-1].m_PID;
                if(param == 0) {

                    m_state = StateRestorePurchases;
                    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_LBSELECTED, 1);
                    gMapShop.RestorePurchases();
                } else
                    if (m_MapToBuy != "") {
                        m_state = StateDesc;
                        // inform parent about description screen enter
                        if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this, CCI_LBSELECTED, 0);
                    }
            }
        }
    }

    void iMapShopView::DoBuy()
    {
        if(m_MapToBuy != "")
            m_state = StateBuy;
        else
            LoadData();
    }

    //    const MapShop::ProductInfo& iMapShopView::CurMap() const
    //    {
    ////        if(m_state != StateDesc) return NULL;
    //
    //        return gMapShop.GetProducts()[m_selScen];
    //    }

    // END OF FILE
