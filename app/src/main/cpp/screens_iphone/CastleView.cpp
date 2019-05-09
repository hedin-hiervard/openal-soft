#include "stdafx.h"
#include "ArmyList.touch.h"
#include "CastleViewCommons.h"
#include "CastleView.h"
#include "TransitionView.h"
#include "Dlg_CreatInfo.h"
#include "ScreenSlider.h"
//#include "NewMapShop/NewMapShop.h"

namespace UI_IPHONE
{

    iBuildView::iBuildView(iViewMgr* pViewMgr, iRect rc, uint32 uid):
    iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled)
    , m_selectedCnst(CTLCNST_INVALID), m_pAni(NULL)
    , m_bBackgroundMode(false)

    {
        SetViewMode(CVM_MAIN);
    }

    void iBuildView::OnModalDlgAdded()
    {
        if(m_viewMode != CVM_EMPTY) {
            m_prevViewMode = m_viewMode;
            SetViewMode(CVM_EMPTY);
        }
    }

    void iBuildView::OnModalDlgRemoved()
    {
        SetViewMode(m_prevViewMode);
    }

    void iBuildView::UpdateSize()
    {
        iSize sidebtnsize = gGfxMgr.Dimension(PDGG(BTN_CASTLE_RECRUIT_BIG));
    }


    void iBuildView::Update()
    {
        bool bBuild = false;
        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++)
            if(m_pCastle->Cnst(xx)->GetState() == iCtlCnst::Available || m_pCastle->Cnst(xx)->GetState() == iCtlCnst::NotEnoughRes)
            {
                bBuild = true;
                break;
            }
    }

    void iBuildView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 102) {
            if(cmd == CCI_RELEASE)
                SetViewMode(CVM_MAIN);
            else if(cmd == CCI_HOLD)
                SetViewMode(CVM_BUILD);
        }
    }

    sint16 iBuildView::GetCtlSprite( CTL_CONSTR cnst, uint32& idx )
    {
        uint32 xx=0;
        while(CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][1] != cnst) xx++;
        check(CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][0] != -1);
        idx = xx;
        return CTL_GFX_INDEX[m_pCastle->Proto()->Type()][xx][0];
    }


    void iBuildView::SetCastle(iCastle* pCastle)
    {
        m_pCastle = pCastle;
    }

    sint32 iBuildView::GetBldInfoClicked( const iPoint& pos)
    {
        iRect rc = AlignRect(iSize(480, 320), iRect(iPoint(), gApp.Surface().GetSize()), AlignCenter);

        iPoint p = pos - m_Rect.point() - rc.point();
        for(uint32 xx=0; xx< m_bldinfolist.GetSize(); xx++)
            if(m_bldinfolist[xx].rc.PtInRect(p))
                return m_bldinfolist[xx].bldidx;
        return -1;
    }

    sint32 iBuildView::SelectBuiltMGuild( sint32 clicked )
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

    sint32 iBuildView::GetBuildingClicked( const iPoint& pos, bool bBuilt )
    {
        iRect rc = AlignRect(iSize(480, 320), iRect(iPoint(), gApp.Surface().GetSize()), AlignCenter);

        // get the building clicked and set the highlight flag
        sint32 clicked = -1;
        uint32 minidx = -1;
        iPoint p = pos - m_Rect.point() - rc.point();
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



    void iBuildView::SetViewMode( CastleViewMode mode )
    {
        m_viewMode = mode;
        m_selectedCnst = CTLCNST_INVALID;

        // remove the playing animation if any
        if(m_pAni) {
            delete m_pAni;
            m_pAni = NULL;
        }

        //gGame.CastleView()->Update();
    }

    void iBuildView::OnMouseUp( const iPoint& pos, MouseId mID, MouseButtonId mbID )
    {
        m_selectedCnst = CTLCNST_INVALID;

        if(mID == 0 && m_viewMode == CVM_BUILD) {
        }
        Invalidate();
    }

    void iBuildView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {
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
    void iBuildView::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        if(GetChildByPos(pos - m_Rect.point())) return;

        if(m_viewMode != CVM_BUILD && m_viewMode != CVM_MAIN) return; // only for build mode

        // get the building clicked
        sint32 clicked = GetBuildingClicked(pos, m_viewMode == CVM_MAIN);

        if(m_viewMode == CVM_MAIN)
            clicked = SelectBuiltMGuild(clicked);

        if(clicked != -1 && m_viewMode == CVM_MAIN && m_pCastle->Cnst(clicked)->GetState() == iCtlCnst::Built) {
            iCtlCnst* pCnst = m_pCastle->Cnst(clicked);
            if(iDwellCtlCnst* pDwel = dynamic_cast<iDwellCtlCnst*>(pCnst)) {
                gGame.CastleView()->SetScreen(iCastleView::RecruitScreen);
                gGame.CastleView()->SetRecruitCreatureType(pDwel->CrType());
                return;
            } else if(iTavernCtlCnst* pTav = dynamic_cast<iTavernCtlCnst*>(pCnst)) {
                gGame.CastleView()->SetScreen(iCastleView::TavernScreen);
                return;
            } else if(iMGuildCtlCnst* pMGuild = dynamic_cast<iMGuildCtlCnst*>(pCnst)) {
                gGame.CastleView()->SetScreen(iCastleView::MGuildScreen);
                return;
            } else if(pCnst->Type() == CTLCNST_MARKETPLACE) {
                gGame.CastleView()->SetScreen(iCastleView::MarketScreen);
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
                m_viewMode = CVM_MAIN;
                gGame.CastleView()->Update();
                /* create animation */
                m_pAni = new iCFSAni(iCFSAni::CFSA_BLD_ALPHA, fix32(2.0f), m_pCastle->Cnst(clicked)->Type(), 0);


                UpdateSize();
                Invalidate();
            } else {
            }
            m_selectedCnst = CTLCNST_INVALID;
            return;
        }
    }

    bool iBuildView::Process( fix32 t )
    {
        if(m_pAni) {
            if(m_pAni->Process(t)) {
                delete m_pAni;
                m_pAni = NULL;
            }
            Invalidate();
        }

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

        double m1 = (double)rc1.w * rc1.h / 2000, m2 = (double)rc2.w * rc2.h / 2000;

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


    void iBuildView::OnCompose()
    {
        iRect rc = AlignRect(iSize(480, 320), iRect(iPoint(), gApp.Surface().GetSize()), AlignCenter);
        gApp.Surface().Fill(cColor_Black);

        bool bDarken = m_viewMode != CVM_MAIN || gApp.ViewMgr().HasModalDlg() || m_bBackgroundMode;
        bool bBknd = m_bBackgroundMode || gApp.ViewMgr().HasModalDlg();
        //bDarken = false;
        if(!m_pCastle) return;
        iSortArray<listElem> compList; // a list of sprites to compose arranged by z-order

        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++)
        {
            if(m_pCastle->Cnst(xx)->IsBuilt()) {
                // if higher magic guild is built, skip this one
                if(m_pCastle->Cnst(xx)->Type() >= CTLCNST_MAGEGUILD_L1 && m_pCastle->Cnst(xx)->Type() <= CTLCNST_MAGEGUILD_L4) {
                    iCtlCnst* pHigherG = m_pCastle->Cnsts().GetCnst((CTL_CONSTR)(m_pCastle->Cnst(xx)->Type() + 1));
                    if((pHigherG && pHigherG->IsBuilt()))
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
            else if(!bBknd || (m_selectedCnst == CTLCNST_INVALID || (m_pCastle->Cnst(xx)->Type() == m_selectedCnst)))
            {

                if(m_pCastle->Cnst(xx)->GetState() == iCtlCnst::Available && m_viewMode == CVM_BUILD && !bBknd) {
                    uint32 idx;
                    listElem elem;
                    elem.sid = GetCtlSprite(m_pCastle->Cnst(xx)->Type(), idx);
                    elem.high = m_pCastle->Cnst(xx)->Type() == m_selectedCnst ? HLT_SELECTED : HLT_AVAILABLE;
                    elem.alpha = 255;
                    elem.ctlidx = xx;
                    compList.Insert(elem, idx);
                } else if(m_pCastle->Cnst(xx)->GetState() == iCtlCnst::NotEnoughRes && m_viewMode == CVM_BUILD && !bBknd) {
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
                gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), rc, cInvalidRect, iGfxManager::EfxNone, false, -1, compList[xx].value.alpha);
            }
        }

        gApp.Surface().Darken50Rect(rc);
        gApp.Surface().Darken50Rect(rc);

        // compose list: darkened sprites
        for(sint32 xx=compList.Size()-1; xx>=0; xx--)
        {
            if(compList[xx].value.high != HLT_DARKENED) {
                gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), rc, cInvalidRect, iGfxManager::EfxNone, false, -1, compList[xx].value.alpha);
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

                    gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), rc, cInvalidRect, efx, false, -1, compList[xx].value.alpha);
                    if(compList[xx].value.high == HLT_SELECTED || compList[xx].value.high == HLT_BUILT || compList[xx].value.high == HLT_TOUCHED)
                        gGfxMgr.BlitUniversal(compList[xx].value.sid, gApp.Surface(), rc, cInvalidRect, iGfxManager::EfxGlow, false, -1, compList[xx].value.alpha);
                    //gApp.Surface().FillRect(gGfxMgr.ObjRect(compList[xx].value.sid), 0, 128);
                }
            }
        }


        m_bldinfolist.RemoveAll();
