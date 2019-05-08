#include "stdafx.h"
#include "Dlg_LeaveGuards.h"
#include "Dlg_ArmyRoom.h"
#include "Dlg_FightGuard.h"
#include "Dlg_Rewards.h"
#include "Dlg_WitchHut.h"
#include "Dlg_Recruit.touch.h"
#include "Dlg_PuzzleMap.touch.h"
#include "CastleView.h"
#include "Dlg_CreatInfo.h"
#include "HarborView.h"
#include "serialize.h"
#include "helpers.h"
//#include "UpgradeView.touch.h"
#include "UpgradeView.h"

#include "OverlandView.h"
#include "OverlandView.touch.h"
/*
 *  Common construction type
 */
iHero* iMapCnst::Visitor() const
{ 
    return DynamicCast<iHero*>(gGame.Map().m_CoverMap.GetAt(Pos().x,Pos().y)); 
}

/*
 *  Visitables
 */
void iVisCnst::NewDay(){}

void iVisCnst::NewWeek(const iWeekDesc& weekDesk) {
	// grow guard
	for(uint32 xx=0; xx<7; xx++) {
		if(Army().At(xx).Type() != CREAT_UNKNOWN)
			GrowArmyGroup(xx, 1);
	}
}

bool iVisCnst::Activate(iHero* pVisitor, bool bActive, bool& bDisappear)
{
	if(m_bDisabled) return false;

    if(bActive)
        gTutorial.Trigger(TUT_VISITABLES);
    
    if (!Guard().Empty()) {
        if (bActive) {
            iDlg_FightGuard dlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], Guard(), pVisitor->Owner()->PlayerId(), VL_BASIC);
            uint32 res = dlg.DoModal();
            if (res == DRC_YES) {
                iBattleInfo bi(pVisitor, this);
#ifdef MULTIPLAYER
                gMPMgr.OnAttackVisCnst(pVisitor, this);
#endif              
                gGame.BeginBattle(bi);
            }
            else if( res == DRC_QUICK ){
            
                iBattleInfo bi(pVisitor, this);
                gGame.BeginBattle(bi, true);
            }
        } else {
#ifdef MULTIPLAYER
            if(pVisitor->Owner()->PlayerType() == PT_REMOTE) return false;
#endif
            iBattleInfo bi(pVisitor, this);
            gGame.BeginBattle(bi);
        }
        return false;
	} else {
		if (bActive) {

			if(!(m_seenIntro & (1 << pVisitor->Owner()->PlayerId()))) {
				if(!m_customMsg.Empty()) {
					iTextDlg dlg(&gApp.ViewMgr(),_T(""),gGame.Map().GetText(TET_VIS_INTROMSG, this),pVisitor->Owner()->PlayerId());
					dlg.DoModal();					
				}
				m_seenIntro |= (1 << pVisitor->Owner()->PlayerId());
			}

			if(pVisitor->MeetsReqs(m_reqs) && !(m_seenIntro & (1 << (8 + pVisitor->Owner()->PlayerId())))) {
				if(!m_okRewards.IsEmpty())
					pVisitor->ReceiveRewards(m_okRewards, bActive, _T(""), gGame.Map().GetText(TET_VIS_OKREWMSG, this).CStr());
				else {
					if(!gGame.Map().GetText(TET_VIS_OKREWMSG, this).Empty()) {
						iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_VIS_OKREWMSG, this), PID_NEUTRAL);
						dlg.DoModal();
					}
					m_seenIntro |= (1 << (8 + pVisitor->Owner()->PlayerId()));
				}


				m_okRewards.RemoveAll();
				m_reqs.RemoveAll();
				m_permFailRewards.RemoveAll();
				m_customMsg.SetEmpty();
					switch(m_okRewBeh) {
					case CRB_USUAL:
						break;
					case CRB_DISAPPEAR:	
						bDisappear = true;
						return false;					
					case CRB_DISABLE:
						m_bDisabled = true;
						return false;				
					}
			} else if(pVisitor->PermanentlyFailedReqs(m_reqs) && !(m_seenIntro & (1 << (8 + pVisitor->Owner()->PlayerId())))) {
				if(!m_permFailRewards.IsEmpty())
					pVisitor->ReceiveRewards(m_permFailRewards, bActive, _T(""), gGame.Map().GetText(TET_VIS_PERMFAILMSG, this).CStr());
				else {
					if(!gGame.Map().GetText(TET_VIS_PERMFAILMSG, this).Empty()) {
						iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_VIS_PERMFAILMSG, this), PID_NEUTRAL);
						dlg.DoModal();
					}
					m_seenIntro |= (1 << (8 + pVisitor->Owner()->PlayerId()));
				}

				m_okRewards.RemoveAll();
				m_reqs.RemoveAll();
				m_permFailRewards.RemoveAll();
				m_customMsg.SetEmpty();

				switch(m_permFailRewBeh) {
						case CRB_USUAL:
							break;
						case CRB_DISAPPEAR:	
							bDisappear = true;
							return false;					
						case CRB_DISABLE:
							m_bDisabled = true;
							return false;	
				}
			} else if(m_reqs.GetSize() > 0 && !(m_seenIntro & (1 << (8 + pVisitor->Owner()->PlayerId())))) {
				if (!m_curFailMessage.Empty()) {
					iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_VIS_CURFAILMSG, this), pVisitor->Owner()->PlayerId());
					dlg.DoModal();
				}
				return false;
			}
			
			if (!m_bCustomGUI){
				if (Visited(pVisitor)) {
					iTextDlg dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()],gTextMgr[m_pProto->VisMessage()],pVisitor->Owner()->PlayerId());
					dlg.DoModal();
					return false;
				} else {
					iTextDlg dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()],gTextMgr[m_pProto->Message()],pVisitor->Owner()->PlayerId());
					dlg.DoModal();
				}			
			}
		} 
	}

    OnActivate(pVisitor, bActive);
    return true;
}

void iVisCnst::OnActivate(iHero* pVisitor, bool bActive)
{
    m_Visitors.Add(pVisitor->Proto()->m_protoId);
}

void iVisCnst::Unvisit(const iHero* pVisitor) 
{
    m_Visitors.Remove(pVisitor->Proto()->m_protoId);
}

bool iVisCnst::Visited(const iHero* pVisitor) const
{
    return (m_Visitors.Find(pVisitor->Proto()->m_protoId) != -1);
}

