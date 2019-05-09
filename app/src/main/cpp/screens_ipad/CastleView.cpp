#include "stdafx.h"
#include "ArmyList.touch.h"
#include "CastleView.h"
#include "CastleView.h"
#include "TransitionView.h"
#include "Dlg_CreatInfo.h"
// #include "NewMapShop/NewMapShop.h"

namespace UI_IPAD
{

    /*
     iDockView::	iDockView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 dt)
     : iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHero(NULL), m_CurState(DS_BUILD), m_dock_type(dt),m_pCmdHandler(pCmdHandler),
     m_pHarbor(NULL)
     {
     AddChild(m_pBtn = new iImgBtn(pViewMgr, this, iRect(), 102, PDGG(BTN_SPLIT_SMALL), -1, iStringT(), Visible | Enabled));
     SwitchState(DS_CANT);
     }

     //void iDockView::UpdateSize()
     //{
     //	m_pBtn->SetRect(iRect((m_Rect.w - 80) / 2, m_Rect.h - 2 * DEF_BTN_HEIGHT, 80, DEF_BTN_HEIGHT));
     //	m_pBtn->Invalidate();
     //}


     iPlayer* iDockView::GetOwner()
     {
     return m_pHarbor->GetOwner(); //m_pHero ? m_pHero->Owner() : NULL;
     }

     void iDockView::OnCompose()
     {
     SpriteId sid, sidbase;
     iRect rc = GetScrRect();

     rc.y += 15;

     iPlayer *pPlayer = GetOwner() ? GetOwner() :  (m_pHero ? m_pHero->Owner() : NULL);
     check(pPlayer);


     sidbase = SHIP_TYPE_DESC[m_dock_type].sprite;

     if (m_CurState == DS_BUILD)
     {
     sid = sidbase + 37;
     iRect spr = AlignRect(gGfxMgr.Dimension(sid), rc, AlignTop) - gGfxMgr.Anchor(sid);
     gGfxMgr.Blit(sid, gApp.Surface(), spr);
     rc.y += spr.h;
     iMineralSet cost = SHIP_TYPE_DESC[m_dock_type].cost;
     iRect tmprc = iRect(rc.x, 0, rc.w, m_pBtn->GetScrPos().y);
     ComposeMineralSet(cost, pPlayer->Minerals(), AlignRect(GetMineralSetComposeSize(cost, true), tmprc, AlignBottom), true);

     }
     else if (m_CurState == DS_OCCUPY)
     {
     sid = sidbase + 43;
     gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[TRID_MSG_HARBOR_BOARD], iRect(rc.x + 5, rc.y + 10, m_Rect.w - 10, 20));
     iPoint pts = iPoint(rc.x + rc.w / 2, rc.y + rc.h / 2);


     //		 TODO: return the image of the harbor to this screen
     //		if (m_is_harbor)
     //		{
     //			iIsoMetric im(im_coef);
     //			iVisCnst_Harbor *vsh = (iVisCnst_Harbor*)(m_pHero->GetLocation());
     //			const iVisCnstT* pCnstT = vsh->Proto();
     //			for (uint32 cvr = 0; cvr < pCnstT->AvatarsCount(); ++cvr)
     //			{
     //				const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pCnstT->AvatarEntry(cvr).aid];
     //				iPoint cpos(pCnstT->AvatarEntry(cvr).ox, pCnstT->AvatarEntry(cvr).oy);
     //				for (uint32 sid = 0; sid < avatar.SpritesCount(); ++sid)
     //				{
     //					uint32 id = avatar.GetSpriteId(sid);
     //					iPoint opos = im.Map2Screen(cpos) + pts + avatar.GetSpriteOffset(sid);
     //					gGfxMgr.Blit(id, gApp.Surface(), opos);
     //				}
     //			}
     //
     //		}
     gGfxMgr.Blit(sid, gApp.Surface(), pts + iPoint(-60, -80));
     }
     else if (m_CurState == DS_BUSY)
     {
     sid = sidbase + 2;
     gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[TRID_MSG_HARBOR_OCCUPIED], iRect(rc.x + 5, rc.y + 120, m_Rect.w - 10, 50));
     iSize sz = gGfxMgr.Dimension(sid);
     gGfxMgr.BlitTile(sid, gApp.Surface(), iRect(35 + rc.x + (rc.w - sz.w) / 2, 69 + rc.y + 20, sz.w, sz.h));
     }
     else if (m_CurState == DS_CANT)
     {
     iRect rc = GetScrRect();
     iSize sz = gTextComposer.GetTextBoxSize(gTextMgr[TRID_MSG_HARBOR_NOSHIP], rc.w, dlgfc_plain);
     rc.y += (rc.h - sz.h) / 2;
     gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[TRID_MSG_HARBOR_NOSHIP], rc);
     }
     }

     void iDockView::SetHarbor(IHarbor* pHarbor)
     {
     m_pHarbor = pHarbor;
     UpdateControls();
     }

     void iDockView::SetHero(iHero *pHero)
     {
     m_pHero = pHero;
     UpdateControls();
     }

     void iDockView::UpdateControls()
     {
     if(!m_pHarbor) { SwitchState(DS_CANT); return; }

     if (m_pHarbor->IsBoardable() && m_pHarbor->GetShip(m_dock_type) && m_pHarbor->GetShip(m_dock_type)->IsLoaded())
     SwitchState(DS_BUSY);
     else if(m_pHarbor->IsBoardable() && m_pHarbor->GetShip(m_dock_type))
     SwitchState(DS_OCCUPY);
     else if(!m_pHarbor->GetShip(m_dock_type) && m_pHarbor->ShipsAvailable(m_dock_type) > 0 && m_pHarbor->GetShipPos(m_dock_type) != cInvalidPoint)
     SwitchState(DS_BUILD);
     else
     SwitchState(DS_CANT);
     }


     void iDockView::BuildShip()
     {
     iPoint pt = m_pHarbor->GetShipPos(m_dock_type);
     iPlayer *pPlayer = GetOwner() ? GetOwner() :  (m_pHero ? m_pHero->Owner() : NULL);
     check(pPlayer)

     // calculate ship coordinates
     if (pt == cInvalidPoint)
     {
     return;
     }

     // decrease player resources
     pPlayer->AddMineralSet(-SHIP_TYPE_DESC[m_dock_type].cost);
     // decrease available ship count
     m_pHarbor->ShipBought(m_dock_type);

     gGame.Map().AddShip(pt, m_dock_type);
     // update UI state
     UpdateControls();
     }


     void iDockView::OccupyShip()
     {
     check(m_pHero && m_pHarbor->IsBoardable());

     iShip* pShip = m_pHarbor->GetShip(m_dock_type);

     if(pShip)
     pShip->Occupy(m_pHero);
     }


     void iDockView::SwitchState(DOCK_STATE NewState)
     {
     // switch button label and visibility
     switch (NewState)
     {
     case DS_NONE:
     check(0);
     break;
     case DS_BUILD:
     {
     iPlayer* pPlayer = GetOwner();
     iMineralSet cost = SHIP_TYPE_DESC[m_dock_type].cost;
     sint32 has = pPlayer->Minerals().Has(cost);
     m_pBtn->SetVisible(has > 0);
     ((iImgBtn*)m_pBtn)->SetSprite(PDGG(BTN_BUY), PDGG(BTN_BUY_PRESSED));
     }
     break;
     case DS_OCCUPY:
     m_pBtn->SetVisible(true);
     break;
     case DS_BUSY:
     m_pBtn->SetVisible(false);
     break;
     case DS_CANT:
     m_pBtn->SetVisible(false);
     break;
     }
     m_CurState = NewState;
     Invalidate();
     }

     void iDockView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
     {
     uint32 uid = pView->GetUID();
     if ( uid == 102 )
     {
     if (m_CurState == DS_BUILD)
     {
     BuildShip();
     }
     else if (m_CurState == DS_OCCUPY)
     {
     OccupyShip();
     gGame.HideView(iChildGameView::CV_HARBOR);
     }
     }
     }


     // Mineral button
     iMineralBtn::iMineralBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, MINERAL_TYPE mt)
     : iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_mt(mt), m_bSelected(false) {}

     void iMineralBtn::SetState(bool bSeleted)
     {
     m_bSelected = bSeleted;
     Invalidate();
     }

     void iMineralBtn::SetQuant(FractionCoeff nval)
     {
     m_quant = nval;
     Invalidate();
     }

     void iMineralBtn::OnCompose()
     {
     iRect rc = GetScrRect();

     iStringT txt;
     if (!m_quant.IsValid())
     txt = gTextMgr[TRID_NA];
     else if (m_quant.num == 0 || m_quant.num >= m_quant.denum)
     txt = FormatNumber(m_quant.num);
     else
     txt = iFormat(_T("%d : %d"), m_quant.num, m_quant.denum);

     ComposeResourceIcon(m_mt, rc, m_bSelected, true, txt, true);
     }

     // Market view
     iMarketPanel::iMarketPanel( iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, PanelType type) : iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled),
     m_pCmdHandler(pHandler), m_selResource(-1), m_type(type)
     {
     for(uint32 xx=0; xx<MINERAL_COUNT; xx++)
     AddChild(m_pBtns[xx] = new iMineralBtn(pViewMgr, this, iRect(), 100 + xx, (MINERAL_TYPE)(MINERAL_GOLD + xx)));
     }

     void iMarketPanel::UpdateSize()
     {
     iRect rc = GetRect();

     iRect crc = iRect(iPoint(), iSize(rc.w / 3, rc.h / 3));
     for(uint32 xx=0; xx<MINERAL_COUNT; xx++) {
     m_pBtns[xx]->SetRect(crc);
     crc.x += crc.w;
     if(crc.x2() >= (sint32)rc.w) {
     if(m_type == MP_LEFT || xx < MINERAL_COUNT - 2) // the alignment of the last (incomplete) row differs
     crc.x = 0;
     else {
     crc.x = rc.w * 2 / 3;
     }
     crc.y += crc.h;
     }
     }
     }

     void iMarketPanel::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
     {
     Deselect();
     ((iMineralBtn*)pView)->SetState(true);
     m_selResource = pView->GetUID() - 100;
     m_pCmdHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, m_selResource);
     }

     MINERAL_TYPE iMarketPanel::SelectedMineral()
     {
     return (MINERAL_TYPE)m_selResource;
     }

     void iMarketPanel::Deselect()
     {
     if(m_selResource != -1)
     m_pBtns[m_selResource]->SetState(false);
     m_selResource = -1;
     }

     iMarketView::iMarketView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool is_harbor)
     : iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler),
     m_marketsCount(1), m_is_harbor(is_harbor)
     {
     AddChild(m_pLeftPanel = new iMarketPanel(pViewMgr, this, iRect(), 100, iMarketPanel::MP_LEFT));
     AddChild(m_pRightPanel = new iMarketPanel(pViewMgr, this, iRect(), 101, iMarketPanel::MP_RIGHT));

     AddChild(m_pAmountChooser = new iAmountChooserGlow(pViewMgr, this, iRect(), 102));
     AddChild(m_pTradeBtn = new iImgBtn(pViewMgr, this, iRect(), 103, PDGG(BTN_SPLIT_SMALL), -1, iStringT(), Visible | Enabled));
     }


     void iMarketView::SetOwner(iPlayer* pOwner)
     {
     m_pOwner = pOwner;

     m_pLeftPanel->Deselect();
     m_pRightPanel->Deselect();
     Update();

     }

     void iMarketPanel::UpdateAsLeftSide(const iMineralSet &ms)
     {
     for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) {
     m_pBtns[xx]->SetQuant( FractionCoeff(ms.quant[xx], 1));
     }
     }

     void iMarketPanel::UpdateAsRightSide(MINERAL_TYPE from, sint32 marketIdx)
     {
     if (from == MINERAL_UNKNOWN) {
     for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) m_pBtns[xx]->SetQuant(FractionCoeff());
     } else {
     for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) {
     MINERAL_TYPE to = (MINERAL_TYPE)(xx);
     if (from != to)	{
     FractionCoeff nval = MineralExchRate(from, to, marketIdx).GetNormalized();
     m_pBtns[xx]->SetQuant(nval);
     } else {
     m_pBtns[xx]->SetQuant(FractionCoeff());
     }
     }
     }
     }


     void iMarketView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
     {
     uint32 uid = pView->GetUID();
     if ( uid == 102 ) {
     // Slider
     OnSliderPosChanged();
     } else if ( uid == 103 ) {
     // Trade button
     check(m_pOwner);
     MINERAL_TYPE mfrom = m_pLeftPanel->SelectedMineral();
     MINERAL_TYPE mto = m_pRightPanel->SelectedMineral();
     FractionCoeff nval = MineralExchRate(mfrom, mto, m_marketsCount).GetNormalized();
     sint32 amount = m_pAmountChooser->CurPos();
     m_pOwner->AddMineral(mfrom, -nval.denum * amount);
     m_pOwner->AddMineral(mto, nval.num * amount);
     m_pLeftPanel->Deselect();
     m_pRightPanel->Deselect();
     gGame.CastleView()->Update();
     } else {
     // click on some mineral in the panels: update information
     Update();
     }
     }



     void iMarketView::UpdateSize()
     {
     iRect rc = GetRect();
     m_pTradeBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) - iPoint(0, 15));
     m_pAmountChooser->SetRect(AlignChild(iAmountChooserGlow::sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pTradeBtn->GetPos().y));
     m_pAmountChooser->UpdateSize();
     iRect free = iRect(0, 15, rc.w, m_pAmountChooser->GetRect().y - 15);
     iSize ps = iSize((free.w - gGfxMgr.Dimension(PDGG(MARKET_BALANCE)).w) / 2, free.h);
     m_pLeftPanel->SetRect(AlignRect(ps, free, AlignLeft));
     m_pRightPanel->SetRect(AlignRect(ps, free, AlignRight));
     m_pLeftPanel->UpdateSize();
     m_pRightPanel->UpdateSize();
     }

     void iMarketView::OnCompose()
     {
     iRect rc = GetScrRect();
     ComposeGlowBkgnd(gApp.Surface(), rc);
     gGfxMgr.Blit(PDGG(MARKET_BALANCE), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(MARKET_BALANCE)), m_Rect, AlignCenter) - iPoint(0, 15));
     //gApp.Surface().FillRect(m_pRightPanel->GetScrRect(), 0);
     //gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rc.point(), gTextMgr[TRID_KINGDOM_RESOURCES], iRect(rc.x+rc.w/4-m_cellSiz.w*3/2,rc.y,m_cellSiz.w*3,16), AlignTop);
     //gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rc.point(), gTextMgr[TRID_AVAILABLE_FOR_TRADE], iRect(rc.x+rc.w/2 + rc.w/4-m_cellSiz.w*3/2,rc.y,m_cellSiz.w*3,16), AlignTop);

     if(m_pTradeBtn->IsVisible()) {
     MINERAL_TYPE mfrom = m_pLeftPanel->SelectedMineral();
     MINERAL_TYPE mto = m_pRightPanel->SelectedMineral();
     FractionCoeff nval = MineralExchRate(mfrom, mto, m_marketsCount).GetNormalized();
     sint32 fh = gTextComposer.GetFontHeight(castlemarketfc.fontSize);
     iRect trc(rc.x, m_pAmountChooser->GetScrRect().y - fh, rc.w, fh);
     sint32 amount = m_pAmountChooser->CurPos();
     gTextComposer.TextBoxOut(castlemarketfc, gApp.Surface(), iFormat(gTextMgr[TRID_MSG_MARKET_SFORS], Mineral2Text(mfrom, nval.denum * amount, false).CStr(), Mineral2Text(mto, nval.num * amount).CStr()), trc);
     }

     }

     void iMarketView::Update()
     {
     if (m_is_harbor)
     m_marketsCount = 5; // harbor-dock have fixed exchange rate
     else
     m_marketsCount = iMIN<sint32>(9, m_pOwner->GetMarketIdx());

     m_pLeftPanel->UpdateAsLeftSide(m_pOwner->Minerals());
     m_pRightPanel->UpdateAsRightSide(m_pLeftPanel->SelectedMineral(), m_marketsCount);

     bool dall = m_pLeftPanel->SelectedMineral() == MINERAL_UNKNOWN || m_pRightPanel->SelectedMineral() == MINERAL_UNKNOWN ||
     m_pLeftPanel->SelectedMineral() == m_pRightPanel->SelectedMineral();

     if (!dall) {
     sint32 lnum = m_pOwner->Mineral(m_pLeftPanel->SelectedMineral());
     sint32 rnum = MineralExchRate(m_pLeftPanel->SelectedMineral(), m_pRightPanel->SelectedMineral(), m_marketsCount).GetNormalized().denum;
     m_pAmountChooser->SetVisible((sint32)(lnum / rnum) > 0);
     m_pAmountChooser->SetMax(lnum/rnum);
     } else {
     m_pAmountChooser->SetVisible(false);
     }

     m_pTradeBtn->SetVisible(false);
     m_pAmountChooser->SetCurPos(0);

     }

     void iMarketView::OnSliderPosChanged()
     {
     m_pTradeBtn->SetVisible(m_pAmountChooser->CurPos() > 0);
     }

     const iSize iMarketView::sm_defSize = iSize(510, 310);

     iMGuildView::iMGuildView(iViewMgr* pViewMgr, iRect rc, uint32 uid): iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
     {
     sint32 cellW, cellH;
     cellW = 75;
     cellH = 45;

     uint32 xx=0;
     for (uint32 lvl = 0; lvl<5; ++lvl){
     uint32 scnt = 5-lvl;
     iPoint pos(m_Rect.w / 2 - (cellW*scnt+3*(scnt-1))/2, 1 + (5-lvl-1)*cellH);
     pos.y += 20;

     for (uint32 spid=0; spid<scnt; ++spid) {
     AddChild(m_pSpellBtn[xx] = new iSpellBtn(pViewMgr,this,iRect(pos.x,pos.y,cellW,cellH),110 + xx));
     ++xx;
     pos.x += cellW+3;
     }
     }
     }

     void iMGuildView::OnCompose()
     {
     iRect rc = GetScrRect();
     ComposeGlowBkgnd(gApp.Surface(), rc);
     rc.y += 25;
     gTextComposer.TextBoxOut(castletopicfc, gApp.Surface(), gTextMgr[TRID_CTLCNST_COMMON_MGUILD], rc);
     }

     void iMGuildView::UpdateSize()
     {
     iRect rc = GetScrRect();
     sint32 cellW = (m_Rect.w - 60) / 5;
     sint32 cellH = (m_Rect.h - 60) / 5;
     uint32 xx=0;
     sint32 offset = 0;
     offset += gTextComposer.GetTextBoxSize(gTextMgr[TRID_CTLCNST_COMMON_MGUILD], rc.w, castletopicfc).h;

     for (uint32 lvl = 0; lvl<5; ++lvl){
     uint32 scnt = 5-lvl;
     iPoint pos(m_Rect.w / 2 - (cellW*scnt+3*(scnt-1))/2, offset + 1 + (5-lvl-1)*cellH);

     for (uint32 spid=0; spid<scnt; ++spid) {
     iView *p = GetChildById(110 + xx);
     if (p)
     {
     p->SetRect(iRect(pos.x,pos.y,cellW,cellH));
     p->UpdateSize();
     p->Invalidate();
     }
     ++xx;
     pos.x += cellW+3;
     }
     }
     }

     void iMGuildView::SetCastle(iCastle *pCastle)
     {
     m_pCastle = pCastle;
     Update();
     }

     void iMGuildView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
     {
     uint32 uid = pView->GetUID();
     if (uid >= 110 && uid < 125) {
     iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[m_pSpellBtn[uid-110]->GetSpell()->NameKey()], gTextMgr[m_pSpellBtn[uid-110]->GetSpell()->DescKey(MSL_NONE)], m_pSpellBtn[uid-110]->GetSpell()->Icon(),m_pCastle->Owner());
     dlg.DoModal();
     }
     }

     void iMGuildView::Update()
     {
     if (m_pCastle) {
     uint32 xx=0;
     for (uint32 lvl=0; lvl<5; ++lvl) {
     iCtlCnst* pCnst = m_pCastle->FindCnst((CTL_CONSTR)(CTLCNST_MAGEGUILD_L1+lvl));
     uint32 tcnt = (5-lvl);
     if (pCnst && pCnst->IsBuilt()) {
     iMGuildCtlCnst* pMGuild = DynamicCast<iMGuildCtlCnst*>(pCnst);
     check(pMGuild);
     uint32 spid;
     for (spid=0; spid < pMGuild->SpellsCount(); ++spid, ++xx) m_pSpellBtn[xx]->SetSpell(gGame.ItemMgr().m_spellMgr.Spell(pMGuild->Spell(spid)));
     for (uint32 fsp=spid; fsp<tcnt; ++fsp, ++xx) m_pSpellBtn[xx]->SetSpell(NULL);
     } else {
     for (uint32 fsp=0; fsp<tcnt; ++fsp, ++xx) m_pSpellBtn[xx]->SetSpell(NULL);
     }
     }
     } else {
     for (uint32 xx=0; xx<15; ++xx) m_pSpellBtn[xx]->SetSpell(NULL);
     }
     }



     void iDockView::UpdateSize()
     {
     m_pBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottom) - iPoint(0, 15));
     }


     iRecrSet::iRecrSet(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, uint32 uid, const iRect& rc)
     : iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
     , m_pCmdHandler(pCmdHandler), m_pCastle(NULL), m_selectedCT(CREAT_UNKNOWN)
     {
     for (uint32 xx=0; xx<6; ++xx) {
     AddChild(m_pBtns[xx] = new iRecrButton(pViewMgr, this, iRect(), 100 + xx, xx));
     }
     }

     void iRecrSet::SetCastle(iCastle* pCastle)
     {
     m_pCastle = pCastle;
     m_selectedCT = CREAT_UNKNOWN;
     }

     void iRecrSet::UpdateSize()
     {
     iRect rc = GetScrRect();
     iRect crc = iRect(0, 0, rc.w / 6, rc.h);
     for (uint32 xx=0; xx<6; ++xx) {
     m_pBtns[xx]->SetRect(crc);
     crc.x += rc.w / 6;
     }

     }


     void iRecrSet::SetSelectedCT(CREATURE_TYPE ct) {
     m_selectedCT = ct;
     for (uint32 xx=0; xx<6; ++xx) {
     m_pBtns[xx]->SetSelected(m_pBtns[xx]->Dwelling() && m_pBtns[xx]->Dwelling()->CrType() == ct);
     }
     Update();
     }

     // Recruit button
     iRecrButton::iRecrButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 idx)
     : iButton(pViewMgr, pCmdHandler, rect, uid, Enabled|Visible), m_pDwel(NULL), m_bSelected(false), m_idx(idx)
     {
     }


     void iRecrButton::SetDwelling(iDwellCtlCnst* pDwel)
     {
     m_pDwel = pDwel;
     Invalidate();
     }


     void iRecrButton::OnCompose()
     {

     iRect rc = GetScrRect();

     uint16 clr = RGB16(144, 160, 152);
     iRect bot = AlignRect(iSize(rc.w - (m_idx == 5 ? 6 : 0), 10), rc, AlignBottom) + (m_idx < 5 ? iPoint(5, 1) : iPoint(2, 1));
     gGfxMgr.BlitTile(PDGG(DLG_HTILES_GLOW), gApp.Surface(), bot);

     if(m_idx < 5) {
     iRect right = AlignRect(iSize(10, rc.h - 10), rc, AlignRight) ;//+ iPoint(0, 3);
     gGfxMgr.BlitTile(PDGG(DLG_VTILES_GLOW), gApp.Surface(), right);
     }

     if(m_bSelected) {
     iRect bkrect(rc);
     if(m_idx > 0)
     bkrect.x -= 3;
     else
     bkrect.DeflateRect(5, 0, 0, 0);
     bkrect.DeflateRect(0, 7, 5, 5);

     if(m_idx == 0) {
     uint16 sid = PDGG(DLG_CORNERS_PRESSED);
     iSize sz = gGfxMgr.Dimension(sid);
     gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopLeft));
     bkrect.DeflateRect(10, 0, 0, 0);
     gApp.Surface().FillRect(bkrect, clr);
     iRect left = iRect(bkrect.x - 8, bkrect.y + sz.h - 7, sz.w - 5, bkrect.h - sz.h + 7);
     gApp.Surface().FillRect(left, clr);
     }
     else if(m_idx == 5) {
     uint16 sid = PDGG(DLG_CORNERS_PRESSED) + 1;
     iSize sz = gGfxMgr.Dimension(sid);
     bkrect.DeflateRect(0, 0, 10, 0);
     gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopRight) + iPoint(4, 0));
     gApp.Surface().FillRect(bkrect, clr);
     iRect right = iRect(bkrect.x2(), bkrect.y + sz.h - 7, sz.w - 5, bkrect.h - sz.h + 7);
     gApp.Surface().FillRect(right, clr);
     } else
     gApp.Surface().FillRect(bkrect, clr);
     } else {
     if(m_idx == 0) {
     uint16 sid = PDGG(DLG_CORNERS_GLOW);
     iSize sz = gGfxMgr.Dimension(sid);
     gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopLeft));
     }
     else if(m_idx == 5) {
     uint16 sid = PDGG(DLG_CORNERS_GLOW) + 1;
     iSize sz = gGfxMgr.Dimension(sid);
     gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTopRight) + iPoint(4, 0));
     }
     }

     rc.y += 10;

     if (m_pDwel) {
     // Name
     rc.y += gTextComposer.TextBoxOut(creatinfofc_qty, gApp.Surface(), iStringT(gTextMgr[m_pDwel->CrType()*3+TRID_CREATURE_PEASANT_F2]), rc);

     // Creature icon
     SpriteId sid = PDGG(MINIMON_GLOW) + m_pDwel->CrType();
     iRect icRect = AlignRect(iSize(55, 55), rc, AlignTopLeft);
     iPoint sprpos = GetRealSpritePos(sid, icRect, false, AlignBottom) - iPoint(0, 10);
     gGfxMgr.Blit(sid,gApp.Surface(), sprpos);

     sint32 icx2 = sprpos.x + gGfxMgr.ObjRect(sid).w;

     iRect trc = iRect(iPoint(icx2 + 5, icRect.y), iPoint(rc.x2(), icRect.y2()));

     // Available
     trc.y += gTextComposer.TextBoxOut(creatinfofc_small, gApp.Surface(), iFormat(_T("%s: %d"), gTextMgr[TRID_AVAILABLE], m_pDwel->CrCount()), trc);

     // Growth
     trc.y += gTextComposer.TextBoxOut(creatinfofc_small, gApp.Surface(), iFormat(_T("%s: %d"), gTextMgr[TRID_GROWTH], m_pDwel->Growth()), trc);

     iSize sz = gTextComposer.GetTextSize( iFormat(_T("%s: %d"), gTextMgr[TRID_GROWTH], m_pDwel->Growth()), creatinfofc_small);
     trc.x += (trc.w - sz.w) / 2;
     trc.y += 5;

     // Cost
     //iSize sz = gTextComposer.GetTextSize(iStringT(MineralSet2Text(CREAT_DESC[m_pDwel->CrType()].cost)), creatinfofc_small);
     gTextComposer.TextOut(creatinfofc_small, gApp.Surface(), trc.point(), iStringT(MineralSet2Text(CREAT_DESC[m_pDwel->CrType()].cost)));
     }
     }


     void iRecrSet::OnCompose()
     {
     iRect rc = GetScrRect();
     }

     void iRecrSet::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
     {
     check(m_pCastle);
     check(m_pBtns[pView->GetUID()-100]->Dwelling());
     iDwellCtlCnst* pDwel = m_pBtns[pView->GetUID()-100]->Dwelling();
     iCreatGroup cg(pDwel->CrType(), pDwel->CrCount());
     m_pCmdHandler->iCMDH_ControlCommand(this, cmd, pDwel->CrType());
     #ifdef	MULTIPLAYER
     gMPMgr.OnCastleUpdate(m_pCastle);
     #endif

     }

     void iRecrSet::Update()
     {
     sint32 c=0;
     for (uint32 xx=0; xx<m_pCastle->Cnsts().Count(); ++xx) {
     if (iDwellCtlCnst* pDwel = DynamicCast<iDwellCtlCnst*>(m_pCastle->Cnst(xx))) {
     if (pDwel->IsBuilt()) {
     check(c<6);
     m_pBtns[c]->SetEnabled(true);
     m_pBtns[c]->SetDwelling(pDwel);
     } else {
     m_pBtns[xx]->SetEnabled(false);
     m_pBtns[xx]->SetDwelling(NULL);
     }
     c++;
     }
     }
     for(uint32 xx=c; xx<6; xx++)
     {
     m_pBtns[xx]->SetEnabled(false);
     m_pBtns[xx]->SetDwelling(NULL);
     }
     }
     */

