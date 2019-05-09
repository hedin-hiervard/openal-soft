#include "Stdafx.h"
#include "serialize.h"
#include "helpers.h"
#include "Dlg_TimeEvent.h"
#include "lzo/lzo.h"
//#include "xxc/xxc.app.h"
//#include "xxc/xxc.file.h"
//#include "xxc/xxc.sechwid.h"
//#include "xxc/xxc.security.h"

#ifdef MULTIPLAYER
#include "Remote_Player.h"
#endif

#include "OverlandView.h"
#include "OverlandView.touch.h"
#include "Observer.h"

//////////////////////////////////////////////////////////////////////////
const sint32 SCR_DIFMODIF[DFC_COUNT] = { 70, 100, 120, 140, 160 };
const sint32 SCR_SIZMODIF[MSIZ_COUNT] = { 120, 100, 80, 60 };

//#pragma optimization_level(1)
/*
 *  
 */
iGameWorld::iGameWorld()
: m_pCurPlayer(NULL), m_pActPlayer(NULL), m_pVanItem(NULL), m_CurDay(1), m_pObserver(NULL), 
m_globalProperties(0), m_nextWeek(NULL)
{}

iGameWorld::~iGameWorld()
{ 
    if(m_pObserver) delete m_pObserver;
    CleanupGameWorld(); 
}

uint32 iGameWorld::CalcGameScore() const
{
    // m_CurDay
    sint32 bscore = 200 - m_CurDay * SCR_SIZMODIF[m_mapSiz] / 100;
    if (bscore < 1) return 0;
    return bscore * SCR_DIFMODIF[m_Difficulty] / 100;
}

iBaseMapObject* iGameWorld::GetCellObj(const iPoint& cell, sint32 radius)
{
    for(sint32 r=0; r<=radius; r++)
        for(sint32 x=-r; x<=r; x++)
            for(sint32 y=-r; y<=r; y++) {
                iBaseMapObject* obj;
                if (IsValidPos(cell.x +x,cell.y+y) && (obj = m_CoverMap.GetAt(cell.x+x,cell.y+y)))
                    return obj;
            }
    return 0;
}

iBaseMapObject* iGameWorld::GetCellObj(const iPoint& cell, iStringT& name)
{
    if ( iBaseMapObject* pObj = m_CoverMap.GetAt(cell.x,cell.y)) return pObj;

    uint16 avatar = GetAt(cell.x,cell.y).avatar;
    if ( avatar != 0xFFFF ){
        if ( ( avatar & 0xFF00) == 0xFF00) {
            // path element
            name = gTextMgr[gGame.ItemMgr().m_PathElProts[avatar & 0xFF]->NameKey()];
        } else {
            // avatar title
            name = gTextMgr[gGame.ItemMgr().m_AvatarMgr[avatar].Name()];
        }
    } else {
        // surface type
        iCell c = GetAt(cell.x,cell.y);
        sint32 upSurf = iMAX(iMAX(iMAX(c.SurfNode(0),c.SurfNode(1)),c.SurfNode(2)),c.SurfNode(3));
        sint32 btSurf = iMIN(iMIN(iMIN(c.SurfNode(0),c.SurfNode(1)),c.SurfNode(2)),c.SurfNode(3));
        if (btSurf == STYPE_WATER && btSurf != upSurf) name = gTextMgr[TRID_SURF_COAST];
        else name = gTextMgr[TRID_SURF_WATER+(sint16)upSurf];
#ifdef _DEBUG
        name.Addf(_T(" (%d,%d)"),cell.x,cell.y);
#endif
    }

    return NULL;
}
//
void iGameWorld::StartGame(PLAYER_ID curPlayer, bool bAfterLoad)
{
    check(!m_pCurPlayer);

	if(!bAfterLoad)
		gMapEnum.MarkMapStarted(m_Name[GLNG_ENGLISH]);
	
    // Set current player (first player must be PT_HUMAN type)
    iPlayer* pCurPlayer = (curPlayer==PID_NEUTRAL)?(*m_Players.First()):FindPlayer(curPlayer);
  //  check(pCurPlayer && pCurPlayer->PlayerType() == PT_HUMAN);
    m_pCurPlayer = pCurPlayer;

    // 

    if (m_gameMode == iMapInfo::GM_SPLAYER) 
	{
		m_pActPlayer = m_pCurPlayer->PlayerType() == PT_HUMAN ? m_pCurPlayer : NULL;
	} else if(m_gameMode == iMapInfo::GM_HOTSEAT)
	{
		SetNewActor(m_pCurPlayer->PlayerType() == PT_HUMAN ? m_pCurPlayer : NULL, false);
	}
#ifdef MULTIPLAYER
    else if (m_gameMode == iMapInfo::GM_NETWORK) { // find the acting player
        for (iGameWorld::iPLIt pit = gGame.Map().m_Players.First(); pit != gGame.Map().m_Players.End(); ++pit) {
            if((*pit)->PlayerType() == PT_HUMAN) {
                SetNewActor(*pit, false);
                break;
            }
        }
    }
#endif
	if(!m_pActPlayer) {
		m_pActPlayer = m_pObserver = new iObserver_Player();
		if(m_pCurPlayer->CurHero())
			gGame.MainView()->Composer().CenterView(m_pCurPlayer->CurHero()->Pos());
		else if(m_pCurPlayer->CurCastle())
			gGame.MainView()->Composer().CenterView(m_pCurPlayer->CurCastle()->Pos());
	}
    
#ifndef MULTIPLAYER
    gGame.OnPlayerChanged(m_pCurPlayer, true);
    m_pCurPlayer->OnSelect(false, bAfterLoad);
#else
    gGame.OnPlayerChanged(m_pCurPlayer, m_pActPlayer == m_pCurPlayer);  
    m_pActPlayer->OnSelect(false, bAfterLoad);
#endif	
}

//
iPlayer* iGameWorld::AddPlayer(PLAYER_TYPE_MASK playerTypeMask, PLAYER_ID pid, NATION_TYPE nation, PLAYER_TYPE pt)
{
    iPlayer *pPlayer; 
    if (pt == PT_HUMAN) pPlayer = new iPlayer(playerTypeMask, pid,nation,iSize(m_Width,m_Height));
    else if (pt == PT_COMPUTER) pPlayer = new iAI_Player(playerTypeMask, pid,nation,iSize(m_Width,m_Height));
#ifdef MULTIPLAYER
    else if (pt == PT_REMOTE) pPlayer = new iRemote_Player(playerTypeMask, pid,nation,iSize(m_Width,m_Height));
#endif
    else return NULL;
    m_Players.Add(pPlayer);
    return pPlayer;
}

void iGameWorld::ReplacePlayer(iPlayer* o, iPlayer *n)
{
    iPLIt it = m_Players.Find(o);       
    iPLIt prev = it; prev--;
    m_Players.Remove(it);
    m_Players.InsertAfter(prev, n);
    if(m_pCurPlayer == o) m_pCurPlayer = n;
}

void iGameWorld::RemovePlayer( iPlayer* pPlayer, LOSE_CONDITION_TYPE lc )
{
#ifdef MULTIPLAYER
    gMPMgr.OnPlayerDefeated(pPlayer->PlayerId());
#endif
	// check if the player was already removed
	if(pPlayer->Removed()) return;

	// this player is not on the list
	if(m_Players.Find(pPlayer) == m_Players.End()) return;

    // Release all constructions and castles
    iPlayer::iCLIt cit = pPlayer->m_Cnsts.First();
    while (cit != pPlayer->m_Cnsts.End()){
        iOwnCnst* pCnst = *cit;
        cit = pPlayer->m_Cnsts.Remove(cit);
        pCnst->SetOwner(PID_NEUTRAL);
        m_OwnCnsts.Add(pCnst);
    }

	// when de-owning castles DON'T check for victory to avoid reentering this procedure again
    iPlayer::iCtLIt ctit = pPlayer->CastleFirst();
    while (ctit != (iPlayer::iCtLIt)pPlayer->CastleEnd()){
        iCastle* pCastle= *ctit;
        ctit = pPlayer->RemoveCastle(*ctit, false);
        pCastle->SetOwner(PID_NEUTRAL);
        m_Castles.Add(pCastle);
    }

	// dismiss all heroes NOT checking for victory to avoid reentering this procedure again
	while(pPlayer->HeroCount() > 0)
		RemoveHero(*(pPlayer->HeroFirst()), true, false);	
	

    // are we removing active player?
    bool bAct = (m_pActPlayer == pPlayer);
    // are we removing current player?
    bool bCur = (m_pCurPlayer == pPlayer);
    // are we removing human player?
    //bool bHum = (pPlayer->PlayerType() == PT_HUMAN);  

	pPlayer->SetRemoved(true);

    // Find Next Actor
    iPlayer* pNextActor = NULL;

#ifdef MULTIPLAYER
	FIXME: CHECK VICTORY CONDITIONS
    if(bAct && gMPMgr.IsMultiplayer()) {
        bool stay;
        if(m_Players.Count() <= 2) {        
            for (iGameWorld::iPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit){
                if(m_pActPlayer != *pit) {
                    gMPMgr.OnPlayerWon(pit->PlayerId());
                    break;
                }
            }   
            //gMPMgr.DisconnectFromServer();
            // Show Defeat dlg and exit game
            gGame.OnDefeat(true);
            return; 
        } 
        else if((gMPMgr.IsReplaying() || gMPMgr.GetHostType() == iMPManager::HT_HOST)) stay = true;
        else {
            gMPMgr.DisablePRA();
            iQuestDlg qdlg(&gApp.ViewMgr(), gTextMgr[TRID_DEFEATED], gTextMgr[TRID_STAY_OBSERVER_QSTN], PID_NEUTRAL);
            if (qdlg.DoModal() == DRC_YES)
                stay = true;        
            else {              
                gMPMgr.OnPlayerDropped(m_pActPlayer->PlayerId());
                gGame.OnDefeat(true);
                //gMPMgr.DisconnectFromServer();                
                return;
            }
        }
        if(stay) {
            m_pObserver = new iObserver_Player();
            m_pActPlayer = m_pObserver;
        } else {
            gMPMgr.OnPlayerDropped(m_pActPlayer->PlayerId());
        }
    }
    else
#endif

	// if a removed player was defeated, show him defeat message and exit the game if he is the only active one
	// this has NOTHING to do with victory or lose conditions
    if (bAct) {
        // Find another human player
        //iPlayer* pNewActPlayer = NULL;
        for (iPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit) {
            if ( *pit != pPlayer && (*pit)->PlayerType() == PT_HUMAN) {
                pNextActor = *pit;
                break;
            }
        }
        if (!pNextActor) {
            // Show Defeat dlg and exit game
            gGame.OnDefeat(lc, true);
            return;
        } else {
            // Show Defeat dlg 
            gGame.OnDefeat(lc, false);
        }
    }

    // SiGMan: I moved check here from (*) to declare victory 
    // before new day comes #0000245
    // Check for victory condition
    VICTORY_CONDITION_TYPE vc = m_pActPlayer->MeetsVictoryCondition();
	if(m_pActPlayer != pPlayer && vc != VC_INVALID) 
	{        
		gGame.OnVictory(vc);
        return; // Hedin: added this due to #00000459 (bogus it's 'x' player's turn appears after victory in hotseat)
    }

	
    // If Current player vanqushed, then select next player
    if (bCur) {
        // Selects next player in the list
        // if iteration is over
        //      - choose first player
        //      - increment day and month
        // set new actor if human player
        //  - update m_pActPlayer
        //  - notify OnActorChanged
        // notify OnPlayerChanged
        // notify player OnSelect
        NextPlayer();
    }

    // Select new actor if not already selected
    if (bAct && pPlayer == m_pActPlayer && pNextActor) {        
        SetNewActor(pNextActor, false);
    }

	// (*)

	// Delete player
	PLAYER_ID id = pPlayer->PlayerId();

	iPLIt ppli(pPlayer);
	m_Players.Delete(ppli);


    // Player 'x' Vanquished
    // i moved this to the end, because we can receive 'player dropped' while in modal state, 
    // and it will try to replace player with AI if he is found in the list. Hedin
	if(!gGame.IsExitingToMenu())
		gGame.OnPlayerVanquished(id);
}

