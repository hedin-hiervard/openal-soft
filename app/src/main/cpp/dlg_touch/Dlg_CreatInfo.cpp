#include "stdafx.h"
#include "Dlg_CreatInfo.h"
#include "helpers.h"

//////////////////////////////////////////////////////////////////////////
class iPerksInfoDlg : public iBaseGameDlg
{
public:
    iPerksInfoDlg(iViewMgr* pViewMgr, CREATURE_TYPE ct)
    : iBaseGameDlg(pViewMgr), m_ct(ct)
    {
        // Init perks list
        for (uint16 nn=0; nn<CPERKS_COUNT; ++nn) {
            if (CREAT_DESC[ct].perks & (1<<nn)) m_perks.Add(nn);
        }
    }
    
    void OnCreateDlg()
    {
        iRect clRect = ClientRect();
        //AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
    }
    
protected:
    void DoCompose(const iRect& clRect)
    {
        iRect rc(clRect);
        rc.y += iBaseGameDlg::GetTopOffset();
        
        // title
        rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[TRID_CREATURE_PEASANT_F2+m_ct*3], rc);
        
        
        // perks
        for (uint32 nn=0; nn<m_perks.GetSize(); ++nn) {
            sint32 h = gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[TRID_CPERK_DOUBLESHOT+m_perks[nn]], iRect(rc.x+30,rc.y, rc.w-30, rc.h));
            BlitIcon(gApp.Surface(), PDGG(CREAT_PERKS)+m_perks[nn], iRect(rc.x,rc.y,30,h));
            rc.y += h;
            rc.y += 25;
        }
    }
    
    iSize ClientSize() const
    {
        sint32 w = iBaseGameDlg::GetDefaultWidth();
        sint32 h = iBaseGameDlg::GetTopOffset();
        
        // title
        h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_CREATURE_PEASANT_F2+m_ct*3], w, dlgfc_hdr).h;
        
        // perks
        for (uint32 nn=0; nn<m_perks.GetSize(); ++nn) {
            h += gTextComposer.GetTextBoxSize(gTextMgr[TRID_CPERK_DOUBLESHOT+m_perks[nn]], w, dlgfc_plain).h;
            h += gTextComposer.GetFontHeight(dlgfc_plain.fontSize);
        }
        
        return iSize(w,h);
    }
    
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        EndDialog(pView->GetUID());
    }
    
    void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {
        EndDialog(0);
    }
    
    iSimpleArray<uint16>	m_perks;
    CREATURE_TYPE			m_ct;
};

class iTooltipablePerksView
{
public:
    virtual void OnMouseEntered(const iPoint& pos, MouseId mID)=0;
    virtual void OnMouseLeave(const iPoint& pos, MouseId mID)=0;
};

iStringT perkmsg = iStringT();
//////////////////////////////////////////////////////////////////////////
class iPerksBtn : public iButton, public iTooltipablePerksView
{
public:
    iPerksBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, sint16 sid, CREATURE_TYPE ct)
    : iButton(pViewMgr, pCmdHandler, rect, uid, Visible | Enabled), sId(sid), m_ct(ct)
    {
        
    }
    
    void SetType(CREATURE_TYPE ct) {
        
        // Init perks list
        m_perks.RemoveAll();
        for (uint16 nn=0; nn<CPERKS_COUNT; ++nn) {
            if (CREAT_DESC[ct].perks & (1<<nn)) m_perks.Add(nn);
        }
        m_ct = ct;
        Invalidate();
    }
    
    void OnCompose()
    {
        iRect rc = GetScrRect();
        gGfxMgr.Blit(PDGG(CREAT_PERKS)+sId, gApp.Surface(), iPoint(rc.x, rc.y));
    }
    
    void OnMouseEntered(const iPoint& pos, MouseId mID){
        
        perkmsg = gTextMgr[TRID_CPERK_DOUBLESHOT+sId];
        return;
    }
    
    void OnMouseLeave(const iPoint& pos, MouseId mID){
        
        perkmsg = _T("");
        return;
    }
    
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
    }
    
    CREATURE_TYPE			m_ct;
    sint16                  sId;
    iSimpleArray<uint16>	m_perks;
};
//////////////////////////////////////////////////////////////////////////

void iCreatDescView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
#ifndef PC_VERSION
    iPerksInfoDlg idlg(m_pMgr, m_type);
    idlg.DoModal();
#endif
}