void iVisCnst::Serialize(iDynamicBuffer& dbuff)
{
    // Guard
    ::Serialize(dbuff, Guard());
    // Custom message     
	::Serialize(dbuff, m_customMsg);
	::Serialize(dbuff, m_curFailMessage);
	::Serialize(dbuff, m_okMessage);
	::Serialize(dbuff, m_permFailMessage);
	::Serialize(dbuff, m_reqs);
	::Serialize(dbuff, m_okRewards);
	::Serialize(dbuff, m_permFailRewards);	
	dbuff.Write((uint8)m_okRewBeh);
	dbuff.Write((uint8)m_permFailRewBeh);
	dbuff.Write((uint8)m_bDisabled);
	dbuff.Write(m_seenIntro);

    // Visitors
    dbuff.Write((uint16)m_Visitors.GetSize());
    for (uint32 vtrid=0; vtrid<m_Visitors.GetSize(); ++vtrid) dbuff.Write(m_Visitors[vtrid]);
    // flag
    dbuff.Write((uint8)1);
}

uint8 iVisCnst::Unserialize(iDynamicBuffer& dbuff)
{
    // Guard
    UnserializeArmy(dbuff);
    InitArmy();
    // Custom messages, requirements and rewards
    ::Unserialize(dbuff, m_customMsg);
	::Unserialize(dbuff, m_curFailMessage);
	::Unserialize(dbuff, m_okMessage);
	::Unserialize(dbuff, m_permFailMessage);
	::Unserialize(dbuff, m_reqs);
	::Unserialize(dbuff, m_okRewards);
	::Unserialize(dbuff, m_permFailRewards);
	uint8 beh;
	dbuff.Read(beh); m_okRewBeh = (CNST_REW_BEH)beh;
	dbuff.Read(beh); m_permFailRewBeh = (CNST_REW_BEH)beh;
	uint8 dis; dbuff.Read(dis);
	m_bDisabled = dis > 0;
	dbuff.Read(m_seenIntro);
    // Visitors
    uint16 vcnt; dbuff.Read(vcnt);
    while (vcnt--) {
        uint16 visitor; dbuff.Read(visitor);
        m_Visitors.Add(visitor);
    }
    // flag
    uint8 flag;
    dbuff.Read(flag);
    return flag;
}

//////////////////////////////////////////////////////////////////////////
void iVisCnst_Stable::NewWeek(const iWeekDesc& weekDesk)
{
    iVisCnst::NewWeek(weekDesk);
    m_Visitors.RemoveAll();
}

void iVisCnst_Stable::OnActivate(iHero* pVisitor, bool bActive)
{
    iVisCnst::OnActivate(pVisitor, bActive);
    iVisCnstT_Stable* pStable = DynamicCast<iVisCnstT_Stable*>(m_pProto);
    check(pStable);
    pVisitor->AddMoves(pStable->ActPts());
    pVisitor->Enchs().Add(m_pProto->Type(), FSK_LOGISTICS, ENDUR_NEXTWEEK, pStable->ActPts());
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif

}

bool iVisCnst_Stable::Visited(const iHero* pVisitor) const
{
    return pVisitor->Enchs().Registered(m_pProto->Type());
}
//////////////////////////////////////////////////////////////////////////
void iVisCnst_Gazebo::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->VisMessage()], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        sint32 exp = pVisitor->ConvExpPts(1000);
		if (bActive) {
            iRewardsCtr rew;
			rew.Add(iRewardItem(RIT_EXPERIENCE, 0, pVisitor->NoExping() ? 0 : exp));
            iDlg_Rewards dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], pVisitor, rew);
            dlg.DoModal();
        }
        pVisitor->ReceiveExperience(exp);
    }

    iVisCnst::OnActivate(pVisitor, bActive);
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}
//////////////////////////////////////////////////////////////////////////
void iVisCnst_ManaSource::NewWeek(const iWeekDesc& weekDesk)
{
    iVisCnst::NewWeek(weekDesk);
    m_Visitors.RemoveAll();
}

sint32 iVisCnst_ManaSource::ManaPts() const
{
    iVisCnstT_ManaSource* pManaSource = DynamicCast<iVisCnstT_ManaSource*>(m_pProto);
    check(pManaSource);
    return pManaSource->ManaPts();
}

void iVisCnst_ManaSource::OnActivate(iHero* pVisitor, bool bActive)
{
    iVisCnst::OnActivate(pVisitor, bActive);
    sint32 act_pts = pVisitor->MaxManaPts() * ManaPts();
    if (pVisitor->ManaPts() < act_pts) pVisitor->ManaPts() = act_pts;
    pVisitor->Enchs().Add(m_pProto->ProtoId() | iEnchItem::PROVIDER_FLAG, FSK_INVALID, ENDUR_NEXTWEEK, 0);
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}

bool iVisCnst_ManaSource::Visited(const iHero* pVisitor) const
{
    return pVisitor->Enchs().Registered(m_pProto->ProtoId() | iEnchItem::PROVIDER_FLAG);
}
//////////////////////////////////////////////////////////////////////////
void iVisCnst_MLModifier::NewWeek(const iWeekDesc& weekDesk)
{
    iVisCnst::NewWeek(weekDesk);
    m_Visitors.RemoveAll();
}

void iVisCnst_MLModifier::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->VisMessage()], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        iVisCnstT_MLModifier* pProto = DynamicCast<iVisCnstT_MLModifier*>(m_pProto);
        check(pProto);
        if (bActive) {
            iRewardsCtr rew;
            if (pProto->MoraleModifier()) rew.Add(iRewardItem(RIT_MORALE, 0, pProto->MoraleModifier()));
            if (pProto->LuckModifier()) rew.Add(iRewardItem(RIT_LUCK, 0, pProto->LuckModifier()));
            iDlg_Rewards dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], pVisitor, rew);
            dlg.DoModal();
        }
        if (pProto->MoraleModifier()) pVisitor->Enchs().Add(m_pProto->ProtoId() | iEnchItem::PROVIDER_FLAG, FSK_MORALE, ENDUR_NEXTBATTLE, pProto->MoraleModifier());
        if (pProto->LuckModifier()) pVisitor->Enchs().Add(m_pProto->ProtoId() | iEnchItem::PROVIDER_FLAG, FSK_LUCK, ENDUR_NEXTBATTLE, pProto->LuckModifier());
    }

    iVisCnst::OnActivate(pVisitor, bActive);
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}