void iGameWorld::SetHeroPos(iHero* pHero, const iPoint& pos)
{
    // Check for map construction entrance
    if (iBaseMapObject* pMapObj = m_CoverMap.GetAt(pos.x,pos.y)){
        // it must be map construction
        if (DynamicCast<iCastle*>(pMapObj) || DynamicCast<iVisCnst*>(pMapObj) || DynamicCast<iOwnCnst*>(pMapObj)){
            pHero->SetLocation((iMapCnst*)pMapObj);
        } else {
            check(0);
        }
    } else {
        pHero->SetLocation(NULL);
    }

    m_CoverMap.GetAt(pos.x,pos.y) = pHero;
 //   DWORD dw = (DWORD)&(m_CoverMap.GetAt(pos.x,pos.y));

    // Hide the hero
    if (pHero->Artifacts().HasArtType(ARTT_CURSWORD)) {
        for (iPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit) {
            if ( (*pit)->PlayerId() != pHero->Owner()->PlayerId() ){
                (*pit)->FogMap().DiscoverMap(pos.x, pos.y, 2, true);                
            }
        }
    }
}

void iGameWorld::SetShipPos(iShip* pShip, const iPoint& pos)
{  /*
    // Check for map construction entrance
    if (iBaseMapObject* pMapObj = m_CoverMap.GetAt(pos.x,pos.y)){
        // it must be map construction
        // CtllTODO: remove castle support?
     /*   if (DynamicCast<iCastle*>(pMapObj) || DynamicCast<iVisCnst*>(pMapObj) || DynamicCast<iOwnCnst*>(pMapObj)){
            pShip->SetLocation((iMapCnst*)pMapObj);
        } else {
            check(0);
        }
    } else {
        pShip->SetLocation(NULL);
    } */

    m_CoverMap.GetAt(pos.x,pos.y) = pShip;

    // Hide the ship
    // CtllTODO: should ship support this?
    /*
    if (pHero->Artifacts().HasArtType(ARTT_CURSWORD)) {
        for (iPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit) {
            if ( (*pit)->PlayerId() != pShip->Owner()->PlayerId() ){
                (*pit)->FogMap().DiscoverMap(pos.x, pos.y, 2, true);                
            }
        }
    } */
}



iHero* iGameWorld::AddHero(iHero* pHero, iPlayer* pOwner, const iPoint& pos, sint8 angle, bool bAfterLoad, bool bUpdateFog)
{
    pHero->Init(pOwner, pos, angle, bAfterLoad);
    pHero->Owner()->AddHero(pHero, bUpdateFog);
    SetHeroPos(pHero, pos);
    return pHero;
}

void iGameWorld::RemoveHero(iHero* pHero, bool bResetArmy, bool bCheckVictory)
{	
    if(pHero->Pos() == cInvalidPoint) 
        return;

    iPlayer* pPlayer = pHero->Owner();
    check(DynamicCast<iHero*>(m_CoverMap.GetAt(pHero->Pos().x,pHero->Pos().y)));

#ifdef MULTIPLAYER
    gMPMgr.OnHeroDefeated(pHero, bResetArmy);
#endif
    if (pHero->GetLocation()) { 
        m_CoverMap.GetAt(pHero->Pos().x,pHero->Pos().y) = pHero->GetLocation();
    } else {
        m_CoverMap.GetAt(pHero->Pos().x,pHero->Pos().y) = NULL;
    }

    if (pHero->InShip())
    {
        iShip* pShip = pHero->GetShip();
        pShip->HeroDeath(pHero);
        RemoveShip(pShip);
    }

    pPlayer->RemoveHero(pHero, bCheckVictory);
    pHero->Deinit(bResetArmy);
    gGame.ItemMgr().HeroesMgr().Put(pHero);
    m_pCurPlayer->UpdateVisItems();		
}

void iGameWorld::MoveHero(const iPoint& oldPos, const iPoint& newPos)
{
    iHero* pHero = DynamicCast<iHero*>(m_CoverMap.GetAt(oldPos.x,oldPos.y));
    check(pHero);

#ifdef MULTIPLAYER      
    gMPMgr.OnHeroMoved(oldPos, newPos, pHero->Moves());
#endif


    // Leave construction if required
    if (iMapCnst* pMapCnst = pHero->GetLocation()) {
        m_CoverMap.GetAt(oldPos.x,oldPos.y) = pMapCnst;
    } else {
        m_CoverMap.GetAt(oldPos.x,oldPos.y) = NULL;
    }

    SetHeroPos(pHero, newPos);
}



iVisCnst* iGameWorld::AddVisCnst(iVisCnst* pCnst)
{
    m_VisCnsts.Add(pCnst);
    for (uint32 xx=0; xx<pCnst->Proto()->CoversCount(); ++xx) {
        sint32 px = pCnst->Pos().x+pCnst->Proto()->CoverEntry(xx).ox;
        sint32 py = pCnst->Pos().y+pCnst->Proto()->CoverEntry(xx).oy;
        if (m_CoverMap.IsValidPos(px,py)) {
            m_CoverMap.GetAt(px,py) = pCnst;
        } else {
            check(0 == "Construction's cell is out of map!");
        }
    }
    return pCnst;
}

void iGameWorld::RemoveVisCnst(iVisCnst* pCnst)
{
    m_VisCnsts.Remove(iVCIt(pCnst));
    for (uint32 xx=0; xx<pCnst->Proto()->CoversCount(); ++xx) {
        sint32 px = pCnst->Pos().x+pCnst->Proto()->CoverEntry(xx).ox;
        sint32 py = pCnst->Pos().y+pCnst->Proto()->CoverEntry(xx).oy;
        if (m_CoverMap.IsValidPos(px,py)) {
			iBaseMapObject *bmo = m_CoverMap.GetAt(px,py);
			if(DynamicCast<iVisCnst*>(bmo))
				m_CoverMap.GetAt(px,py) = NULL;
			else if(iHero* pHero = DynamicCast<iHero*>(bmo)) {
				pHero->SetLocation(NULL);
			}
        } else {
            check(0 == "Construction's cell is out of map!");
        }
    }    
	delete pCnst;
}


iOwnCnst* iGameWorld::AddOwnCnst(iOwnCnst* pCnst, bool bUpdateFog)
{
    if (pCnst->Owner() != PID_NEUTRAL) {
        FindPlayer(pCnst->Owner())->AddCnst(pCnst, bUpdateFog);
    } else {
        m_OwnCnsts.Add(pCnst);
    }

    for (uint32 xx=0; xx<pCnst->Proto()->CoversCount(); ++xx){
        sint32 px = pCnst->Pos().x+pCnst->Proto()->CoverEntry(xx).ox;
        sint32 py = pCnst->Pos().y+pCnst->Proto()->CoverEntry(xx).oy;
        if (m_CoverMap.IsValidPos(px, py)) {
            m_CoverMap.GetAt(px,py) = pCnst;
        } else {
            //check(0 == "Construction's cell is out of map!");
        }
    }
    return pCnst;
}

void iGameWorld::RemoveOwnCnst(iOwnCnst* pCnst)
{
	if (pCnst->Owner() != PID_NEUTRAL) {
		iPlayer* oldPlayer = FindPlayer(pCnst->Owner());		
		oldPlayer->m_Cnsts.Remove(oldPlayer->m_Cnsts.Find(pCnst));		
		oldPlayer->UpdateFogMap();
	} else {
		m_OwnCnsts.Remove(iOCIt(pCnst));
	}

	for (uint32 xx=0; xx<pCnst->Proto()->CoversCount(); ++xx){
        sint32 px = pCnst->Pos().x+pCnst->Proto()->CoverEntry(xx).ox;
        sint32 py = pCnst->Pos().y+pCnst->Proto()->CoverEntry(xx).oy;
        if (m_CoverMap.IsValidPos(px, py)) {
			iBaseMapObject *bmo = m_CoverMap.GetAt(px,py);
			if(DynamicCast<iOwnCnst*>(bmo))
				m_CoverMap.GetAt(px,py) = NULL;
			else if(iHero* pHero = DynamicCast<iHero*>(bmo)) {
				pHero->SetLocation(NULL);
			}
        } else {
            //check(0 == "Construction's cell is out of map!");
        }
    }
    pCnst;
}


iCastle* iGameWorld::AddCastle(iCastle* pCastle, bool bUpdateFog)
{
    if (pCastle->Owner() != PID_NEUTRAL) {
		bool bGameResult;
        FindPlayer(pCastle->Owner())->AddCastle(pCastle, bGameResult, bUpdateFog);
    } else {
        m_Castles.Add(pCastle);
    }

    for (uint32 xx=0; xx<pCastle->Proto()->CoversCount(); ++xx){
        sint32 ox = pCastle->Pos().x + pCastle->Proto()->CoverEntry(xx).ox;
        sint32 oy = pCastle->Pos().y + pCastle->Proto()->CoverEntry(xx).oy;
        if (m_CoverMap.IsValidPos(ox,oy)) {
            m_CoverMap.GetAt(ox,oy) = pCastle;
        } else {
            check(0 == "Castle's cell is out of map!");
        }
    }

    return pCastle;
}

iMapItem* iGameWorld::AddMapItem(iMapItem* pMapItem)
{
    m_MapItems.Add(pMapItem);
    m_CoverMap.GetAt(pMapItem->Pos().x,pMapItem->Pos().y) = pMapItem;
    return pMapItem;
}

void iGameWorld::RemoveMapItem(iMapItem* pMapItem)
{
    m_CoverMap.GetAt(pMapItem->Pos().x,pMapItem->Pos().y) = NULL;
    m_MapItems.Remove(iMIt(pMapItem));
    delete pMapItem;
}

iMapEvent* iGameWorld::AddMapEvent(iMapEvent* pMapEvent)
{
    m_MapEvents.Add(pMapEvent);
    return pMapEvent;
}

void iGameWorld::RemoveMapEvent(iMapEvent* pMapEvent)
{
#ifdef MULTIPLAYER
    gMPMgr.OnMapEventRemove(pMapEvent);
#endif
    m_MapEvents.Remove(iEIt(pMapEvent));
    delete pMapEvent;
}

iMapEvent* iGameWorld::FindMapEvent(const iPoint& cell, PLAYER_ID pid)
{   
    for (iMapEventList::Iterator meit = m_MapEvents.First(); meit != m_MapEvents.End(); ++meit) {
        if ((*meit)->Pos() == cell) {
            if (pid == PID_NEUTRAL ||  ((*meit)->PlayerMask() & (1<<pid)) != 0 ) return *meit;
            else return NULL;
        }
    }
    return NULL;
}