iCreatDescView::iCreatDescView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, iRect rc, uint32 uid, uint32 flags, iBattleGroup *pGroup)
: iView(pViewMgr, rc, GENERIC_VIEWPORT,uid,Enabled|Visible), m_pCmdHandler(pCmdHandler),
m_type(CREAT_UNKNOWN), m_pGroup(pGroup), m_flags(flags), m_ms(NULL), m_bCost(false), m_bEntered(false)
{
#ifdef PC_VERSION
    iRect clRect = GetScrRect();
    sint32 top = clRect.y + 15;
    sint32 w = 320;
    top += gTextComposer.GetTextBoxSize(gTextMgr[m_type*3+TRID_CREATURE_PEASANT_F2], w, dlgfc_hdr).h;
    top += 55;
    if(m_flags & CDV_COST) {
        if(!(m_flags & CDV_GLOW))
            top += gTextComposer.GetTextBoxSize(iStringT(_T("#FFED")) + MineralSet2Text(GetCost() * iMAX(m_amount, 1)),
                                                w, rewardqfc).h;
        else {
            top += GetMineralSetComposeSize(GetCost() * iMAX(m_amount, 1), true).h;
        }
    }
#endif
}

void iCreatDescView::OnCompose()
{
    iRect rc = GetScrRect();
   	
    if(m_flags & CDV_BKND){
#ifndef PC_VERSION
        rc.y += iBaseGameDlg::GetTopOffset();
        rc.w += 100;
        rc.h -= 50;
        ComposeGlowBkgnd(gApp.Surface(), rc);
#endif
        ComposeGlowBkgnd(gApp.Surface(), rc);
    }
    
    if(m_type == CREAT_UNKNOWN) return;
    
    rc.y += iBaseGameDlg::GetTopOffset();
    
    iTextComposer::FontConfig hdrfc = dlgfc_hdr;
    iTextComposer::FontConfig titlefc = castletitlefc;
    iTextComposer::FontConfig normalfc = creatinfofc_normal;
    iTextComposer::FontConfig smallfc = creatinfofc_small;
    iTextComposer::FontConfig qtyfc = creatinfofc_qty;
    
    if(m_flags & CDV_BKND) {
    }
    
    // title
    if(m_flags & CDV_TITLE) {
        rc.y += gTextComposer.TextBoxOut(hdrfc, gApp.Surface(), gTextMgr[m_type*3+TRID_CREATURE_PEASANT_F2], rc);
#ifdef PC_VERSION
        rc.y += iBaseGameDlg::GetTopOffset();
#endif
    }
    
    // icon
    if(!(m_flags & CDV_GLOW)) {
        ComposeCreatureCell(gApp.Surface(), AlignRect(CreatureCellSize(), rc, AlignTop), m_type, m_amount, VL_EXPERT);
        rc.y += CreatureCellSize().h;
    } else {
        
        SpriteId sid = PDGG(MINIMON_GLOW) + m_type;
#ifdef PC_VERSION
        iRect icRect = AlignRect(iSize(55, 40), rc, AlignTop);
#else
        iRect icRect = AlignRect(gGfxMgr.Dimension(sid), rc, AlignTop);
#endif
        gGfxMgr.Blit(sid, gApp.Surface(), GetRealSpritePos(sid, icRect, false, AlignBottom));
        
        iStringT txt = iFormat(_T("%d"), m_amount);
        iSize sz = gTextComposer.GetTextSize(txt, titlefc);
        gTextComposer.TextOut(titlefc, gApp.Surface(), icRect.point() - iPoint(5 + sz.w, 0) + iPoint(0, icRect.h - sz.h), txt);
#ifdef PC_VERSION
        rc.y += 55;
#else
        rc.y += icRect.h;
#endif
    }
    
    // props
    
    iRect hrz, left, right;
    iStringT tout;
#ifndef PC_VERSION
    if( m_flags & CDV_STATS ){
#endif
        hrz = AlignRect(iSize(rc.w, gTextComposer.GetFontHeight(normalfc.fontSize)), rc, AlignTop);
        left = AlignRect(iSize(hrz.w / 2, hrz.h), hrz, AlignLeft);
        right = AlignRect(iSize(hrz.w / 2, hrz.h), hrz, AlignRight);
        
        // attack
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_ATTACK_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        tout.Setf(_T("%d"), m_pGroup ? m_pGroup->AttackSkill() : CREAT_DESC[m_type].attack);
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        // defence
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_DEFENCE_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        tout.Setf(_T("%d"), m_pGroup ? m_pGroup->DefenceSkill() : CREAT_DESC[m_type].defence);
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        // shots
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_SHOTS_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        if (CREAT_DESC[m_type].shots)
            if(m_pGroup) {
                tout.Setf(_T("%d/%d"), m_pGroup->Shots(), CREAT_DESC[m_type].shots);
            } else
                tout.Setf(_T("%d"),CREAT_DESC[m_type].shots);
            else tout.Setf(_T("-"));
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        // damage
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_DAMAGE_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        if(m_pGroup) {
            if(m_pGroup->MinDamage() == m_pGroup->MaxDamage())
                tout.Setf(_T("%d"),m_pGroup->MaxDamage());
            else
                tout.Setf(_T("%d - %d"),m_pGroup->MinDamage(), m_pGroup->MaxDamage());
        } else
            tout.Setf(_T("%d - %d"),CREAT_DESC[m_type].damage_min,CREAT_DESC[m_type].damage_max);
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        // health
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_HEALTH_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        if(m_pGroup)
            tout.Setf(_T("%d/%d"), m_pGroup->HitPointsLeft(), m_pGroup->HitPoints());
        else
            tout.Setf(_T("%d"),CREAT_DESC[m_type].hits);
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        // speed
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_SPEED_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        if(m_pGroup)
            tout.Setf(_T("%d (%d)"), CREAT_DESC[m_type].speed, m_pGroup->Speed());
        else
            tout.Setf(_T("%d"),CREAT_DESC[m_type].speed);
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        rc.y = left.y;
#ifndef PC_VERSION
    }
    
    iSimpleArray<iSize> szs;
#endif
    
    // Perks
    iSimpleArray<uint16> perks;
    for (uint16 nn=0; nn<CPERKS_COUNT; ++nn) {
        if (CREAT_DESC[m_type].perks & (1<<nn)) {
            perks.Add(nn);
#ifndef PC_VERSION
            szs.Add(iIphoneBtn::sm_defSizeDeluxe);
#endif
        }
        
    }
    
    sint32 ht = gTextComposer.GetFontHeight(smallfc.fontSize);
    
#if 0
    //    if( m_flags & CDV_LINEAR_PERKS )
    //        rc.y += GetRectsHeight(rc.w, szs);
#else
    if(m_flags & CDV_LINEAR_PERKS){
        
        uint32 w=0;
        
        for (uint32 nn=0; nn<perks.GetSize(); ++nn)
            w = iMAX(w, gTextComposer.GetTextSize(gTextMgr[TRID_CPERK_DOUBLESHOT+perks[nn]], smallfc).w);
        
        for (uint32 nn=0; nn<perks.GetSize(); ++nn) {
            
            rc.y += 20;
            
            iRect tmprc = iRect(rc.x, rc.y + (ht * floor((double)nn)), rc.w, ht);
            iSize sz = gTextComposer.GetTextSize(gTextMgr[TRID_CPERK_DOUBLESHOT+perks[nn]], smallfc);
            
            iRect txtrc;
            
            txtrc = AlignRect(iSize(w,sz.h), tmprc, AlignCenter);
            
            txtrc = iRect( txtrc.x1()+30, txtrc.y1(),sz.w, txtrc.size().h );
            
            sint32 h = gTextComposer.TextBoxOut(creatinfofc_small, gApp.Surface(), gTextMgr[TRID_CPERK_DOUBLESHOT+perks[nn]], txtrc);
            BlitIcon(gApp.Surface(), PDGG(CREAT_PERKS)+perks[nn], iRect(txtrc.point()+ iPoint(-35, 0), iSize(0, 20)), 140);
        }
    }
    else if(m_flags & CDV_LINEAR_BATTLE_PERKS)
    {
        for (uint32 nn=0; nn<perks.GetSize(); ++nn) {
            
            rc.y += 20;
            
            uint32 pw = gGfxMgr.Dimension( PDGG(CREAT_PERKS)+perks[nn] ).w;
            
            iRect tmprc = iRect(rc.x, rc.y + (ht * floor((double)nn)), rc.w, ht);
            iSize sz = gTextComposer.GetTextSize(gTextMgr[TRID_CPERK_DOUBLESHOT+perks[nn]], smallfc) + iSize(pw+10,0);
            
            iRect txtrc;
            
            txtrc = AlignRect(sz, tmprc, AlignLeft);
            
            txtrc = iRect( txtrc.x1()+pw+5, txtrc.y1(), txtrc.size().w, txtrc.size().h );
            
            BlitIcon(gApp.Surface(), PDGG(CREAT_PERKS)+perks[nn], iRect(txtrc.point(), iSize(0, 20)), 140);
            
            txtrc.x += 5;
            
            sint32 h = gTextComposer.TextBoxOut(creatinfofc_small, gApp.Surface(), gTextMgr[TRID_CPERK_DOUBLESHOT+perks[nn]], txtrc);
        }
        
        rc.y += perks.GetSize() * ht;
    } else if( m_flags & CDV_NOTITLE_PERKS ){
        
        rc.y += 25;
        
        
        if( m_bEntered ){
            
            iSize perksz = gGfxMgr.Dimension(PDGG(CREAT_PERKS));
            
            iRect tmprc = iRect(rc.point(), iSize(rc.w, perksz.h));
            tmprc.y += perksz.h + 20;
            tmprc = AlignRect(gTextComposer.GetTextSize(perkmsg, smallfc), tmprc, AlignCenter);
            
            gTextComposer.TextOut(smallfc, gApp.Surface(),tmprc.point(), perkmsg, tmprc, AlignCenter);
        }
    }else{
        if(gUseIpadUI)
        {
            rc.y += 25;
            
            for (uint32 nn=0; nn<perks.GetSize(); ++nn) {
                
                if( nn != 0 && nn%2 == 0 )//|| perks.GetSize() == 1 )
                    rc.y += 25;
                
                iRect tmprc = iRect(rc.x, rc.y + (ht * floor((double)nn / 2)), rc.w, ht);
                iSize sz = gTextComposer.GetTextSize(gTextMgr[TRID_CPERK_DOUBLESHOT+perks[nn]], smallfc);
                iRect tmprc2 = AlignRect(iSize(tmprc.w / 2, tmprc.h), tmprc, nn%2 == 0 ? AlignLeft : AlignRight);
                //iRect tmprc2 = AlignRect(sz, tmprc, nn%2 == 0 ? AlignRight : AlignRight);
                
                iRect txtrc;
                
                if(perks.GetSize() > 1) {
                    txtrc = AlignRect(sz, tmprc2, (nn%2 == 0) ?  AlignRight : AlignLeft);
                    if(nn % 2 == 0)
                        txtrc -= iPoint(25, 0);
                    else
                        txtrc += iPoint(25, 0);
                }
                else{
                    txtrc = AlignRect(sz, tmprc, AlignCenter/*nn%2 == 0 ? AlignRight : AlignLeft*/);
                    rc.y += ht;
                }
                
                //gApp.Surface().FillRect(txtrc, nn * 100);
                sint32 h = gTextComposer.TextBoxOut(smallfc, gApp.Surface(), gTextMgr[TRID_CPERK_DOUBLESHOT+perks[nn]], txtrc);
                
                BlitIcon(gApp.Surface(), PDGG(CREAT_PERKS)+perks[nn], iRect(txtrc.point() - iPoint(25, 0), iSize(0, 20)), 140);
                /*rc.y += h;
                 rc.y += 8;
                 */
                
            }
            rc.y += ht * ceil((double)perks.GetSize() / 2);
        }
    }
    
    rc.y += 15;
#endif
    left.y = right.y = rc.y;
    
    
    // morale, luck & spells only in combat
    if(m_pGroup) {
        // morale
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_MORALE_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        tout.Setf(_T("%d"), m_pGroup->Morale());
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        // luck
        gTextComposer.TextOut(normalfc, gApp.Surface(),left.point(), iStringT(gTextMgr[TRID_SKILL_LUCK_S])+_T(" :"), left, AlignRight);
        left.y += hrz.h;
        tout.Setf(_T("%d"), m_pGroup->Luck());
        gTextComposer.TextOut(qtyfc, gApp.Surface(),right.point(),tout, right,AlignLeft);
        right.y = left.y;
        
        // spells
        iRect src = iRect(rc.x, left.y + 15, rc.w, rc.h);
        if (m_pGroup->SpellList().GetSize()) {
            uint32 cnt = iMIN<uint32>(5,m_pGroup->SpellList().GetSize());
            for (uint32 xx=0; xx<cnt; ++xx) {
                if (m_pGroup->SpellList()[xx].m_duration == iBattleGroup::iSpellEntry::DurInfinit)
                    src.y += gTextComposer.TextBoxOut(smallfc, gApp.Surface(), iFormat(_T("%s (-)"), gTextMgr[m_pGroup->SpellList()[xx].m_pSpell->SpellLabel()]),src);
                else
                    src.y += gTextComposer.TextBoxOut(smallfc, gApp.Surface(), iFormat(_T("%s (%d)"), gTextMgr[m_pGroup->SpellList()[xx].m_pSpell->SpellLabel()], m_pGroup->SpellList()[xx].m_duration),src);
            }
        }
        if(m_pGroup->GetPoisonDuration() > 0)
            src.y += gTextComposer.TextBoxOut(smallfc, gApp.Surface(), iFormat(_T("%s (%d)"), gTextMgr[TRID_EFNAME_POISON], m_pGroup->GetPoisonDuration()),src);
    }
    
    // cost
    if(m_flags & CDV_COST) {
        if(!(m_flags & CDV_GLOW)) {
            iStringT txt = iStringT(_T("#FFED")) + MineralSet2Text(GetCost() * m_amount, true);
            iSize sz = gTextComposer.GetTextSize(txt, normalfc);
            iRect trc = AlignRect(sz, GetScrRect(), AlignBottom);
            gTextComposer.TextBoxOut(normalfc, gApp.Surface(), txt, trc);
        } else {
            iRect trc = AlignRect(GetMineralSetComposeSize(GetCost() * iMAX(m_amount, 1), false), GetScrRect(), AlignBottom);
            ComposeMineralSet(GetCost() * iMAX(m_amount, 1), *m_ms, trc, false, true);
        }
    }
}