    iCreatureRecruitView::iCreatureRecruitView(iViewMgr* pViewMgr, IViewCmdHandler *pCmdHandler, uint32 uid)
    : iView(pViewMgr, iRect(), GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler)
    {
        AddChild(m_pRecrSet = new iRecrSet(pViewMgr,this, 101, iRect()));

        AddChild(m_pDescription = new iCreatDescView(pViewMgr, this, iRect(), 106, CDV_COST | CDV_GLOW, NULL));
        AddChild(m_pAmount = new iAmountChooserGlow(pViewMgr, this, iRect(), 100, true));

        AddChild(m_pRecruitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 105, PDGG_IPAD(BTN_OK_DELUXE), -1, iStringT(), Enabled));
        AddChild(m_pBuyGoldBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 1100, PDGG_IPAD(BTN_BUY_DELUXE), -1));

        AddChild(m_pInfoBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 106, PDGG(BTN_INFO_DELUXE), PDGG(BTN_DELUXE_PRESSED)));
    }


    void iCreatureRecruitView::RecruitCreatures(uint32 count)
    {
        CREATURE_TYPE ct = m_pRecrSet->SelectedCT();
        iMineralSet cost = CREAT_DESC[ct].cost;
        // Check space in target army
        if (m_pCastle->Garrison().CanAddGroup((m_pRecrSet)->SelectedCT())) {
            m_pCastle->AddGroupToArmy(ct, count);
            m_pCastle->GetDwelling(ct)->CrCount() -= count;
            gGame.Map().FindPlayer(m_pCastle->Owner())->AddMineralSet(-cost * count);

            m_pAmount->SetMax(iMIN<sint32>(m_pCastle->GetDwelling(ct)->CrCount(), gGame.Map().FindPlayer(m_pCastle->Owner())->Minerals().Has(cost)));
            m_pAmount->SetCurPos(0);
            m_pDescription->SetType((m_pRecrSet)->SelectedCT());
            m_pDescription->SetAmount(0);
            m_pDescription->SetMinerals(&m_pCastle->GetOwner()->Minerals());
#ifdef	MULTIPLAYER
            gMPMgr.OnCastleUpdate(m_pCastle);
#endif
        } else {
            iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_NO_ROOM], PID_NEUTRAL);
            tdlg.DoModal();
        }

        m_pAmount->SetCurPos(0);
        m_pDescription->SetAmount(0);

        Update();
        Invalidate();
    }

    void iCreatureRecruitView::SetSelectedCT(CREATURE_TYPE ct)
    {
        m_pRecrSet->SetSelectedCT(ct);
        iMineralSet cost = CREAT_DESC[ct].cost;
        m_pAmount->SetMax(iMIN<sint32>(m_pCastle->GetDwelling(ct)->CrCount(), gGame.Map().FindPlayer(m_pCastle->Owner())->Minerals().Has(cost)));
        m_pAmount->SetCurPos(0);
        m_pDescription->SetType(ct);
        m_pDescription->SetAmount(0);
        Update();
    }

    void iCreatureRecruitView::OnMouseWheelDown(sint16 line, MouseId mID){

        sint32 pos = m_pAmount->CurPos();
        //#ifdef OS_MACOS
        //    if( line < 0 )
        //#else
        //    if( line > 0 )
        //#endif
        //        pos--;
        //    else
        //        pos--;
        pos += iSIGN(line);

        m_pAmount->SetCurPos(pos);

        m_pDescription->SetAmount(m_pAmount->CurPos());
        Update();
        Invalidate();
    }

    void iCreatureRecruitView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if(uid == 101) {
            if(cmd == CCI_BTNCLICK) {	 // creature cell single click
                SetSelectedCT((CREATURE_TYPE)param);
                Update();
            } else if(cmd = CCI_BTNDBLCLICK) { // creature cell double click
                SetSelectedCT((CREATURE_TYPE)param);
                RecruitCreatures(m_pAmount->GetMax());
            }
        } else if(uid == 105) {
            RecruitCreatures(m_pAmount->CurPos());
        } else if(uid == 1100)
        {
            // gMapShop.InitiateBuy("uk.co.euroskore.pk2_remastered.goldpack");
        } else if(uid == 100) {
            m_pDescription->SetAmount(m_pAmount->CurPos());
            Update();
            Invalidate();

        } else if(uid == 106) { // info btn click
            iArmyOwner fake;
            CREATURE_TYPE ct = m_pRecrSet->SelectedCT();
            if(ct == CREAT_UNKNOWN) return;
            fake.AddGroupToArmy(ct, 1);
            iDlg_CreatInfo dlg(&gApp.ViewMgr(), PID_NEUTRAL, &fake, 0, iFurtSkills(), false, 0, iCreatBonusList());
            dlg.DoModal();
        }
    }

    iSize iCreatureRecruitView::GetSize()
    {
        sint32 w = iCastleView::sm_defSubviewWidth + 100;
        sint32 h = 0;

        // recr button
        h += iIphoneBtn::sm_defSize.h;

        // amount chooser
        h += iAmountChooserGlow::sm_defSize.h;

        iSize sz = iSize(0, 0);
        for (uint32 xx=0; xx<m_pCastle->Cnsts().Count(); ++xx)
            if(m_pCastle->Cnst(xx))
                if (iDwellCtlCnst* pDwel = DynamicCast<iDwellCtlCnst*>(m_pCastle->Cnst(xx)))
                    if(pDwel->IsBuilt()) {
                        iSize tmpsz = m_pDescription->GetSize(w, pDwel->CrType(), pDwel->CrCount(), CDV_COST | CDV_GLOW, NULL);
                        sz.w = iMAX(sz.w, tmpsz.w);
                        sz.h = iMAX(sz.h, tmpsz.h);
                    }

        // description
        h += sz.h;

        // recr set
        h += 115;

        return iSize(w, h);
    }

    void iCreatureRecruitView::UpdateSize()
    {
        m_pRecruitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) - iPoint(0, 10));
        m_pBuyGoldBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) - iPoint(0, 10) + iPoint(iIphoneBtn::sm_defSizeDeluxe.w, 0));
        m_pAmount->SetRect(AlignChild(iAmountChooserGlow::sm_defSize, AlignBottom) - iPoint(0, m_Rect.h - m_pRecruitBtn->GetRect().y-14));
        m_pInfoBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) - iPoint(iIphoneBtn::sm_defSizeDeluxe.w, 10));

        m_pAmount->UpdateSize();
        iSize sz = iSize(0, 0);
        for (uint32 xx=0; xx<m_pCastle->Cnsts().Count(); ++xx)
            if (iDwellCtlCnst* pDwel = DynamicCast<iDwellCtlCnst*>(m_pCastle->Cnst(xx)))
                if(pDwel->IsBuilt()) {
                    iSize tmpsz = m_pDescription->GetSize(m_Rect.w, pDwel->CrType(), pDwel->CrCount(), CDV_GLOW | CDV_COST , NULL);
                    sz.w = iMAX(sz.w, tmpsz.w);
                    sz.h = iMAX(sz.h, tmpsz.h);
                }

        if(sz.w == 0)
            m_pDescription->SetRect(AlignChild(iSize(100, 100) , AlignBottom) - iPoint(0, m_Rect.h - m_pAmount->GetRect().y));
        else
            m_pDescription->SetRect(AlignChild(sz , AlignBottom) - iPoint(0, m_Rect.h - m_pAmount->GetRect().y));
        m_pDescription->UpdateSize();


        m_pRecrSet->SetRect(iRect(0, 0, m_Rect.w, m_pDescription->GetRect().y));
        m_pRecrSet->UpdateSize();
    }

    void iCreatureRecruitView::SetCastle( iCastle* pCastle )
    {
        m_pCastle = pCastle;
        m_pRecrSet->SetCastle(pCastle);
        m_pRecrSet->SetSelectedCT(CREAT_UNKNOWN);
        m_pDescription->SetType(CREAT_UNKNOWN);
        Update();
        Invalidate();
    }

    void iCreatureRecruitView::Update()
    {
        m_pRecrSet->Update();
        m_pAmount->SetVisible(m_pRecrSet->SelectedCT() != CREAT_UNKNOWN && m_pCastle->GetOwner()->Minerals().Has(CREAT_DESC[m_pRecrSet->SelectedCT()].cost) &&
                              m_pCastle->GetDwelling(m_pRecrSet->SelectedCT())->CrCount() > 0);
        m_pRecruitBtn->SetVisible(m_pRecrSet->SelectedCT() != CREAT_UNKNOWN && m_pAmount->IsVisible() && m_pAmount->CurPos() > 0);
        m_pDescription->SetMinerals(&m_pCastle->GetOwner()->Minerals());
    }

    void iCreatureRecruitView::OnCompose()
    {
        iRect rc = GetScrRect();
        ComposeGlowBkgnd(gApp.Surface(), rc, true, false);

        /*if(m_pRecrSet->SelectedCT() != CREAT_UNKNOWN && !m_pCastle->GetOwner()->Minerals().Has(CREAT_DESC[m_pRecrSet->SelectedCT()].cost) &&
         m_pCastle->GetDwelling(m_pRecrSet->SelectedCT())->CrCount() > 0)
         {
         const iMineralSet& ms = CREAT_DESC[m_pRecrSet->SelectedCT()].cost;
         iRect minrc = AlignRect(GetMineralSetComposeSize(ms, true), m_pAmount->GetRect() + rc.point(), AlignCenter);
         ComposeMineralSet(ms, m_pCastle->GetOwner()->Minerals(), minrc, true);
         }*/

    }

    ////
    // TAVERN & DOCK VIEW
    ////



    ////////////////////

    const sint32 iCastleView::sm_defSubviewWidth = 840;
    const iSize iCastleView::sm_defSubviewSize = iSize(840, 470);

    iCastleView::iCastleView()
    : ICastleView(false, CV_OVERLAND)
    , m_pCastle(NULL)
    , m_bGestureComplete(false)
    , m_selectedCnst(CTLCNST_INVALID)
    , m_pAni(NULL)