bool iVisCnst_MLModifier::Visited(const iHero* pVisitor) const
{
    return pVisitor->Enchs().Registered(m_pProto->ProtoId() | iEnchItem::PROVIDER_FLAG);
}
//////////////////////////////////////////////////////////////////////////
void iVisCnst_PSModifier::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->VisMessage()], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        iVisCnstT_PSModifier* pPSModifier = DynamicCast<iVisCnstT_PSModifier*>(m_pProto);
        check(pPSModifier);
        if (bActive) {
            iRewardsCtr rew;
            for (uint32 psid = 0; psid<PRSKILL_COUNT; ++psid) {
                if (pPSModifier->PSModifier().val[psid]) rew.Add(iRewardItem(RIT_FURTSKILL, FSK_ATTACK + psid, pPSModifier->PSModifier().val[psid]));
            }
            iDlg_Rewards dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], pVisitor, rew);
            dlg.DoModal();
        }
        pVisitor->ConstFurtSkills() += pPSModifier->PSModifier();
    }

    iVisCnst::OnActivate(pVisitor, bActive);
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}
//////////////////////////////////////////////////////////////////////////
void iVisCnst_Obelisk::OnActivate(iHero* pVisitor, bool bActive)
{
    check(!Visited(pVisitor));
    uint16 opened = pVisitor->Owner()->OpenObelisk();
    m_Visitors.Add(pVisitor->Owner()->PlayerId());
    if (bActive) {
        iPuzzleMapDlg pmdlg(&gApp.ViewMgr(), *pVisitor->Owner(), opened);
        pmdlg.DoModal();
    } else {
        if(iAI_Player* pOwner = DynamicCast<iAI_Player*>(pVisitor->Owner())) {
            check(pOwner);
            pOwner->UpdateUltSeekPoint();
        }
    }
}

bool iVisCnst_Obelisk::Visited(const iHero* pVisitor) const
{
    for (uint32 xx=0; xx<m_Visitors.GetSize(); ++xx) if (!pVisitor->Owner() || m_Visitors[xx] == pVisitor->Owner()->PlayerId()) return true;
    return false;
}

//////////////////////////////////////////////////////////////////////////

void iVisCnst_Harbor::NewWeek(const iWeekDesc& weekDesk)
{
    m_shipsCount[0] = 1;
    m_shipsCount[1] = 1;
}

void iVisCnst_Harbor::OnActivate(iHero* pVisitor, bool bActive)
{       
	if(bActive) {
		gGame.HarborView()->SetHero(pVisitor);
		gGame.HarborView()->SetHarbor(this);
		gGame.ShowView(iChildGameView::CV_HARBOR);        		
	}
}

bool iVisCnst_Harbor::Visited(const iHero* pVisitor) const
{
    return m_shipsCount[0] == 0 && m_shipsCount[1] == 0;
}


// implementation of IHarbor

bool iVisCnst_Harbor::IsShipBuildSupported() { return true; }
uint8 iVisCnst_Harbor::NumOfDocks() { return 2; }

iPlayer* iVisCnst_Harbor::GetOwner()
{
	return NULL;
}

bool iVisCnst_Harbor::IsBoardable() { return true; }

void iVisCnst_Harbor::ShipBought(uint8 dock)
{
    check(dock < 2);
    if (m_shipsCount[dock])    
    {
        m_shipsCount[dock]--;
    }    
}

uint8 iVisCnst_Harbor::ShipsAvailable(uint8 dock)
{
    check(dock < 2);
    return m_shipsCount[dock];
}

iShip* iVisCnst_Harbor::GetShip( uint8 dockType )
{
	iPoint pt = GetShipPos(dockType);
	if (iBaseMapObject* pMapObj = gGame.Map().m_CoverMap.GetAt(pt.x, pt.y))	
		if(iHero* pHero = dynamic_cast<iHero*>(pMapObj))
			return pHero->GetShip();
		else
			return dynamic_cast<iShip*>(pMapObj);
	else
		return NULL;
}

iPoint iVisCnst_Harbor::GetShipPos( uint8 dockType )
{
	iPoint pt = Pos();
	// offset to dock
	if (dockType)
		pt += iSize(-3, -3);
	else
		pt += iSize(-3, 0); 
	return pt;
}


void iVisCnst_Harbor::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // Write creatures count
    dbuff.Write((uint8)m_shipsCount[0]);
    dbuff.Write((uint8)m_shipsCount[1]);
}

uint8 iVisCnst_Harbor::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
        m_shipsCount[0] = 1;
        m_shipsCount[1] = 1;
    } else {
        // Read data
        dbuff.Read((uint8&)m_shipsCount[0]);
        dbuff.Read((uint8&)m_shipsCount[1]);
    }
    return flag;
}

//////////////////////////////////////////////////////////////////////////

void iVisCnst_WarAcademy::NewWeek(const iWeekDesc& weekDesk)
{
    m_Visitors.RemoveAll();
}

void iVisCnst_WarAcademy::NewDay()
{    
}

static void UpgradeArmy(iArmy &arm)
{
    /*uint32 type = gGame.Map().Rand(1);
    for (sint32 xx = 0; xx < 7; ++xx) 
    {
        iCreatGroup& cg = arm.At(xx);
        cg.Upgrade(type);
    }
	*/
}

void iVisCnst_WarAcademy::OnActivate(iHero* pVisitor, bool bActive)
{     
    if (bActive) 
    {   
		gGame.UpgradeView()->SetHero(pVisitor); 
		gGame.ShowView(iChildGameView::CV_UPGRADE);        
		
    } 
    else if (iAI_Player* pOwner = DynamicCast<iAI_Player*>(pVisitor->Owner())) 
    {           
        check(pOwner);
        iVisCnstT_WarAcademy* pProto = DynamicCast<iVisCnstT_WarAcademy*>(m_pProto);
        check(pProto);
        pOwner->ProcessUpgradeCreatures(pVisitor);
    }

    // CotullTODO: need do this only if player upgrade something...
    iVisCnst::OnActivate(pVisitor, bActive);

#ifdef MULTIPLAYER    
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}

bool iVisCnst_WarAcademy::Visited(const iHero* pVisitor) const
{
    return iVisCnst::Visited(pVisitor);
}


void iVisCnst_WarAcademy::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // Write creatures count
//    dbuff.Write((uint8)m_shipsCount[0]);
//    dbuff.Write((uint8)m_shipsCount[1]);
}

uint8 iVisCnst_WarAcademy::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
//        m_shipsCount[0] = 1;
//        m_shipsCount[1] = 1;
    } else {
        // Read data
//        dbuff.Read((uint8&)m_shipsCount[0]);
//        dbuff.Read((uint8&)m_shipsCount[1]);
    }
    return flag;
}

void iVisCnst_WarAcademy::UpgradeBought(uint32 dock)
{
// CtllTODO: finish
}

bool iVisCnst_WarAcademy::IsUpgradeAvailable(uint32 dock)
{
    // CtllTODO: finish
    return true;    
}