void iCreatDescView::SetType(CREATURE_TYPE tp)
{
    m_type = tp;
    m_bEntered = false;
    
    if( m_flags & CDV_NOTITLE_PERKS ){
        
        if(gUseIpadUI)
        {
            // Perks
            iSimpleArray<uint16> perks;
            for (uint16 nn=0; nn<CPERKS_COUNT; ++nn)
                if (CREAT_DESC[m_type].perks & (1<<nn))
                    perks.Add(nn);
            
            for (uint32 nn=0; nn<m_pPerks.GetSize(); ++nn)
                RemoveChild(m_pPerks.At(nn));
            
            m_pPerks.RemoveAll();
            
            if(m_type == CREAT_UNKNOWN ) return;
            
            for (uint32 nn=0; nn<perks.GetSize(); ++nn){
                
                iPerksBtn* pBtn;
                
                AddChild(pBtn = new iPerksBtn(&gApp.ViewMgr(), this, iRect(), 500+nn, perks[nn], tp));
                m_pPerks.Add(pBtn);
                
            }
        }
        UpdateButtons();
    }
}

//#ifdef OS_IPHONE
void iCreatDescView::UpdateButtons()
{
    if(!gUseIpadUI)
    {
        for (uint32 nn=0; nn<m_pPerks.GetSize(); ++nn)
            RemoveChild(m_pPerks.At(nn));
        
        m_pPerks.RemoveAll();
        
        if(m_flags & CDV_NOTITLE_PERKS) {
            
            // Perks
            iSimpleArray<uint16> perks;
            for (uint16 nn=0; nn<CPERKS_COUNT; ++nn)
                if (CREAT_DESC[m_type].perks & (1<<nn))
                    perks.Add(nn);
            
            
            if(m_type == CREAT_UNKNOWN ) return;
            
            for (uint32 nn=0; nn<perks.GetSize(); ++nn){
                
                iPerksBtn* pBtn;
                
                AddChild(pBtn = new iPerksBtn(&gApp.ViewMgr(), this, iRect(), 500+nn, perks[nn], m_type));
                m_pPerks.Add(pBtn);
                
            }
            
            uint32 offset = 20;
            iSize perksz = gGfxMgr.Dimension(PDGG(CREAT_PERKS));
            iSize tmpsz = iSize(m_pPerks.GetSize()*( perksz.w + offset ), perksz.h);
            iRect tmprc = AlignRect(tmpsz, m_Rect, AlignBottom);
            
            tmprc.y -= 35;
            
            for (uint32 nn=0; nn<m_pPerks.GetSize(); ++nn) {
                
                iRect perkrc = iRect(tmprc.point(), perksz);
                
                m_pPerks.At(nn)->SetRect(perkrc);
                
                tmprc.x += perksz.w + offset;
            }
        }
        
        else
        {
            if(m_type == CREAT_UNKNOWN || !(m_flags & CDV_NOTITLE_PERKS) ) return;
            
            iRect rc = GetScrRect();
            rc.y = iBaseGameDlg::GetTopOffset();
            rc.x = 0;
            // title
            if(m_flags & CDV_TITLE){
                rc.y += gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), gTextMgr[m_type*3+TRID_CREATURE_PEASANT_F2], rc);
                rc.y += iBaseGameDlg::GetTopOffset();
            }
            //icon
            rc.y += 55;
            //props
            rc.y += gTextComposer.GetFontHeight(creatinfofc_normal.fontSize) * 6;
            rc.y += 5;
            
            iRect tmprc = rc;
            uint32 offset = 20;
            iSize perksz = gGfxMgr.Dimension(PDGG(CREAT_PERKS));
            iSize tmpsz = iSize(m_pPerks.GetSize()*( perksz.w + offset ), perksz.h);
            
            tmprc.h = tmpsz.h;
            tmprc = AlignRect(tmpsz, tmprc, AlignCenter);
            
            for (uint32 nn=0; nn<m_pPerks.GetSize(); ++nn) {
                
                iRect perkrc = iRect(tmprc.point(), perksz);
                
                m_pPerks.At(nn)->SetRect(perkrc);
                
                tmprc.x += perksz.w + offset;
            }
            Invalidate();
        }
    }
    //... and add new perk buttons
    /*
     sint32 h = 0;//iBaseGameDlg::GetTopOffset();
     sint32 w = m_Rect.w;
     
     // title
     if(m_flags & CDV_TITLE) {
     iSize sz = gTextComposer.GetTextSize(gTextMgr[m_type*3+TRID_CREATURE_PEASANT_F2], dlgfc_hdr);
     h += sz.h;
     //h += iBaseGameDlg::GetTopOffset();
     }
     
     // icon
     h += gGfxMgr.Dimension(PDGG(MINIMON_GLOW) + m_type).h;
     
     //h += iBaseGameDlg::GetTopOffset();
     
     // props
     h += gTextComposer.GetFontHeight(normalfc.fontSize) * 6;
     
     //h += iBaseGameDlg::GetTopOffset();
     
     // Perks
     iSimpleArray<uint16>	perks;
     iSimpleArray<iSize> szs;
     for (uint16 nn=0; nn<CPERKS_COUNT; ++nn) {
     if (CREAT_DESC[m_type].perks & (1<<nn))  {
     perks.Add(nn);
     szs.Add(iIphoneBtn::sm_defSize);
     }
     }
     
     //	for(uint32 xx=0; xx<perks.GetSize(); xx++) {
     //		iPoint offs = GetRectOffset(w, xx, szs);
     //		iRect irc = iRect(offs.x, h + offs.y, szs[xx].w, szs[xx].h);
     //		AddChild(new iImgBtn(&gApp.ViewMgr(), this, irc, 100 + perks[xx], PDGG(CREAT_PERKS) + perks[xx], -1, iStringT(), Visible|Enabled, AlignCenter));
     //	}
     */
}