#if defined PC_VERSION
    , m_bMarket(false)
    , m_bTavern(false)
    , m_bRecruit(false)
    , m_bBuild(false)
#endif
    {

        AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 100, true, true));
#ifndef PC_VERSION
        AddChild(m_pBuildBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 102, PDGG(BTN_CASTLE_BUILDINGS_BIG), PDGG(BTN_CASTLE_BUILDINGS_BIG_PRESSED), _T("Build"), Visible | Enabled, AlignLeft));
        AddChild(m_pRecrBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 104, PDGG(BTN_CASTLE_RECRUIT_BIG), PDGG(BTN_CASTLE_RECRUIT_BIG_PRESSED), _T("Recruit"), Visible | Enabled));
        AddChild(m_pDismissBtn =  new iImgBtn(&gApp.ViewMgr(), this, iRect(), 116, PDGG(BTN_DISMISS), PDGG(BTN_DISMISS_PRESSED)));
#else
        AddChild(m_pBuildBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 102, PDGG(BTN_BUILD_DELUXE), -1, gTextMgr[TRID_HOVER_BUILD], Visible | Enabled, AlignBottomLeft));
        AddChild(m_pRecrBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 104, PDGG(BTN_CASTLE_RECRUIT), -1, gTextMgr[TRID_HOVER_RECRUIT], Visible | Enabled, AlignBottom));
