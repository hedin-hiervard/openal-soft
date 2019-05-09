#include "stdafx.h"
#include "helpers.h"

//////////////////////////////////////////////////////////////////////////
 
/*const iStringT& iCastle::Name() const
{ 
	if(m_bIsCustomName)
		return gGame.Map().GetText(TET_CASTLE_CNAME, (void*)this); 
	else {
		m_Name = gTextMgr[m_NameKey];
		return m_Name;
	}
}*/

iStringT& iCastle::Name()
{ 
	if(m_bIsCustomName)
		return (iStringT&)gGame.Map().GetText(TET_CASTLE_CNAME, (void*)this); 
	else {
		m_Name = gTextMgr[m_NameKey];
		return m_Name;	
	}
}

iPlayer* iCastle::GetArmyOwnerPlayer() { 
	return gGame.Map().FindPlayer(m_Owner); 
}

iCastle::iCastle(iCastleT* pProto, const iPoint& pos, const iStringT& cname, bool bCreateHero)
: iMapCnst(pos), m_pProto(pProto), m_Owner(PID_NEUTRAL), m_bCanBuild(true), m_Cnsts(this), m_bCreateHero(bCreateHero), 
   m_scoutMod(0), m_shipsCount(1)
{
	// Castle's name
	if (!cname.Empty() && cname.GetUInt() == 0) { m_Name = cname; m_bIsCustomName = true; m_NameKey = 0; }
	else { 
		if(cname.GetUInt() == 0)
			m_NameKey = gGame.ItemMgr().m_CastleRepos.GetCastleName(m_pProto->Type());
		else
			m_NameKey = cname.GetUInt();
		m_Name = gTextMgr[m_NameKey];
		m_bIsCustomName = false;
	}

	// Init constructions
	for (uint32 xx=0; xx<CTLCNST_COUNT; ++xx){
		if (CTLCNSTS_DESC[xx].caps.Support(m_pProto->Type(),m_pProto->Size())) {
			iCtlCnst* pCtlCnst = NULL;
			CTL_CONSTR cnstId = (CTL_CONSTR)xx;
			switch (CTLCNSTS_DESC[xx].type) {
			case CCT_GENERIC: pCtlCnst = new iCtlCnst(this, cnstId); break;
			case CCT_DWELLING: pCtlCnst = new iDwellCtlCnst(this, cnstId, (CREATURE_TYPE)CTLCNSTS_DESC[cnstId].fparam); break;
			case CCT_MAGEGUILD: pCtlCnst = new iMGuildCtlCnst(this, cnstId,CTLCNSTS_DESC[cnstId].fparam); break;
			case CCT_TAVERN: pCtlCnst = pCtlCnst = new iTavernCtlCnst(this, cnstId); break;
			case CCT_MAGICNODE: pCtlCnst = new iMagicNodeCtlCnst(this, cnstId); break;
			case CCT_MINE: pCtlCnst = new iMineCtlCnst(this, cnstId, (MINERAL_TYPE)CTLCNSTS_DESC[cnstId].fparam, CTLCNSTS_DESC[cnstId].sparam); break;
			case CCT_PERM_FSK_MOD: pCtlCnst = new iPermFSKModCtlCnst(this, cnstId, (FURTHER_SKILLS)CTLCNSTS_DESC[cnstId].fparam, CTLCNSTS_DESC[cnstId].sparam); break;
			case CCT_DWELL_ENC: pCtlCnst = new iDwellEncCtlCnst(this, cnstId, (CTL_CONSTR)CTLCNSTS_DESC[cnstId].fparam, CTLCNSTS_DESC[cnstId].sparam); break;
			case CCT_OBSERVPOST: pCtlCnst = new iObsPostCtlCnst(this, cnstId); break;
			case CCT_LIBRARY: pCtlCnst = new iLibraryCtlCnst(this, cnstId); break;
			case CCT_MANAVORTEX: pCtlCnst = new iManaVtxCtlCnst(this, cnstId); break;
			case CCT_TREASURY: pCtlCnst = new iTreasuryCtlCnst(this, cnstId); break;
			case CCT_MYSTICPOUND: pCtlCnst = new iMystPoundCtlCnst(this, cnstId); break;
			case CCT_NECRAMPLIFIER: pCtlCnst = new iNecrAmpCtlCnst(this, cnstId); break;
			case CCT_COVEROFDARKNESS: pCtlCnst = new iCovOfDarkCtlCnst(this, cnstId); break;

            case CCT_LODGE: pCtlCnst = new iLodgeCtlCnst(this, cnstId); break;
            case CCT_MERCHANT: pCtlCnst = new iMerchantCtlCnst(this, cnstId); break;

			default: check("Unknown construction type" == 0);
			};
			m_Cnsts.Add(pCtlCnst);
		}
	}
	
	UpdateCnsts();
}