iMapMarker* iGameWorld::AddMapMarker(iMapMarker* pMapMarker)
{
    m_MapMarkers.Add(pMapMarker);
    return pMapMarker;
}

iBaseMapObject* iGameWorld::FindTeleportDestination(iVisCnst_NewTeleport* self, uint8 idx)
{
    if(idx == 0) return NULL; // this teleport leads to nowhere
    iSimpleArray<iBaseMapObject*> list;
    for (iMapMarkerList::Iterator Mrkit = m_MapMarkers.First(); Mrkit != m_MapMarkers.End(); ++Mrkit) {
        if ((*Mrkit)->InIndex() == idx)
        {
            list.Add(*Mrkit);
        }
    }   

	for (iVCIt vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) {
        
        if ((*vcit)->Proto()->Type() == VCNST_NEWTELEPORT && 
            ((iVisCnst_NewTeleport*)(*vcit))->InIndex() == idx && 
            (*vcit) != self) {
            list.Add(*vcit);
        }
    }
    
    if(list.GetSize() == 0) return NULL;
    else if(list.GetSize() == 1) return list[0];
    else return list[gGame.Map().Rand(list.GetSize())]; 
}

iSimpleArray<iBaseMapObject*> iGameWorld::FindTeleportDestinations(iVisCnst_NewTeleport* self, uint8 idx)
{
    iSimpleArray<iBaseMapObject*> list;
    for (iMapMarkerList::Iterator Mrkit = m_MapMarkers.First(); Mrkit != m_MapMarkers.End(); ++Mrkit) {
        if ((*Mrkit)->InIndex() == idx)
        {
            list.Add(*Mrkit);
        }
    }   

	for (iVCIt vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) {
        if ((*vcit)->Proto()->Type() == VCNST_NEWTELEPORT && 
            ((iVisCnst_NewTeleport*)(*vcit))->InIndex() == idx && 
            (*vcit) != self) {
            list.Add(*vcit);
        }
    }
    return list;
}

iSimpleArray<iBaseMapObject*> iGameWorld::FindTeleportOrigins(iVisCnst_NewTeleport* self, uint8 idx)
{
    iSimpleArray<iBaseMapObject*> list;
    
	for (iVCIt vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) {
        if ((*vcit)->Proto()->Type() == VCNST_NEWTELEPORT && 
            ((iVisCnst_NewTeleport*)(*vcit))->OutIndex() == idx && 
            (*vcit) != self) {
            list.Add(*vcit);
        }
    }
    
    return list;
}

iVisCnst_NewTeleport* iGameWorld::FindNewTeleport(sint32 x, sint32 y)
{
	for (iVCIt vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) 
        if ((*vcit)->Proto()->Type() == VCNST_NEWTELEPORT && 
            (*vcit)->Pos().x == x && (*vcit)->Pos().y == y)
            return (iVisCnst_NewTeleport*)(*vcit);      
    return NULL;
}


iMapGuard* iGameWorld::AddMapGuard(iMapGuard* pMapGuard)
{
    m_MapGuards.Add(pMapGuard);
    m_CoverMap.GetAt(pMapGuard->Pos().x,pMapGuard->Pos().y) = pMapGuard;
    return pMapGuard;
}

void iGameWorld::RemoveMapGuard(iMapGuard* pMapGuard)
{
    m_CoverMap.GetAt(pMapGuard->Pos().x,pMapGuard->Pos().y) = NULL;
    m_MapGuards.Remove(iGIt(pMapGuard));
    delete pMapGuard;
}

void iGameWorld::CaptureCastle(iHero* pHero, iCastle* pCastle)
{
}

bool iGameWorld::CanDig(const iPoint& cell) const
{
    iCell c = GetAt(cell.x,cell.y);
    // Not passable cell
    if (c.SurfNode(0) == STYPE_WATER || c.SurfNode(1) == STYPE_WATER || c.SurfNode(2) == STYPE_WATER || c.SurfNode(3) == STYPE_WATER || m_PassMap.GetAt(cell.x, cell.y) <= 0 || GetAt(cell.x,cell.y).avatar != 0xFFFF) return false;
    iBaseMapObject* pObj = m_CoverMap.GetAt(cell.x,cell.y);
    // No object
    if (!pObj) return true;
    if (pObj->Disap()) {
        if (iHero* pHero = DynamicCast<iHero*>(pObj)) return pHero->GetLocation() == NULL;
        return true;
    }

    return  false;

    /*
    iCell c = GetAt(cell.x,cell.y);
    if (c.SurfNode(0) == STYPE_WATER || c.SurfNode(1) == STYPE_WATER || c.SurfNode(2) == STYPE_WATER || c.SurfNode(3) == STYPE_WATER) return false;
    return m_PassMap.GetAt(cell.x, cell.y) > 0 && (!m_CoverMap.GetAt(cell.x,cell.y) || (DynamicCast<iHero*>(m_CoverMap.GetAt(cell.x,cell.y)) && !DynamicCast<iHero*>(m_CoverMap.GetAt(cell.x,cell.y))->GetLocation())) && GetAt(cell.x,cell.y).avatar == 0xFFFF;
    */
}

const SpriteId hole_sprites[STYPE_COUNT] = {
    0xFFFF,         // water
    PDGG(HOLES) + 0, // sand (beach)
    PDGG(HOLES) + 1, // dirt
    PDGG(HOLES) + 1, // grass
    PDGG(HOLES) + 1, // swamp
    PDGG(HOLES) + 2, // lava
    PDGG(HOLES) + 3, // wasteland
    PDGG(HOLES) + 4, // desert
    PDGG(HOLES) + 1, // snow
    PDGG(HOLES) + 4, // ndesert
    PDGG(HOLES) + 1, // pavement
    PDGG(HOLES) + 3  // nwasteland
};

bool iGameWorld::Dig(const iPoint& cell)
{
#ifdef MULTIPLAYER
    gMPMgr.OnDig(cell);
#endif
    check(CanDig(cell));
    iCell c = GetAt(cell.x,cell.y);
    sint32 upSurf = iMAX(iMAX(iMAX(c.SurfNode(0),c.SurfNode(1)),c.SurfNode(2)),c.SurfNode(3));
    check(upSurf != STYPE_WATER);
    uint16 aid = gGame.ItemMgr().m_AvatarMgr.FindAvatarBySprite(hole_sprites[upSurf]);
    check(aid != 0xFFFF);
    GetAt(cell.x,cell.y).avatar = aid;
    if (m_posUltimate == cell) {
        m_statUltimate = true;
        return true;
    }
    return false;
}

bool iGameWorld::InitUltimateArt()
{
    if (m_statUltimate) return 0;
    if (m_radUltimate != 0 || m_posUltimate == iPoint(-1,-1)) {
        if (m_radUltimate == 0) m_radUltimate = 255;
        iRect rcMap(8,8, m_Width-15, m_Height-15);
        if (m_posUltimate != iPoint(-1,-1)) {
            iRect rcUlt(m_posUltimate.x - m_radUltimate, m_posUltimate.y - m_radUltimate, m_radUltimate*2+1, m_radUltimate*2+1);
            iClipper::IntersectRect(rcMap, rcMap, rcUlt);
        }
        do {
            m_posUltimate.x = rcMap.x + m_rand.Rand(rcMap.w);
            m_posUltimate.y = rcMap.y + m_rand.Rand(rcMap.h);
        } while (!CanDig(m_posUltimate));
    }
    check(IsValidPos(m_posUltimate.x, m_posUltimate.y) && (m_statUltimate  || CanDig(m_posUltimate)));
    return true;
}

//////////////////////////////////////////////////////////////////////////
bool iGameWorld::InitMap(sint32 w, sint32 h)
{
    m_PassMap.Init(w,h);
    m_CoverMap.Init(w,h);
    m_CoverMap.FillMem(0);
    return Init(w,h);
}

void iGameWorld::CleanupGameWorld()
{
    Cleanup();
    m_PassMap.Cleanup();
    m_CoverMap.Cleanup();
    m_VisCnsts.DeleteAll();
    m_OwnCnsts.DeleteAll();
    m_Castles.DeleteAll();
    m_Ships.DeleteAll();
    m_MapItems.DeleteAll();
    m_MapGuards.DeleteAll();
    m_MapEvents.DeleteAll();
    m_MapMarkers.DeleteAll();
    m_Players.DeleteAll();
    m_teleports.RemoveAll();
		
	if(m_nextWeek) delete m_nextWeek;
	m_nextWeek = NULL;
	
    m_pCurPlayer = m_pActPlayer = NULL;
}

void iGameWorld::Process(fix32 t)
{
#ifdef MULTIPLAYER
    if(!gMPMgr.IsReplaying())
#endif
        if (CurPlayer()) CurPlayer()->Process(t);

    //
    if (m_pVanItem) {
        if (m_pVanItem->Vanished()) {
            delete m_pVanItem->DettachContent();
            delete m_pVanItem;
            m_pVanItem = NULL;
        } else {
            m_pVanItem->Process(t);
        }
    }
#ifdef ROYAL_BOUNTY
	// auto end turn
	/*if(m_pCurPlayer == m_pActPlayer && m_pCurPlayer->CurHero() && m_pCurPlayer->CurHero()->Exhausted()) {
		gGame.Map().NextPlayer();
	}*/
#endif
	
}

const iStringT iGameWorld::MapName() const
{
    if((1 << gSettings.GetLanguage()) & m_supportedLang) return m_Name[gSettings.GetLanguage()];
    else return m_Name[GLNG_ENGLISH];
}

const iStringT iGameWorld::MapEnglishName() const
{
    return m_Name[GLNG_ENGLISH];
}