//////////////////////////////////////////////////////////////////////////
void iVisCnst_Dwelling::NewWeek(const iWeekDesc& weekDesk)
{
    if (weekDesk.wkType == iWeekDesc::Normal) {
        m_creatCount += m_growth;
    } else if (weekDesk.wkType == iWeekDesc::Plague) {
        m_creatCount /= 2;
    } else if (weekDesk.wkType == iWeekDesc::CreatPlus) {
        m_creatCount += m_growth;
        if (weekDesk.crType == m_creatType) m_creatCount += 5;
    } else if (weekDesk.wkType == iWeekDesc::CreatDoubles) {
        m_creatCount += m_growth;
        if (weekDesk.crType == m_creatType) m_creatCount *= 2;
    }
}

void iVisCnst_Dwelling::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iCreatGroup cg(m_creatType, m_creatCount);
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], iFormat(gTextMgr[TRID_OBJ_DWELL_EMPTY], gTextMgr[TRID_CREATURE_PEASANT_F3+cg.Type()*3]), pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        if (bActive) {
            iCreatGroup cg(m_creatType, m_creatCount);
            if (!JoinForFree()) {
                // Buy
                iDlg_Recruit rdlg(&gApp.ViewMgr(), cg, pVisitor->Army(), pVisitor->Owner()->PlayerId(), pVisitor);
#ifndef PC_VERSION
                rdlg.SetRect(iRect(iPoint(0,0), rdlg.ClientSize()));
#endif
                if (rdlg.DoModal() == DRC_OK) {
                    m_creatCount = cg.Count();
                }
            } else {
                // Join Army
#ifdef MULTIPLAYER
                if(!gMPMgr.IsReplaying())
#endif
                {
                    iQuestDlg dj(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], iFormat(gTextMgr[TRID_OBJ_DWELL_RECR_QUEST], gTextMgr[TRID_CREATURE_PEASANT_F3+cg.Type()*3]), pVisitor->Owner()->PlayerId());
                    if (dj.DoModal() == DRC_YES) {
                        iIconDlg idlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], iFormat(gTextMgr[TRID_OBJ_DWELL_RECR_MSG], m_creatCount, gTextMgr[TRID_CREATURE_PEASANT_F3+cg.Type()*3]), PDGG(MINIMON) + cg.Type(), pVisitor->Owner()->PlayerId());
                        idlg.DoModal();
                        if (!pVisitor->AddGroupToArmy(cg.Type(),cg.Count())) {
                            iDlg_ArmyRoom arDlg(&gApp.ViewMgr(), *pVisitor, cg);
                            arDlg.DoModal();
                            }
                        m_creatCount = 0;
                    }
                }
            }
        } else if(iAI_Player* pOwner = DynamicCast<iAI_Player*>(pVisitor->Owner())) {           
            check(pOwner);
            iVisCnstT_Dwelling* pProto = DynamicCast<iVisCnstT_Dwelling*>(m_pProto);
            check(pProto);
            iCreatGroup cg(pProto->CreatType(),m_creatCount);
            if (!JoinForFree()) {
                // Buy
                if (pOwner->ProcessRecruitCreatures(pVisitor, cg)) {
                    m_creatCount -= cg.Count();
                }
            } else {
                // Join
                if (pOwner->ProcessJoinCreatures(pVisitor, cg)) m_creatCount = 0;
            }
        }
#ifdef MULTIPLAYER        
        gMPMgr.OnHeroUpdate(pVisitor);
        gMPMgr.OnVisCnstUpdate(this);
#endif
    }
}

bool iVisCnst_Dwelling::Visited(const iHero* pVisitor) const
{
    return m_creatCount == 0;
}

void iVisCnst_Dwelling::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // Write creatures count
    dbuff.Write(m_creatCount);
}

uint8 iVisCnst_Dwelling::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
        m_creatCount = m_growth;
    } else {
        // Read data
        dbuff.Read(m_creatCount);
    }
    return flag;
}

////////////////////

uint8 iVisCnst_NewTeleport::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);  
    uint8 tmp;
    dbuff.Read(tmp);
    m_inIdx = tmp;
    dbuff.Read(tmp);
    m_outIdx = tmp; 
    return flag;
}


void iVisCnst_NewTeleport::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff); 
    dbuff.Write( (uint8) m_inIdx);  
    dbuff.Write( (uint8) m_outIdx); 
}


//////////////////////////////////////////////////////////////////////////

void iVisCnst_WitchHut::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], iFormat(gTextMgr[TRID_OBJ_WITCHHUT_MSG2], gTextMgr[TRID_HSKILL_ESTATES+m_skill]), pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        if (pVisitor->SecSkills().SkillLevel(m_skill) == SSLVL_NONE && pVisitor->SecSkills().Count() < 8) {
            if (bActive) {
                iWitchHutDlg whdlg(&gApp.ViewMgr(), m_skill, pVisitor->Owner()->PlayerId());
                whdlg.DoModal();
                //iIconDlg idlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], iFormat(gTextMgr[TRID_OBJ_WITCHHUT_MSG1], gTextMgr[TRID_HSKILL_ESTATES+m_skill]), PDGG(SEC_SKILLS) + m_skill, pVisitor->Owner()->PlayerId());
                //idlg.DoModal();
            }
            pVisitor->SecSkills().Set(m_skill, SSLVL_BASIC);
        } else {
            if (bActive) {
                iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], iFormat(gTextMgr[TRID_OBJ_WITCHHUT_MSG3], gTextMgr[TRID_HSKILL_ESTATES+m_skill]), pVisitor->Owner()->PlayerId());
                tdlg.DoModal();
            }
        }
    }
    m_visPlayers.AddUnique(pVisitor->Owner()->PlayerId());
    iVisCnst::OnActivate(pVisitor, bActive);
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}

void iVisCnst_WitchHut::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // Write skill
    dbuff.Write( (uint8) m_skill);
    // Visited players
    dbuff.Write( (uint8) m_visPlayers.GetSize() );
    for (uint32 xx=0; xx<m_visPlayers.GetSize(); ++xx) dbuff.Write((uint8)m_visPlayers[xx]);
}

uint8 iVisCnst_WitchHut::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
        do {
            m_skill = (SECONDARY_SKILLS)gGame.Map().Rand(SECSK_COUNT);
        } while (m_skill == SECSK_NECROMANCY || m_skill == SECSK_LEADERSHIP);
    } else {
        // Skill
        uint8 skill; dbuff.Read(skill);
        m_skill = (SECONDARY_SKILLS)skill;
        // Visited players
        uint8 vpcnt; dbuff.Read(vpcnt);
        for (uint32 xx=0; xx<vpcnt; ++xx) {
            uint8 vpid; dbuff.Read(vpid);
            m_visPlayers.Add( (PLAYER_ID) vpid);
        }
    }
    return flag;
}