void iCastle::Init()
{
	// Init garrison
	if (m_Owner == PID_NEUTRAL && Army().Empty()) {
		GenerateGuards();
	}

	// Pre build constructions for debug
	//if (iCtlCnst* pMoat = m_Cnsts.GetCnst(CTLCNST_MOAT)) pMoat->Build();
	//if (iCtlCnst* pLTurret = m_Cnsts.GetCnst(CTLCNST_LTURRET)) pLTurret->Build();
	//if (iCtlCnst* pMTurret = m_Cnsts.GetCnst(CTLCNST_MTURRET)) pMTurret->Build();
	m_Cnsts.Init();
	UpdateCnsts();

	InitArmy();

	// Create Hero
	if (m_bCreateHero) {
		uint16 selhid = gGame.ItemMgr().HeroesMgr().Select(CTL_HEROES[m_pProto->Type()]);
		if (selhid == iHeroesMgr::INVALID_HERO_ID) selhid = gGame.ItemMgr().HeroesMgr().Select(HTM_ALL);
		if (selhid != iHeroesMgr::INVALID_HERO_ID) {
			iHero* pHero = gGame.ItemMgr().HeroesMgr().Get(selhid);
			iPlayer* pOwner = gGame.Map().FindPlayer(m_Owner);
			check(pHero && pOwner);
			gGame.Map().AddHero(pHero, pOwner, m_Pos, HERO_ORIENT[m_pProto->Orient()], false);
		} else {
			check(0);
		}
	}
}

void iCastle::SetOwner(PLAYER_ID newOwner)
{
	if (newOwner == m_Owner) return;

	PLAYER_ID oldOwner = m_Owner;
	m_Owner = newOwner;

	if (oldOwner != PID_NEUTRAL) {
		iPlayer *pOldOwn = gGame.Map().FindPlayer(oldOwner);
		if(pOldOwn)
			pOldOwn->FurtSkills().Value(FSK_MIN_GOLD) -= CTL_INCOME;
	}
	if (newOwner != PID_NEUTRAL) gGame.Map().FindPlayer(newOwner)->FurtSkills().Value(FSK_MIN_GOLD) += CTL_INCOME;
	m_Cnsts.OnOwnerChanged(newOwner, oldOwner);
}

void iCastle::OnSelect()
{
    gTutorial.Trigger(TUT_CASTLE_SELECTED);
	gGame.OnCastleChanged(this);
}

void iCastle::OnVisitorEnter(iHero* pHero)
{
	m_Cnsts.OnVisitorEnter(pHero);
}

void iCastle::OnVisitorLeave(iHero* pHero)
{
	m_Cnsts.OnVisitorLeave(pHero);
}

void iCastle::NewDay()
{
	m_bCanBuild = true;
	m_Cnsts.NewDay();
	m_Cnsts.UpdateState(m_bCanBuild);
}

void iCastle::NewWeek(const iWeekDesc& weekDesk)
{
	m_Cnsts.NewWeek(weekDesk);
	if (m_Owner == PID_NEUTRAL) {
		GenerateGuards();
	}

    // reset number of available ships
    m_shipsCount = 1;
}

uint32 iCastle::GetConstrCaps()
{
	return 0;
}

iTavernCtlCnst* iCastle::GetTavern(bool bOnlyBuilt)
{
	if (iTavernCtlCnst* pTavern = DynamicCast<iTavernCtlCnst*>(m_Cnsts.GetCnst(CTLCNST_TAVERN))) {
		if (!bOnlyBuilt || pTavern->IsBuilt()) return pTavern;
	}
	return NULL;
}

uint32 iCastle::GarrisonPower(bool bOnlyFort) const
{
	uint32 result = 0;
	
	if (!bOnlyFort){
		result += Garrison().ArmyPower();
		if (!result) return 0;
	}
	const iCtlCnst* pCnst;
	if (pCnst = m_Cnsts.GetCnst(CTLCNST_LTURRET)) if (pCnst->IsBuilt()) result += 1000;
	if (pCnst = m_Cnsts.GetCnst(CTLCNST_RTURRET)) if (pCnst->IsBuilt()) result += 1000;
	if (pCnst = m_Cnsts.GetCnst(CTLCNST_MTURRET)) if (pCnst->IsBuilt()) result += 1000;
	if (pCnst = m_Cnsts.GetCnst(CTLCNST_MOAT)) if (pCnst->IsBuilt()) result += 1000;
	return result;
}