//#endif

void iCreatDescView::SetGroup( iBattleGroup* pGroup )
{
    m_pGroup = pGroup;
    m_amount = pGroup->Count();
    SetType(m_pGroup->Type());
    Invalidate();
}

void iCreatDescView::OnMouseEntered(const iPoint& pos, MouseId mID){
    
    m_bEntered = true;
}

void iCreatDescView::OnMouseLeave(const iPoint& pos, MouseId mID){
    
    m_bEntered = false;
}

iSize iCreatDescView::GetSize( sint32 w, CREATURE_TYPE type, sint32 amount, uint32 flags, iBattleGroup* pGroup )
{
    sint32 h =
#ifdef PC_VERSION
    iBaseGameDlg::GetTopOffset();
#else
    0;
#endif
    // title
    if(flags & CDV_TITLE) {
#ifdef PC_VERSION
        h += gTextComposer.GetTextBoxSize(gTextMgr[type*3+TRID_CREATURE_PEASANT_F2], w, dlgfc_hdr).h;
        h += 15;
#else
        iSize sz = gTextComposer.GetTextSize(gTextMgr[type*3+TRID_CREATURE_PEASANT_F2], dlgfc_hdr);
        h += sz.h;
        w = iMAX<sint32>(w, sz.w + 10);
#endif
    }
    
    // icon
#ifdef PC_VERSION
    h += 40;
#else
    h += gGfxMgr.Dimension(PDGG(MINIMON_GLOW) + type).h;
#endif
    
    // props
#ifndef PC_VERSION
    if(flags & CDV_STATS)
#endif
        h += gTextComposer.GetFontHeight(creatinfofc_normal.fontSize) * 6;
    
    // Perks
    
    iSimpleArray<uint16>	perks;
    for (uint16 nn=0; nn<CPERKS_COUNT; ++nn) {
        if (CREAT_DESC[type].perks & (1<<nn))
            perks.Add(nn);
    }
    
    if(flags & CDV_LINEAR_PERKS) {
        h += perks.GetSize() * gTextComposer.GetFontHeight(creatinfofc_small.fontSize);
        for(uint32 xx=0; xx<perks.GetSize(); xx++){
            h += 20;
            
            uint32 pw = gGfxMgr.Dimension( PDGG(CREAT_PERKS)+perks[xx] ).w;
            w = iMAX(w, (sint32)gTextComposer.GetTextSize(gTextMgr[TRID_CPERK_DOUBLESHOT+perks[xx]], creatinfofc_small).w + 70 + (sint32)pw+10);
        }
    } else {
        
        if(gUseIpadUI)
        {
            h += 25;
            // check the width, increase if needed
            if(perks.GetSize() > 1) {
                for(sint32 xx=0; xx<perks.GetSize()-1; xx += 2) {
                    iSize sz = gTextComposer.GetTextSize(gTextMgr[TRID_CPERK_DOUBLESHOT+perks[xx]], creatinfofc_small);
                    iSize sz2 = gTextComposer.GetTextSize(gTextMgr[TRID_CPERK_DOUBLESHOT+perks[xx+1]], creatinfofc_small);
                    w = iMAX<sint32>(w, iMAX(sz.w, sz2.w) * 2 + 100);
                    h += 25;
                }
            }
            else
                h += gTextComposer.GetFontHeight(creatinfofc_small.fontSize);
            h += gTextComposer.GetFontHeight(creatinfofc_small.fontSize) * ceil((float)perks.GetSize() / 2);
        } else
        {
            
            iSize perksz = gGfxMgr.Dimension(PDGG(CREAT_PERKS));
            
            h += perksz.h + 20;
        }
    }
    h += 15;
    
    if(pGroup) {
        // morale & luck
        h += gTextComposer.GetFontHeight(creatinfofc_normal.fontSize) * 2;
        
        // spells
        uint32 count = pGroup->SpellList().GetSize();
        if(pGroup->GetPoisonDuration() > 0) count++;
        
        h += count * gTextComposer.GetFontHeight(creatinfofc_small.fontSize) + 55;
    }
    
    // cost
    if(flags & CDV_COST) {
        if(!(flags & CDV_GLOW))
            h += gTextComposer.GetTextBoxSize(iStringT(_T("#FFED")) + MineralSet2Text(CREAT_DESC[type].cost * amount, true), w, creatinfofc_normal).h;
        else
            h += GetMineralSetComposeSize(CREAT_DESC[type].cost * amount, true).h;
        
    }
    
    /*if(flags & CDV_BKND) {
     h += 46;
     }*/
    
    return iSize(w, h);
}