bool iGameWorld::LoadFromFile(const iMapInfo& mapInfo, iFileI* pFile, bool bUpdateVisitors, bool bAfterLoad)
{
    // Reinitz random generator
    uint32 seed = mapInfo.m_rseed;
    if (!seed) seed = GetTickCount();
    m_rand.SetNewSeed(seed);

	CleanupGameWorld();
	
    // Reinit Item manager
    gGame.ItemMgr().OnGameStart();

    // Read data to buffer
    uint32 fileSize = pFile->GetSize();
    iDynamicBuffer dbuff( fileSize );
//  tmpbuff.SetSize( fileSize );
//  pFile->Read(tmpbuff.GetData(),fileSize );
    dbuff.SetSize( pFile->Read(dbuff.GetData(),fileSize ) );


    // XXCTODO
//    // Decrypt
//    uint32 cipherKey = 0;
//    switch ( mapInfo.m_encType ) {
//#if defined(OS_APPLE) || (defined (OS_WINCE) || (defined (OS_WIN32) && defined (DEV_VER)))
//        case MET_MAPDES:    cipherKey = PBKEY_MDMAP;
//            break;
//        case MET_PUBLIC:    cipherKey = PBKEY_PBMAP;
//            break;
//#ifndef IPHONE_LITE
//        case MET_PRIVATE:   
//            uint32 keys[4];
//            xxc::scramble_magic( PBKEY_COMMON, PRIVATE_KEY, keys );
//            cipherKey = keys[0];
//            break;
//#endif
//#else if defined(OS_WIN32)
//        // PC version
//        case MET_PUBLIC:    cipherKey = PBKEY_PBMAP;
//            break;
//        case MET_MAPDES:    cipherKey = PBKEY_MDMAP;
//            break;
//		case MET_PRIVATE:   
//			uint32 keys[4];
//			xxc::scramble_magic( PBKEY_COMMON, PRIVATE_KEY, keys );
//			cipherKey = keys[0];
//			break;
//#endif
//        default: 
//#ifdef HMM_COMPOVERSION
//            cipherKey = PBKEY_MDMAP;
//#else
//
//            return false;
//#endif
//    }
//
//    xxc::cipher_block(PBKEY_COMMON, cipherKey, (uint8*)tmpbuff.GetData(), tmpbuff.GetSize());

//    // Decompress
//    iDynamicBuffer dbuff;
//    LZO::Decompress((const unsigned char*)tmpbuff.GetData(), tmpbuff.GetSize(), dbuff);
//    tmpbuff.Clean();
    
    // Map size
    m_mapSiz = mapInfo.m_Size;

    // Game mode
    m_gameMode = mapInfo.m_gameMode;

    // Difficulty
    m_Difficulty = mapInfo.m_Difficulty;

    // Languages
    m_supportedLang = mapInfo.m_supportedLang;

    // Map id (english name)
    m_Id = mapInfo.m_Id;
    
    // Name and description
    // Map name and description (string of text)
    for(uint8 xx=0; xx<GLNG_COUNT; xx++)
        if(m_supportedLang & (1 << xx)) {
            m_Name[xx] = mapInfo.m_Name[xx];
            m_Desc[xx] = mapInfo.m_Description[xx];
        }
    
    
    // Version and Author
    m_Version = mapInfo.m_Version;
    m_Author = mapInfo.m_Author;

    // Current date
    m_CurDay = mapInfo.m_curDay;

    // Current week descriptor (always initialize as Normal)
    m_weekDesc.Init();

    // Text resources (for all languages (Key, lang_1, lang_2, lang_n)
    uint32 tcnt;
    dbuff.Read(tcnt);
    iStringT key;
    sint16 tet;
    // clear hashes
    m_ctMgr.Reset();
    while (tcnt--){
        dbuff.Read(key);
        dbuff.Read(tet);
        iTextMgr::iTextEntry& te = m_ctMgr.AddTextEntry(key, (TextEntryType)tet);
        for ( uint32 xx=0; xx<GLNG_COUNT; ++xx ) {
            if ( m_supportedLang & (1<<xx) ) {
                dbuff.Read(te.text[xx]);
            }
        }
    }
    
    // Cotulla: moved to iMapInfo
    // global properties
    //dbuff.Read(m_globalProperties);
    m_globalProperties = mapInfo.m_globalProperties;

	// level cap
	dbuff.Read(m_levelCap);

	// victory condition
	uint8 type;
	dbuff.Read(type);
	m_victoryCondition.type = (VICTORY_CONDITION_TYPE)type;
	dbuff.Read(m_victoryCondition.fparam);
	dbuff.Read(m_victoryCondition.sparam);
	dbuff.Read(m_victoryCondition.time);

	// lose condition
	dbuff.Read(type);
	m_loseCondition.type = (LOSE_CONDITION_TYPE)type;
	dbuff.Read(m_loseCondition.fparam);			
	dbuff.Read(m_loseCondition.sparam);	
	
    // load next week schedule
    dbuff.Read(type);
    if(type == 1) {
        if(m_nextWeek) delete m_nextWeek;
        m_nextWeek = new iWeekDesc();
        uint8 tmp;
        dbuff.Read(tmp);
        m_nextWeek->wkType = (iWeekDesc::Type)tmp;
        dbuff.Read(tmp);
        m_nextWeek->crType = (CREATURE_TYPE)tmp;
    }

    // Time Events
    m_timeEventMgr.DeleteAll();
    uint16 cnt; 
    dbuff.Read(cnt);
    iTimeEvent te;
    for (uint16 xx=0; xx<cnt; ++xx) {
        ::Unserialize(dbuff, te.m_name);
        ::Unserialize(dbuff, te.m_text);        
        dbuff.Read(te.m_playerMask);
        dbuff.Read(te.m_fTime);
        sint32 repTime;
        dbuff.Read(repTime);
        te.m_repTime = (iTimeEvent::EventFreq)repTime;
        ::Unserialize(dbuff, te.m_minerals);
        uint32 flags;
        dbuff.Read(flags);
        te.m_flags = flags;
        uint8 wa;
        dbuff.Read(wa);
        te.m_weekAffect = (iTimeEvent::WeekAffect)wa;
        uint8 ct;
        dbuff.Read(ct);
        te.m_crType = (CREATURE_TYPE)ct;
        m_timeEventMgr.AddEvent(te);
    }

    // Surface map dump
    InitMap(mapInfo.m_metrics.w, mapInfo.m_metrics.h);
    
    // Surface and avatars
    dbuff.Read(m_Buff,mapInfo.m_metrics.w*mapInfo.m_metrics.h*sizeof(iCell));

    // Players
    for (uint32 xx=0; xx<mapInfo.m_Players.GetSize(); ++xx){
        NATION_TYPE nation;
        if (mapInfo.m_Players[xx].m_Nation==CTLT_RANDOM) {
            nation = (NATION_TYPE)(NATION_HIGHMEN + Rand(NATION_COUNT-NATION_HIGHMEN));
        } else {
            nation = (NATION_TYPE)(mapInfo.m_Players[xx].m_Nation+NATION_HIGHMEN);
        }

        iPlayer* pPlayer = AddPlayer(mapInfo.m_Players[xx].m_TypeMask, mapInfo.m_Players[xx].m_Id, nation, mapInfo.m_Players[xx].m_Type);
        pPlayer->SetKeys(mapInfo.m_Players[xx].m_keys);     
        if(mapInfo.m_Players[xx].m_curVisitorId != iHeroesMgr::INVALID_HERO_ID)
            pPlayer->UpdateTavernVisitor(gGame.ItemMgr().HeroesMgr().Get(mapInfo.m_Players[xx].m_curVisitorId));

        if (mapInfo.m_bNewGame) {
#ifdef MULTIPLAYER
            if(!gMPMgr.IsMultiplayer())
#endif
                pPlayer->SetMinerals(INITIAL_RESOURCES[(pPlayer->PlayerType() == PT_HUMAN)?0:1][mapInfo.m_Difficulty]);
#ifdef MULTIPLAYER
            else
                pPlayer->SetMinerals(INITIAL_RESOURCES[0][mapInfo.m_Difficulty]);
#endif
        } else {
            pPlayer->SetMinerals(mapInfo.m_Players[xx].m_Minerals);
        }
    }

    // Fog maps and puzzle cards
    for (uint32 xx=0; xx<mapInfo.m_Players.GetSize(); ++xx) {
        iPlayer* pPlayer = *(m_Players.First() + xx);
        // Puzzles
        dbuff.Read(pPlayer->m_openedObelisks);
        if (pPlayer->m_openedObelisks != 0xFFFF) {
            dbuff.Read(pPlayer->m_puzzleCards, 45);
        } else {
            // init default puzzle cards
            pPlayer->m_openedObelisks = 0;
            iSimpleArray<uint8> cards(45);
            for (uint32 cid=0; cid<45; ++cid) cards.Add(cid);
            cards.RemoveAt(22);
            pPlayer->m_puzzleCards[44] = 22;
            for (uint8 pp=0; pp < 44; ++pp) {
                uint8 rval = (uint8)m_rand.Rand(cards.GetSize());
                pPlayer->m_puzzleCards[pp] = cards[rval];
                cards.RemoveAt(rval);
            }
        }

        // Fog map
        uint32 fms; dbuff.Read(fms);
        if (fms) {
            dbuff.Read(pPlayer->FogMap().GetFogMapPtr(), fms);
        }
    }

    // Visitables
    uint16 vcCount; dbuff.Read(vcCount);
    for (uint32 vcId=0; vcId<vcCount; ++vcId){
        // Proto Id
        uint16 proto;   dbuff.Read(proto);
        // Position
        iPoint pos;     Unserialize(dbuff, pos);
        // Create object
        iVisCnst* pVisCnst = gGame.ItemMgr().m_VisProts[proto]->CreateObject(pos);
        // Unserialize
        pVisCnst->Unserialize(dbuff);
        // Add object
        AddVisCnst(pVisCnst);
    }

    // Ownerables
    uint16 ocCount; dbuff.Read(ocCount);
    for (uint32 ocId=0; ocId<ocCount; ++ocId){
        // Proto Id
        uint16 proto;   dbuff.Read(proto);
        // Owner Id
        sint8 owner;    dbuff.Read(owner);
        // Position
        iPoint pos;     Unserialize(dbuff, pos);
        // Create object
        iOwnCnst* pOwnCnst = gGame.ItemMgr().m_OwnProts[proto]->CreateObject(pos);
        pOwnCnst->SetOwner((PLAYER_ID)owner);        
		
		pOwnCnst->Unserialize(dbuff);
        // Add object
        AddOwnCnst(pOwnCnst, false);
    }

    // Castles
    uint16 cCount; dbuff.Read(cCount);
    iSimpleArray<iCastle*> ctlList;
    for (uint32 cId=0; cId<cCount; ++cId){
        // Proto Id
        uint16 proto;   dbuff.Read(proto);
        // Custom name
        iStringT cname;         
        Unserialize(dbuff, cname);

        // Owner
        sint8 owner;    dbuff.Read(owner);
        // Position
        iPoint pos;     Unserialize(dbuff, pos);
        // Flags
        uint8 flags; dbuff.Read(flags); 
        // if castle type is random (only for new game)
        if (gGame.ItemMgr().IsRandomCtl(proto)) {
            // main castle
            if (flags & 0x01) {
                check(owner != PID_NEUTRAL);
                iPlayer* pPlayer = FindPlayer((PLAYER_ID)owner);
                check(pPlayer);
                // change proto according to owner's nation (same configuration, but different nation)
                proto = gGame.ItemMgr().AlterCtlNation(proto, (CTL_TYPE)(pPlayer->Nation()-NATION_HIGHMEN));
            } else {
                // change proto to random nation (same configuration, but different nation)
                proto = gGame.ItemMgr().AlterCtlNation(proto, (CTL_TYPE)Rand(CTLT_COUNT-1));
            }
        }
        
        // Create object
        iCastle* pCastle = new iCastle(gGame.ItemMgr().m_CastleProts[proto],pos, cname, (flags & 0x2) != 0);
        // Garrison
        pCastle->UnserializeArmy(dbuff);
        // Can build
        uint8 canBuild; dbuff.Read(canBuild); pCastle->SetBuilt(canBuild>0);
        // Constructions
        pCastle->m_Cnsts.Unserialize(dbuff);
        if (owner != PID_NEUTRAL){
            // Set owner
            pCastle->SetOwner((PLAYER_ID)owner);
            // Update constructions
            pCastle->UpdateCnsts();
        } 
        // Init and add an object
        ctlList.Add(pCastle);
        AddCastle(pCastle, false);
    }

    // Obelisks count
    dbuff.Read(m_obelisksCount);

    // Ultimate artifact position and radius
    ::Unserialize(dbuff, m_posUltimate);
    dbuff.Read(m_radUltimate);
    dbuff.Read(m_statUltimate);

    // Map items
    uint16 mCount; dbuff.Read(mCount);
    while(mCount--) {
        // Map item type
        uint8 type;     dbuff.Read(type);
        // Position
        iPoint pos;     Unserialize(dbuff, pos);
        // Guard
        iArmy guard;    Unserialize(dbuff, guard);
        // Message
        iStringT msg;   
        Unserialize(dbuff, msg);
        
        // Read item speciffic data and create an object
        iMapItem* pItem = NULL;
        if (type == MAPITEM_MINERAL) {
            sint8 mtype;
            sint32 mcount;
            dbuff.Read(mtype);
            dbuff.Read(mcount);
            pItem = new iMapItem_Mineral(pos,(MINERAL_TYPE)mtype,mcount);
        } else if (type == MAPITEM_MANACRYSTAL) {
            sint32 mcount;
            dbuff.Read(mcount);
            pItem = new iMapItem_ManaCrystal(pos,mcount);
        } else if (type == MAPITEM_CAMPFIRE) {
            sint32 goldCount, addMinCount;
            sint8 addMinType;
            dbuff.Read(goldCount);
            dbuff.Read(addMinType);
            dbuff.Read(addMinCount);
            pItem = new iMapItem_CampFire(pos, goldCount, (MINERAL_TYPE)addMinType, addMinCount);
        } else if (type == MAPITEM_CHEST) {
            sint32 count;
            dbuff.Read(count);
            pItem = new iMapItem_Chest(pos, count);
        } else if (type == MAPITEM_LAMP) {
            sint32 count;
            dbuff.Read(count);
            pItem = new iMapItem_Lamp(pos, count);
        } else if (type == MAPITEM_ARTIFACT) {
            uint16 aidx;
            dbuff.Read(aidx);
            if (aidx >= ARTT_RAND) {
                // random artifact
                ART_LEVEL_TYPE alt = (ART_LEVEL_TYPE)(aidx-ARTT_RAND);
                aidx = gGame.ItemMgr().m_artMgr.SelectRandomArtifact(alt);
            }
            pItem = new iMapItem_Artifact(pos,aidx);
        } else if (type == MAPITEM_SPELLSCROLL) {
            uint16 spellIdx;
            dbuff.Read(spellIdx);
            pItem = new iMapItem_SpellScroll(pos,spellIdx);
        } else if (type == MAPITEM_KEYGUARD) {
            uint8 key; dbuff.Read(key);
            pItem = new iMapItem_KeyGuard(pos,key);
        } else {
            check(0);
        }

        // Init guard
        pItem->SetArmy(guard);
        pItem->InitArmy();
        // Init message
        pItem->Message() = msg;
        // Add object
        AddMapItem(pItem);
    }

    // Map markers
    uint16 mrkCount; dbuff.Read(mrkCount);
    while(mrkCount--) {
        iPoint pos;     Unserialize(dbuff, pos);
        iMapMarker* pMarker = new iMapMarker(pos);
        uint8 tmp;
        dbuff.Read(tmp);
        pMarker->InIndex() = tmp;
        AddMapMarker(pMarker);
    }

    // Map events
    uint16 meCount; dbuff.Read(meCount);
    while(meCount--) {
        iPoint pos;     Unserialize(dbuff, pos);
        iMapEvent* pEvent = new iMapEvent(pos);
        pEvent->UnserializeArmy(dbuff);
        pEvent->InitArmy();
        Unserialize(dbuff, pEvent->RewardMessage());
        Unserialize(dbuff, pEvent->AttackMessage());        
        Unserialize(dbuff, pEvent->RFailMessage());     
        dbuff.Read(pEvent->PlayerMask());
        Unserialize(dbuff, pEvent->Rewards());
        Unserialize(dbuff, pEvent->Reqs());
        dbuff.Read(pEvent->Flags());        
        AddMapEvent(pEvent);
    }

    // Map guards
    uint16 gCount; dbuff.Read(gCount);
    while(gCount--) {
        // Guard group
        iCreatGroup cg; Unserialize(dbuff, cg);
        // Join value
        sint8 rjoinVal; dbuff.Read(rjoinVal);
        sint32 joinVal = rjoinVal;
        if (joinVal == RAND_VAL)  joinVal = gGame.Map().Rand(100);
        // Disposition
        uint8 disp;     dbuff.Read(disp);
        // Grow flag
        uint8 bNotGrow; dbuff.Read(bNotGrow);
        // Position
        iPoint pos;     Unserialize(dbuff, pos);
		// Disabled flag
		uint8 bDisabled; dbuff.Read(bDisabled);
        // Create an object
        iMapGuard* pGuard = new iMapGuard(pos, cg.Type(), cg.Count(), joinVal, (GUARD_DISPOS)disp,bNotGrow>0, bDisabled>0);        
		
		pGuard->Unserialize(dbuff);
								
        // Add object
        AddMapGuard(pGuard);
    }

    // Heroes
    uint16 hCount; dbuff.Read(hCount);
    for (uint32 xx=0; xx<hCount; ++xx){
        // Proto Id
        uint16 proto; dbuff.Read(proto);
        // Get object from hero manager
        iHero* pHero = gGame.ItemMgr().HeroesMgr().Get(proto);
        check(pHero);
        // Unserialize
        pHero->Unserialize(dbuff, true, bAfterLoad, true);
        // Setup position
        SetHeroPos(pHero, pHero->Pos());
    }

    // Hero manager
    uint16 hmCount; dbuff.Read(hmCount);
    for (uint32 xx=0; xx<hmCount; ++xx){
        // Proto Id
        uint16 proto; dbuff.Read(proto);
        // Get object from hero manager
        iHero* pHero = gGame.ItemMgr().HeroesMgr().Get(proto);
        check(pHero);
        // Unserialize
        pHero->Unserialize(dbuff, false, bAfterLoad, true);
        // Put hero back to hero manager
        gGame.ItemMgr().HeroesMgr().Put(pHero);
    }

    // Ships (neutral)
    uint16 sCount; dbuff.Read(sCount);
    for (uint32 xx = 0; xx < sCount; ++xx)
    {        
        iShip* pShip = new iShip();
        m_Ships.Add(pShip);

        // Unserialize
        pShip->Unserialize(dbuff, true, bAfterLoad, true, NULL);
        // Setup position
        SetShipPos(pShip, pShip->Pos());
    }

    // Init castles
    for (uint32 xx=0; xx<ctlList.GetSize(); ++xx) ctlList[xx]->Init();

    // Setup Current hero for all human players and update tavern visitors
    uint32 pidx = 0;
    for (iPLIt plit = m_Players.First(); plit != m_Players.End(); ++plit, ++pidx) {
        iPlayer* pPlayer = *plit;
        if(bUpdateVisitors)
          pPlayer->UpdateTavernVisitor();

        if (pPlayer->PlayerType() == PT_HUMAN && mapInfo.m_Players[pidx].m_curHeroId != 0xFFFF) {
            pPlayer->PrvSetCurHero(mapInfo.m_Players[pidx].m_curHeroId);
        }
    }

    // Init teleports
    for (iVCIt vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) {
        if ((*vcit)->Proto()->Type() == VCNST_TELEPORT) {
            iVisCnst_Teleport* pTeleport = (iVisCnst_Teleport*)(*vcit);
            iPoint pos = pTeleport->Pos();
            uint8 tidx = pTeleport->Index();
            // Try to find the pair
            for (uint32 nn=0; nn<m_teleports.GetSize(); ++nn) {
                uint8 ntidx = m_teleports[nn]->Index();
                if (ntidx == tidx) {
                    m_teleports[nn]->SetTarget(m_teleports.GetSize());
                    pTeleport->SetTarget(nn);
                    break;
                }
            }
            // Add teleport into list
            m_teleports.Add(pTeleport);
        }
    }
    return true;
}