#endif
        //m_pBuildBtn->SetHoldable(true);

        AddChild(m_pMarketBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 103, PDGG(BTN_CASTLE_MARKET), -1, gTextMgr[TRID_HOVER_MARKET], Visible | Enabled, AlignBottom));
        AddChild(m_pMagicGuildBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 105, PDGG(BTN_SPELLBOOK_DELUXE), -1, gTextMgr[TRID_HOVER_MAGIC_GUID], Visible | Enabled, AlignBottom));
        AddChild(m_pTavernDockBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 106, PDGG(BTN_CASTLE_TAVERN), -1, gTextMgr[TRID_HOVER_TAVERN], Visible | Enabled, AlignBottom));
        AddChild(m_pExitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 107, PDGG(BTN_CANCEL_DELUXE), -1, gTextMgr[TRID_HOVER_END_DLG], Visible | Enabled, AlignBottomRight));

        AddChild(m_pMarketView = new iMarketView(&gApp.ViewMgr(), this, iRect(), 110, false));
        AddChild(m_pMGuildView = new iMGuildView(&gApp.ViewMgr(), iRect(), 111));
        AddChild(m_pTDView = new iTavernDockView(&gApp.ViewMgr(), this, iRect(), 112));
        AddChild( m_pRecrView = new iCreatureRecruitView(&gApp.ViewMgr(), this, 113));

        AddChild(m_pPrevCastleBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 114, PDGG(BTN_SWITCH_CASTLES_DELUXE), PDGG(BTN_SWITCH_CASTLES_DELUXE) + 2));
        AddChild(m_pNextCastleBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 115, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 1, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 3));

        SetViewMode(CVM_MAIN);
        Update();
    }

    void iCastleView::UpdateSize()
    {
        if(!m_pCastle) return;

#ifndef PC_VERSION
        iSize sidebtnsize = gGfxMgr.Dimension(PDGG(BTN_CASTLE_RECRUIT_BIG));
        m_pBuildBtn->SetRect(AlignChild(sidebtnsize, AlignLeft) - iPoint(5, 0));
        m_pRecrBtn->SetRect(AlignChild(sidebtnsize, AlignRight) + iPoint(5, 0));
        m_pTavernDockBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomLeft));
        m_pDismissBtn->SetRect(m_pArmyExchange->GetDismissRect() + m_pArmyExchange->GetPos());

        m_pExitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight));
        m_pMagicGuildBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight) - iPoint(iIphoneBtn::sm_defSize.w * 3 / 2, 0));
        m_pMarketBtn->SetRect(AlignChild(iIphoneBtn::sm_defSize, AlignBottomRight) - iPoint(iIphoneBtn::sm_defSize.w * 3, 0));
#else
        iSize celloffs(0,0);
        celloffs.w = ( GetDefaultSize().w - m_pArmyExchange->GetDefSize(1).w - 6*iIphoneBtn::sm_defSize.w - 10 )/6+1;
        m_pBuildBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomLeft) + iPoint(5,5));
        m_pRecrBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomLeft) + iPoint(5 + iIphoneBtn::sm_defSize.w+celloffs.w, 5));
        m_pTavernDockBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomLeft) + iPoint(5 + 2*(iIphoneBtn::sm_defSize.w+celloffs.w), 5));

        m_pExitBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomRight) + iPoint(0,5));
        m_pMagicGuildBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomRight) - iPoint( iIphoneBtn::sm_defSize.w+celloffs.w, -5));
        m_pMarketBtn->SetRect(AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottomRight) - iPoint(2*(iIphoneBtn::sm_defSize.w+celloffs.w), -5));