//////////////////////////////////////////////////////////////////////////
iDlg_CreatInfo::iDlg_CreatInfo(iViewMgr* pViewMgr, PLAYER_ID pid, iArmyOwner *owner, sint32 idx, const iFurtSkills& furtSkills, bool bCanDismiss, sint32 moraleMod, iCreatBonusList cbl)
: iGlowGameDlg(pViewMgr), m_cGroup(owner->Army().At(idx)), m_furtSkills(furtSkills), m_bCanDismiss(bCanDismiss), m_moraleMod(moraleMod), m_cbl(cbl), m_pOwner(owner),
m_idx(idx)
{
    
}

void iDlg_CreatInfo::OnCreateDlg()
{
    iRect clRect = ClientRect();
    clRect.y += iBaseGameDlg::GetTopOffset();
    
    
    uint32 flags =
#ifdef PC_VERSION
    CDV_TITLE | CDV_GLOW | CDV_STATS | CDV_LINEAR_PERKS;
#else
    CDV_TITLE | CDV_GLOW | CDV_STATS;
    
    if(gUseIpadUI)
        flags |= CDV_LINEAR_PERKS;
    else
        flags |= CDV_NOTITLE_PERKS;
#endif
    
    AddChild(desc = new iCreatDescView(m_pMgr, this, iRect(), 99, flags, NULL));
    
    
    desc->SetRect(AlignRect(desc->GetSize(clRect.w, m_cGroup.Type(), m_cGroup.Count(), flags, NULL), clRect, AlignTop));
    desc->SetType(m_cGroup.Type());
    desc->SetAmount(m_cGroup.Count());
    
    // Buttons
    sint32 bw = iIphoneBtn::sm_defSizeDeluxe.w;
    sint32 hw =
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    0;
#else
    iIphoneBtn::sm_defSizeDeluxe.h;
#endif
    if (m_bCanDismiss) {
        AddChild(m_pBtnDismiss = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(-bw, -hw), 100, PDGG(BTN_DISMISS_DELUXE), -1));
#ifdef PC_VERSION
    }