void iGameWorld::UpdateTavernVisitors() {
    for (iPLIt plit = m_Players.First(); plit != m_Players.End(); ++plit) {
        iPlayer* pPlayer = *plit;   
         pPlayer->UpdateTavernVisitor();
    }
}

bool iGameWorld::SaveToFile(iFileI* pFile, bool MP)
{
    iDynamicBuffer dbuff;
    // Fourcc
    dbuff.Write(GMAP_FILE_HDR_KEY);
    // Map file version
    dbuff.Write(GMAP_FILE_VERSION);

    // Encoding type 
    uint16 enctype;
    if(MP)
        enctype = MET_PUBLIC;
    else
        enctype =
#ifdef OS_WIN32 
        MET_MAPDES;
#else   
        MET_PRIVATE;
#endif

    dbuff.Write(enctype);
    // Save timestamp
	dbuff.Write(GetCurrentTimestamp());

    // Random seed
    dbuff.Write(m_rand.GetSeed());
    // Map size
    dbuff.Write((uint8)m_mapSiz);
    // Map's Supported Languages
    dbuff.Write((uint32)m_supportedLang);   
    // Map id (english name)
    Serialize(dbuff, m_Id);
    
    // Map name and description (string of text)
    for(uint8 xx=0; xx<GLNG_COUNT; xx++)
        if(m_supportedLang & (1 << xx)) {
            ::Serialize(dbuff, m_Name[xx]);
            ::Serialize(dbuff, m_Desc[xx]);
        }
    
    // Map version and author (string of text)
    Serialize(dbuff, m_Version);
    Serialize(dbuff, m_Author);
    // Current date
    dbuff.Write(m_CurDay);
    // Game mode 
    dbuff.Write((uint16)m_gameMode);
#if 0
    if(xxc::MDID_Detected() && Rand(15) > 7)
        dbuff.Write((uint8)1);
#endif

    // Difficulty
    dbuff.Write((sint8)m_Difficulty);
    // Player list
    dbuff.Write((uint16)m_Players.Count());
    iPlayer* players[PID_COUNT] = {NULL};
    for (iPlayerList::Iterator pit = m_Players.First(); pit != m_Players.End(); ++pit) players[(*pit)->PlayerId()] = *pit;
    for (uint32 pid = 0; pid<PID_COUNT; ++pid) {
        const iPlayer* pPlayer = players[pid];
        if (!pPlayer) continue;
        // Player Id
        dbuff.Write((sint8)pPlayer->PlayerId());
        //Nation type
        dbuff.Write((uint8)(pPlayer->Nation()-NATION_HIGHMEN));
        // Player Type Mask
        dbuff.Write((uint8)pPlayer->PlayerTypeMask());
        // Player Type
        dbuff.Write((uint8)pPlayer->PlayerType());
        // Player resources
        Serialize(dbuff, pPlayer->Minerals());
        // Current Hero Id
        if (pPlayer->CurHero()) {
            dbuff.Write(pPlayer->CurHero()->Proto()->m_protoId);
        } else {
            dbuff.Write((uint16)0xFFFF);
        }
        // Current Castle idx
        dbuff.Write((uint16)0xFFFF);
        // Keys
        dbuff.Write(pPlayer->GetKeys());
        // current tavern visitor
        if(pPlayer->TavernVisitor())
            dbuff.Write(pPlayer->TavernVisitor()->Proto()->m_protoId);
        else
            dbuff.Write((uint16)0xFFFF);
    }
    // Current player
    dbuff.Write((uint16)m_pCurPlayer->PlayerId());
    // Map width and height
    dbuff.Write((uint16)m_Width);
    dbuff.Write((uint16)m_Height);

    // Cotulla: GP moved here
    dbuff.Write((uint32)m_globalProperties);

    //////////////////////////////////////////////////////////////////////////
    pFile->Write(dbuff.GetData(), dbuff.GetSize());
    dbuff.Reset();
    //////////////////////////////////////////////////////////////////////////

    // Text resources (for all languages (Key, lang_1, lang_2, lang_n)
    dbuff.Write((uint32)m_ctMgr.GetHash().Count());
    for (iTextMgr::ConstTextIt cit = m_ctMgr.GetHash().First(); cit != m_ctMgr.GetHash().End(); ++cit) {
        const iTextMgr::iTextEntry& te = *cit;
        dbuff.Write(te.key);
        dbuff.Write(te.tet);
        for (uint32 xx=0; xx<GLNG_COUNT; ++xx) {
            if ( m_supportedLang & (1<<xx) ) {
                dbuff.Write(te.text[xx]);
            }
        }
    }           
    
    	// level cap
	dbuff.Write(m_levelCap);

	// victory condition
	dbuff.Write((uint8)m_victoryCondition.type);	
	dbuff.Write(m_victoryCondition.fparam);
	dbuff.Write(m_victoryCondition.sparam);
	dbuff.Write(m_victoryCondition.time);

	// lose condition	
	dbuff.Write((uint8)m_loseCondition.type);
	dbuff.Write(m_loseCondition.fparam);			
	dbuff.Write(m_loseCondition.sparam);	

    // if map has next week affect pending, save it
    if(m_nextWeek) {
        dbuff.Write((uint8)1);
        dbuff.Write((uint8)m_nextWeek->wkType);
        dbuff.Write((uint8)m_nextWeek->crType);
    } else 
        dbuff.Write((uint8)0);

    // Time events
    uint16 cecnt = m_timeEventMgr.EventsCount();
    dbuff.Write(cecnt);
    for (uint32 xx=0; xx<cecnt; ++xx){
        const iTimeEvent& evt = m_timeEventMgr.Event(xx);
        ::Serialize(dbuff, evt.m_name);
        ::Serialize(dbuff, evt.m_text);
        dbuff.Write(evt.m_playerMask);
        dbuff.Write(evt.m_fTime);
        dbuff.Write((sint32)evt.m_repTime);
        ::Serialize(dbuff, evt.m_minerals);
        dbuff.Write(evt.m_flags);       
        dbuff.Write((uint8)evt.m_weekAffect);
        dbuff.Write((uint8)evt.m_crType);
    }
    // Surface and avatars map dump
    dbuff.Write(m_Buff, m_Width*m_Height*sizeof(iCell));
    
    // Fog maps and puzzle cards
    for (iPlayerList::Iterator pit = m_Players.First(); pit != m_Players.End(); ++pit) {
        iPlayer* pPlayer = (*pit);
        // Puzzles
        dbuff.Write(pPlayer->m_openedObelisks);
        dbuff.Write(pPlayer->m_puzzleCards, 45);

        // Fog map
        uint32 siz = pPlayer->FogMap().GetFogMapDataSize();
        dbuff.Write(siz);
        dbuff.Write(pPlayer->FogMap().GetFogMapPtr(), siz);
    }

    // Visitables
    dbuff.Write((uint16)m_VisCnsts.Count());
    for (iVisCnstList::Iterator vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) {
        iVisCnst* pVisCnst = *vcit;
        // Proto Id
        dbuff.Write(pVisCnst->Proto()->ProtoId());
        // Position
        ::Serialize(dbuff, pVisCnst->Pos());
        // Serialize
        pVisCnst->Serialize(dbuff);
    }
    
    // Ownerables
    iSimpleArray<iOwnCnst*> ownCnsts;
    for (iOwnCnstList::Iterator ocit = m_OwnCnsts.First(); ocit != m_OwnCnsts.End(); ++ocit) ownCnsts.Add(*ocit);
    for (iPlayerList::Iterator pit = m_Players.First(); pit != m_Players.End(); ++pit)
        for (iPlayer::iCLIt ocit = (*pit)->m_Cnsts.First(); ocit != (*pit)->m_Cnsts.End(); ++ocit) ownCnsts.Add(*ocit);
    dbuff.Write((uint16)ownCnsts.GetSize());
    for (uint32 ocidx=0; ocidx<ownCnsts.GetSize(); ++ocidx) {
        // Proto Id
        dbuff.Write(ownCnsts[ocidx]->Proto()->ProtoId());
        // Owner Id
        dbuff.Write((sint8)ownCnsts[ocidx]->Owner());
        // Position
        ::Serialize(dbuff, ownCnsts[ocidx]->Pos());
       
		ownCnsts[ocidx]->Serialize(dbuff);
    }

    
    // Castles
    iSimpleArray<iCastle*> castleList;
    for (iCastleList::Iterator ctlit = m_Castles.First(); ctlit != m_Castles.End(); ++ctlit) castleList.Add(*ctlit);
    for (iPlayerList::Iterator pit = m_Players.First(); pit != m_Players.End(); ++pit)
        for (iPlayer::iCtLIt ctlit = (*pit)->CastleFirst(); ctlit != (*pit)->CastleEnd(); ++ctlit) castleList.Add(*ctlit);
    dbuff.Write((uint16)castleList.GetSize());
    for (uint32 ctlidx=0; ctlidx<castleList.GetSize(); ++ctlidx) {
        // Proto Id
        dbuff.Write(castleList[ctlidx]->Proto()->ProtoId());
        // Name
        if(castleList[ctlidx]->NameKey() != 0)
            ::Serialize(dbuff, iFormat(_T("%d"), castleList[ctlidx]->NameKey()));
        else
            ::Serialize(dbuff, castleList[ctlidx]->Name());
        // Owner
        dbuff.Write((sint8)castleList[ctlidx]->Owner());
        // Position
        ::Serialize(dbuff, castleList[ctlidx]->Pos());
        // Flags (always zero for saved map)
        dbuff.Write((uint8)0);
        // Garrison
        ::Serialize(dbuff, castleList[ctlidx]->Garrison());
        // Can build
        dbuff.Write((uint8)castleList[ctlidx]->CanBuild());
        // Constructions
        castleList[ctlidx]->m_Cnsts.Serialize(dbuff);
    }

    // Obelisks count
    dbuff.Write(m_obelisksCount);

    // Ultimate artifact position and radius
    ::Serialize(dbuff, m_posUltimate);
    dbuff.Write((uint8)0);
    dbuff.Write(m_statUltimate);

    // Map items
    dbuff.Write((uint16)m_MapItems.Count());
    for (iMapItemList::Iterator miit = m_MapItems.First(); miit != m_MapItems.End(); ++miit) {
        iMapItem* pMapItem = *miit;
        // Map item type
        dbuff.Write((uint8)pMapItem->Type());
        // Position
        ::Serialize(dbuff, pMapItem->Pos());
        // Guard
        ::Serialize(dbuff, pMapItem->Guard());
        // Message
        ::Serialize(dbuff, pMapItem->Message());
        // Item speciffic data
        pMapItem->Serialize(dbuff);
    }

    // Map markers
    dbuff.Write((uint16)m_MapMarkers.Count());
    for (iMapMarkerList::Iterator meit = m_MapMarkers.First(); meit != m_MapMarkers.End(); ++meit) {
        iMapMarker* pMapMarker = *meit;
        ::Serialize(dbuff, pMapMarker->Pos());
        dbuff.Write(pMapMarker->InIndex());
    }


    // Map events
    dbuff.Write((uint16)m_MapEvents.Count());
    for (iMapEventList::Iterator meit = m_MapEvents.First(); meit != m_MapEvents.End(); ++meit) {
        iMapEvent* pMapEvent = *meit;
        ::Serialize(dbuff, pMapEvent->Pos());
        ::Serialize(dbuff, pMapEvent->Guard());
        ::Serialize(dbuff, pMapEvent->RewardMessage());
        ::Serialize(dbuff, pMapEvent->AttackMessage());
        ::Serialize(dbuff, pMapEvent->RFailMessage());
        dbuff.Write(pMapEvent->PlayerMask());
        ::Serialize(dbuff, pMapEvent->Rewards());
        ::Serialize(dbuff, pMapEvent->Reqs());
        dbuff.Write(pMapEvent->Flags());
    }

    // Map guards
    dbuff.Write((uint16)m_MapGuards.Count());
    for (iMapGuardList::Iterator mgit = m_MapGuards.First(); mgit != m_MapGuards.End(); ++mgit) {
        iMapGuard* pMapGuard = *mgit;
        // Guard group
        ::Serialize(dbuff, pMapGuard->Creatures());
        // Join value
        dbuff.Write((sint8)pMapGuard->JoinVal());
        // Disposition
        dbuff.Write((uint8)pMapGuard->Disposition());
        // Grow flag
        dbuff.Write((uint8)pMapGuard->NotGrow());
		// Position
        ::Serialize(dbuff, pMapGuard->Pos());
        
		// disabled flag
		dbuff.Write((uint8)pMapGuard->Disabled());
        
		pMapGuard->Serialize(dbuff);
    }

    // Heroes
	uint16 hcnt = 0;
	for (iPlayerList::Iterator pit = m_Players.First(); pit != m_Players.End(); ++pit) 
		for (iPlayer::iHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit) 
			hcnt++;
	dbuff.Write(&hcnt,sizeof(hcnt));
	for (iPlayerList::Iterator pit = m_Players.First(); pit != m_Players.End(); ++pit) {
		for (iPlayer::iHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit) {
			(*hit)->Serialize(dbuff);
		}
	}

    // Hero manager
    dbuff.Write((uint16)gGame.ItemMgr().HeroesMgr().m_heroes.GetSize());
    for (uint32 xx=0; xx<gGame.ItemMgr().HeroesMgr().m_heroes.GetSize(); ++xx){
        // Serialize
        gGame.ItemMgr().HeroesMgr().m_heroes[xx]->Serialize(dbuff);
    }

    // Ships (neutral), owned ship save with hero
    uint16 scnt = 0;
    for (iSLIt ocit = m_Ships.First(); ocit != m_Ships.End(); ++ocit) 
    {
        if ((*ocit)->IsLoaded() == false) scnt++;    
    }
    dbuff.Write(&scnt,sizeof(scnt));
    for (iSLIt ocit = m_Ships.First(); ocit != m_Ships.End(); ++ocit) 
    {
        iShip *pShip = *ocit;
        if (!pShip->IsLoaded()) // only if ship without hero
        {
            pShip->Serialize(dbuff);
        }
    }

    
    // Save to file
    pFile->Write(dbuff.GetData(), dbuff.GetSize());

    return true;
}