sint32 iCastle::Scouting() const
{
	return m_pProto->Scouting() + m_scoutMod;
}

void iCastle::Build(iCtlCnst* pCnst)
{
	check(m_bCanBuild);
	gGame.Map().FindPlayer(Owner())->AddMineralSet(-pCnst->Price());
	pCnst->Build();
#ifndef _DEBUG
	m_bCanBuild = false;
#endif
	UpdateCnsts();
#ifdef MULTIPLAYER
	gMPMgr.OnCastleUpdate(this);	
#endif

}

void iCastle::Build(uint32 cnstIdx)
{
	Build(m_Cnsts[cnstIdx]);
#ifdef MULTIPLAYER
	gMPMgr.OnCastleUpdate(this);	
#endif

}

void iCastle::UpdateCnsts()
{
	m_Cnsts.UpdateState(m_bCanBuild);
}

void iCastle::GenerateGuards()
{
	for (uint32 xx=0; xx<3; ++xx) {
		if ( gGame.Map().Rand(3) == 0 ) {
			CREATURE_TYPE ct = (CREATURE_TYPE)(CREAT_PEASANT + m_pProto->Type()*6 + xx);
			AddGroupToArmy(ct, CREAT_DESC[ct].growth);
		}
	}
}

bool iCastle::CheckPathToShip(iPlayer* pPlayer, const iPoint &pt)
{
    bool res = false;
    iPoint src(Pos());
    iPath path(src);  
    // replace cells at ship and around possible ship location... (3x3, 9 cells)

    iGameWorld::iPassMap& pm = gGame.Map().m_PassMap; //pPlayer->PassMap();

    // modify pass map, so ship is passable
/*
    if (pm.IsValidPos(pt.x, pt.y)) 
    {
        oldps = pm.GetAt(pt.x, pt.y);
        pm.GetAt(pt.x, pt.y) = 0;     // path cost doesn't matter for us
    }*/


    iPathFinder pf(pm);
    if (pf.FindPathToShip(src, pt, path, CSTL_PASS_MASK, 0xFF) != -1)
    {   
   /*     path.DumpPath();
        size_t l = path.m_Path.GetSize();
        if (l > 1)            
        {   
            const iPoint &pos1 = path.m_Path[l - 2].m_Pos;
            sint8 s1 = pm.GetAt(pos1.x, pos1.y);
            if (s1 == PASSMAP_PWATER || (s1 != PASSMAP_PWATER && s1 != PASSMAP_SWATER))
            {
                res = true;
            }
        }  */      
        res = true;
    }

    // restore pass map back
/*
    if (pm.IsValidPos(pt.x, pt.y)) 
    {
        pm.GetAt(pt.x, pt.y) = oldps; 
    }*/

    return res;
}


// IHarbor implementation

uint8 iCastle::NumOfDocks() { return 1; }

iPlayer* iCastle::GetOwner()
{
	return gGame.Map().FindPlayer(Owner());
}

bool iCastle::IsBoardable() { return false; }

void iCastle::ShipBought(uint8 dock)
{
	check(dock == 0);
	check(m_shipsCount > 0);
    m_shipsCount--;        
}

uint8 iCastle::ShipsAvailable(uint8 dock)
{
    check(dock == 0)
	return m_shipsCount;
}

bool iCastle::IsShipBuildSupported()
{
	if(!gGame.Map().IsShipsEnabled())
		return false;
    
	// check 4 x 4 area around castle
    bool result = false;
    const iPoint &pos = Pos();
    if (Owner() == PID_NEUTRAL) return result;

	for (uint32 cvr=0; cvr<Proto()->AvatarsCount(); ++cvr){
		const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[Proto()->AvatarEntry(cvr).aid];
		iPoint cpos(Pos().x+Proto()->AvatarEntry(cvr).ox, Pos().y+Proto()->AvatarEntry(cvr).oy);
		for (sint32 xx = cpos.x - 4; xx < cpos.x + 4; xx++)
		{
			for (sint32 yy = cpos.y - 4; yy < cpos.y + 4; yy++)
			{
				if (!gGame.Map().IsValidPos(xx, yy))
				{
					continue;
				}           

				iCell &c = gGame.Map().GetAt(xx, yy);
				if (c.surf != STYPE_WATER)
				{
					// not solid water
					continue;
				}
				
				if (!gGame.Map().IsCellNearBeach(iPoint(xx, yy)))
				{
					// cell isn't near beach
					continue;
				}

				if (!CheckPathToShip(gGame.Map().FindPlayer(Owner()), iPoint(xx, yy)))
				{
					// hero can't reach the ship
					continue;
				}
				// at least one cell with water around castle
				result = true;
				break;
			}
		}
	}
    return result;
}