#endif
    AddChild(m_pBtnOk = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(bw, -hw), DRC_OK, PDGG(BTN_OK_DELUXE), -1));
#ifndef PC_VERSION
}
#endif
}
#ifdef PC_VERSION
void iDlg_CreatInfo::UpdateSize(){
    
    Center();
    
    iRect clRect = ClientRect();
    clRect.y += 15;
    
    desc->SetRect( AlignRect(desc->GetSize(clRect.w, m_cGroup.Type(), m_cGroup.Count(), false, NULL), clRect, AlignTop) );
    
    sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_CREAT_DISMISS], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_OK], btnfc_normal).w) + 30;
    
    if (m_bCanDismiss) {
        m_pBtnDismiss->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0) );
    }
    m_pBtnOk->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0) );
}
#else
void iDlg_CreatInfo::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    if(!m_bCanDismiss)
        EndDialog(0);
}
#endif
iSize iDlg_CreatInfo::ClientSize() const
{
    sint32 w = iBaseGameDlg::GetDefaultWidth();
    sint32 h = iBaseGameDlg::GetTopOffset();
#ifdef PC_VERSION
    h += 15;	
#endif
    uint32 perks;
#ifdef PC_VERSION
    perks = CDV_TITLE | CDV_COST | CDV_GLOW | CDV_LINEAR_PERKS;
#else
    perks = CDV_TITLE | CDV_GLOW | CDV_STATS;
    if(gUseIpadUI)
        perks |=  CDV_LINEAR_PERKS;
    else
        perks |= CDV_NOTITLE_PERKS;
#endif
    
    
    iSize sz = iCreatDescView::GetSize(w, m_cGroup.Type(), m_cGroup.Count(), perks, NULL);
    
    w = iMAX<sint32>(sz.w, w);
    
    h += sz.h;
    
    h += DEF_BTN_HEIGHT;
    
    return iSize(w, iMIN<sint32>(h, gApp.Surface().GetHeight()));
}