void iGameWorld::InitPassMap()
{
    const iCell* pSrc = GetPtr();
    sint8* pDst  = m_PassMap.GetPtr();
    sint32 mpy,mpx;
    // Surface
    for (mpy = 0; mpy<m_Height; ++mpy) 
    {
        for (mpx = 0; mpx<m_Width; ++mpx, pSrc++, pDst++ ) 
        {
            // old one before ships
            //sint32 btSurf = iMIN(iMIN(iMIN(pSrc->SurfNode(0),pSrc->SurfNode(1)),pSrc->SurfNode(2)),pSrc->SurfNode(3));
            // ships: mark full water cells as -128 and mark not full as -127            
            // STYPE_WATER is 0, so solid water cell is 0x0000
            if (pSrc->surf == STYPE_WATER)
            {                   
                // solid water cell 
                *pDst = PASSMAP_SWATER;
            }
            else
            {
                sint32 btSurf = iMIN(iMIN(iMIN(pSrc->SurfNode(0),pSrc->SurfNode(1)),pSrc->SurfNode(2)),pSrc->SurfNode(3));
                if (btSurf == 0)
                {
                    // not solid water cell
                    *pDst = PASSMAP_PWATER;
                }
                else
                {
                    *pDst = SURF_MOVE_COST[btSurf];
                }
            }
        }
    }

    // Avatars
    pSrc = GetPtr();
    for (mpy = 0; mpy<m_Height; ++mpy) {
        for (mpx = 0; mpx<m_Width; ++mpx, pSrc++) {
            if (pSrc->avatar != 0xFFFF) {
                if ( (pSrc->avatar & 0xFF00) == 0xFF00) {
                    // path element
                    if (m_PassMap.IsValidPos(mpx,mpy) && gGame.ItemMgr().m_PathElProts[pSrc->avatar & 0xFF]->PassModVal() != 0) m_PassMap.GetAt(mpx,mpy) = gGame.ItemMgr().m_PathElProts[pSrc->avatar & 0xFF]->PassModVal();
                } else {
                    // avatar
                    const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pSrc->avatar];
                    for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
                        sint32 ox = mpx+avatar.PassModEntry(pmid).ox;
                        sint32 oy = mpy+avatar.PassModEntry(pmid).oy;
                        if (m_PassMap.IsValidPos(ox,oy)) m_PassMap.GetAt(ox,oy) = avatar.PassModEntry(pmid).mod;
                    }
                }
            }
        }
    }

    // Visitables
    for (iVisCnstList::Iterator vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) {
        iVisCnst* pCnst = *vcit;
        const iVisCnstT* pProto = pCnst->Proto();
        for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
            const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
            for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
                sint32 ox = pCnst->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
                sint32 oy = pCnst->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
                if (m_PassMap.IsValidPos(ox,oy)) m_PassMap.GetAt(ox,oy) = avatar.PassModEntry(pmid).mod;
            }
        }
    }

    // Neutral ownerables
    for (iOwnCnstList::Iterator ocit = m_OwnCnsts.First(); ocit != m_OwnCnsts.End(); ++ocit) {
        iOwnCnst* pCnst = *ocit;
        const iOwnCnstT* pProto = pCnst->Proto();
        for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
            const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
            for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
                sint32 ox = pCnst->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
                sint32 oy = pCnst->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
                if (m_PassMap.IsValidPos(ox,oy)) m_PassMap.GetAt(ox,oy) = avatar.PassModEntry(pmid).mod;
            }
        }
    }

    // Neutral castles
    for (iCastleList::Iterator ctit = m_Castles.First(); ctit != m_Castles.End(); ++ctit) {
        iCastle* pCastle = *ctit;
        const iCastleT* pProto = pCastle->Proto();
        for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
            const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
            for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
                sint32 ox = pCastle->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
                sint32 oy = pCastle->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
                if (m_PassMap.IsValidPos(ox,oy)) m_PassMap.GetAt(ox,oy) = avatar.PassModEntry(pmid).mod;
            }
        }
    }

    for (iPlayerList::Iterator pit = m_Players.First(); pit != m_Players.End(); ++pit) {
        // Ownerables
        for (iOwnCnstList::Iterator ocit = (*pit)->m_Cnsts.First(); ocit != (*pit)->m_Cnsts.End(); ++ocit) {
            iOwnCnst* pCnst = *ocit;
            const iOwnCnstT* pProto = pCnst->Proto();
            for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
                const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
                for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
                    sint32 ox = pCnst->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
                    sint32 oy = pCnst->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
                    if (m_PassMap.IsValidPos(ox,oy)) m_PassMap.GetAt(ox,oy) = avatar.PassModEntry(pmid).mod;
                }
            }
        }

        // Castles
        for (iCastleList::Iterator ctit = (*pit)->CastleFirst(); ctit != (*pit)->CastleEnd(); ++ctit) {
            iCastle* pCastle = *ctit;
            const iCastleT* pProto = pCastle->Proto();
            for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
                const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
                for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
                    sint32 ox = pCastle->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
                    sint32 oy = pCastle->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
                    if (m_PassMap.IsValidPos(ox,oy)) m_PassMap.GetAt(ox,oy) = avatar.PassModEntry(pmid).mod;
                }
            }
        }
    }
}