#endif

        m_pMarketView->SetRect(AlignChild(iMarketView::sm_defSize, AlignCenter));
        m_pMarketView->UpdateSize();

        m_pMGuildView->SetRect(AlignChild(sm_defSubviewSize, AlignCenter));
        m_pMGuildView->UpdateSize();

        m_pTDView->SetRect(AlignChild(iTavernDockView::sm_defSize, AlignCenter));
        m_pTDView->UpdateSize();

        m_pRecrView->SetRect(AlignChild(m_pRecrView->GetSize(), AlignCenter));// + iPoint(0, 8));
        m_pRecrView->UpdateSize();

        m_pPrevCastleBtn->SetRect(AlignChild(iSize(50, 50), AlignTop) - iPoint(300, 0));
        m_pNextCastleBtn->SetRect(AlignChild(iSize(50, 50), AlignTop) + iPoint(300, 0));

        m_pArmyExchange->SetRect(AlignChild(iArmyExchange::GetDefSize(2), AlignBottom)
#if defined( PC_VERSION )
                                 + iPoint(celloffs.w/2,1)
#endif
                                 );
        m_pArmyExchange->SetPos(m_pArmyExchange->GetPos() + iPoint(0, 3 * iArmyListEx_touch::sm_defCellSize.h / 4));
        m_pArmyExchange->UpdateSize();
    }


    iCastleView::~iCastleView()
    {

    }

    iStringT castleThemes[CTLT_COUNT] = {
        _T("Achaidh_Cheide"),   // citadel
        _T("Thunderhead"),      // stronghold
        _T("Ibn Al-Noor"),	    // tower
        _T("Eastern_Thought"),  // dungeon
        _T("Finding_Movement"), // fortress
        _T("Frost_Waltz"),       // necropolis
        _T("Pale_Rider")       // variags
    };


    void iCastleView::SetCastle( iCastle* pCastle )
    {
        m_pCastle = pCastle;
        m_pCastle->UpdateCnsts();
        m_pRecrView->SetCastle(m_pCastle);
        m_pMarketView->SetOwner(gGame.Map().FindPlayer(m_pCastle->Owner()));
        m_pMGuildView->SetCastle(m_pCastle);
        m_pTDView->SetCastle(m_pCastle);

        m_pArmyExchange->SetTopOwner(m_pCastle, true);
        UpdateSize();
        Update();
        iStringT str = castleThemes[m_pCastle->Proto()->Type()] + _T(".mp3");
        iStringT tmp = gMusicPath + castleThemes[m_pCastle->Proto()->Type()] + _T(".mp3");
        gMusicMgr.Play(tmp, true);
    }

    sint16 iCastleView::GetCtlSprite( CTL_CONSTR cnst, uint32& idx )
    {
        uint32 xx=0;
        while(CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][1] != cnst) xx++;
        check(CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][0] != -1);
        idx = xx;
        return CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][0];
    }

    void iCastleView::OnTripleSwipeLeft(sint32 length)
    {
        if(length > 20 && !m_bGestureComplete) {
            gGame.Map().CurPlayer()->NextCastle();
            SetCastle(gGame.Map().CurCastle());
            m_bGestureComplete = true;
        }
    }

    void iCastleView::OnTripleSwipeRight(sint32 length)
    {
        if(length > 20 && !m_bGestureComplete) {
            gGame.Map().CurPlayer()->PrevCastle();
            SetCastle(gGame.Map().CurCastle());
            m_bGestureComplete = true;
        }
    }

    void iCastleView::OnMouseUp( const iPoint& pos, MouseId mID, MouseButtonId mbID )
    {
        m_selectedCnst = CTLCNST_INVALID;
        /*
         if(mID == 0 && m_viewMode == CVM_BUILD) {
         m_pBuildBtn->Unhighlight();
         if(!(m_pBuildBtn->GetButtonState() & iButton::Pressed))
         SetViewMode(CVM_MAIN);
         }
         */
        Invalidate();
    }

    void iCastleView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {
        if(mID == 0) m_bGestureComplete = false;

        /*#ifdef OS_IPHONE
         if(!mID == 1) return;
         #endif
         */
        if(m_viewMode != CVM_BUILD && m_viewMode != CVM_MAIN) return; // only for build & main mode

        if(GetChildByPos(pos - m_Rect.point())) return;

        sint32 clicked = GetBuildingClicked(pos, m_viewMode == CVM_MAIN);

        if(clicked != -1) {

            // if we are clicking the built ones, move to the latest MGuild
            if(m_viewMode == CVM_MAIN)
                clicked = SelectBuiltMGuild(clicked);

            m_selectedCnst = m_pCastle->Cnst(clicked)->Type();
            Invalidate();
        }
    }

    void iCastleView::OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID){

        OnMouseClick( pos, mID, mbID );
    }

    void iCastleView::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        if(GetChildByPos(pos - m_Rect.point())) return;

        if(m_viewMode == CVM_MARKET || m_viewMode == CVM_MGUILD ||
           m_viewMode == CVM_TAVERNDOCK || m_viewMode == CVM_RECRUIT)
        {
            SetHighlighted(CVM_EMPTY);
            SetViewMode(CVM_MAIN);
            return;
        }

        if(m_viewMode != CVM_BUILD && m_viewMode != CVM_MAIN) return; // only for build mode

        // get the building clicked
        sint32 clicked = GetBuildingClicked(pos, m_viewMode == CVM_MAIN);

        if(m_viewMode == CVM_MAIN)
            clicked = SelectBuiltMGuild(clicked);

        if(clicked != -1 && m_viewMode == CVM_MAIN && m_pCastle->Cnst(clicked)->GetState() == iCtlCnst::Built) {
            iCtlCnst* pCnst = m_pCastle->Cnst(clicked);
            if(iDwellCtlCnst* pDwel = dynamic_cast<iDwellCtlCnst*>(pCnst)) {
                SetViewMode(CVM_RECRUIT);
                m_pRecrView->SetSelectedCT(pDwel->CrType());
                return;
            } else if(iTavernCtlCnst* pTav = dynamic_cast<iTavernCtlCnst*>(pCnst)) {
                SetViewMode(CVM_TAVERNDOCK);
                return;
            } else if(iMGuildCtlCnst* pMGuild = dynamic_cast<iMGuildCtlCnst*>(pCnst)) {
                SetViewMode(CVM_MGUILD);
                return;
            } else if(pCnst->Type() == CTLCNST_MARKETPLACE) {
                SetViewMode(CVM_MARKET);
                return;
            } else {
                iTextDlg dlg(&gApp.ViewMgr(), pCnst->Name(), pCnst->Desc(), PID_NEUTRAL);
                dlg.DoModal();
                return;
            }
        }

        if(clicked != -1 && m_viewMode == CVM_BUILD && m_pCastle->Cnst(clicked)->GetState() == iCtlCnst::Available) {
            m_selectedCnst = m_pCastle->Cnst(clicked)->Type();
            iDlg_BuildCnstCnfrm dlg(&gApp.ViewMgr(), m_pCastle->Cnst(clicked));
            if(dlg.DoModal() == DRC_OK) {
                if(m_pCastle->Cnst(clicked)->GetState() == iCtlCnst::Available) {
                    m_pCastle->Build(clicked);
                    gSfxMgr.PlaySound(CSND_EXP);
                }
                Update();
                /* create animation */
                m_pAni = new iCFSAni(iCFSAni::CFSA_BLD_ALPHA, fix32(2.0f), m_pCastle->Cnst(clicked)->Type(), 0);

                m_pBuildBtn->Release();
                UpdateSize();
                Invalidate();
            } else {
                m_pBuildBtn->Release();
                //			if(!(m_pBuildBtn->GetButtonState() & iButton::Pressed))
                //				SetViewMode(CVM_MAIN);
            }
            m_selectedCnst = CTLCNST_INVALID;
            return;
        }
    }

    bool iCastleView::Process( fix32 t )
    {
        if(m_pAni) {
            if(m_pAni->Process(t)) {
                delete m_pAni;
                m_pAni = NULL;
            }
            Invalidate();
        }

#if defined PC_VERSION
        if( m_bBuild ){

            m_bBuild = false;

            SetViewMode(m_viewMode == CVM_BUILD ? CVM_MAIN : CVM_BUILD);
            SetHighlighted( m_viewMode == CVM_BUILD ? CVM_BUILD : CVM_MAIN );
        }
        else if( m_bTavern ){

            m_bTavern = false;

            SetViewMode(m_viewMode == CVM_TAVERNDOCK ? CVM_MAIN : CVM_TAVERNDOCK);
            SetHighlighted( m_viewMode == CVM_TAVERNDOCK ? CVM_TAVERNDOCK : CVM_MAIN );
        }
        else if( m_bRecruit ){

            m_bRecruit = false;

            SetViewMode(m_viewMode == CVM_RECRUIT ? CVM_MAIN : CVM_RECRUIT);
            SetHighlighted( m_viewMode == CVM_RECRUIT ? CVM_RECRUIT : CVM_MAIN );
        }
        else if( m_bMarket ){

            m_bMarket = false;

            SetViewMode(m_viewMode == CVM_MARKET ? CVM_MAIN : CVM_MARKET);
            SetHighlighted( m_viewMode == CVM_MARKET ? CVM_MARKET : CVM_MAIN );
        }
#endif
        return true;
    }


    bool IntersectRect(iRect &dst_rect, const iRect &src_rect1, const iRect &src_rect2)
    {
        sint32 x1 = iMAX(src_rect1.x, src_rect2.x);
        sint32 y1 = iMAX(src_rect1.y, src_rect2.y);
        sint32 x2 = iMIN(src_rect1.x2(), src_rect2.x2()) + 1;
        sint32 y2 = iMIN(src_rect1.y2(), src_rect2.y2()) + 1;

        if ( (x2 - x1) > 0 && (y2 - y1) > 0) {
            dst_rect = iRect(x1,y1,x2-x1,y2-y1);
            return true;
        }
        else return false;
    }


    sint32 GetLen(iPoint p)
    {
        return sqrt((double)(p.x * p.x + p.y * p.y));
    }

    iPoint SetLen(iPoint p, sint32 len)
    {
        if(len == 0) return iPoint();
        sint32 cl = GetLen(p);
        p.x = p.x * cl / len;
        p.y = p.y * cl / len;
        return p;
    }

    sint32 sqrt(uint32 q){
        return sqrt(double(q));
    }

    bool PushApart( iRect& rc1, iPoint& c1, iRect& rc2, iPoint &c2, double coef )
    {
        iRect inter;
        if(!IntersectRect(inter, rc1, rc2))
            return false;
        iPoint vect;

        vect.x = inter.w / 2;
        vect.y = inter.h / 2;

        sint32 m1 = rc1.w * rc1.h / 2000, m2 = rc2.w * rc2.h / 2000;

        if(rc1.x < rc2.x && rc1.x2() < rc2.x2()) {
            rc1.x -= vect.x / m1;
            rc2.x += vect.x / m2;
        } else if(rc1.x > rc2.x && rc1.x2() > rc2.x2()) {
            rc2.x -= vect.x / m2;
            rc1.x += vect.x / m1;
        }

        if(rc1.y < rc2.y && rc1.y2() < rc2.y2()) {
            rc1.y -= vect.y / m1;
            rc2.y += vect.y / m2;
        } else if(rc1.y > rc2.y && rc1.y2() > rc2.y2()) {
            rc2.y -= vect.y / m2;
            rc1.y += vect.y / m1;
        }

        return true;
    }

    bool CheckEdges( const iRect& host, iRect& rc )
    {
        bool bRes = false;
        if(rc.x < 50 + host.x && rc.y < 50 + host.y && (bRes = true)) { rc.x = 50; rc.y = 50; }
        if(rc.x2() > host.x2() - 50 && rc.y2() > host.y2() - 50 && (bRes = true))
        {
            rc.x -= rc.x2() - host.x2() + 50;
            rc.y -= rc.y2() - host.y2() + 50;
        }

        if(rc.x < 15 + host.x && (bRes = true)) rc.x = 15;
        if(rc.x2() > host.x2() && (bRes = true)) rc.x -= rc.x2() - host.x2();

        if(rc.y < 15 + host.y && (bRes = true)) rc.y = 15;
        if(rc.y2() > host.y2() && (bRes = true)) rc.y -= rc.y2() - host.y2();


        return bRes;
    }

    enum HighlightType {
        HLT_NONE,
        HLT_AVAILABLE,
        HLT_SELECTED,
        HLT_NOTENOUGH,
        HLT_DARKENED,
        HLT_TOUCHED,
        HLT_BUILT
    };
    typedef struct { sint16 sid; HighlightType high; uint32 ctlidx; uint8 alpha; } listElem;


    void iCastleView::DoCompose()
    {

        //	if(gApp.ViewMgr().HasModalDlg()) {
        //		gGfxMgr.Blit(PDGG(HERO_DIALOG_BACK), gApp.Surface(), iPoint());
        //		SetMode(CVM_MAIN);
        //		return;
        //	}

        bool bDarken = m_viewMode != CVM_MAIN || gApp.ViewMgr().HasModalDlg();

        if(!m_pCastle) return;

        iSortArray<listElem> compList; // a list of sprites to compose arranged by z-order

        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++)
        {
            if(m_pCastle->Cnst(xx)->IsBuilt()) {
                // if higher magic guild is built, skip this one
                if(m_pCastle->Cnst(xx)->Type() >= CTLCNST_MAGEGUILD_L1 && m_pCastle->Cnst(xx)->Type() <= CTLCNST_MAGEGUILD_L4) {
                    iCtlCnst* pHigherG = m_pCastle->Cnsts().GetCnst((CTL_CONSTR)(m_pCastle->Cnst(xx)->Type() + 1));
                    if((pHigherG && pHigherG->IsBuilt()) || m_viewMode == CVM_BUILD)
                        continue;
                }
                uint32 idx;
                listElem elem;
                elem.sid = GetCtlSprite(m_pCastle->Cnst(xx)->Type(), idx);
                if(m_selectedCnst == m_pCastle->Cnst(xx)->Type())
                    elem.high = HLT_TOUCHED;
                else
                    elem.high = bDarken ? HLT_DARKENED : HLT_NONE;
                elem.ctlidx = xx;
                if(m_pAni && m_pAni->m_type == iCFSAni::CFSA_BLD_ALPHA && m_pAni->m_fparam == m_pCastle->Cnst(xx)->Type())
                {
                    elem.alpha = m_pAni->m_sparam;
                    if(m_pAni->m_sparam > 128)
                        elem.high = HLT_BUILT;
                }
                else
                    elem.alpha = 255;
                compList.Insert(elem, idx);
            }
            else if(!gApp.ViewMgr().HasModalDlg() || (m_selectedCnst == CTLCNST_INVALID || (m_pCastle->Cnst(xx)->Type() == m_selectedCnst)))
            {

                if(m_pCastle->Cnst(xx)->GetState() == iCtlCnst::Available && m_viewMode == CVM_BUILD) {
                    uint32 idx;
                    listElem elem;
                    elem.sid = GetCtlSprite(m_pCastle->Cnst(xx)->Type(), idx);
                    elem.high = m_pCastle->Cnst(xx)->Type() == m_selectedCnst ? HLT_SELECTED : HLT_AVAILABLE;
                    elem.alpha = 255;
                    elem.ctlidx = xx;
                    compList.Insert(elem, idx);
                } else if(m_pCastle->Cnst(xx)->GetState() == iCtlCnst::NotEnoughRes && m_viewMode == CVM_BUILD) {
                    uint32 idx;
                    listElem elem;
                    elem.sid = GetCtlSprite(m_pCastle->Cnst(xx)->Type(), idx);
                    elem.high = HLT_NOTENOUGH;
                    elem.alpha = 255;
                    elem.ctlidx = xx;
                    compList.Insert(elem, idx);
                }
            }
        }

        // add 'eternal' sprites (like background)
        uint32 xx=0;
        while(CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][0] != -1) {
            if(CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][1] == -1) {
                listElem elem;
                elem.sid = CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][0];
                elem.high = bDarken ? HLT_DARKENED : HLT_NONE;
                elem.alpha = 255;
                compList.Insert(elem, xx);
            }
            xx++;
        }
        check(compList.Size() > 0);

        // compose list: darkened sprites
        for(sint32 xx=compList.Size()-1; xx>=0; xx--)
        {
            if(compList[xx].value.high == HLT_DARKENED) {
                gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), m_Rect.point(), cInvalidRect, iGfxManager::EfxNone, false, -1, compList[xx].value.alpha);
            }
        }

        gApp.Surface().Darken50Rect(m_Rect);
        gApp.Surface().Darken50Rect(m_Rect);

        //gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        //gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));

        // compose list: darkened sprites
        for(sint32 xx=compList.Size()-1; xx>=0; xx--)
        {
            if(compList[xx].value.high != HLT_DARKENED) {
                gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), m_Rect.point(), cInvalidRect, iGfxManager::EfxNone, false, -1, compList[xx].value.alpha);
                if(compList[xx].value.high != HLT_NONE) {
                    iGfxManager::Effects efx;
                    if(compList[xx].value.high == HLT_AVAILABLE)
                        efx = iGfxManager::EfxVioletGlow;
                    else if(compList[xx].value.high == HLT_SELECTED || compList[xx].value.high == HLT_BUILT || compList[xx].value.high == HLT_TOUCHED)
                        efx = iGfxManager::EfxGlow;
                    else if(compList[xx].value.high == HLT_NOTENOUGH)
                        efx = iGfxManager::EfxRedGlow;
                    else if(compList[xx].value.high == HLT_DARKENED)
                        efx = iGfxManager::EfxDarken;

                    gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), m_Rect.point(), cInvalidRect, efx, false, -1, compList[xx].value.alpha);
                    if(compList[xx].value.high == HLT_SELECTED || compList[xx].value.high == HLT_BUILT || compList[xx].value.high == HLT_TOUCHED)
                        gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), m_Rect.point(), cInvalidRect, iGfxManager::EfxGlow, false, -1, compList[xx].value.alpha);
                    //gApp.Surface().FillRect(gGfxMgr.ObjRect(compList[xx].value.sid), 0, 128);
                }
            }
        }


        m_bldinfolist.RemoveAll();
        iRect scrrect = GetScrRect();

        // compose list: highlighted objects info
        if(!gApp.ViewMgr().HasModalDlg()) {
            for(sint32 xx=compList.Size()-1; xx>=0; xx--) {
                if(compList[xx].value.high == HLT_AVAILABLE || compList[xx].value.high == HLT_NOTENOUGH || compList[xx].value.high == HLT_SELECTED) {
                    // info of a structure
                    iRect inforc = gTextComposer.GetTextSize(m_pCastle->Cnst(compList[xx].value.ctlidx)->Name(), castlebldfc);
                    iSize costsz = GetMineralSetComposeSize(m_pCastle->Cnst(compList[xx].value.ctlidx)->Price(), true, true);
                    inforc.w = iMAX(inforc.w, costsz.w);
                    inforc.h += costsz.h;

                    iRect objrect = gGfxMgr.ObjRect(compList[xx].value.sid);
                    inforc = AlignRect(inforc.size(), objrect, AlignCenter);

                    bldinfo bi;
                    bi.bldidx = compList[xx].value.ctlidx;
                    bi.rc = inforc + scrrect.point();
                    bi.text =  m_pCastle->Cnst(compList[xx].value.ctlidx)->Name();
                    bi.cost = m_pCastle->Cnst(compList[xx].value.ctlidx)->Price();
                    bi.costsz = costsz;
                    bi.center = inforc.Center();
                    if(iDwellCtlCnst* pDwel = dynamic_cast<iDwellCtlCnst*>(m_pCastle->Cnst(compList[xx].value.ctlidx)))
                        bi.crtype = pDwel->CrType();
                    else
                        bi.crtype = CREAT_UNKNOWN;

                    m_bldinfolist.Add(bi);
                }
            }
        }

        const uint32 tIter = 100;

        uint32 iter = 0;
        do {
            for(uint32 xx=0; xx<m_bldinfolist.GetSize(); xx++) {
                for(uint32 yy=0; yy<m_bldinfolist.GetSize(); yy++)
                    if(xx != yy) {
                        //bDirty = PushApart(bldinfolist[xx].rc, bldinfolist[yy].rc, 1.0-((double)iter / tIter)) || bDirty;
                        PushApart(m_bldinfolist[xx].rc, m_bldinfolist[xx].center, m_bldinfolist[yy].rc, m_bldinfolist[yy].center, 1.0-((double)iter / tIter));
                    }
            }
            for(uint32 xx=0; xx<m_bldinfolist.GetSize(); xx++) {
                CheckEdges( scrrect, m_bldinfolist[xx].rc);
            }

            iter++;
        } while(iter < tIter);

        for(uint32 xx=0; xx<m_bldinfolist.GetSize(); xx++) {
            if(m_bldinfolist[xx].crtype != CREAT_UNKNOWN) {
                SpriteId sid = PDGG(MINIMON_GLOW) + m_bldinfolist[xx].crtype;

                iSize tsz = gTextComposer.GetTextSize( m_bldinfolist[xx].text, castlebldfc);
                iRect txt = AlignRect(tsz, m_bldinfolist[xx].rc, AlignTop);

                iRect icRect = AlignRect(gGfxMgr.Dimension(sid), txt, AlignLeft);
                icRect.x -= gGfxMgr.ObjRect(sid).w + 5;

                iSize screensize = gApp.Surface().GetSize();

                if( icRect.x < (screensize.w - 1024)/2 ){

                    sint32 offset = (screensize.w - 1024)/2 - icRect.x + 5;

                    icRect.x += offset;

                    m_bldinfolist[xx].rc.x += offset;
                }

                gGfxMgr.Blit(sid,gApp.Surface(),icRect.point()-gGfxMgr.Anchor(sid));
            }
            else{

                iSize tsz = gTextComposer.GetTextSize( m_bldinfolist[xx].text, castlebldfc);
                iRect txt = AlignRect(tsz, m_bldinfolist[xx].rc, AlignTop);

                iSize screensize = gApp.Surface().GetSize();

                if( txt.x < (screensize.w - 1024)/2 ){

                    sint32 offset = (screensize.w - 1024)/2 - txt.x + 5;

                    txt.x += offset;

                    m_bldinfolist[xx].rc.x += offset;
                }

                if( txt.y < (screensize.h - 768)/2 ){

                    sint32 offset = (screensize.h - 768)/2 - txt.y;

                    txt.y += offset;

                    m_bldinfolist[xx].rc.y += offset;
                }
            }

            iStringT str = m_bldinfolist[xx].text;

            gTextComposer.TextOut(castlebldfc, gApp.Surface(), m_bldinfolist[xx].rc.point(), m_bldinfolist[xx].text, m_bldinfolist[xx].rc, AlignTop);
            ComposeMineralSet(m_bldinfolist[xx].cost, m_pCastle->GetOwner()->Minerals(), AlignRect(m_bldinfolist[xx].costsz, m_bldinfolist[xx].rc, AlignBottom), true, false, false, true);
        }

        if(m_viewMode != CVM_BUILD) {
            // castle name
            iRect rc = GetScrRect();
            rc.y += 45;
            rc.y += gTextComposer.TextBoxOut(castletitlefc, gApp.Surface(), m_pCastle->Name(), rc);

            // resources
            //		ComposeResourcesBarGlow(rc, m_pCastle->GetOwner()->Minerals());
        }

        //	uint32 toffs = 10;
        //	iRect rect( m_Rect );

        // tile background
        //	iRect bkRect(rect);
        //	bkRect.DeflateRect(10);
        //gGfxMgr.BlitTile(bDecs?PDGG(BKTILE2):PDGG(BKTILE), surf,bkRect);
        //	surf.FillRect(bkRect, cColor_White, bTransp ? 64 : 178);

        /*
         // top/bottom tiles
         gGfxMgr.BlitTile(PDGG(DLG_HTILES), surf, iRect(rect.x+toffs,rect.y + 8,rect.w-toffs*2,3));
         gGfxMgr.BlitTile(PDGG(DLG_HTILES), surf, iRect(rect.x+toffs,rect.y2()-9,rect.w-toffs*2, 3));

         // left/right tiles
         gGfxMgr.BlitTile(PDGG(DLG_VTILES), surf, iRect(rect.x + 8,rect.y+toffs,3, rect.h-toffs*2));
         gGfxMgr.BlitTile(PDGG(DLG_VTILES), surf, iRect(rect.x2()-9,rect.y+toffs,3, rect.h-toffs*2));

         // corners
         //if (bDecs){
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+0, surf, iPoint(rect.x+3,rect.y+3));
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+1, surf, iPoint(rect.x2()-30,rect.y+3));
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+2, surf, iPoint(rect.x+3,rect.y2()-30));
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+3, surf, iPoint(rect.x2()-30,rect.y2()-30));
         */
        /*
         // top/bottom tiles
         gGfxMgr.BlitTile(PDGG(DLG_HTILES), gApp.Surface(), iRect(rect.x+toffs,rect.y-3,rect.w-toffs*2,3));
         gGfxMgr.BlitTile(PDGG(DLG_HTILES), gApp.Surface(), iRect(rect.x+toffs,rect.y2()+1,rect.w-toffs*2, 3));

         // left/right tiles
         gGfxMgr.BlitTile(PDGG(DLG_VTILES), gApp.Surface(), iRect(rect.x-3,rect.y+toffs,3, rect.h - toffs*2));
         gGfxMgr.BlitTile(PDGG(DLG_VTILES), gApp.Surface(), iRect(rect.x2()+1,rect.y+toffs,3, rect.h - toffs*2));

         // corners
         //if (bDecs){
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+0, gApp.Surface(), iPoint(rect.x-8,rect.y-8));
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+1, gApp.Surface(), iPoint(rect.x2()-20,rect.y-8));
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+2, gApp.Surface(), iPoint(rect.x-8,rect.y2()-20));
         gGfxMgr.Blit(PDGG(DLG_CORNERS)+3, gApp.Surface(), iPoint(rect.x2()-20,rect.y2()-20));
         */
    }

    void iCastleView::SetHighlighted( iCastleView::CastleViewMode cm ){
#ifdef PC_VERSION
        m_pBuildBtn->SetHighlightedBtn( CVM_BUILD == cm );
        m_pMarketBtn->SetHighlightedBtn( CVM_MARKET == cm );
        m_pRecrBtn->SetHighlightedBtn( CVM_RECRUIT == cm );
        m_pMagicGuildBtn->SetHighlightedBtn( CVM_MGUILD == cm );
        m_pTavernDockBtn->SetHighlightedBtn( CVM_TAVERNDOCK == cm );
        m_pExitBtn->SetHighlightedBtn( false );
        m_pMarketView->SetHighlighted( false );
        m_pTDView->SetHighlighted( false );
#endif
    }

    void iCastleView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 102) {
            //		if(cmd == CCI_RELEASE)
            //			SetViewMode(CVM_MAIN);
            //		else if(cmd == CCI_HOLD)
            //			SetViewMode(CVM_BUILD);
            SetViewMode(m_viewMode == CVM_BUILD ? CVM_MAIN : CVM_BUILD);
            SetHighlighted( m_viewMode == CVM_BUILD ? CVM_BUILD : CVM_MAIN );
        } else if(uid == 103) {
            SetViewMode(m_viewMode == CVM_MARKET ? CVM_MAIN : CVM_MARKET);
            SetHighlighted( m_viewMode == CVM_MARKET ? CVM_MARKET : CVM_MAIN );
        } else if(uid == 104) {
            SetViewMode(m_viewMode == CVM_RECRUIT ? CVM_MAIN : CVM_RECRUIT);
            SetHighlighted( m_viewMode == CVM_RECRUIT ? CVM_RECRUIT : CVM_MAIN );
        } else if(uid == 105){
            SetViewMode(m_viewMode == CVM_MGUILD ? CVM_MAIN : CVM_MGUILD);
            SetHighlighted( m_viewMode == CVM_MGUILD ? CVM_MGUILD : CVM_MAIN );
        } else if(uid == 106){
            SetViewMode(m_viewMode == CVM_TAVERNDOCK ? CVM_MAIN : CVM_TAVERNDOCK);
            SetHighlighted( m_viewMode == CVM_TAVERNDOCK ? CVM_TAVERNDOCK : CVM_MAIN );
        } else if(uid == 107){
            m_pExitBtn->SetHighlighted( false );
            Update();
            gGame.HideView(CV_CASTLE);
        } else if(uid == 112) { // tavern dock view event: hero has been recruited or ship built
            Update();
            SetViewMode(CVM_MAIN);
            SetHighlighted(CVM_MAIN);
        } else if(uid == 114) {
            m_pCastle->GetOwner()->PrevCastle();
#ifdef PC_VERSION
            m_pBuildBtn->SetHighlightedBtn(m_viewMode == CVM_BUILD);
#endif
        } else if(uid == 115) {
            m_pCastle->GetOwner()->NextCastle();
#ifdef PC_VERSION
            m_pBuildBtn->SetHighlightedBtn(false);
#endif
        } else if(uid == 116) { // dismiss button
            check(m_pCastle->Visitor() && !m_pCastle->Visitor()->IsCrucial());
            iQuestDlg qdlg(m_pMgr, m_pCastle->Visitor()->Name(), gTextMgr[TRID_MSG_HDISMISS],  m_pCastle->Visitor()->Owner()->PlayerId());
            if (qdlg.DoModal() == DRC_YES) {
                // Remove hero from the map
                gGame.Map().RemoveHero( m_pCastle->Visitor(), true);
                Update();
            }
        }
        else if(uid == 100 && cmd == CCI_BTNCLICK && param == iArmyExchange::BT_HERO) {
            /*check(m_pCastle->Visitor());
             if (gGame.Map().CurHero() != m_pCastle->Visitor()) m_pCastle->Visitor()->Owner()->SetCurHero(m_pCastle->Visitor());
             gGame.ShowView(iChildGameView::CV_HERO);
             */
        }
    }


    void iCastleView::OnSwipeUp(sint32 length) {
#ifdef OS_IPHONE
        if(length < 40 || gApp.ViewMgr().DragGlyph() ||
           dynamic_cast<iListBox*>(gApp.ViewMgr().CapturedView()) ||
           dynamic_cast<iAmountChooser*>(gApp.ViewMgr().CapturedView()))
            return;

        if(!m_bGestureComplete) {
            m_bGestureComplete = true;
            iDib castle(gApp.Surface().GetSize(), iDib::RGB);
            gApp.Surface().CopyToDibXY(castle, iPoint());
            iTransitionView* trView = new iTransitionView(&gApp.ViewMgr(),
                                                          iTransitionView::TVM_VRT_OVERLAY,
                                                          iTransitionView::MM_MANUAL,
                                                          castle, gGame.PeekScreen());
            trView->TransferMouseState(this);
            //		gApp.ViewMgr().ReleaseViewCapture();
            gApp.ViewMgr().SetViewCapture(trView);
            iTransitionView::TV_RESULT res = (iTransitionView::TV_RESULT)trView->DoModal();
            delete trView;
            if(res == iTransitionView::TVR_UP) {
                gGame.HideViewDelayed(CV_CASTLE);
            }
            //m_bGestureComplete = true;
            //Invalidate();
        }
#endif
    }

    void iCastleView::SetViewMode( CastleViewMode mode )
    {
        m_viewMode = mode;

        m_pMarketView->SetVisible(mode == CVM_MARKET);
        m_pRecrView->SetVisible(mode == CVM_RECRUIT);
        m_pMGuildView->SetVisible(mode == CVM_MGUILD);
        m_pTDView->SetVisible(mode == CVM_TAVERNDOCK);

        m_pBuildBtn->SetHighlighted(mode == CVM_BUILD);
        m_pRecrBtn->SetHighlighted(mode == CVM_RECRUIT);
        m_pMarketBtn->SetHighlighted(mode == CVM_MARKET);
        m_pMagicGuildBtn->SetHighlighted(mode == CVM_MGUILD);
        m_pTavernDockBtn->SetHighlighted(mode == CVM_TAVERNDOCK);

        Update();
    }

    void iCastleView::Update()
    {
        if(!m_pCastle) return;

        m_pCastle->UpdateCnsts();

        m_pArmyExchange->SetBottomOwner(m_pCastle->Visitor(), true);

        bool bBuild = false;
        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++)
            if(m_pCastle->Cnst(xx)->GetState() == iCtlCnst::Available || m_pCastle->Cnst(xx)->GetState() == iCtlCnst::NotEnoughRes)
            {
                bBuild = true;
                break;
            }
        m_pBuildBtn->SetVisible(m_pCastle->CanBuild() && bBuild);
        m_pBuildBtn->Unhighlight();

        m_pMagicGuildBtn->SetVisible(m_viewMode != CVM_BUILD && m_pCastle->IsMGuildBuilt());
        m_pMarketBtn->SetVisible(m_pCastle->FindCnst(CTLCNST_MARKETPLACE)->IsBuilt() && m_viewMode != CVM_BUILD);
        m_pRecrBtn->SetVisible(false);
        if(m_viewMode != CVM_BUILD) {
            for (uint32 xx=0; xx<m_pCastle->Cnsts().Count(); ++xx)
                if (iDwellCtlCnst* pDwel = DynamicCast<iDwellCtlCnst*>(m_pCastle->Cnst(xx)))
                    if(pDwel->IsBuilt()) {
                        m_pRecrBtn->SetVisible(true);
                        break;
                    }
        }

        m_pTavernDockBtn->SetVisible(m_viewMode != CVM_BUILD && m_pCastle->FindCnst(CTLCNST_TAVERN)->IsBuilt());
        m_pExitBtn->SetVisible(m_viewMode != CVM_BUILD);

        m_pPrevCastleBtn->SetVisible(m_pCastle->GetOwner()->CastleCount() > 1 && m_viewMode != CVM_BUILD && m_viewMode != CVM_EMPTY);
        m_pNextCastleBtn->SetVisible(m_pCastle->GetOwner()->CastleCount() > 1 && m_viewMode != CVM_BUILD && m_viewMode != CVM_EMPTY);

        m_pArmyExchange->SetVisible(m_viewMode != CVM_BUILD && m_viewMode != CVM_EMPTY);