bool iVisCnst_WitchHut::CustomPopupText(const iHero* pVisitor, iStringT& ctext) const
{
    if (pVisitor && m_visPlayers.Find(pVisitor->Owner()->PlayerId()) != -1 ) {
        ctext = gTextMgr[TRID_HSKILL_ESTATES+m_skill];
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
void iVisCnst_Shrine::OnActivate(iHero* pVisitor, bool bActive)
{
    if (pVisitor->CanLearnSpell(m_spell)) {
        if (bActive) {
            iIconDlg idlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], iStringT(gTextMgr[m_pProto->Message()])+_T("\"")+gTextMgr[TRID_SPNAME_MAGICARROW+m_spell]+_T("\"."), PDGG(SPELL_ICONS) + m_spell, pVisitor->Owner()->PlayerId());
            idlg.DoModal();
        }
        pVisitor->LearnSpell(m_spell);
    } else {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], iStringT(gTextMgr[m_pProto->Message()])+_T("\"")+gTextMgr[TRID_SPNAME_MAGICARROW+m_spell]+_T("\". ")+gTextMgr[TRID_OBJ_SHRINE_NOWISDOM], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    }
    m_visPlayers.AddUnique(pVisitor->Owner()->PlayerId());
    iVisCnst::OnActivate(pVisitor, bActive);
#ifdef MULTIPLAYER
    gMPMgr.OnVisCnstUpdate(this);
#endif

}

void iVisCnst_Shrine::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // Write spell
    dbuff.Write( (uint16) m_spell);
    // Visited players
    dbuff.Write( (uint8) m_visPlayers.GetSize() );
    for (uint32 xx=0; xx<m_visPlayers.GetSize(); ++xx) dbuff.Write((uint8)m_visPlayers[xx]);
}

uint8 iVisCnst_Shrine::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
        iSpellList spells;
        SelectSpells(spells, SpellFilter(SPT_MASK_ALL, SPELL_LEVEL_MASKS[((iVisCnstT_Shrine*)m_pProto)->SpellLevel()], MST_MASK_ALL), 1, MSP_INVALID);
        check(spells.GetSize());
        m_spell = (MAGIC_SPELL)spells[0];
    } else {
        // Skill
        uint16 spell; dbuff.Read(spell);
        m_spell = (MAGIC_SPELL)spell;
        // Visited players
        uint8 vpcnt; dbuff.Read(vpcnt);
        for (uint32 xx=0; xx<vpcnt; ++xx) {
            uint8 vpid; dbuff.Read(vpid);
            m_visPlayers.Add( (PLAYER_ID) vpid);
        }
    }
    return flag;
}

bool iVisCnst_Shrine::CustomPopupText(const iHero* pVisitor, iStringT& ctext) const
{
    if (pVisitor && m_visPlayers.Find(pVisitor->Owner()->PlayerId()) != -1 ) {
        ctext = gTextMgr[TRID_SPNAME_MAGICARROW+m_spell];
        return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
void iVisCnst_Treasury::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], iFormat(gTextMgr[TRID_OBJ_TREASURY_VISITED], gTextMgr[m_pProto->NameKey()]), pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
		pVisitor->ReceiveRewards(m_rewards, bActive, gTextMgr[m_pProto->NameKey()],gTextMgr[TRID_OBJ_TREASURY_DISCOVER]);
        m_bVisited = true;
    }
    iVisCnst::OnActivate(pVisitor, bActive);
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}

void iVisCnst_Treasury::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // Is visited
    dbuff.Write((uint8)(m_bVisited?1:0));
    // Rewards
    ::Serialize(dbuff, m_rewards);
}

uint8 iVisCnst_Treasury::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
        check(Guard().Empty());
        iVisCnstT_Treasury* pProto = DynamicCast<iVisCnstT_Treasury*>(m_pProto);
        check(pProto);
        iTrVarCtr& tvar = pProto->GetVarCtr();
        m_bVisited = false;
        uint32 val = gGame.Map().Rand(100);
        bool bSel = false;
        for (uint32 xx=0; xx<tvar.VariantsCount(); ++xx) {
            if (val <= tvar.GetVariant(xx).probability) {
                SetArmy(tvar.GetVariant(xx).guards);
                m_rewards = tvar.GetVariant(xx).rewards;
                bSel = true;
                break;
            }
            val -= tvar.GetVariant(xx).probability;
        }
        check(bSel);
        // Initialize reward item (if necessary)
        for (uint32 xx=0; xx<m_rewards.GetSize(); ++xx) {
            if (m_rewards[xx].m_type == RIT_ARTIFACT) {
                // random artifact
                m_rewards[xx].m_fParam = gGame.ItemMgr().m_artMgr.SelectRandomArtifact((ART_LEVEL_TYPE)m_rewards[xx].m_fParam);
            } else if (m_rewards[xx].m_type == RIT_MAGICSPELL) {
                // random spell
                iSpellList spells;
                if (m_rewards[xx].m_fParam == 0) SelectSpells(spells, SpellFilter(SPT_MASK_ALL, SPL_MASK_ALL, MST_MASK_ALL), 1, MSP_INVALID);
                else SelectSpells(spells, SpellFilter(SPT_MASK_ALL, SPELL_LEVEL_MASKS[m_rewards[xx].m_fParam-1], MST_MASK_ALL), 1, MSP_INVALID);
                check(spells.GetSize());
                m_rewards[xx].m_fParam = spells[0];
            }
        }
    } else {
        // Is visited
        uint8 bVisited;
        dbuff.Read(bVisited);
        m_bVisited = (bVisited>0)?true:false;
        // Rewards
        ::Unserialize(dbuff, m_rewards);
    }
    return flag;
}