void iDlg_CreatInfo::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == DRC_OK) {
        EndDialog(DRC_OK);
    } else if (uid == 100) {
        iQuestDlg dlg(m_pMgr, gTextMgr[m_cGroup.Type()*3+TRID_CREATURE_PEASANT_F2], gTextMgr[TRID_MSG_QDISMISS], PID_NEUTRAL);
        if (dlg.DoModal() == DRC_YES) {
            m_pOwner->ResetArmyGroup(m_idx);
            EndDialog(DRC_OK);
        }
    }
}

void iDlg_CreatInfo::DoCompose( const iRect& rect )
{
    
}

void iDlg_CreatInfo::OnCompose(){
    
    if(!gUseIpadUI)
    {
        gApp.Surface().Fill(cColor_Black);
        iSize sz = gGfxMgr.Dimension(PDGG(HERO_DIALOG_BACK));
        gGfxMgr.Blit(PDGG(HERO_DIALOG_BACK), gApp.Surface(), AlignRect(sz, GetScrRect(), AlignCenter));
    } else
    {
        // darken the underlying image
        //gApp.Surface().DarkenBWRect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        if(gGame.ActViewType() != iChildGameView::CV_CASTLE) {
            gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
            gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        }
        
        if( gGame.ActViewType() != iChildGameView::CV_CASTLE )
            ComposeGlowBkgnd(gApp.Surface(), m_Rect, gGame.ActViewType() == iChildGameView::CV_OVERLAND || gGame.ActViewType() == iChildGameView::CV_BATTLE);
    }
    iRect rc(m_Rect);
    
    if(gUseIpadUI)
        rc.DeflateRect(DLG_GLOW_FRAME_SIZE);
    
    DoCompose(rc);
}