iPoint iCastle::GetShipPos(uint8 dock)
{
	check(dock == 0);
    // check 4 x 4 area around castle
	iPoint res;
    const iPoint &pos = Pos();
    if (Owner() == PID_NEUTRAL) return cInvalidPoint;
    iPlayer * pPlayer = gGame.Map().FindPlayer(Owner());

    if (!pPlayer->PassMap().IsValidPos(pos.x, pos.y))
    {
        return cInvalidPoint;        
    }
        
    // save old pass value and put new 
    // we don't care about path cost, so we put 0 as passable
    sint8 oldc = pPlayer->PassMap().GetAt(pos.x, pos.y);
    pPlayer->PassMap().GetAt(pos.x, pos.y) = 0;

	for (uint32 cvr=0; cvr<Proto()->AvatarsCount(); ++cvr){
		const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[Proto()->AvatarEntry(cvr).aid];
		iPoint cpos(Pos().x+Proto()->AvatarEntry(cvr).ox, Pos().y+Proto()->AvatarEntry(cvr).oy);
		for (sint32 xx = cpos.x - 4; xx < cpos.x + 4; xx++)
		{
			for (sint32 yy = cpos.y - 4; yy < cpos.y + 4; yy++)
			{
				if (!gGame.Map().IsValidPos(xx, yy))
				{
					continue;
				}           

				iCell &c = gGame.Map().GetAt(xx, yy);
				if (c.surf != STYPE_WATER)
				{
					// not solid water
					continue;
				}

				if (gGame.Map().m_CoverMap.GetAt(xx, yy))
				{
					// busy with some object already
					continue;
				}

				if (pPlayer->FogMap().IsHidden(iPoint(xx, yy)) || pPlayer->FogMap().IsInvisible(iPoint(xx, yy)))
				{
					// not visible skip
					continue;
				}

				if (!gGame.Map().IsCellNearBeach(iPoint(xx, yy)))
				{
					// cell isn't near beach
					continue;
				}

				if (!CheckPathToShip(pPlayer, iPoint(xx, yy)))
				{
					// hero can't reach the ship
					continue;
				}

				res.x = xx;
				res.y = yy;
				// restore old value in pass map
				pPlayer->PassMap().GetAt(pos.x, pos.y) = oldc;
				return res;
			}
		}
	}
    pPlayer->PassMap().GetAt(pos.x, pos.y) = oldc;    
    return cInvalidPoint;
}

iShip* iCastle::GetShip( uint8 dockType )
{
	iPoint pt = GetShipPos(dockType);
	if(pt == cInvalidPoint) return NULL;
	if (iBaseMapObject* pMapObj = gGame.Map().m_CoverMap.GetAt(pt.x, pt.y))	
		return  DynamicCast<iShip*>(pMapObj);
	else
		return NULL;
}

iDwellCtlCnst* iCastle::GetDwelling( CREATURE_TYPE ct )
{
	for (uint32 xx=0; xx < Cnsts().Count(); ++xx) 
		if (iDwellCtlCnst* pDwel = DynamicCast<iDwellCtlCnst*>(Cnst(xx)))
			if(pDwel->IsBuilt() && pDwel->CrType() == ct) {
				return pDwel;
			}
	return NULL;
}

bool iCastle::IsMGuildBuilt()
{
	for (uint32 xx=0; xx < Cnsts().Count(); ++xx) 
		if((Cnst(xx)->Type() == CTLCNST_MAGEGUILD_L1 && Cnst(xx)->IsBuilt()) ||
			(Cnst(xx)->Type() == CTLCNST_MAGEGUILD_L2 && Cnst(xx)->IsBuilt()) ||
			(Cnst(xx)->Type() == CTLCNST_MAGEGUILD_L3 && Cnst(xx)->IsBuilt()) ||
			(Cnst(xx)->Type() == CTLCNST_MAGEGUILD_L4 && Cnst(xx)->IsBuilt()) ||
			(Cnst(xx)->Type() == CTLCNST_MAGEGUILD_L5 && Cnst(xx)->IsBuilt()))
				return true;
			
	return false;
}

// END OF FILE