//////////////////////////////////////////////////////////////////////////
void iVisCnst_Teleport::OnActivate(iHero* pVisitor, bool bActive)
{
	if(bActive)
		gSfxMgr.PlaySound(CSND_TELEPORT);
    if ((IsTwoWay() || IsEntrance()) && m_tgtIdx != -1) {
        iVisCnst_Teleport* pTarget = gGame.Map().GetTeleport(m_tgtIdx);
        check(pTarget);
        // Check for visitor at target cell
        if (iHero* pTeleportVisitor = DynamicCast<iHero*>(gGame.Map().m_CoverMap.GetAt(pTarget->Pos().x,pTarget->Pos().y))) {
            check(pTeleportVisitor->GetLocation() == pTarget);
            if (pTeleportVisitor->Owner()->PlayerId() != pVisitor->Owner()->PlayerId()) {
#ifdef MULTIPLAYER
                if(!gMPMgr.IsReplaying())
#endif
                    pVisitor->AttackHero(pTeleportVisitor);
            }
            else pVisitor->MeetHero(pTeleportVisitor);
        } else {
            gGame.Map().MoveHero(pVisitor->Pos(), pTarget->Pos());
            pVisitor->SetPos(pTarget->Pos());
            pVisitor->Owner()->UpdateFogMap();
            gGame.OnHeroPosChanged(pVisitor, pVisitor->Pos());
            gGame.OnHeroChanged(pVisitor);
            gGame.OnHeroTeleport(pVisitor, Pos(), pTarget->Pos());
        }
    } else {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(), gTextMgr[((iVisCnstT_Teleport*)m_pProto)->NameKey()], gTextMgr[TRID_OBJ_TELEPORT_EXIT_MSG], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    }
}

uint8 iVisCnst_Teleport::Index() const
{
    return ((iVisCnstT_Teleport*)m_pProto)->Index();
}

bool iVisCnst_Teleport::IsEntrance() const
{
    return (bool)((iVisCnstT_Teleport*)m_pProto)->IsEntrance();
}

bool iVisCnst_Teleport::IsTwoWay() const
{
    return (bool)((iVisCnstT_Teleport*)m_pProto)->IsTwoWay();
}

bool iVisCnst_Teleport::CustomPopupText(const iHero* pVisitor, iStringT& ctext) const
{
    if (IsTwoWay()) ctext = gTextMgr[TRID_OBJ_TELEPORT_TWOWAY];
    else if (IsEntrance()) ctext = gTextMgr[TRID_OBJ_TELEPORT_ONEWAY_ENT];
    else ctext = gTextMgr[TRID_OBJ_TELEPORT_ONEWAY_EXT];
    return true;
}


//////////////////////////////////////////////////////////////////////////
void iVisCnst_NewTeleport::OnActivate(iHero* pVisitor, bool bActive)
{
	if(bActive)
		gSfxMgr.PlaySound(CSND_TELEPORT);   
    	
    iBaseMapObject* pTarget = gGame.Map().FindTeleportDestination(this, OutIndex());
    if(!pTarget) return; // nowhere to teleport
    // Check for visitor at target cell
    if (iHero* pTeleportVisitor = DynamicCast<iHero*>(gGame.Map().m_CoverMap.GetAt(pTarget->Pos().x,pTarget->Pos().y))) {
        check(pTeleportVisitor->GetLocation() == pTarget);
        if (pTeleportVisitor->Owner()->PlayerId() != pVisitor->Owner()->PlayerId()) {
#ifdef MULTIPLAYER
            if(!gMPMgr.IsReplaying())
#endif
                pVisitor->AttackHero(pTeleportVisitor);
        }
        else pVisitor->MeetHero(pTeleportVisitor);
    } else {
        gGame.Map().MoveHero(pVisitor->Pos(), pTarget->Pos());
        pVisitor->SetPos(pTarget->Pos());
        pVisitor->Owner()->UpdateFogMap();
        gGame.OnHeroPosChanged(pVisitor, pVisitor->Pos());
        gGame.OnHeroChanged(pVisitor);
        gGame.OnHeroTeleport(pVisitor, Pos(), pTarget->Pos());
    }
}

bool iVisCnst_NewTeleport::CustomPopupText(const iHero* pVisitor, iStringT& ctext) const
{
    ctext = gTextMgr[TRID_OBJ_TELEPORT_ONEWAY_EXT];
    return true;
}


//////////////////////////////////////////////////////////////////////////
uint8 iVisCnst_Keymaster::GetKeyType() const
{
    return ((iVisCnstT_Keymaster*)m_pProto)->GetKeyType();
}

void iVisCnst_Keymaster::OnActivate(iHero* pVisitor, bool bActive)
{
    pVisitor->Owner()->AddKey(GetKeyType());
	if(bActive) {
		iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()],gTextMgr[m_pProto->VisMessage()], PDGG(NEWHERO_KEYGUARD_STONES) + GetKeyType(), PID_NEUTRAL);
		dlg.DoModal();
	}
}

bool iVisCnst_Keymaster::Visited(const iHero* pVisitor) const
{
    return pVisitor->Owner() && pVisitor->Owner()->HasKey(GetKeyType());
}

//////////////////////////////////////////////////////////////////////////
void iVisCnst_TreeOfKnowledge::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->VisMessage()], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        sint32 exp = pVisitor->LevelBound(pVisitor->Level()+1) - pVisitor->Experience();
		if (bActive) {
            iRewardsCtr rew;
			rew.Add(iRewardItem(RIT_EXPERIENCE, 0,  pVisitor->NoExping() ? 0 : exp));
            iDlg_Rewards dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], pVisitor, rew);
            dlg.DoModal();
        }
        pVisitor->ReceiveExperience(exp);
    }

    iVisCnst::OnActivate(pVisitor, bActive);
#ifdef MULTIPLAYER
    gMPMgr.OnHeroUpdate(pVisitor);
    gMPMgr.OnVisCnstUpdate(this);
#endif
}

//////////////////////////////////////////////////////////////////////////
void iVisCnst_Windmill::NewWeek(const iWeekDesc& weekDesk)
{	
    m_bVisited = false;
    UpdateMinerals();
}

void iVisCnst_Windmill::OnActivate(iHero* pVisitor, bool bActive)
{
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->VisMessage()], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        if (bActive) {
            iRewardsCtr rew;
            rew.Add(iRewardItem(RIT_MINERAL, m_mtype, m_mcnt));
            iDlg_Rewards dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], pVisitor, rew);
            dlg.DoModal();
        }
        pVisitor->Owner()->AddMineral(m_mtype, m_mcnt);
        m_bVisited = true;
    }
#ifdef MULTIPLAYER    
    gMPMgr.OnVisCnstUpdate(this);
#endif

}

bool iVisCnst_Windmill::Visited(const iHero* pVisitor) const
{
    return m_bVisited;
}

void iVisCnst_Windmill::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // 
    dbuff.Write((uint8)m_bVisited);
    dbuff.Write((sint32)m_mtype);
    dbuff.Write(m_mcnt);
}

uint8 iVisCnst_Windmill::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
        m_bVisited = false;
        UpdateMinerals();
    } else {
        // Read data
        uint8 bVisited;
        dbuff.Read(bVisited);
        m_bVisited = (bVisited == 0)?false:true;
        sint32 mtype;
        dbuff.Read(mtype);
        m_mtype = (MINERAL_TYPE)mtype;
        dbuff.Read(m_mcnt);
    }
    return flag;
}