//        iRect scrrect = iRect(0, 0, 480, 320);
        iRect scrrect = GetScrRect();


        // compose list: highlighted objects info
        if(!bBknd) {
            for(sint32 xx=compList.Size()-1; xx>=0; xx--) {
                if(compList[xx].value.high == HLT_AVAILABLE || compList[xx].value.high == HLT_NOTENOUGH || compList[xx].value.high == HLT_SELECTED) {
                    // info of a structure
                    iRect inforc = gTextComposer.GetTextSize(m_pCastle->Cnst(compList[xx].value.ctlidx)->Name(), castlebldfc);
                    iSize costsz = GetMineralSetComposeSize(m_pCastle->Cnst(compList[xx].value.ctlidx)->Price(), false, true);

                    inforc.h += costsz.h;

                    iRect objrect = gGfxMgr.ObjRect(compList[xx].value.sid);
                    inforc = AlignRect(inforc.size(), objrect, AlignCenter);

                    bldinfo bi;
                    bi.bldidx = compList[xx].value.ctlidx;
                    bi.rc = inforc + scrrect.point();
                    bi.text =  m_pCastle->Cnst(compList[xx].value.ctlidx)->Name();
                    bi.center = inforc.Center();
                    bi.cost = m_pCastle->Cnst(compList[xx].value.ctlidx)->Price();
                    bi.costsz = costsz;
                    bi.available = (compList[xx].value.high != HLT_NOTENOUGH);
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
            if(!gUseIpadUI)
            {
                if(m_bldinfolist[xx].crtype != CREAT_UNKNOWN) {
                    SpriteId sid = PDGG(MINIMON_GLOW) + m_bldinfolist[xx].crtype;

                    iSize tsz = gTextComposer.GetTextSize( m_bldinfolist[xx].text, castlebldfc);
                    iRect txt = AlignRect(tsz, m_bldinfolist[xx].rc, AlignTop);

                    iRect icRect = AlignRect(gGfxMgr.Dimension(sid), txt, AlignLeft);
                    icRect.x -= gGfxMgr.ObjRect(sid).w + 5;
                    gGfxMgr.Blit(sid,gApp.Surface(),icRect.point()-gGfxMgr.Anchor(sid));
                }
            }
            gTextComposer.TextOut(castlebldfc, gApp.Surface(), m_bldinfolist[xx].rc.point(), m_bldinfolist[xx].text, m_bldinfolist[xx].rc, AlignTop);
            if(!m_bldinfolist[xx].available)
                ComposeMineralSet(m_bldinfolist[xx].cost, m_pCastle->GetOwner()->Minerals(), AlignRect(m_bldinfolist[xx].costsz, m_bldinfolist[xx].rc, AlignBottom), false, false, false, true, false);
        }
#if 0
        if(m_viewMode != CVM_BUILD) {
            // castle name
            iRect rc = GetScrRect();
            //rc.y += 15;
            rc.y += gTextComposer.TextBoxOut(castletitlefc, gApp.Surface(), m_pCastle->Name(), rc);

            // resources
            ComposeResourcesBarGlow(rc, m_pCastle->GetOwner()->Minerals());
        }
#endif
    }


    ////////////////////


    iCastleView::iCastleView(): ICastleView(false, CV_OVERLAND), m_pCastle(NULL), m_bGestureComplete(false), m_bFalling(false) {

        AddChild(m_pScreenSlider = new iScreenSlider(&gApp.ViewMgr(), this, iRect(), 200));

        BUILD_SLIDE = m_pScreenSlider->AddSlide();
        RECRUIT_SLIDE = m_pScreenSlider->AddSlide();
        MARKET_SLIDE = m_pScreenSlider->AddSlide();
        MGUILD_SLIDE = m_pScreenSlider->AddSlide();
        TAVERN_SLIDE = m_pScreenSlider->AddSlide();
        KO_SLIDE = m_pScreenSlider->AddSlide();

        iSlide *pSlide = m_pScreenSlider->GetSlide(BUILD_SLIDE);
        pSlide->AddChild(m_pBuildView = new iBuildView(&gApp.ViewMgr(), iRect(), 99));

        pSlide = m_pScreenSlider->GetSlide(RECRUIT_SLIDE);
        pSlide->AddChild( m_pRecrView = new iCreatureRecruitView(&gApp.ViewMgr(), this, 113));

        pSlide = m_pScreenSlider->GetSlide(MARKET_SLIDE);
        pSlide->AddChild(m_pMarketView = new iMarketView(&gApp.ViewMgr(), this, iRect(), 110, false));

        pSlide = m_pScreenSlider->GetSlide(MGUILD_SLIDE);
        pSlide->AddChild(m_pMGuildView = new iMGuildView(&gApp.ViewMgr(), iRect(), 111));

        pSlide = m_pScreenSlider->GetSlide(TAVERN_SLIDE);
        pSlide->AddChild(m_pTDView = new iTavernDockView(&gApp.ViewMgr(), this, iRect(), 112));

        pSlide = m_pScreenSlider->GetSlide(KO_SLIDE);
        pSlide->AddChild(m_pKOView = new iKingdomOverviewView(&gApp.ViewMgr(), this, 113));

        Update();
    }

    void iCastleView::OnCompose()
    {
        iRect rc = GetScrRect();
        //if(m_pScreenSlider->CurrentSlide() != BUILD_SLIDE)
        //gApp.Surface().FillRect(rc, cColor_Black);
        m_pBuildView->SetBackgroundMode(m_pScreenSlider->CurrentSlide() != BUILD_SLIDE || m_bFalling);
        if(m_pScreenSlider->CurrentSlide() != BUILD_SLIDE) {
            m_pBuildView->OnCompose();
            //gApp.Surface().Darken50Rect(rc);
            //gApp.Surface().Darken50Rect(rc);
        }
    }

    void iCastleView::UpdateSize()
    {
        if(!m_pCastle) return;

        /*m_pPrevCastleBtn->SetRect(AlignChild(iSize(50, 50), AlignTop) - iPoint(300, 0));
         m_pNextCastleBtn->SetRect(AlignChild(iSize(50, 50), AlignTop) + iPoint(300, 0));
         */
        //m_pDismissBtn->SetRect(m_pArmyExchange->GetDismissRect() + m_pArmyExchange->GetPos());


        iRect rc = GetScrRect();
        m_pScreenSlider->SetRect(rc);
        m_pScreenSlider->UpdateSize();

        // 1: build screen
        iSlide *pSlide = m_pScreenSlider->GetSlide(BUILD_SLIDE);
        rc = pSlide->GetScrRect();
//        m_pBuildView->SetRect(iRect(rc.point(), iSize(480, 320)));
        m_pBuildView->SetRect(rc);
        m_pBuildView->UpdateSize();

        // 2: creature recruit screen
        pSlide = m_pScreenSlider->GetSlide(RECRUIT_SLIDE);
        rc = pSlide->GetScrRect();
        m_pRecrView->SetRect(rc);
        m_pRecrView->UpdateSize();

        // 3: market screen
        pSlide = m_pScreenSlider->GetSlide(MARKET_SLIDE);
        rc = pSlide->GetScrRect();

        m_pMarketView->SetRect(rc);
        m_pMarketView->UpdateSize();

        // 4: mguild screen
        pSlide = m_pScreenSlider->GetSlide(MGUILD_SLIDE);
        rc = pSlide->GetScrRect();

        m_pMGuildView->SetRect(rc);
        m_pMGuildView->UpdateSize();

        // 5: tavern screen
        pSlide = m_pScreenSlider->GetSlide(TAVERN_SLIDE);
        rc = pSlide->GetScrRect();
        m_pTDView->SetRect(rc);
        m_pTDView->UpdateSize();

        // 6: ko screen
        pSlide = m_pScreenSlider->GetSlide(KO_SLIDE);
        rc = pSlide->GetScrRect();
        m_pKOView->SetRect(rc);
        m_pKOView->UpdateSize();

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
        m_pBuildView->SetCastle(m_pCastle);
        m_pRecrView->SetCastle(m_pCastle);
        m_pMarketView->SetOwner(gGame.Map().FindPlayer(m_pCastle->Owner()));
        m_pMGuildView->SetCastle(m_pCastle);
        m_pTDView->SetCastle(m_pCastle);

        //m_pArmyExchange->SetTopOwner(m_pCastle, true);
        UpdateSize();
        Update();
        iStringT tmp = gMusicPath + castleThemes[m_pCastle->Proto()->Type()] + _T(".mp3");
        gMusicMgr.Play(tmp, true);
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

    void iCastleView::iCMDH_ControlCommand( iView* pView, CTRL_CMD_ID cmd, sint32 param )
    {
        uint32 uid = pView->GetUID();
        if(uid == 107)
            gGame.HideView(CV_CASTLE);
        else if(uid == 112) { // tavern dock view event: hero has been recruited or ship built
            Update();
            UpdateSize();
            m_pScreenSlider->SetCurrentSlide(RECRUIT_SLIDE);
        } else if(uid == 114) {
            m_pCastle->GetOwner()->PrevCastle();
        } else if(uid == 115) {
            m_pCastle->GetOwner()->NextCastle();
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
        else if(uid == 200) { // slider changed
            if(param == BUILD_SLIDE)
                gTutorial.Trigger(TUT_CASTLE_BUILDING);
            else if(param == MGUILD_SLIDE)
                gTutorial.Trigger(TUT_CASTLE_MAGIC);
            else if(param == MARKET_SLIDE)
                gTutorial.Trigger(TUT_CASTLE_MARKET);
            else if(param == RECRUIT_SLIDE)
                gTutorial.Trigger(TUT_CASTLE_RECRUIT);
            else if(param == TAVERN_SLIDE)
                gTutorial.Trigger(TUT_CASTLE_TAVERN);
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

    void iCastleView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        if(mID == 0) m_bGestureComplete = false;
    }

    void iCastleView::Update()
    {
        if(!m_pCastle) return;

        m_pCastle->UpdateCnsts();

        //m_pArmyExchange->SetBottomOwner(m_pCastle->Visitor(), true);


        /*	if(m_viewMode != CVM_BUILD) {
         for (uint32 xx=0; xx<m_pCastle->Cnsts().Count(); ++xx)
         if (iDwellCtlCnst* pDwel = DynamicCast<iDwellCtlCnst*>(m_pCastle->Cnst(xx)))
         if(pDwel->IsBuilt()) {
         break;
         }
         }
         */
        //m_pExitBtn->SetVisible(false);
        /*
         m_pPrevCastleBtn->SetVisible(m_pCastle->GetOwner()->CastleCount() > 1 && m_viewMode != CVM_BUILD && m_viewMode != CVM_EMPTY);
         m_pNextCastleBtn->SetVisible(m_pCastle->GetOwner()->CastleCount() > 1 && m_viewMode != CVM_BUILD && m_viewMode != CVM_EMPTY);
         */

        //m_pArmyExchange->SetVisible(false);//m_viewMode != CVM_BUILD && m_viewMode != CVM_EMPTY);
        /*m_pDismissBtn->SetVisible(m_pArmyExchange->IsVisible() && m_pArmyExchange->BottomArmyList()->ArmyOwner());

         m_pDismissBtn->SetVisible(m_pArmyExchange->IsVisible() && m_pArmyExchange->BottomArmyList()->ArmyOwner());
         */
        m_pScreenSlider->EnableSlide(MARKET_SLIDE, m_pCastle->FindCnst(CTLCNST_MARKETPLACE)->IsBuilt());
        m_pScreenSlider->EnableSlide(MGUILD_SLIDE, m_pCastle->IsMGuildBuilt());
        m_pScreenSlider->EnableSlide(TAVERN_SLIDE, m_pCastle->FindCnst(CTLCNST_TAVERN)->IsBuilt());

        m_pBuildView->Update();
        m_pMarketView->Update();
        m_pMGuildView->Update();
        m_pRecrView->Update();
        m_pTDView->Update();
    }



    void iCastleView::OnActivate(bool bActivate)
    {
        Update();
        if(bActivate) {
            // gTutorial.Trigger(TUT_CASTLE);

            m_pScreenSlider->SetCurrentSlide(BUILD_SLIDE);

            m_pBuildView->SetViewMode(iBuildView::CVM_MAIN);
            m_bFalling = true;
            FallDown();


            gTutorial.ShowFingerHelp();

            bool bCanBuild = m_pCastle->CanBuild();
            bool bAllblt = true;
            for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++) {
                if(m_pCastle->Cnst(xx)->GetState() != iCtlCnst::Built)
                    bAllblt = false;
            }
            if(bAllblt) bCanBuild = false;
            m_pBuildView->SetViewMode(bCanBuild ? iBuildView::CVM_BUILD : iBuildView::CVM_MAIN);
            m_pRecrView->SetButtonSelected(-1);
            m_bFalling = false;

        } else {
            m_bTracking[0] = false; // this is a quick and dirty fix for the problem of building being auto-tapped after the castle screen falling from the top. Hedin 10.03.11
            // the real solution is to make sure this variable gets auto falsed automatically, refer to the comment in gxl.view.cpp, iView::MouseDown
        }
    }


    void iCastleView::OnModalDlgAdded()
    {
        m_pBuildView->OnModalDlgAdded();
    }


    void iCastleView::OnModalDlgRemoved()
    {
        m_pBuildView->OnModalDlgRemoved();
    }

    bool iCastleView::Process(fix32 t)
    {
        return m_pBuildView->Process(t);
    }

    void iCastleView::SetScreen(CastleScreen scr)
    {
        switch(scr) {
            case RecruitScreen: m_pScreenSlider->SetCurrentSlide(RECRUIT_SLIDE); break;
            case TavernScreen: m_pScreenSlider->SetCurrentSlide(TAVERN_SLIDE); break;
            case MarketScreen: m_pScreenSlider->SetCurrentSlide(MARKET_SLIDE); break;
            case MGuildScreen: m_pScreenSlider->SetCurrentSlide(MGUILD_SLIDE); break;
        }
    }

    void iCastleView::SetRecruitCreatureType(CREATURE_TYPE ct)
    {
        m_pRecrView->SetCreatureTypeSelected(ct);
    }


    iKingdomOverviewView::iKingdomOverviewView(iViewMgr* pViewMgr, IViewCmdHandler *pCmdHandler, uint32 uid)
    : iView(pViewMgr, iRect(), GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler)
    {
    }


    void iKingdomOverviewView::OnCompose()
    {
        iRect rc = GetScrRect();

        /*   gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(), rc, gTextMgr[TRID_KINGDOM_OVERVIEW], rc, AlignCenter);
         rc.y += gTextComposer.GetFontHeight(dlgfc_hdr.fontSize);;
         */
        gTextComposer.TextOut(dlgfc_stopic, gApp.Surface(), rc, gTextMgr[TRID_DAILY_INCOME], rc, AlignCenter);
        rc.y += gTextComposer.GetFontHeight(dlgfc_hdr.fontSize);

        iMineralSet ms;
        ms.Reset();
        iPlayer* player = gGame.Map().CurPlayer();
        for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) ms.quant[xx] += player->FurtSkills().Value((FURTHER_SKILLS)(FSK_MIN_GOLD+xx));
        for (iPlayer::iCHLIt hit = player->HeroFirst(); hit != player->HeroEnd(); ++hit) {
            for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) ms.quant[xx] += (*hit)->ConstFurtSkills().Value((FURTHER_SKILLS)(FSK_MIN_GOLD+xx)) + (*hit)->VarFurtSkills().Value((FURTHER_SKILLS)(FSK_MIN_GOLD+xx));
        }

        iRect trc = AlignRect(GetMineralSetComposeSize(ms, false, true, true), rc, AlignCenter);
        ComposeMineralSet(ms, ms, trc, false, false, false, true, true);

    }

    void iKingdomOverviewView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {

    }


    class iRecrBtn: public iButton
    {
    public:
        iRecrBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 state):
        iButton(pViewMgr, pCmdHandler, rect, uid, state)
        , m_ct(CREAT_UNKNOWN)
        , m_bSelected(false)
        {}
        void OnCompose() {
            iRect rc = GetScrRect();
            gApp.Surface().FrameRect(rc, cColor_White);
            if(m_ct == CREAT_UNKNOWN) return;
            iSize sz;
            iPoint crc;
            iTextComposer::FontConfig fc(creatqfc);
            uint8 alpha;
            sint16 crsid;
            if(m_bSelected || IsMouseDown(0)) {
                crsid = PDGG(MINIMON_GLOW) + m_ct;
                sz = gGfxMgr.Dimension(crsid);
                crc = GetRealSpritePos(crsid, rc, false, AlignCenter);

                sint16 sid = IsMouseDown(0) ? PDGG(BTN_CASTLE_RECRUIT) : PDGG(BTN_CASTLE_RECRUIT_PRESSED);
                sz = gGfxMgr.Dimension(sid);
//                gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignTop) + iPoint(1, 0));
                alpha = 255;
                fc.cmpProps.faceColor = cColor_White;
            } else {
                crsid = PDGG(MINIMON) + m_ct;
                sz = gGfxMgr.Dimension(crsid);
                crc = GetRealSpritePos(crsid, rc, false, AlignCenter);

                alpha = 255;
                fc.cmpProps.faceColor = cColor_White;
            }

            gGfxMgr.BlitUniversal(crsid, gApp.Surface(), crc, cInvalidRect, IsEnabled() ? iGfxManager::EfxNone : iGfxManager::EfxGlow, false, -1, alpha);
            iStringT txt = iFormat(_T("%d"), m_amount);
            sz = gTextComposer.GetTextSize(txt, fc);
            gTextComposer.TextOut(fc, gApp.Surface(), AlignRect(sz, rc, AlignRight), txt);

        }
        inline void SetSelected(bool bSelected) { m_bSelected = bSelected; }
        inline bool IsSelected() const { return m_bSelected; }
        inline void SetCreatureType(CREATURE_TYPE ct) { m_ct = ct; }
        inline void SetAmount(sint32 amount) { m_amount = amount; }
        inline CREATURE_TYPE GetCreatureType() const { return m_ct; }
    private:
        CREATURE_TYPE m_ct;
        sint32		  m_amount;
        bool		  m_bSelected;
    };



    iCreatureRecruitView::iCreatureRecruitView(iViewMgr* pViewMgr, IViewCmdHandler *pCmdHandler, uint32 uid)
    : iView(pViewMgr, iRect(), GENERIC_VIEWPORT, uid, Visible|Enabled), m_pCmdHandler(pCmdHandler), m_pCastle(NULL)
    {
        AddChild(m_pAmount = new iAmountChooserGlow(pViewMgr, this, iRect(), 100
#ifndef PC_VERSION
                                                    , true
#endif
                                                    ));

        AddChild(m_pRecruitBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 105, PDGG_IPHONE(BTN_OK_DELUXE), -1));

        AddChild(m_pBuyGoldBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 1100, PDGG(BTN_BUY_DELUXE), PDGG(BTN_DELUXE_PRESSED)));

        AddChild(m_pInfoBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 106, PDGG(BTN_INFO_DELUXE), PDGG(BTN_DELUXE_PRESSED)));

        AddChild(m_pArmyExchange = new iArmyExchange(&gApp.ViewMgr(), this, iRect(), 110, true, true
#ifndef PC_VERSION
                                                     , false, false, iSize(50, 35)
#endif
                                                     ));
        AddChild(m_pSplitBtn = new iCheckButton(&gApp.ViewMgr(), this, iRect(), PDGG_IPHONE(BTN_SPLIT_SMALL), -1, 102));
        m_pArmyExchange->SetSplitBtn(m_pSplitBtn);

        for(uint32 xx=0; xx<6; xx++)
        {
            AddChild(m_pCrBtns[xx] = new iRecrBtn(&gApp.ViewMgr(), this, iRect(), 150 + xx, Visible|Enabled));
        }

        m_pAmount->SetVisible(false);
        m_pInfoBtn->SetVisible(false);
    }


    void iCreatureRecruitView::UpdateSize()
    {
        iSize sz = m_pArmyExchange->GetDefSize(2);
        m_pArmyExchange->SetRect(AlignChild(sz, AlignBottom) + iPoint(0, sz.h / 4 - 5));

        bool bVis = m_pCastle && m_pCastle->Visitor();

        if(!bVis)
            m_pArmyExchange->SetPos(m_pArmyExchange->GetPos() + iPoint(0, 22));

        m_pArmyExchange->UpdateSize();

        iRect arc = m_pArmyExchange->GetRect();
        if(m_pSplitBtn)
        {
            m_pSplitBtn->SetRect(iRect(iPoint(arc.x2() - iIphoneBtn::sm_defSize.w + 8, arc.y + 29), iIphoneBtn::sm_defSize));
            if(!bVis) {
                m_pSplitBtn->SetPos(m_pSplitBtn->GetPos() + iPoint(0, 21));
            }
        }
        iRect rc = iRect(0, 0, m_Rect.w, m_pArmyExchange->GetRect().y);
        m_pInfoBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, rc, AlignBottom) - iPoint(iIphoneBtn::sm_defSize.w
#ifdef OS_IPHONE
                                                                                        +17
#endif
                                                                                        , 10));
        m_pRecruitBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, rc, AlignBottom) + iPoint(iIphoneBtn::sm_defSize.w, -10));

        m_pBuyGoldBtn->SetRect(AlignRect(iIphoneBtn::sm_defSize, rc, AlignBottom) + iPoint(iIphoneBtn::sm_defSize.w * 2, -10));
        rc.DeflateRect(0, 0, 0, iIphoneBtn::sm_defSize.h
#ifdef OS_IPHONE
                       - 10
#endif
                       );
        m_pAmount->SetRect(AlignRect(iAmountChooserGlow::sm_defSize, rc, AlignBottom));
        m_pAmount->UpdateSize();
        sz = iSize(64 * 6 + 7 * 5, 42);
        rc = AlignChild(sz, AlignTop);
        iRect crc = iRect(rc.x + 1, 0, 65, rc.h);
        for(uint32 xx=0; xx<6; xx++) {
            m_pCrBtns[xx]->SetRect(crc);
            crc.x += 71;
        }
    }

    void iCreatureRecruitView::OnCompose()
    {
        iRect rc = GetScrRect();
        //	gApp.Surface().FrameRect(m_pAmount->GetScrRect(), cColor_Red);
        //	gApp.Surface().FrameRect(m_pArmyExchange->GetScrRect(), cColor_Red);

        sint32 sid = PDGG(BTN_CASTLE_RECRUIT);
        iRect trc = AlignRect(gGfxMgr.Dimension(sid), rc, AlignTop);
//        gGfxMgr.Blit(sid, gApp.Surface(), trc);

        trc.y += trc.h - 30;
        trc = iRect(rc.x, trc.y, m_Rect.w, m_pAmount->GetRect().y - trc.y);
        CREATURE_TYPE ct = GetSelectedCreatureType();

        // available resources
        const iMineralSet& ms = gGame.Map().FindPlayer(m_pCastle->Owner())->Minerals();
        iRect mrc = AlignRect(GetMineralSetComposeSize(ms, false, true, true), iRect(rc.x, rc.y, rc.w, m_pRecruitBtn->GetRect().y2()), AlignBottom);
        mrc.y += DEF_BTN_HEIGHT - 12;
        ComposeMineralSet(ms, ms, mrc, false, false, false, true, true);


        if(ct != CREAT_UNKNOWN) {
            // show unavailable sign
            if(gGame.Map().FindPlayer(m_pCastle->Owner())->Minerals().Has(CREAT_DESC[ct].cost) <= 0) {
                sint16 sid = PDGG(RES_ICONS_GLOW) + MINERAL_COUNT;
                iSize sz = gGfxMgr.Dimension(sid);
                gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(sz, rc, AlignCenter));
            }
            else {
                //	gApp.Surface().FrameRect(trc, cColor_Red);
                sid = PDGG(MINIMON_GLOW) + ct;
                iRect icRect = AlignRect(gGfxMgr.Dimension(sid), trc, AlignTop);
                gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, icRect, false, AlignBottom));
                iStringT txt = iFormat(_T("%d"), m_pAmount->CurPos());
                iSize sz = gTextComposer.GetTextSize(txt, castletitlefc);
                gTextComposer.TextOut(castletitlefc, gApp.Surface(), icRect.point() - iPoint(55, 0) + iPoint(0, icRect.h - sz.h), txt);

                if(m_pAmount->CurPos() > 0) {
                    trc = iRect(m_pInfoBtn->GetScrPos(), iPoint(m_pRecruitBtn->GetScrRect().x2(), m_pRecruitBtn->GetScrRect().y2()))
#ifdef OS_IPHONE
                    + iPoint(8,0)
#endif
                    ;
                    iMineralSet ms = CREAT_DESC[ct].cost * m_pAmount->CurPos();
                    gTextComposer.TextOut(creatinfofc_qty, gApp.Surface(), iPoint(), MineralSet2Text(ms, false), trc, AlignCenter);
                }
            }
        }
    }

    void iCreatureRecruitView::RecruitCreatures(uint32 count)
    {
        CREATURE_TYPE ct = GetSelectedCreatureType();
        if(ct == CREAT_UNKNOWN) return;

        iMineralSet cost = CREAT_DESC[ct].cost;
        // Check space in target army
        if (m_pCastle->Garrison().CanAddGroup(ct)) {
            m_pCastle->AddGroupToArmy(ct, count);
            m_pCastle->GetDwelling(ct)->CrCount() -= count;
            if(m_pCastle->GetDwelling(ct)->CrCount() <= 0) {
                SetButtonSelected(-1);
            }

            gGame.Map().FindPlayer(m_pCastle->Owner())->AddMineralSet(-cost * count);

            m_pAmount->SetMax(iMIN<sint32>(m_pCastle->GetDwelling(ct)->CrCount(), gGame.Map().FindPlayer(m_pCastle->Owner())->Minerals().Has(cost)));
            m_pAmount->SetCurPos(0);
#ifdef	MULTIPLAYER
            gMPMgr.OnCastleUpdate(m_pCastle);
#endif
        } else {
            iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_NO_ROOM], PID_NEUTRAL);
            tdlg.DoModal();
        }
        Update();
        Invalidate();
    }

    void iCreatureRecruitView::SetCreatureTypeSelected(CREATURE_TYPE ct)
    {
        for(uint32 xx=0; xx<6; xx++) {
            m_pCrBtns[xx]->SetSelected(m_pCrBtns[xx]->GetCreatureType() == ct);
        }
        m_pAmount->SetVisible(ct != CREAT_UNKNOWN && m_pAmount->GetMax() > 0);
        m_pInfoBtn->SetVisible(m_pAmount->IsVisible());
    }

    void iCreatureRecruitView::SetButtonSelected(sint32 idx)
    {
        for(uint32 xx=0; xx<6; xx++) {
            m_pCrBtns[xx]->SetSelected(xx == idx);
        }
        m_pAmount->SetVisible(idx != -1 && m_pAmount->GetMax() > 0);
        m_pInfoBtn->SetVisible(m_pAmount->IsVisible());
    }

    void iCreatureRecruitView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if(uid >= 150 && uid <= 155) { 	// creature cell click
            CREATURE_TYPE ct = ((iRecrBtn*)pView)->GetCreatureType();
            if(ct == CREAT_UNKNOWN) return;
            if(cmd == CCI_BTNCLICK) {	 // creature cell single click
                SetButtonSelected(uid - 150);
                Update();
            } else if(cmd = CCI_BTNDBLCLICK) { // creature cell double click
                SetButtonSelected(uid - 150);
                Update();
                RecruitCreatures(m_pAmount->GetMax());
            }
        } else if(uid == 105) {	// recruit button
            RecruitCreatures(m_pAmount->CurPos());
        } else if(uid == 100) { // amount chooser movement
            m_pRecruitBtn->SetVisible(m_pAmount->CurPos() > 0);
        } else if(uid == 106) { // info btn click
            iArmyOwner fake;
            CREATURE_TYPE ct = GetSelectedCreatureType();
            if(ct == CREAT_UNKNOWN) return;
            fake.AddGroupToArmy(ct, 1);
            iDlg_CreatInfo dlg(&gApp.ViewMgr(), PID_NEUTRAL, &fake, 0, iFurtSkills(), false, 0, iCreatBonusList());
            dlg.DoModal();
        }  else if(uid == 1100)
        {
            // gMapShop.InitiateBuy("uk.co.euroskore.pk2_remastered.goldpack");
        }
    }

    void iCreatureRecruitView::SetCastle( iCastle* pCastle )
    {
        m_pCastle = pCastle;
        Update();
        Invalidate();
    }

    CREATURE_TYPE iCreatureRecruitView::GetSelectedCreatureType() const
    {
        for(uint32 xx=0; xx<6; xx++)
            if(m_pCrBtns[xx]->IsSelected())
                return m_pCrBtns[xx]->GetCreatureType();
        return CREAT_UNKNOWN;
    }

    void iCreatureRecruitView::Update()
    {
        if(!this) return;

        if(m_pArmyExchange)
        {
            m_pArmyExchange->SetTopOwner(m_pCastle, true);
            m_pArmyExchange->SetBottomOwner(m_pCastle->Visitor(), true);
        }

        uint32 btn = 0;
        bool bDone = false;

        // first, clear old buttons
        for(uint32 xx=0; xx<6; xx++)
        {
            m_pCrBtns[xx]->SetCreatureType(CREAT_UNKNOWN);
        }

        for(uint32 xx=0; xx<m_pCastle->Cnsts().Count(); xx++) {
            if(iDwellCtlCnst* cnst = dynamic_cast<iDwellCtlCnst*>(m_pCastle->Cnst(xx))) {
                CREATURE_TYPE ct = cnst->IsBuilt() ? cnst->CrType() : CREAT_UNKNOWN;
                m_pCrBtns[btn]->SetCreatureType(ct);
                m_pCrBtns[btn]->SetAmount(cnst->CrCount());
                m_pCrBtns[btn]->SetEnabled(cnst->CrCount() > 0);
                if(m_pCrBtns[btn]->IsSelected() && ct != CREAT_UNKNOWN) {
                    iMineralSet cost = CREAT_DESC[ct].cost;
                    m_pAmount->SetMax(iMIN<sint32>(m_pCastle->GetDwelling(ct)->CrCount(), gGame.Map().FindPlayer(m_pCastle->Owner())->Minerals().Has(cost)));
                    m_pAmount->SetCurPos(0);
                    bDone = true;
                }
                btn++;
            }
        }
        if(!bDone) {
            m_pAmount->SetMax(0);
        }
        m_pAmount->SetVisible(m_pAmount->GetMax() > 0);
        m_pInfoBtn->SetVisible(m_pAmount->IsVisible());

        m_pRecruitBtn->SetVisible(false);
        m_pInfoBtn->SetVisible(m_pAmount->IsVisible());
    }


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
        //	rc.y += 30;
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
#ifdef PC_VERSION
        m_pRecrHeroView->SetHighlighted(flag);