#ifndef PC_VERSION
        m_pDismissBtn->SetVisible(m_pArmyExchange->IsVisible() && m_pArmyExchange->BottomArmyList()->ArmyOwner());
#endif

        if(m_viewMode == CVM_BUILD && !m_pBuildBtn->IsVisible())
            SetViewMode(CVM_MAIN);
        if(m_viewMode == CVM_MARKET && !m_pMarketBtn->IsVisible())
            SetViewMode(CVM_MAIN);
        if(m_viewMode == CVM_MGUILD && !m_pMagicGuildBtn->IsVisible())
            SetViewMode(CVM_MAIN);
        if(m_viewMode == CVM_RECRUIT && !m_pRecrBtn->IsVisible())
            SetViewMode(CVM_MAIN);
        if(m_viewMode == CVM_TAVERNDOCK && !m_pTavernDockBtn->IsVisible())
            SetViewMode(CVM_MAIN);
#ifndef PC_VERSION
        m_pDismissBtn->SetVisible(m_pArmyExchange->IsVisible() && m_pArmyExchange->BottomArmyList()->ArmyOwner());
#endif
        m_pMarketView->Update();
        m_pMGuildView->Update();
        m_pRecrView->Update();
        m_pTDView->Update();
    }



    void iCastleView::OnActivate(bool bActivate)
    {
        Update();
        if(bActivate) {

            bFirstDarken = true;
            m_selectedCnst = CTLCNST_INVALID;
            SetViewMode(CVM_MAIN);

            // remove the playing animation if any
            if(m_pAni) {
                delete m_pAni;
                m_pAni = NULL;
            }
            FallDown();

        } else {
            m_bTracking[0] = false; // this is a quick and dirty fix for the problem of building being auto-tapped after the castle screen falling from the top. Hedin 10.03.11
            // the real solution is to make sure this variable gets auto falsed automatically, refer to the comment in gxl.view.cpp, iView::MouseDown
        }
    }

    sint32 iCastleView::GetBldInfoClicked( const iPoint& pos)
    {
        iPoint p = pos - m_Rect.point();
        for(uint32 xx=0; xx< m_bldinfolist.GetSize(); xx++)
            if(m_bldinfolist[xx].rc.PtInRect(p))
                return m_bldinfolist[xx].bldidx;
        return -1;
    }

    sint32 iCastleView::GetBuildingClicked( const iPoint& pos, bool bBuilt )
    {
        // get the building clicked and set the highlight flag
        sint32 clicked = -1;
        uint32 minidx = -1;
        iPoint p = pos - m_Rect.point();
        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++) {
            if(m_pCastle->Cnst(xx)->GetState() == iCtlCnst::Available || (bBuilt && m_pCastle->Cnst(xx)->GetState() == iCtlCnst::Built)) {
                uint32 idx;
                sint16 sid  = GetCtlSprite(m_pCastle->Cnst(xx)->Type(), idx);
                if(gGfxMgr.IsInObj(sid, p) && idx < minidx) {
                    minidx = idx;
                    clicked = xx;
                }
            }
        }
        if(clicked == -1)
            return GetBldInfoClicked(pos);
        else return clicked;
    }

    void iCastleView::OnModalDlgAdded()
    {
        if(m_viewMode != CVM_EMPTY) {
            m_prevViewMode = m_viewMode;
            SetViewMode(CVM_EMPTY);
        }
    }

    void iCastleView::OnModalDlgRemoved()
    {
        SetViewMode(m_prevViewMode);
    }

    sint32 iCastleView::SelectBuiltMGuild( sint32 clicked )
    {
        bool bMGuild = false;
        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++) {
            if(xx == clicked && dynamic_cast<iMGuildCtlCnst*>(m_pCastle->Cnst(xx)))
                bMGuild = true;
        }
        if(!bMGuild) return clicked;

        sint32 res = clicked;
        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++) {
            if(dynamic_cast<iMGuildCtlCnst*>(m_pCastle->Cnst(xx)) && m_pCastle->Cnst(xx)->IsBuilt())
                res = xx;
        }
        return res;
    }