void iVisCnst_Windmill::UpdateMinerals()
{
    m_mtype = (MINERAL_TYPE)(MINERAL_ORE + gGame.Map().Rand(6)); 
    m_mcnt = 3 + gGame.Map().Rand(4);
}

//////////////////////////////////////////////////////////////////////////
void iVisCnst_WeeklyMin::NewWeek(const iWeekDesc& weekDesk)
{
    m_bVisited = false;
}

void iVisCnst_WeeklyMin::OnActivate(iHero* pVisitor, bool bActive)
{
    iVisCnstT_WeeklyMin* pProto = (iVisCnstT_WeeklyMin*)m_pProto;
    if (Visited(pVisitor)) {
        if (bActive) {
            iTextDlg tdlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->VisMessage()], pVisitor->Owner()->PlayerId());
            tdlg.DoModal();
        }
    } else {
        if (bActive) {
            iRewardsCtr rew;
            for (uint32 mm=0; mm<MINERAL_COUNT; ++mm) {
                if (pProto->Minerals().quant[mm]) rew.Add(iRewardItem(RIT_MINERAL, mm, pProto->Minerals().quant[mm]));
            }
            iDlg_Rewards dlg(&gApp.ViewMgr(),gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], pVisitor, rew);
            dlg.DoModal();
        }
        for (uint32 mm=0; mm<MINERAL_COUNT; ++mm) {
            if (pProto->Minerals().quant[mm]) 
                pVisitor->Owner()->AddMineral((MINERAL_TYPE)mm, pProto->Minerals().quant[mm]); 
        }
        
        m_bVisited = true;
    }
#ifdef MULTIPLAYER
    gMPMgr.OnVisCnstUpdate(this);
#endif
}

bool iVisCnst_WeeklyMin::Visited(const iHero* pVisitor) const
{
    return m_bVisited;
}

void iVisCnst_WeeklyMin::Serialize(iDynamicBuffer& dbuff)
{
    // Common data serialization
    iVisCnst::Serialize(dbuff);
    // 
    dbuff.Write((uint8)m_bVisited);
}

uint8 iVisCnst_WeeklyMin::Unserialize(iDynamicBuffer& dbuff)
{
    uint8 flag = iVisCnst::Unserialize(dbuff);
    if (flag == 0) {
        // Init by default
        m_bVisited = false;
    } else {
        // Read data
        uint8 bVisited;
        dbuff.Read(bVisited);
        m_bVisited = (bVisited == 0)?false:true;
    }
    return flag;
}
/*
 *  Ownerables
 */
uint32 RandCreaturesCount(CREATURE_TYPE type);

iOwnCnst::iOwnCnst(iOwnCnstT* pProto, const iPoint& pos) 
: iMapCnst(pos), m_pProto(pProto), m_Owner(PID_NEUTRAL), m_bDisabled(false), m_seenIntro(0)
{
	InitArmyRandoms();
}

iPlayer* iOwnCnst::GetArmyOwnerPlayer() { 
	return gGame.Map().FindPlayer(m_Owner); 
}

void iOwnCnst::NewDay() {}

void iOwnCnst::NewWeek(const iWeekDesc& weekDesk) 
{
    if (m_Owner == PID_NEUTRAL) {
        // grow guardians population for neutral construction
        for (uint32 xx=0; xx<7; ++xx) {
            if (Guard().At(xx).IsValid()) GrowArmyGroup(xx);
        }
    }
}

bool iOwnCnst::Activate(iHero* pVisitor, bool bActive, bool& bDisappear)
{
	if(m_bDisabled) return false;


    if (m_Owner != pVisitor->Owner()->PlayerId() && !Guard().Empty()) {
        if (bActive) {
            iDlg_FightGuard dlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], Guard(), pVisitor->Owner()->PlayerId(), VL_BASIC);
            uint32 res = dlg.DoModal();
            if ( res == DRC_YES) {
                iBattleInfo bi(pVisitor, this);
#ifdef MULTIPLAYER
                gMPMgr.OnAttackOwnCnst(pVisitor, this);
#endif              
                gGame.BeginBattle(bi);
            }
            else if( res == DRC_QUICK ){
                
                iBattleInfo bi(pVisitor, this);
                gGame.BeginBattle(bi, true);
            }
        } else {
#ifdef MULTIPLAYER
            if(pVisitor->Owner()->PlayerType() == PT_REMOTE) return false;
#endif

            iBattleInfo bi(pVisitor, this);
#ifdef MULTIPLAYER
            gMPMgr.OnAttackOwnCnst(pVisitor, this);
#endif
            gGame.BeginBattle(bi);
		}
        return false;
    } else if (m_Owner == pVisitor->Owner()->PlayerId()) {
        if (bActive) {
            iDlg_LeaveGuards lgdlg(&gApp.ViewMgr(), pVisitor, this, TRID_LEAVE_GUARDS);
            lgdlg.DoModal();
#ifdef MULTIPLAYER
            gMPMgr.OnOwnCnstUpdate(this);
            gMPMgr.OnHeroUpdate(pVisitor);
#endif
            return true;
        } else {
            // not implemented
            //check(0);
        }
    } else {
		// change the ownership
		if(!(m_seenIntro & (1<<pVisitor->Owner()->PlayerId()))) {				
			if(!m_introMessage.Empty()) {
				iTextDlg dlg(&gApp.ViewMgr(),_T(""),gGame.Map().GetText(TET_OWN_INTROMSG, this),pVisitor->Owner()->PlayerId());
				dlg.DoModal();					
			}			
			m_seenIntro |= (1<<pVisitor->Owner()->PlayerId());
		}

			
		if(pVisitor->MeetsReqs(m_reqs) && !(m_seenIntro & (1 << (8 + pVisitor->Owner()->PlayerId())))) {
			if(!m_okRewards.IsEmpty())
				pVisitor->ReceiveRewards(m_okRewards, bActive, _T(""), gGame.Map().GetText(TET_OWN_OKREWMSG, this).CStr());
			else {
				if(!gGame.Map().GetText(TET_OWN_OKREWMSG, this).Empty()) {
					iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_OWN_OKREWMSG, this), PID_NEUTRAL);
					dlg.DoModal();
				}
				m_seenIntro |= (1 << (8 + pVisitor->Owner()->PlayerId()));
			}
			m_okRewards.RemoveAll();
			m_reqs.RemoveAll();
			m_permFailRewards.RemoveAll();
			m_introMessage.SetEmpty();

			switch(m_okRewBeh) {
				case CRB_USUAL:
					break;
				case CRB_DISAPPEAR:	
					bDisappear = true;
					return false;					
				case CRB_DISABLE:
					m_bDisabled = true;
					return false;				
			}
		} else if(pVisitor->PermanentlyFailedReqs(m_reqs)) {
			if(!m_permFailRewards.IsEmpty())
				pVisitor->ReceiveRewards(m_permFailRewards, bActive, _T(""), gGame.Map().GetText(TET_OWN_PERMFAILMSG, this).CStr());
			else {
				if(!gGame.Map().GetText(TET_OWN_PERMFAILMSG, this).Empty()) {
					iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_OWN_PERMFAILMSG, this), PID_NEUTRAL);
					dlg.DoModal();
				}
				m_seenIntro |= (1 << (8 + pVisitor->Owner()->PlayerId()));
			}
			m_okRewards.RemoveAll();
			m_reqs.RemoveAll();
			m_permFailRewards.RemoveAll();
			m_introMessage.SetEmpty();

			switch(m_permFailRewBeh) {
				case CRB_USUAL:
					break;
				case CRB_DISAPPEAR:	
					bDisappear = true;
					return false;
				case CRB_DISABLE:
					m_bDisabled = true;
					return false;
			}
		} else if(m_reqs.GetSize() > 0 && !(m_seenIntro & (1 << (8 + pVisitor->Owner()->PlayerId())))) {
			if (!m_curFailMessage.Empty()) {
				iTextDlg dlg(&gApp.ViewMgr(), _T(""), gGame.Map().GetText(TET_OWN_CURFAILMSG, this), pVisitor->Owner()->PlayerId());
				dlg.DoModal();
			}
			return false;
		}	


        if (m_Owner != PID_NEUTRAL) {
            iPlayer* oldPlayer = gGame.Map().FindPlayer(m_Owner);
            check(oldPlayer);
            oldPlayer->m_Cnsts.Remove(oldPlayer->m_Cnsts.Find(this));
            //oldPlayer->m_Cnsts.Remove(iPlayer::iCLIt(this));
            oldPlayer->UpdateFogMap();
        } else {
            gGame.Map().m_OwnCnsts.Remove(iGameWorld::iOCIt(this));
        }
        SetOwner(pVisitor->Owner()->PlayerId());
        pVisitor->Owner()->AddCnst(this);