void iGameWorld::SetNewActor(iPlayer* pNewActor, bool bAttack)
{
    if (m_gameMode == iMapInfo::GM_SPLAYER) {
        // in case of single player, actor always initialized and cannot be changed
		check(m_pActPlayer);
		if(m_pActPlayer && m_pActPlayer->PlayerType() != PT_OBSERVER) 
		{
			check(m_pActPlayer == pNewActor);
		}
    } else if (m_gameMode == iMapInfo::GM_HOTSEAT) {
		if(!m_pActPlayer || m_pActPlayer->PlayerType() != PT_OBSERVER) 
		{
			bool bActorChanged = (m_pActPlayer != pNewActor);
			m_pActPlayer = pNewActor;
			if (bActorChanged && pNewActor) gGame.OnActorChanged(pNewActor, bAttack);
		}
    }
#ifdef MULTIPLAYER
    else if (m_gameMode == iMapInfo::GM_NETWORK) {
        //bool bActorChanged = (m_pActPlayer != pNewActor);
        //m_pActPlayer = pNewActor;
        //if (bActorChanged) gGame.OnActorChanged(pNewActor, bAttack);
    } 
#endif
    else {
        check(0);
    }
}

sint32 iGameWorld::HumanPlayers() const 
{
	sint32 res = 0;
	for (iCPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit) 
		if(pit->PlayerType() == PT_HUMAN)
			res++;
	return res;
}

iPlayer* iGameWorld::NextPlayer()
{
#ifndef ROYAL_BOUNTY	
// autosave
#ifdef MULTIPLAYER
	if(!gMPMgr.IsMultiplayer()) {
#endif
		// Autosave         
		if ((m_pCurPlayer->PlayerType() == PT_HUMAN && gSettings.GetEntryValue(CET_AUTOSAVE) > 0) || (HumanPlayers() == 0 && ++iPLIt(m_pCurPlayer) == m_Players.End())) {
			gGame.Autosave();
		}
#ifdef MULTIPLAYER
	}
#endif
#endif		
    check(m_pCurPlayer);
    bool bNewWeek = false;

    iPLIt nit = ++iPLIt(m_pCurPlayer);
    if ( nit == m_Players.End() ) {
        NewDay();
        if ( m_CurDay % 7 == 1 ) {
            bNewWeek = true;
            NewWeek();
        }
        nit = m_Players.First();
    }
    m_pCurPlayer = *nit;

    //
    if (m_pCurPlayer->PlayerType() == PT_HUMAN) {
        SetNewActor(m_pCurPlayer, false);		
    }

#ifdef MULTIPLAYER
    if(gMPMgr.IsMultiplayer() && gGame.Map().CurPlayer() == gGame.Map().ActPlayer())
            gSfxMgr.PlaySound(CSND_PICKUP01);   
#endif
    

    if(!gGame.IsExitingToMenu()) // a fix to 'message notify left when exited to menu' bug
        m_pCurPlayer->OnSelect((m_CurDay % 7 == 1 && m_CurDay > 1), false);

    gGame.OnPlayerChanged(m_pCurPlayer, m_pActPlayer == m_pCurPlayer);

#ifdef MULTIPLAYER
    gMPMgr.OnNewTurn(m_pCurPlayer->PlayerId()); 
#endif

    return m_pCurPlayer;
}

bool iGameWorld::IsFirstPlayer(iPlayer *plr)
{
    return iPLIt(plr) == m_Players.First();
}

void iGameWorld::NewDay()
{
    for (iPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit) (*pit)->NewDay();
    for (iVCIt vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) (*vcit)->NewDay();
    for (iOCIt ocit = m_OwnCnsts.First(); ocit != m_OwnCnsts.End(); ++ocit) (*ocit)->NewDay();    

    m_CurDay ++;
}

void iGameWorld::ProcessTimeEvents(iPlayer* pActor)
{
    for (uint32 xx=0; xx<m_timeEventMgr.EventsCount(); ++xx) {
        const iTimeEvent& evt = m_timeEventMgr.Event(xx);
        if (((evt.m_flags & TEF_HUMANENABLED) && pActor->PlayerType() == PT_HUMAN ||
             (evt.m_flags & TEF_AIENABLED) && pActor->PlayerType() == PT_COMPUTER) &&
            evt.IsConform(pActor->PlayerId(), m_CurDay)) {
            for (uint32 mm = 0; mm<MINERAL_COUNT; ++mm) {
                if (evt.m_minerals.quant[mm]) pActor->AddMineral((MINERAL_TYPE)mm, evt.m_minerals.quant[mm]);
            }

            if(evt.m_weekAffect != iTimeEvent::NoAffect) {
                if(m_nextWeek) delete m_nextWeek;
                m_nextWeek = new iWeekDesc();
                m_nextWeek->wkType = (iWeekDesc::Type)((uint8)evt.m_weekAffect - 1);
                m_nextWeek->crType = evt.m_crType;
            }

            if (pActor->PlayerType() == PT_HUMAN) {
                iDlg_TimeEvent tedlg(&gApp.ViewMgr(), pActor->PlayerId(), evt);
                tedlg.DoModal();
            }
        }
    }
}

void iGameWorld::NewWeek()
{
    // Prepare new week descriptor
    m_weekDesc.bNewMonth = ((m_CurDay/7)%4) == 0;
    uint32 rval = m_rand.Rand(100);

    if(m_nextWeek != NULL) {
        m_weekDesc = *m_nextWeek;
        delete m_nextWeek;
        m_nextWeek = NULL;
    } else if(m_globalProperties & GMP_NO_SPEC_WEEKS) {
        m_weekDesc.wkType = iWeekDesc::Normal;
        m_weekDesc.crType = CREAT_UNKNOWN;
    }
#ifdef MULTIPLAYER
    else if(gMPMgr.IsMultiplayer()) {
        m_weekDesc.wkType = iWeekDesc::Normal;
        m_weekDesc.crType = CREAT_UNKNOWN;
    } 
#endif
    else {
    if (m_weekDesc.bNewMonth) {
        m_weekDesc.fparam = rval % 10;
        if ( rval < 20 ) {
            // Doubles (20%)
            m_weekDesc.wkType = iWeekDesc::CreatDoubles;
            m_weekDesc.crType = (CREATURE_TYPE)(CREAT_PEASANT + m_rand.Rand(6)*6 + m_rand.Rand(3));
            GenerateGuards(m_weekDesc.crType, m_CurDay/7);
        } else if ( rval < 40) {
            // Plague (20%)
            m_weekDesc.wkType = iWeekDesc::Plague;
            m_weekDesc.crType = CREAT_UNKNOWN;
        } else {
            // Normal (60%)
            m_weekDesc.wkType = iWeekDesc::Normal;
            m_weekDesc.crType = CREAT_UNKNOWN;
        }
    } else {
        m_weekDesc.fparam = rval % 15;
        if ( rval < 40) {
            // +5 (40%)
            m_weekDesc.wkType = iWeekDesc::CreatPlus;
            m_weekDesc.crType = (CREATURE_TYPE)(CREAT_PEASANT + m_rand.Rand(6)*6 + m_rand.Rand(6));
        } else {
            // Normal (60%)
            m_weekDesc.wkType = iWeekDesc::Normal;
            m_weekDesc.crType = CREAT_UNKNOWN;
        }
    }
    }

    for (iPLIt pit = m_Players.First(); pit != m_Players.End(); ++pit) (*pit)->NewWeek(m_weekDesc);
    for (iVCIt vcit = m_VisCnsts.First(); vcit != m_VisCnsts.End(); ++vcit) (*vcit)->NewWeek(m_weekDesc);
    for (iOCIt ocit = m_OwnCnsts.First(); ocit != m_OwnCnsts.End(); ++ocit) (*ocit)->NewWeek(m_weekDesc);
    for (iGIt git = m_MapGuards.First(); git != m_MapGuards.End(); ++git) (*git)->NewWeek(m_weekDesc);
    for (iCtIt ctit = m_Castles.First(); ctit != m_Castles.End(); ++ctit) (*ctit)->NewWeek(m_weekDesc);
}