//    void iDockView::CheckResources()
//    {
//        if (m_CurState == DS_BUILD)
//        {
//            // check resources for ship build
//            iPlayer* pPlayer = GetOwner();
//            if (pPlayer)
//                m_pBtn->SetEnabled(pPlayer->Minerals().Has(SHIP_TYPE_DESC[m_dock_type].cost) != 0);
//            else
//                m_pBtn->SetEnabled(false);
//            m_pBtn->Invalidate();
//        }
//    }
//

    const iSize iTavernDockView::sm_defSize = iSize(650, 390);

    iTavernDockView::iTavernDockView( iViewMgr* pViewMgr, IViewCmdHandler *pHandler, const iRect& rc, uint32 uid ) : iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled),
    m_pDockView(NULL), m_pRecrHeroView(NULL), m_pHandler(pHandler)
    {
        AddChild(m_pRecrHeroView = new iRecruitHeroView(pViewMgr, this, iRect(), 100));
        AddChild(m_pDockView = new iDockView(pViewMgr, this, iRect(), 101, 0));
    }

    void iTavernDockView::SetCastle( iCastle *pCastle )
    {
        m_pRecrHeroView->SetCastle(pCastle);
        m_pDockView->SetHarbor(pCastle);
        m_pCastle = pCastle;
        Update();
    }

    void iTavernDockView::OnCompose()
    {
        iRect rc = GetScrRect();
        //ComposeGlowBkgnd(gApp.Surface(), rc);
        rc.y += 30;
        iStringT str;
        if(m_pRecrHeroView->IsVisible() && m_pDockView->IsVisible())
            str = iFormat(_T("%s/%s"), gTextMgr[TRID_CTLCNST_COMMON_TAVERN], gTextMgr[TRID_CTLCNST_COMMON_SHIPYARD]);
        else if(m_pRecrHeroView->IsVisible())
            str = iFormat(_T("%s"), gTextMgr[TRID_CTLCNST_COMMON_TAVERN]);
        else
            str = iFormat(_T("%s"), gTextMgr[TRID_CTLCNST_COMMON_SHIPYARD]);
        rc.y += gTextComposer.TextBoxOut(castletitlefc, gApp.Surface(), str, rc);
    }

    void iTavernDockView::UpdateSize()
    {
        iRect rc = GetScrRect();
        rc.y += 30;
        rc.y += gTextComposer.GetTextBoxSize(iFormat(_T("%s/%s"), gTextMgr[TRID_CTLCNST_COMMON_TAVERN], gTextMgr[TRID_CTLCNST_COMMON_SHIPYARD]), rc.w, castletitlefc).h;
        rc.y += 15;

        rc.h -= (rc.y - GetScrRect().y);

        if(m_pDockView->IsVisible() && m_pRecrHeroView->IsVisible()) {
            iSize sz = iSize(rc.w / 2, rc.h);
            m_pRecrHeroView->SetRect(AlignChild(sz, AlignLeft));
            m_pDockView->SetRect(AlignChild(sz, AlignRight));
        } else if(m_pDockView->IsVisible()) {
            m_pDockView->SetRect(AlignChild(rc.size(), AlignBottom));
        } else if(m_pRecrHeroView->IsVisible()) {
            m_pRecrHeroView->SetRect(AlignChild(rc.size(), AlignBottom));
        }
        m_pRecrHeroView->UpdateSize();
        m_pDockView->UpdateSize();
    }

    void iTavernDockView::SetHighlighted( bool flag ){

        //m_pRecrHeroView->SetHighlighted(flag);
    }

    void iTavernDockView::iCMDH_ControlCommand( iView *pView, CTRL_CMD_ID cmd, sint32 param )
    {
        m_pHandler->iCMDH_ControlCommand(this, cmd, param);
    }

    void iTavernDockView::Update()
    {
        m_pRecrHeroView->SetVisible(m_pCastle->GetTavern(true) != NULL);
        m_pDockView->SetVisible(m_pCastle->IsShipBuildSupported());

        m_pRecrHeroView->Update();
        m_pDockView->UpdateControls();
    }

    void iDlg_BuildCnstCnfrm::OnCreateDlg()
    {
        sint32 bw = iIphoneBtn::sm_defSize.w;
        AddChild(m_pBtnBuild = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(bw, 0), 100, PDGG(BTN_BUILD_DELUXE), -1));
        AddChild(m_pBtnCancel = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) - iPoint(bw, 0), 101, PDGG(BTN_CANCEL_DELUXE), -1));
    }

    void iDlg_BuildCnstCnfrm::UpdateSize(){

        Center();

        sint32 bw = iIphoneBtn::sm_defSize.w;
        m_pBtnBuild->SetRect( AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + iPoint(bw, 0) );
        m_pBtnCancel->SetRect( AlignChild(iIphoneBtn::sm_defSize, AlignBottom) - iPoint(bw, 0) );
    }

    void iDlg_BuildCnstCnfrm::DoCompose( const iRect& rect )
    {
        iRect rc(rect);
        rc.y += iBaseGameDlg::GetTopOffset();

        if(iDwellCtlCnst* pCnst = dynamic_cast<iDwellCtlCnst*>(m_pCnst)) {
            iSize tsz = gTextComposer.GetTextSize(m_pCnst->Name(), dlgfc_hdr);
            iRect txt = AlignRect(tsz, rc, AlignTop);
            iRect crc = AlignRect(iSize(55, 55), txt, AlignLeft);

            SpriteId sid = PDGG(MINIMON_GLOW) + pCnst->CrType();
            gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, crc - iPoint(55, 0) - iPoint(0, crc.h - tsz.h), false, AlignBottom));

            //ComposeCreatureIcon(gApp.Surface(), crc, pCnst->CrType(), false, false);
        }

        rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), m_pCnst->Name(), rc);
        rc.y += 15;
        rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), m_pCnst->Desc(), rc);

        iSize sz = GetMineralSetComposeSize(m_pCnst->Price(), true);
        ComposeMineralSet(m_pCnst->Price(), m_pCnst->Owner()->GetOwner()->Minerals(), AlignRect(sz, rc, AlignTop), false);

        rc.y += sz.h + 15;

        iStringT dependtext;
        iSimpleArray<uint16> descList;
        m_pCnst->Owner()->Cnsts().DescendantsList(m_pCnst, descList, 2);
        for(uint32 xx=0; xx<descList.GetSize(); xx++)
            dependtext += (xx > 0 ? _T(", ") : _T("-> ")) + m_pCnst->Owner()->FindCnst((CTL_CONSTR)descList[xx])->Name();
        rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), dependtext, rc);
    }

    iSize iDlg_BuildCnstCnfrm::ClientSize() const
    {
        sint32 w = iBaseGameDlg::GetDefaultWidth();
        sint32 h = iBaseGameDlg::GetTopOffset();

        iStringT dependtext;
        iSimpleArray<uint16> descList;
        m_pCnst->Owner()->Cnsts().DescendantsList(m_pCnst, descList, 2);
        for(uint32 xx=0; xx<descList.GetSize(); xx++)
            dependtext += m_pCnst->Owner()->FindCnst((CTL_CONSTR)descList[xx])->Name() + _T(", ");

        // title
        h += gTextComposer.GetTextBoxSize(m_pCnst->Name(), w, dlgfc_hdr).h;

        h += 15;

        // desc
        h += gTextComposer.GetTextBoxSize(m_pCnst->Desc(), w, dlgfc_plain).h;

        // cost
        h += GetMineralSetComposeSize(m_pCnst->Price(), true).h;

        // offset
        h += 15;

        // descendants
        h += gTextComposer.GetTextBoxSize(dependtext, w, dlgfc_plain).h;

        // buttons
        h += DEF_BTN_HEIGHT;


        return iSize(w, h);
    }

    void iDlg_BuildCnstCnfrm::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 100)
            EndDialog(DRC_OK);
        else if(uid == 101)
            EndDialog(DRC_CANCEL);
    }

    iDlg_BuildCnstCnfrm::iDlg_BuildCnstCnfrm( iViewMgr* pViewMgr, iCtlCnst* pCnst ) : iGlowGameDlg(pViewMgr), m_pCnst(pCnst)
    {

    }

}