#ifdef MULTIPLAYER
        gMPMgr.OnOwnCnstChangedOwner(this);
#endif
    }
    
    OnActivate(pVisitor, bActive);
    return true;
}

void iOwnCnst::OnActivate(iHero* pVisitor, bool bActive)
{
    if (bActive) {
        gSfxMgr.PlaySound(CSND_FLAG_MINE);
        if (m_pProto->MessageIcon() != 0xFFFF) {
            iIconDlg dlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], m_pProto->MessageIcon(), pVisitor->Owner()->PlayerId());
            dlg.DoModal();
        } else {
            iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[m_pProto->NameKey()], gTextMgr[m_pProto->Message()], pVisitor->Owner()->PlayerId());
            dlg.DoModal();
        }
    }
}

void iOwnCnst::SetOwner(PLAYER_ID newOwner)
{
    PLAYER_ID oldOwner = m_Owner;
    m_Owner = newOwner;
    OnOwnerChanged(newOwner, oldOwner);
    gTutorial.Trigger(TUT_OWNERABLES);
}

void iOwnCnst::OnOwnerChanged(PLAYER_ID newOwner, PLAYER_ID oldOwner)
{
}

//////////////////////////////////////////////////////////////////////////
void iOwnCnst_FurtSkill::OnOwnerChanged(PLAYER_ID newOwner, PLAYER_ID oldOwner)
{
    iOwnCnstT_FurtSkill* pProto = DynamicCast<iOwnCnstT_FurtSkill*>(m_pProto);
    check(pProto);
    if (newOwner == oldOwner) return;
    iFurtSkills nAmount = pProto->FurtSkills();
    // subtract income
    if (oldOwner != PID_NEUTRAL) gGame.Map().FindPlayer(oldOwner)->FurtSkills() -= nAmount;
    // add income
    if (newOwner != PID_NEUTRAL) {
        iPlayer* newOwnerP = gGame.Map().FindPlayer(newOwner);
        // TRICK: #3 - double income from ownerables (only gold & minerals)
        iAI_Player* aiOwner = DynamicCast<iAI_Player*>(newOwnerP);
        if ( aiOwner && (aiOwner->m_HackedTricks & 0xa != 0)) {
            for( uint32 n = FSK_MIN_GOLD; n != FSK_COUNT; n++ ) {
                sint32& val = nAmount.Value((FURTHER_SKILLS)n);
                val = (val*2 + iMAX<sint32>(val,1) )/2;
            }
        }
        // -------------------------
        gGame.Map().FindPlayer(newOwner)->FurtSkills() += nAmount;
    }
}

void iOwnCnst::Serialize(iDynamicBuffer& dbuff)
{
	// Guard
	::Serialize(dbuff, Guard());
	
	// reqs, msgs and rewards
	::Serialize(dbuff, IntroMessage());
	::Serialize(dbuff, CurFailMessage());
	::Serialize(dbuff, OKRewardMessage());
	::Serialize(dbuff, PermFailRewardMessage());
	::Serialize(dbuff, Reqs());
	::Serialize(dbuff, OKRewards());
	::Serialize(dbuff, PermFailRewards());	
	dbuff.Write((uint8)OkRewBehaviour());
	dbuff.Write((uint8)PermFailRewBehaviour());
	dbuff.Write((uint8)m_bDisabled);
	dbuff.Write(m_seenIntro);
}

void iOwnCnst::Unserialize(iDynamicBuffer& dbuff)
{
	// Guard
	UnserializeArmy(dbuff);

	InitArmyRandoms();
	
	// reqs, rewards and messages		
	::Unserialize(dbuff, IntroMessage());
	::Unserialize(dbuff, CurFailMessage());
	::Unserialize(dbuff, OKRewardMessage());
	::Unserialize(dbuff, PermFailRewardMessage());
	::Unserialize(dbuff, Reqs());
	::Unserialize(dbuff, OKRewards());
	::Unserialize(dbuff, PermFailRewards());		
	uint8 beh;
	dbuff.Read(beh); OkRewBehaviour() = (CNST_REW_BEH)beh;
	dbuff.Read(beh); PermFailRewBehaviour() = (CNST_REW_BEH)beh;
	
	uint8 dis; dbuff.Read(dis);
	m_bDisabled = dis > 0;
	dbuff.Read(m_seenIntro);
}