void iGameWorld::GenerateGuards(CREATURE_TYPE ct, sint32 week)
{
    // Prepare events map
    iMapT<uint8> emap(m_PassMap.GetWidth(), m_PassMap.GetHeight());
    emap.FillMem(0);
    for (iMapEventList::Iterator meit = m_MapEvents.First(); meit != m_MapEvents.End(); ++meit) {
        emap.GetAt((*meit)->Pos().x, (*meit)->Pos().y) = 1;
    }

    // Generate guards
    for (sint32 yy=0; yy<m_Height; yy +=16 ) {
        for (sint32 xx=0; xx<m_Width; xx +=16 ) {
            for (uint32 cnt=0; cnt<6; ++cnt) {
                sint32 px = xx + m_rand.Rand(16);
                sint32 py = yy + m_rand.Rand(16);
                if (m_PassMap.GetAt(px,py) >= 0 && m_CoverMap.GetAt(px, py) == NULL && emap.GetAt(px, py) == 0) {
                    iMapGuard* pGuard = new iMapGuard(iPoint(px,py), ct, RANDOM_QUANTITY, m_rand.Rand(100), GDISP_AGGRESSIVE, false, false);
                    AddMapGuard(pGuard);
                }
            }
        }
    }

    /*
    sint32 count = (m_Width*m_Height)/64;
    while (count) {
        sint32 px = 1+m_rand.Rand(m_Width-2);
        sint32 py = 1+m_rand.Rand(m_Height-2);
        if (m_PassMap.GetAt(px,py) >= 0 && m_CoverMap.GetAt(px, py) == NULL) {
            iMapGuard* pGuard = new iMapGuard(iPoint(px,py), ct, RANDOM_QUANTITY, gGame.Map().Rand(100), GDISP_AGGRESSIVE, false);
            pGuard->Creatures().Grow(week);
            AddMapGuard(pGuard);
            count--;
        }
    }*/
}

iStringT MakeTextEntryName(TextEntryType t, const void* pObj)
{	
    if (t == TET_DEFAULT) {
        return L"";
    } else if (t == TET_MAP_NAME) {
        return L"Map name";
    } else if (t == TET_MAP_DESC) {
        return L"Map Description";
    } else if (t == TET_TEVENT_TEXT) {
        iTimeEvent* pTE = (iTimeEvent*)pObj;
        return iFormat(L"Time Event (%s)", pTE->m_name.CStr());
    } else if (t == TET_HERO_CNAME) {
		iHero* pHero = (iHero*)pObj;
        return iFormat(_T("Hero at (%d,%d) custom name"), pHero->Pos().x, pHero->Pos().y);
    } else if (t == TET_SHIP_CNAME) {
        iShip* pShip = (iShip*)pObj;
        return iFormat(_T("Ship at (%d,%d) custom name"), pShip->Pos().x, pShip->Pos().y);        
    } else if (t == TET_MAPITEM_MSG) {
		iMapItem* pMapItem = (iMapItem*)pObj;
        return iFormat(_T("Map item at (%d,%d) message"), pMapItem->Pos().x, pMapItem->Pos().y);
    } else if (t == TET_MAPGUARD_INTROMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MAPGUARD_OKREWMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) OK reward message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MAPGUARD_CURFAILMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) currently failing reqs message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MAPGUARD_PERMFAILMSG) {
        iMapGuard* pMapGuard = (iMapGuard*)pObj;
        return iFormat(_T("Map guard at (%d,%d) perm failed reqs message"), pMapGuard->Pos().x, pMapGuard->Pos().y);
    } else if (t == TET_MEVENT_REWMSG) {
        iMapEvent* pMapEvent = (iMapEvent*)pObj;
        return iFormat(_T("Map event at (%d,%d) reward message"), pMapEvent->Pos().x, pMapEvent->Pos().y);
    } else if (t == TET_MEVENT_RFAILMSG) {
        iMapEvent* pMapEvent = (iMapEvent*)pObj;
        return iFormat(_T("Map event at (%d,%d) req failure message"), pMapEvent->Pos().x, pMapEvent->Pos().y);
    } else if (t == TET_MEVENT_ATTMSG) {
        iMapEvent* pMapEvent = (iMapEvent*)pObj;
        return iFormat(_T("Map event at (%d,%d) attack message"), pMapEvent->Pos().x, pMapEvent->Pos().y);
    } else if (t == TET_VIS_INTROMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) custom message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_VIS_OKREWMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) OK reward message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_VIS_CURFAILMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) currently failing reqs message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_VIS_PERMFAILMSG) {
        iVisCnst* pVisCnst = (iVisCnst*)pObj;
        return iFormat(_T("Visitable at (%d,%d) perm failed reqs message"), pVisCnst->Pos().x, pVisCnst->Pos().y);
    } else if (t == TET_OWN_INTROMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) intro message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_OWN_OKREWMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) OK reward message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_OWN_CURFAILMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) currently failing reqs message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_OWN_PERMFAILMSG) {
        iOwnCnst* pOwnCnst = (iOwnCnst*)pObj;
        return iFormat(_T("Ownerable at (%d,%d) perm failed reqs message"), pOwnCnst->Pos().x, pOwnCnst->Pos().y);
    } else if (t == TET_CASTLE_CNAME) {
        iCastle* pCastle = (iCastle*)pObj;
        return iFormat(_T("Castle at (%d,%d) custom message"), pCastle->Pos().x, pCastle->Pos().y);
    }

    check(0);
    return L"";
}

iStringT dummy = _T("");


const iStringT& iGameWorld::GetText(const iStringT& key) const {
    if(key == _T("") || !m_ctMgr.HasEntry(key)) return dummy;  // not found, return empty string
    
    const iTextMgr::iTextEntry &res = m_ctMgr.GetEntry(key);        
    if((1 << gSettings.GetLanguage()) & m_supportedLang)
        return res.text[gSettings.GetLanguage()];
    else
        return res.text[GLNG_ENGLISH];
}

const iStringT& iGameWorld::GetEnglishText(const iStringT& key) const{
    if(key == _T("") || !m_ctMgr.HasEntry(key)) return dummy;  // not found, return empty string
    
    const iTextMgr::iTextEntry &res = m_ctMgr.GetEntry(key);    
    return res.text[GLNG_ENGLISH];
}

const iStringT& iGameWorld::GetText(TextEntryType t, void* pObj) const {
    iStringT key = MakeTextEntryName(t, pObj);  
    return GetText(key);
}

#ifdef MULTIPLAYER
PLAYER_ID iGameWorld::RealCurPlayer()
{
    if(!gGame.BattleWrapper()) 
        return CurPlayer()->PlayerId();
    else 
        return ((iBattleWrapper*)gGame.BattleWrapper())->Engine().TurnSeq().CurUnit()->Owner()->Owner();
}
#endif

//////////////////////////////////////////////////////////////////////////

iShip* iGameWorld::AddShip(const iPoint& pos, uint32 img_type)
{
    // create ship as neutral at first
    iShip *pShip = new iShip(pos, img_type);

    m_Ships.Add(pShip);
    gGame.OnAddShip(pShip);    
    SetShipPos(pShip, pos);
    m_pCurPlayer->UpdateVisItems();
    return pShip;
}

void iGameWorld::RemoveShip(iShip *pShip)
{
    m_Ships.Remove(pShip);
	m_CoverMap.GetAt(pShip->Pos().x, pShip->Pos().y) = NULL;
}

// this function used in iCastle, when we need to find position for ship spawn
bool iGameWorld::IsCellNearBeach(const iPoint &ps)
{   
    // check 8 cells around specified cell
    // surf == 0 means that cell is solid water

    if (IsValidPos(ps.x - 1, ps.y - 1))
    {    
        iCell &c = GetAt(ps.x - 1, ps.y - 1);
        if (c.surf != 0) return true;
    }
    if (IsValidPos(ps.x, ps.y - 1))
    {    
        iCell &c = GetAt(ps.x, ps.y - 1);
        if (c.surf != 0) return true;
    }
    if (IsValidPos(ps.x + 1, ps.y - 1))
    {    
        iCell &c = GetAt(ps.x + 1, ps.y - 1);
        if (c.surf != 0) return true;
    }

    if (IsValidPos(ps.x - 1, ps.y))
    {    
        iCell &c = GetAt(ps.x - 1, ps.y);
        if (c.surf != 0) return true;
    }
    if (IsValidPos(ps.x + 1, ps.y))
    {    
        iCell &c = GetAt(ps.x + 1, ps.y);
        if (c.surf != 0) return true;
    }

    if (IsValidPos(ps.x - 1, ps.y + 1))
    {    
        iCell &c = GetAt(ps.x - 1, ps.y + 1);
        if (c.surf != 0) return true;
    }
    if (IsValidPos(ps.x, ps.y + 1))
    {    
        iCell &c = GetAt(ps.x, ps.y + 1);
        if (c.surf != 0) return true;
    }
    if (IsValidPos(ps.x + 1, ps.y + 1))
    {    
        iCell &c = GetAt(ps.x + 1, ps.y + 1);
        if (c.surf != 0) return true;
    }
    return false;
}

uint8 spriteCrop[STYPE_COUNT] = {
	7, // water
	0, // dirt
	0, // grass
	0, // swamp
	0, // lava
	0, // wasteland
	0, // desert
	0, // snow
	0, // ndesert
	0, // pavement
	0, // nwasteland
	0
};

uint8 iGameWorld::PosCropsSprites( const iPoint& pos ) const
{	
	uint8 res = 0;
	res += spriteCrop[GetAt(pos.x, pos.y).SurfNode(0)];
	res += spriteCrop[GetAt(pos.x, pos.y).SurfNode(1)];
	res += spriteCrop[GetAt(pos.x, pos.y).SurfNode(2)];
	res += spriteCrop[GetAt(pos.x, pos.y).SurfNode(3)];
	return res / 4;
}

void iGameWorld::SetupVC()
{
	// init the losing condition
	if(m_loseCondition.type == LC_LOSE_HERO) {
		m_loseCondition.sparam = PID_NEUTRAL;
		for (iGameMap::iCPLIt pit = PlayerFirst(); pit != PlayerEnd(); ++pit) 
			for (iPlayer::iCHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit) 
				if(hit->Proto()->m_protoId == m_loseCondition.fparam)
					m_loseCondition.sparam = pit->PlayerId();
	} else if(m_loseCondition.type == LC_LOSE_CASTLE) {
		m_loseCondition.sparam = PID_NEUTRAL;
		for (iGameMap::iCPLIt pit = PlayerFirst(); pit != PlayerEnd(); ++pit) 
			for (iPlayer::iCCtLIt ctlit = (*pit)->CastleFirst(); ctlit != (*pit)->CastleEnd(); ++ctlit)
				if(ctlit->Owner() == pit->PlayerId() && ctlit->Pos().x == (sint32) (m_loseCondition.fparam & 0xFFFF) && (sint32)(ctlit->Pos().y == m_loseCondition.fparam >> 16))
					m_loseCondition.sparam = pit->PlayerId();		
	}
}

// END OF FILE