#endif
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
        AddChild(m_pBtnBuild = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSize, AlignBottom) + iPoint(bw, -35), 100, PDGG(BTN_BUILD_DELUXE), -1));
        AddChild(m_pBtnCancel = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSize, AlignBottom) - iPoint(bw, 35), 101, PDGG(BTN_CANCEL_DELUXE), -1));
    }

    void iDlg_BuildCnstCnfrm::UpdateSize()
    {
        Center();

        sint32 bw = iIphoneBtn::sm_defSizeDeluxe.w;
        m_pBtnBuild->SetRect( AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(bw, -35) );
        m_pBtnCancel->SetRect( AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) - iPoint(bw, 35) );
    }

    void iDlg_BuildCnstCnfrm::DoCompose( const iRect& rect )
    {
        iRect rc(rect);
        // gApp.Surface().FrameRect(rc, cColor_Red);
        //	rc.y += iBaseGameDlg::GetTopOffset();

        if(iDwellCtlCnst* pCnst = dynamic_cast<iDwellCtlCnst*>(m_pCnst)) {
            iSize tsz = gTextComposer.GetTextSize(m_pCnst->Name(), dlgfc_hdr);
            iRect txt = AlignRect(tsz, rc, AlignTop);
            iRect crc = AlignRect(iSize(55, 55), txt, AlignLeft);

            SpriteId sid = PDGG(MINIMON_GLOW) + pCnst->CrType();
            gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, crc - iPoint(55, 0) - iPoint(0, crc.h - tsz.h - 20), false, AlignBottom));

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

        rc.y += DEF_BTN_HEIGHT + 5;

        const iMineralSet& ms = gGame.Map().FindPlayer(m_pCnst->Owner()->Owner())->Minerals();
        iRect trc = AlignRect(GetMineralSetComposeSize(ms, false, true, true), rc, AlignTop);
        ComposeMineralSet(ms, ms, trc, false, false, false, true, true);

        rc.y += trc.h;
    }

    iSize iDlg_BuildCnstCnfrm::ClientSize() const
    {
        sint32 w = iBaseGameDlg::GetDefaultWidth();
        sint32 h = 0;

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

        // minerals
        h += 30;

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
