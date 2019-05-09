#include "stdafx.h"
#include "ai.h"
#include "Dlg_NewWeek.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"

/*
 *	Player
 */

iPlayer::iPlayer(PLAYER_TYPE_MASK playerTypeMask, PLAYER_ID playerId, NATION_TYPE nation, const iSize& map_siz)
: m_playerTypeMask(playerTypeMask)
, m_PlayerId(playerId)
, m_Nation(nation)
, m_pCurHero(NULL)
, m_pCurCastle(NULL)
, m_bDirtPassMap(true)
, m_pTavernVisitor(NULL)
, m_bHeroMovedFlag(false)
, m_bRemoved(false)
{
	m_Minerals.Reset();
#ifndef MULTIPLAYER
	m_FogMap.InitFogMap(map_siz.w,map_siz.h, gGame.Map().DifficultyLevel() != DFC_EASY);
#else
	m_FogMap.InitFogMap(map_siz.w,map_siz.h, gGame.Map().DifficultyLevel() != DFC_EASY  
		|| gMPMgr.IsMultiplayer());
#endif
	
	//
	//m_FogMap.OpenWholeMap();
	//
	
	m_passMap.Init(gGame.Map().m_PassMap.GetWidth(),gGame.Map().m_PassMap.GetHeight());
}


iPlayer::~iPlayer()
{
	// Cleanup tavern visitor
	if (m_pTavernVisitor) {
		gGame.ItemMgr().HeroesMgr().Put(m_pTavernVisitor);
		m_pTavernVisitor = NULL;
	}
	m_FogMap.CleanupFogMap();
	m_passMap.Cleanup();
}

iHero* iPlayer::AddHero(iHero* pHero, bool bUpdateFog)
{
	m_Heroes.Add(pHero);
	if (!m_pCurHero) { m_pCurHero = pHero; m_pCurCastle = NULL; }
	if (bUpdateFog) UpdateFogMap();
	gGame.OnAddHero(pHero);
	return pHero;
}

iHero* iPlayer::RemoveHero(iHero* pHero, bool bCheckVictory)
{
	iHLIt nhit = m_Heroes.Remove(pHero);
	if (pHero == m_pCurHero) {
		if (m_Heroes.Count()) {
			SetCurHero(*m_Heroes.First());
		} else if(m_Castles.Count()) {
			SetCurCastle(*m_Castles.First());			
		}
	}
	UpdateFogMap();
	gGame.OnDelHero(pHero);
	if(bCheckVictory)
		gGame.CheckPlayerWinAndLose(this);
	return pHero;
}


uint32 iPlayer::CurHeroIdx() const
{
	uint32 idx = 0;
	for (iHeroList::ConstIterator hit = m_Heroes.First(); hit != m_Heroes.End(); ++hit, ++idx) if ( *hit == m_pCurHero) break;
	return idx;
}

iOwnCnst* iPlayer::AddCnst(iOwnCnst* pCnst, bool bUpdateFog)
{
	m_Cnsts.Add(pCnst);
	if (bUpdateFog) UpdateFogMap();
	return pCnst;
}

iCastle* iPlayer::AddCastle(iCastle* pCastle, bool& bGameResult, bool bUpdateFog)
{
	m_Castles.Add(pCastle);
	//if (!m_pCurCastle) { m_pCurCastle = pCastle; m_pCurHero = NULL; }
	if (bUpdateFog) UpdateFogMap();
	gGame.OnAddCastle(pCastle);
	bGameResult = gGame.CheckPlayerWinAndLose(this);
	return pCastle;
}

iCastle* iPlayer::RemoveCastle(iCastle* pCastle, bool bCheckVictory)
{
	iCtLIt ctit = m_Castles.Remove(pCastle);
	if (pCastle == m_pCurCastle) {
		if (m_Castles.Count()) 
			m_pCurCastle = *m_Castles.First();
		else if(m_Heroes.Count()) 
			SetCurHero(*m_Heroes.First());
	}
	UpdateFogMap();
	gGame.OnDelCastle(pCastle);
	if(bCheckVictory)
		gGame.CheckPlayerWinAndLose(this);
	return *ctit;
}

uint32 iPlayer::CurCastleIdx() const
{
	uint32 idx = 0;
	bool bFound = false;
	for (iCastleList::ConstIterator ctit = m_Castles.First(); ctit != m_Castles.End(); ++ctit, ++idx) if ( *ctit == m_pCurCastle) { bFound = true; break; }
	check(bFound);
	return idx;
}

sint32 iPlayer::GetMineral(MINERAL_TYPE mtype)
{ 
	return m_Minerals.quant[mtype];
}


sint32 iPlayer::AddMineral(MINERAL_TYPE mtype, sint32 count, bool bShowMessage)
{ 
	iMineralSet was = m_Minerals;
	m_Minerals.quant[mtype] = iMAX<sint32>(m_Minerals.quant[mtype]+count,0); 
#ifdef MULTIPLAYER
	CHECKME: why active player?
	if(gGame.Map().ActPlayer() == this)
		gMPMgr.OnResourcesUpdate(gGame.Map().ActPlayer()->PlayerId(), m_Minerals);
#endif
	
	gGame.MainView()->OnPlayerReceivedMinerals(this, was, m_Minerals);
	
	// got the mineral ?
	if(gGame.CheckPlayerWinAndLose(this))
		return 0;
	
	return m_Minerals.quant[mtype];
}

void iPlayer::SetMinerals(const iMineralSet& ms)
{
	m_Minerals = ms;
	// got the mineral ?
	if(gGame.CheckPlayerWinAndLose(this))
		return;

#ifdef MULTIPLAYER
	if(gGame.Map().ActPlayer() == this)
		gMPMgr.OnResourcesUpdate(gGame.Map().ActPlayer()->PlayerId(), m_Minerals);
#endif
}

void iPlayer::AddMineralSet(const iMineralSet& ms)
{	
	iMineralSet was = m_Minerals;
	m_Minerals += ms;

	for(uint32 xx=0; xx<MINERAL_COUNT; xx++)
	{
		m_Minerals.quant[xx] = iMAX(m_Minerals.quant[xx], 0);
	}
	
	gGame.MainView()->OnPlayerReceivedMinerals(this, was, m_Minerals);

#ifdef MULTIPLAYER
	if(gGame.Map().ActPlayer() == this)
		gMPMgr.OnResourcesUpdate(gGame.Map().ActPlayer()->PlayerId(), m_Minerals);
#endif

}
	

bool iPlayer::NewDay()
{	
	for (iHLIt hit = m_Heroes.First(); hit != m_Heroes.End(); ++hit) (*hit)->NewDay();    
	for (iCLIt cit = m_Cnsts.First(); cit != m_Cnsts.End(); ++cit) (*cit)->NewDay();
	for (iCtLIt ctit = m_Castles.First(); ctit != m_Castles.End(); ++ctit) (*ctit)->NewDay();

	// Process player's minerals income
	for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) { 
		AddMineral((MINERAL_TYPE)xx, m_furtSkills.Value((FURTHER_SKILLS)(FSK_MIN_GOLD+xx)));
	}
	UpdateFogMap();	
    
    if(PlayerType() == PT_HUMAN) {
        switch(gGame.Map().GameDay()) {
            case 2: gTutorial.Trigger(TUT_HERO_CASTLE_SELECT); break;
            case 3:
                if(!gUseIpadUI)
                    gTutorial.Trigger(TUT_MINIMAP);
                break;
            case 4: gTutorial.Trigger(TUT_INFO_MODE); break;
            case 5: gTutorial.Trigger(TUT_HORSE_BUTTON); break;
        }
    }
    
	return false;
}

void iPlayer::NewWeek(const iWeekDesc& weekDesk)
{
	// Update tavern visitor
	UpdateTavernVisitor();

	//
	for (iHLIt hit = m_Heroes.First(); hit != m_Heroes.End(); ++hit) (*hit)->NewWeek(weekDesk);
	for (iCLIt cit = m_Cnsts.First(); cit != m_Cnsts.End(); ++cit) (*cit)->NewWeek(weekDesk);
	for (iCtLIt ctit = m_Castles.First(); ctit != m_Castles.End(); ++ctit) (*ctit)->NewWeek(weekDesk);
}


void iPlayer::DiscoverFullMap()
{
	FogMap().DiscoverMap(0, 0, 512, false);
}

void iPlayer::OnSelect(bool bNewWeek, bool bAfterLoad)
{
	//check(PlayerType() == PT_HUMAN);
	// Show new week dialog
#ifndef ROYAL_BOUNTY        
	if (bNewWeek) {
		iDlg_NewWeek dlg(&gApp.ViewMgr(), m_PlayerId, gGame.Map().WeekDesc());
		dlg.DoModal();
	}
	// show current day
	uint32 days = gGame.Map().m_CurDay-1;
	gGame.AddMsg(iFormat(_T("#F4B4%s: #F9F9%d  #F4B4%s: #F9F9%d  #F4B4%s: #F9F9%d"),gTextMgr[TRID_MONTH], days/28+1,gTextMgr[TRID_WEEK], (days%28)/7+1,gTextMgr[TRID_DAY],days%7+1));
#endif

	// update fog map
	UpdateFogMap();

	// Select current hero and castle
	if (!m_pCurHero && m_Heroes.Count()){
		SetCurHero(*m_Heroes.First());
	} else if(!m_pCurHero && !m_pCurCastle && m_Castles.Count()) {
		SetCurCastle(*m_Castles.First());
	}
	if (m_pCurHero) m_pCurHero->OnSelect(bAfterLoad);
	else if (m_pCurCastle) m_pCurCastle->OnSelect();

	// Process time events
	if (!bAfterLoad) gGame.Map().ProcessTimeEvents(this);

	if(gGame.CheckPlayerWinAndLose(this))
		return;
	
    if(gGame.Map().GameDay() == 1) {
#ifndef ROYAL_BOUNTY
        const iVictoryCondition& vc = gGame.Map().VictoryCondition();
                
        iStringT vcText, lcText;
        
        switch(vc.type) {
            case VC_DEFEAT_ALL_ENEMIES:
                vcText = gTextMgr[TRID_VC_DEFEAT_ALL_ENEMIES];
                break;
            case VC_DEFEAT_HERO:
                for (iGameMap::iCPLIt pit = gGame.Map().PlayerFirst(); pit != gGame.Map().PlayerEnd(); ++pit) 
                    for (iPlayer::iCHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit) 
                        if(hit->Proto()->m_protoId == vc.fparam) {
                            vcText = iFormat(gTextMgr[TRID_VC_DEFEAT_HERO], hit->Name().CStr());                      
                        }
                break;
            case VC_CAPTURE_CASTLE:
                for (iPlayer::iCtLIt ctlit = CastleFirst(); ctlit != CastleEnd(); ++ctlit)
                    if(ctlit->Pos().x == (sint32) (vc.fparam & 0xFFFF) && (sint32)(ctlit->Pos().y == vc.fparam >> 16))
                        vcText = iFormat(gTextMgr[TRID_VC_CAPTURE_CASTLE], ctlit->Name().CStr());                      
                break;
            case VC_SURVIVE:
                vcText = iFormat(gTextMgr[TRID_VC_SURVIVE], vc.fparam);
                break;
            case VC_RESOURCE:
                vcText = iFormat(gTextMgr[TRID_VC_RESOURCE], vc.sparam, gTextMgr[TRID_MINERAL_GOLD + vc.fparam]);
                break;
            case VC_ARTIFACT:
                vcText = iFormat(gTextMgr[TRID_VC_ARTIFACT], gTextMgr[gGame.ItemMgr().m_artMgr[vc.fparam].NameKey(NULL)]);
                break;
            case VC_MONSTER:
            {
                vcText = iFormat(gTextMgr[TRID_VC_MONSTER], vc.sparam, gTextMgr[TRID_CREATURE_PEASANT_F3 + vc.fparam * 3]);
                break;
            }
            case VC_SPELL:
                vcText = iFormat(gTextMgr[TRID_VC_SPELL], gTextMgr[gGame.ItemMgr().m_spellMgr.Spell((MAGIC_SPELL)vc.fparam)->NameKey()]);
                break;
            case VC_ULTIMATE_ART:
                vcText = gTextMgr[TRID_VC_ULTIMATE_ART];
                break;
        }    
        
        const iLoseCondition& lc = gGame.Map().LoseCondition();
        
        switch(gGame.Map().LoseCondition().type) {
            case LC_DEFAULT:
                lcText = gTextMgr[TRID_LC_NOTHING];
                break;
            case LC_LOSE_HERO:
                for (iPlayer::iCHLIt hit = HeroFirst(); hit != HeroEnd(); ++hit) 
                    if(hit->Proto()->m_protoId == lc.fparam && m_PlayerId == lc.sparam)
                        lcText = iFormat(gTextMgr[TRID_LC_LOSE_HERO], hit->Name().CStr());
                break;
            case LC_LOSE_CASTLE:
                for (iPlayer::iCtLIt ctlit = CastleFirst(); ctlit != CastleEnd(); ++ctlit)
                    if(ctlit->Pos().x == (sint32) (lc.fparam & 0xFFFF) && (sint32)(ctlit->Pos().y == lc.fparam >> 16))
                        lcText = iFormat(gTextMgr[TRID_LC_LOSE_CASTLE], ctlit->Name().CStr());
                break;
            }
        
        iTextDlg dlg_vc(&gApp.ViewMgr(), _T(""), iFormat(_T("#S1%s\n#S0%s\n\n#S1%s\n#S0%s\n%s"), gTextMgr[TRID_VICTORY_CONDITION], vcText.CStr(),
                                                         gTextMgr[TRID_LOSE_CONDITION], lcText.CStr(),
#ifdef PLAYCITY
                                                         _T("\nНажмите для продолжения...")
#else
                                                         _T("")
#endif
                                                         ), PID_NEUTRAL);
        dlg_vc.DoModal();
#endif
        gTutorial.Trigger(TUT_GAME_SENSE);
        gTutorial.Trigger(TUT_MAP_CONTROLS);
    }
    
}

void iPlayer::OnCaptureCastle(iCastle* pNewCastle)
{
	//SetCurCastle(pNewCastle);
}

void iPlayer::NextHeroCastle()
{
	if (!m_pCurHero && !m_Heroes.Count() && !m_pCurCastle && !m_Castles.Count()) return;
	
	// a hero
	if(m_pCurHero) {
		// hero exhausted
		if(m_pCurHero->Exhausted()) {
			if(m_bHeroMovedFlag) {
				// move to end of queue
				m_Heroes.Remove(m_pCurHero);
				m_Heroes.Add(m_pCurHero);
				m_bHeroMovedFlag = false;
			}
			iHLIt hit(m_pCurHero);
			hit++;
			// search for next exhausted
			while (hit != m_Heroes.End() && !(*hit)->Exhausted()) hit++;
			
			if(hit != m_Heroes.End()) { // found next exhausted, select him
				SetCurHero(*hit);
				return;
			} else { // no more exhausted heroes found
				// search for FIRST unexhausted hero
				hit = m_Heroes.First();
				
				while (hit != m_Heroes.End() && (*hit)->Exhausted()) hit++;
				
				// there is an unexhausted hero, select him
				if(hit != m_Heroes.End()) {
					SetCurHero(*hit);
					return;
				} else { // there is no unexausted heroes
					if(m_Castles.Count()) { // there are castles, select first
						m_pCurHero = NULL;
						SetCurCastle(*m_Castles.First());
						return;
					} else { // no castles, select FIRST hero (exhausted, obviously)
						SetCurHero(*m_Heroes.First());
						return;
					}
				}
			}	
		} else { // hero unexhausted
			// hero just moved
			if(m_bHeroMovedFlag) {
				// move to end of queue
				m_Heroes.Remove(m_pCurHero);
				m_Heroes.Add(m_pCurHero);
				
				iHLIt hit(m_Heroes.First());
				// search for FIRST unexhausted hero
				while (hit != m_Heroes.End() && (*hit)->Exhausted()) hit++;
				
				// there is an unexhausted hero, select him
				if(hit != m_Heroes.End()) {
					SetCurHero(*hit);
					return;
				} else { // there are no MORE unexhausted heroes (we have at least 1 - current)
					if(m_Castles.Count()) { // there are castles, select first
						m_pCurHero = NULL;
						SetCurCastle(*m_Castles.First());
						return;
					} else { // no castles, select FIRST hero (this is doubtful - he can be either ex, or unex)
						SetCurHero(*m_Heroes.First());
						return;
					}
				}
			} else { // hero didn't move right now
				iHLIt hit(m_pCurHero);
				hit++;
				// search for next unexhausted hero
				while (hit != m_Heroes.End() && (*hit)->Exhausted()) hit++;
				
				// next unexausted found, select him
				if(hit != m_Heroes.End()) {
					SetCurHero(*hit);
					return;
				} else { // no more unexhausted found
					// if there are castles, select first
					if(m_Castles.Count()) {
						m_pCurHero = NULL;
						SetCurCastle(*m_Castles.First());
						return;
					} else { // no castles, search for first exhausted hero
						iHLIt hit = m_Heroes.First();
						// search for first exhausted hero
						while (hit != m_Heroes.End() && !(*hit)->Exhausted()) hit++;
						
						// found exhausted hero, select him
						if(hit != m_Heroes.End()) {
							SetCurHero(*hit);
							return;
						} else { // no exhausted, no unexausted, no castles, select FIRST HERO
							SetCurHero(*m_Heroes.First());
							return;
						}
					}
				}
			}
		}
	} else { // a castle
		iCtLIt ctit(m_pCurCastle);
		if (++ctit == m_Castles.End()) { // this is last castle
			if(m_Heroes.Count()) { // there are heroes
				iHLIt hit = m_Heroes.First();
				// search for first exhausted hero
				while (hit != m_Heroes.End() && !(*hit)->Exhausted()) hit++;
				
				// found exhausted hero, select him
				if(hit != m_Heroes.End()) {
					SetCurHero(*hit);
					return;
				} else { // no exhausted, select first hero (unexhausted, obviously)
					m_pCurCastle = NULL;
					SetCurHero(*m_Heroes.First());
					return;
				}
			} else { // there is no heroes, select first castle
				SetCurCastle(*m_Castles.First());
				return;
			}
		} else { // not last castle, select next castle
			SetCurCastle(*ctit);
			return;
		}
	}
}


void iPlayer::PrevHeroCastle()
{
	if (!m_pCurHero && !m_Heroes.Count() && !m_pCurCastle && !m_Castles.Count()) return;
	if(m_pCurHero) {
		iHLIt hit(m_pCurHero);
		if (hit == m_Heroes.First()) {
			if(m_Castles.Count()) {
				m_pCurHero = NULL;
				SetCurCastle(*m_Castles.Last());
			}
			else
				SetCurHero(*m_Heroes.Last());
			return;
		} 
		SetCurHero(*(--hit));
		return;
	} else if(m_pCurCastle) {
		iCtLIt ctit(m_pCurCastle);
		if (ctit == m_Castles.First()) {
			if(m_Heroes.Count()) {
				m_pCurCastle = NULL;
				SetCurHero(*m_Heroes.Last());
			}
			else
				SetCurCastle(*m_Castles.Last());
			return;
		}
		SetCurCastle(*(--ctit));
		return;
	}
}


iHero* iPlayer::NextHero()
{
	if (!m_Heroes.Count()) return NULL;
	if(!m_pCurHero) SetCurHero(*m_Heroes.First());
	else {
		iHLIt hit(m_pCurHero);
		if (++hit == m_Heroes.End()) hit = m_Heroes.First();
		SetCurHero(*hit);
	}
	return m_pCurHero;
}

iHero* iPlayer::PrevHero()
{
	if (!m_Heroes.Count()) return NULL;
	if(!m_pCurHero) SetCurHero(*m_Heroes.First());
	else {
		iHLIt hit(m_pCurHero);
		if (hit == m_Heroes.First()) hit = m_Heroes.Last();
		else --hit;
		SetCurHero(*hit);
	}
	return m_pCurHero;
}

void iPlayer::PrvSetCurHero(uint16 hid)
{
	check(hid != 0xFFFF);
//	iHero* pHero = NULL;
	for (iHLIt hit = m_Heroes.First(); hit != m_Heroes.End(); ++hit) {
		if ( (*hit)->Proto()->m_protoId == hid ) {
			m_pCurHero = (*hit);
			m_pCurCastle = NULL;
			return;
		}
	}
	check(0);
}

void iPlayer::SetCurHero(iHero* pHero)
{
	m_pCurCastle = NULL;
	m_pCurHero = pHero;
	if(pHero && pHero->Pos() != cInvalidPoint) 
		m_pCurHero->OnSelect(false);
	m_bDirtPassMap = true;
	m_bHeroMovedFlag = false;
}


iCastle* iPlayer::NextCastle()
{
	if (!m_Castles.Count()) return NULL;
	if(!m_pCurCastle) { SetCurCastle(*m_Castles.First()); }
	else {
		iCtLIt ctit(m_pCurCastle);
		if (++ctit == m_Castles.End()) ctit = m_Castles.First();
		SetCurCastle(*ctit);
	}
	return m_pCurCastle;
}

iCastle* iPlayer::PrevCastle()
{
	if (!m_Castles.Count()) return NULL;
	if(!m_pCurCastle) { SetCurCastle(*m_Castles.First()); }
	else {
		iCtLIt ctit(m_pCurCastle);
		if (ctit == m_Castles.First()) ctit = m_Castles.Last();
		else --ctit;
		SetCurCastle(*ctit);
	}
	return m_pCurCastle;
}


void iPlayer::SetCurCastle(iCastle* pCastle)
{
	m_pCurHero = NULL;
	m_pCurCastle = pCastle;
	if(pCastle)
		m_pCurCastle->OnSelect();
	m_bHeroMovedFlag = false;
}

void iPlayer::Process(fix32 t)
{
	// Process heroes
	if (m_pCurHero) {
		m_pCurHero->Process(t);
	}
}

//////////////////////////////////////////////////////////////////////////
bool iPlayer::UpdateVisItems()
{
	iVisItemList tmpItems;
	iGameWorld& map = gGame.Map();

	// Map items
	for (iGameWorld::iMIt mit = map.m_MapItems.First(); mit != map.m_MapItems.End(); ++mit) {
		iPoint p((*mit)->Pos());
		if ( !m_FogMap.IsInvisible(p)) {
			tmpItems.AddItem(*mit);
		}
	}

	// Map guards
	for (iGameWorld::iGIt git = map.m_MapGuards.First(); git != map.m_MapGuards.End(); ++git) {
		iPoint p((*git)->Pos());
		if (!m_FogMap.IsInvisible(p)) {
			tmpItems.AddItem(*git);
		}
	}

	// Visitables
	for (iGameWorld::iVCIt vcit = map.m_VisCnsts.First(); vcit != map.m_VisCnsts.End(); ++vcit) {
		iPoint p((*vcit)->Pos());
		if ( !m_FogMap.IsInvisible(p) ) {
			tmpItems.AddItem(*vcit);
		}
	}

    // Ships
    for (iGameWorld::iSLIt ocit = map.m_Ships.First(); ocit != map.m_Ships.End(); ++ocit) 
    {
        if (!(*ocit)->IsLoaded()) {
            iPoint p((*ocit)->Pos());
            if ( !m_FogMap.IsInvisible(p) ) 
            {
                tmpItems.AddItem(*ocit);
            }
        } else {
		    iPoint p((*ocit)->Pos()); 
            if ( !m_FogMap.IsHidden(p) ) 
            {
                tmpItems.AddItem(*ocit);
            }	
		}
    }

	// Unoccupied constructions and castles
	{
		for (iGameWorld::iOCIt ocit = map.m_OwnCnsts.First(); ocit != map.m_OwnCnsts.End(); ++ocit) {
			iPoint p((*ocit)->Pos());
			if ( !m_FogMap.IsInvisible(p) ) {
				tmpItems.AddItem(*ocit);
			}
		}
		for (iGameWorld::iCtIt ctit = map.m_Castles.First(); ctit != map.m_Castles.End(); ++ctit) {
			const iCastleT* pProto = (*ctit)->Proto();
			for (uint32 cvid=0; cvid<pProto->CoversCount(); ++cvid) {
				iPoint p((*ctit)->Pos().x + pProto->CoverEntry(cvid).ox, (*ctit)->Pos().y + pProto->CoverEntry(cvid).oy);
				if (!m_FogMap.IsInvisible(p)){
					tmpItems.AddItem(*ctit);
					break;
				}
			}
		}
	}

	// Owned constructions, castles and heroes
	for (iGameWorld::iPLIt plit = map.m_Players.First(); plit != map.m_Players.End(); ++plit) {
		for (iCLIt ocit = (*plit)->m_Cnsts.First(); ocit != (*plit)->m_Cnsts.End(); ++ocit) {
			iPoint p((*ocit)->Pos());
			if ( !m_FogMap.IsInvisible(p) ) {
				tmpItems.AddItem(*ocit);
			}
		}


		// here is a problem: previously we added a castle to a vislist if we saw a little bit of it (no entrance, possibly)
		// this is dangerous, because AI could try to enter such castle, using iHero::MakePath and will fail
		// old variant of AI used iTargetScanner::MakePath that will "cheat" a bit and find it's way to castle's entrance even if we didn't see it yet
		// Hedin, 28.10.2010
		for (iCtLIt ctit = (*plit)->m_Castles.First(); ctit !=(*plit)->m_Castles.End(); ++ctit) {
			const iCastleT* pProto = (*ctit)->Proto();
			if ( !m_FogMap.IsInvisible(ctit->Pos()) ) {
				tmpItems.AddItem(*ctit);
			}
			/*for (uint32 cvid=0; cvid<pProto->CoversCount(); ++cvid) {
				iPoint p((*ctit)->Pos().x + pProto->CoverEntry(cvid).ox, (*ctit)->Pos().y + pProto->CoverEntry(cvid).oy);
				if (!m_FogMap.IsInvisible(p)){
					tmpItems.AddItem(*ctit);
					break;
				}
			}*/

		}
		
		//if ( (*plit)->PlayerId() == PlayerId() ) continue;
		for (iHLIt hit = (*plit)->m_Heroes.First(); hit != (*plit)->m_Heroes.End(); ++hit) {
			iPoint p((*hit)->Pos());
			if ( !m_FogMap.IsHidden(p) ) {
				tmpItems.AddItem(*hit);
			}
		}
	}

	// Find new items
	m_newVisItems.Cleanup();
	m_oldVisItems.Cleanup();
	uint32 oCnt = m_visItems.Size();
	uint32 tCnt = tmpItems.Size();
	uint32 oPos=0;
	uint32 tPos=0;
	while (1) {
		if (tPos == tCnt) {
			while (oPos < oCnt) m_oldVisItems.AddItem(m_visItems[oPos++].value);
			break;
		}
		if (oPos == oCnt) {
			while (tPos < tCnt) {
				m_newVisItems.AddItem(tmpItems[tPos].value);
				++tPos;
			}
			break;
		}

		if (tmpItems[tPos].value == m_visItems[oPos].value && tmpItems[tPos].value->Pos() == m_visItems[oPos].value->Pos()) {
			++oPos;
			++tPos;
		} else {
			if (tmpItems[tPos].idx > m_visItems[oPos].idx){
				while (oPos < oCnt && tmpItems[tPos].idx > m_visItems[oPos].idx){
					m_oldVisItems.AddItem(m_visItems[oPos].value);
					++oPos;
				}
				continue;
				}
			m_newVisItems.AddItem(tmpItems[tPos].value);
			++tPos;
		}
	}
	m_visItems.Init(tmpItems);
	
	if (m_newVisItems.Size() || OldVisItems().Size()) m_bDirtPassMap = true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
void iPlayer::ResetVistItemChanges()
{
	m_newVisItems.Cleanup();
	m_oldVisItems.Cleanup();
}

sint32 iPlayer::GetMarketIdx()
{
	sint32 result = 0;
	for (iCastleList::Iterator ctit=m_Castles.First(); ctit!=m_Castles.End(); ++ctit) {
		iCtlCnst* pMarket = (*ctit)->FindCnst(CTLCNST_MARKETPLACE);
		if (pMarket && pMarket->IsBuilt()) result ++;
		iCtlCnst* pMerchant = (*ctit)->FindCnst(CTLCNST_MERCHANT);
		if(pMerchant && pMerchant->IsBuilt()) result++;
	}	
	return result;
}

//////////////////////////////////////////////////////////////////////////

extern bool openAllMap;

void iPlayer::UpdateFogMap()
{	
	if(this != gGame.Map().ActPlayer() || !openAllMap)
		m_FogMap.ResetFog();
	else {
		DiscoverFullMap();
		return;
	}
#ifdef MULTIPLAYER
	if(gMPMgr.IsMultiplayer()
		
#ifdef DEBUG_FEATURES
		/*|| PlayerType() == PT_HUMAN*/
#endif
		)
		m_FogMap.DiscoverMap(0, 0, gGame.Map().GetWidth() + gGame.Map().GetHeight());
#endif
	if (gSettings.ShowEnemyTurn() && PlayerType() == PT_HUMAN) {
		for (iGameWorld::iPLIt pit = gGame.Map().m_Players.First(); pit != gGame.Map().m_Players.End(); ++pit) {
			for (iHLIt hit=(*pit)->m_Heroes.First(); hit!=(*pit)->m_Heroes.End(); ++hit)
				m_FogMap.DiscoverMap((*hit)->Pos().x,(*hit)->Pos().y,(*hit)->Scouting(), false);
			for (iCLIt cit=(*pit)->m_Cnsts.First(); cit!=(*pit)->m_Cnsts.End(); ++cit)
				m_FogMap.DiscoverMap((*cit)->Pos().x,(*cit)->Pos().y, (*cit)->Proto()->Scouting(), false);
			for (iCtLIt ctit=(*pit)->m_Castles.First(); ctit!=(*pit)->m_Castles.End(); ++ctit) 
				m_FogMap.DiscoverMap((*ctit)->Pos().x,(*ctit)->Pos().y, (*ctit)->Scouting(), false);
		}
	} else {
		for (iHLIt hit=m_Heroes.First(); hit!=m_Heroes.End(); ++hit)
			m_FogMap.DiscoverMap((*hit)->Pos().x,(*hit)->Pos().y,(*hit)->Scouting(), false);
		for (iCLIt cit=m_Cnsts.First(); cit!=m_Cnsts.End(); ++cit)
			m_FogMap.DiscoverMap((*cit)->Pos().x,(*cit)->Pos().y, (*cit)->Proto()->Scouting(), false);
		for (iCtLIt ctit=m_Castles.First(); ctit!=m_Castles.End(); ++ctit)
			m_FogMap.DiscoverMap((*ctit)->Pos().x,(*ctit)->Pos().y, (*ctit)->Scouting(), false);
	}
	
	uint8 dcfg, fcfg;
	const iPoint pt(49, 69);
	const iFogMap& fm = m_FogMap;
	fm.GetFogCfg(pt, dcfg, fcfg);

	UpdateVisItems();
	m_bDirtPassMap = true;

	if (gSettings.ShowEnemyTurn() && PlayerType() != PT_HUMAN && gGame.Map().ActPlayer()) {
		gGame.Map().ActPlayer()->UpdateFogMap();
	}
}
//////////////////////////////////////////////////////////////////////////
void iPlayer::UpdatePassMapLocNearShip(const iPoint &pt)
{
    if (m_passMap.IsValidPos(pt.x, pt.y))
    {
         sint32 res = m_passMap.GetAt(pt.x, pt.y);
        if (res == SURF_MOVE_COST_PWATER) 
        {
            res = SURF_MOVE_COST[STYPE_SAND];
            m_passMap.GetAt(pt.x, pt.y) = (sint8)res;
        }
    }
}

void iPlayer::UpdatePassMap()
{
	if (!m_bDirtPassMap) return;
	m_bDirtPassMap=false;
	bool bConstPass = (PlayerType() == PT_COMPUTER);

	sint32 mbonus = m_pCurHero ? m_pCurHero->MoveCostBonus() : 0;
	
	m_passMap.CopyFrom(gGame.Map().m_PassMap);
	for (sint32 yy=0; yy<m_passMap.GetHeight(); ++yy){
		for (sint32 xx=0; xx<m_passMap.GetWidth(); ++xx){
			sint32 res;
			if (m_FogMap.IsInvisible(iPoint(xx,yy))) {//
				res = -1;
			} else {
				iBaseMapObject* pObj = gGame.Map().m_CoverMap.GetAt(xx,yy);
				if (pObj && pObj->Pos() == iPoint(xx,yy) && (!pObj->Disap() || bConstPass || !m_FogMap.IsInvisible(pObj->Pos()))) {
					res = -2;
				} else {
					res = m_passMap.GetAt(xx,yy);
				}
			}
			if (mbonus && res > SURF_MOVE_COST[STYPE_DIRT]) res -= (res-SURF_MOVE_COST[STYPE_DIRT]) * mbonus / 100;
			m_passMap.GetAt(xx,yy) = (sint8)res;
		}
	}

#if 0
    // ships - check around ship 3x3 area and replace part_water to ground (so heroes can reach the ship)
    for (iGameWorld::iSLIt ctit = gGame.Map().m_Ships.First(); ctit != gGame.Map().m_Ships.End(); ++ctit) 
    {       
        iShip* pShip = *ctit;
        const iPoint &pt = pShip->Pos();
        
        if (m_FogMap.IsInvisible(pt))
            continue;
        if (m_FogMap.IsHidden(pt))
            continue;

        UpdatePassMapLocNearShip(pt - iPoint(-1, -1));
        UpdatePassMapLocNearShip(pt - iPoint( 0, -1));
        UpdatePassMapLocNearShip(pt - iPoint( 1, -1));

        UpdatePassMapLocNearShip(pt - iPoint(-1,  1));
        UpdatePassMapLocNearShip(pt - iPoint( 0,  1));
        UpdatePassMapLocNearShip(pt - iPoint( 1,  1));

        UpdatePassMapLocNearShip(pt - iPoint(-1,  0));
        UpdatePassMapLocNearShip(pt - iPoint( 1,  0));
    }
#endif
}
                               
bool iPlayer::NeedStopHero() const
{
	return (m_newVisItems.Size() || m_oldVisItems.Size());
}

bool iPlayer::EncounteredHero() const {
	
	for(uint32 xx=0; xx < m_newVisItems.Size(); xx++) {
		iBaseMapObject* pObj = m_newVisItems[xx].value;
		if(dynamic_cast<iHero*> (pObj))
			return true;
	}
	return false;
}

void iPlayer::UpdateTavernVisitor(iHero* pNewVisitor)
{
	if (m_pTavernVisitor) {
		gGame.ItemMgr().HeroesMgr().Put(m_pTavernVisitor);
		m_pTavernVisitor = NULL;
	}
	if (pNewVisitor) {
		m_pTavernVisitor = pNewVisitor;
	} else {
		if (m_Castles.Count()) {
			uint32 htm = 0;
			// Prepare list of nations according to owned castles
			for (iCtLIt ctlit = m_Castles.First(); ctlit != m_Castles.End(); ++ctlit) htm |= CTL_HEROES[(*ctlit)->Proto()->Type()];
			uint16 selhid = gGame.ItemMgr().HeroesMgr().Select(htm);
			if (selhid == iHeroesMgr::INVALID_HERO_ID) 
				selhid = gGame.ItemMgr().HeroesMgr().Select(HTM_ALL);
			if (selhid != iHeroesMgr::INVALID_HERO_ID) 
				m_pTavernVisitor = gGame.ItemMgr().HeroesMgr().Get(selhid);
		} 
	}
}

iHero* iPlayer::RecruitTavernVisitor(iCastle* pTarget)
{
	iHero* pCurVisitor = m_pTavernVisitor;
	m_pTavernVisitor = NULL;
	AddMineralSet(-pCurVisitor->Proto()->m_Cost);
	gGame.Map().AddHero(pCurVisitor, this, pTarget->Pos(), HERO_ORIENT[pTarget->Proto()->Orient()], false);
#ifdef MULTIPLAYER
	gMPMgr.OnRecruitHero(pCurVisitor, pTarget);
	gMPMgr.OnHeroUpdate(pCurVisitor);
#endif
	UpdateTavernVisitor();
	SetCurHero(pCurVisitor);
    gTutorial.Trigger(TUT_HIRED_HERO);
	return pCurVisitor;
}

void iPlayer::AddKey(uint8 key) 
{ 
	check(key<6); m_keys  |= (1<<key); 
#ifdef MULTIPLAYER
		if(this == gGame.Map().ActPlayer()) {
			gMPMgr.OnKeysUpdate(m_keys);
		}
#endif
	}

uint16 iPlayer::OpenObelisk( ) 
{ 
	uint16 oldOpened = OpenedPuzzles();
	++m_openedObelisks;
#ifdef MULTIPLAYER
	gMPMgr.OnOpenObelisksChanged(m_openedObelisks);
#endif
	return OpenedPuzzles() - oldOpened;
}

uint8 iPlayer::OpenedPuzzles() const 
{ 
	uint16 ocnt = gGame.Map().ObelisksCount();
	if (!ocnt) return 0;
	return iCLAMP<uint8>(0,TotalPuzzles(),((TotalPuzzles() * m_openedObelisks) / ocnt)); 
}

uint8 iPlayer::PuzzledCard(uint8 idx) const 
{ 
	check(idx<TotalPuzzles()); 
	return m_puzzleCards[idx]; 
}


iPlayer::iPlayer(iPlayer& other):
	m_playerTypeMask(other.m_playerTypeMask),
	m_PlayerId(other.m_PlayerId),
	m_Nation(other.m_Nation),
	m_pCurHero(other.m_pCurHero),
	m_pCurCastle(other.m_pCurCastle),
	m_bDirtPassMap(other.m_bDirtPassMap),
	m_pTavernVisitor(other.m_pTavernVisitor),
	m_bHeroMovedFlag(false)
{
	
	this->m_bDirtPassMap = other.m_bDirtPassMap;
	this->m_keys = other.m_keys;
	this->m_furtSkills = other.m_furtSkills;
	this->m_Minerals = other.m_Minerals;
	this->m_openedObelisks = other.m_openedObelisks;
	
//	this->m_Castles = other.m_Castles;
//	this->m_Cnsts = other.m_Cnsts;			
//	this->m_Heroes = other.m_Heroes;
//	this->m_FogMap = other.m_FogMap;	
//  this->m_passMap = other.m_passMap;	

	//this->m_newVisItems = other.m_newVisItems;	
	//this->m_oldVisItems = other.m_oldVisItems;
	//this->m_visItems = other.m_visItems;
	
	memcpy(this->m_puzzleCards, other.m_puzzleCards, 45 * sizeof(uint8));
	

	/* pass the property */

	/* ownerables */
	{
	iCLIt cit = other.m_Cnsts.First();
	while (cit != other.m_Cnsts.End()) {
		iOwnCnst* pCnst = *cit;
		cit = other.m_Cnsts.Remove(cit);
		m_Cnsts.Add(pCnst);
		pCnst->SetOwner(PlayerId());
	}
	}

	/* castles */
	{
	iCtLIt cit = other.m_Castles.First();
	while (cit != other.m_Castles.End()) {
		iCastle* pCnst = *cit;
		cit = other.m_Castles.Remove(cit);
		m_Castles.Add(pCnst);
		pCnst->SetOwner(PlayerId());
	}
	}

	/* heroes */
	{
	iHLIt cit = other.m_Heroes.First();
	while (cit != other.m_Heroes.End()) {
		iHero* pCnst = *cit;
		cit = other.m_Heroes.Remove(cit);
		m_Heroes.Add(pCnst);
		pCnst->SetOwner(this);
	}
	}

	this->UpdateVisItems();
	//this->UpdatePassMap();
	this->UpdateFogMap();
}

	
VICTORY_CONDITION_TYPE iPlayer::MeetsVictoryCondition() const
{
	if(!gGame.Started()) return VC_INVALID;
	const iVictoryCondition& vc = gGame.Map().VictoryCondition();
	
	if(vc.time != 0 && vc.time < gGame.Map().GameDay()) // out of time, we didn't win
		return VC_INVALID;

	switch(vc.type) {
	case VC_DEFEAT_ALL_ENEMIES:
		{				
		iGameMap::iCPLIt nit = ++iGameMap::iCPLIt(gGame.Map().PlayerFirst());
		return (nit == gGame.Map().PlayerEnd()) ? VC_DEFEAT_ALL_ENEMIES : VC_INVALID;		
		}
	case VC_DEFEAT_HERO:
		for (iGameMap::iCPLIt pit = gGame.Map().PlayerFirst(); pit != gGame.Map().PlayerEnd(); ++pit) 
			for (iPlayer::iCHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit) 
				if(hit->Proto()->m_protoId == vc.fparam)
					return VC_INVALID;
		return (vc.sparam != m_PlayerId) ? VC_DEFEAT_HERO : VC_INVALID;	
	case VC_CAPTURE_CASTLE:
		/*if(PlayerType() == PT_COMPUTER)
			return ;
			*/
		for (iPlayer::iCCtLIt ctlit = CastleFirst(); ctlit != CastleEnd(); ++ctlit)
			if(ctlit->Pos().x == (sint32) (vc.fparam & 0xFFFF) && (sint32)(ctlit->Pos().y == vc.fparam >> 16))
				return (vc.sparam != m_PlayerId) ? VC_CAPTURE_CASTLE : VC_INVALID;
		return VC_INVALID;
	case VC_SURVIVE:
		if(PlayerType() == PT_COMPUTER)
			return VC_INVALID;

		return (gGame.Map().GameDay() > vc.fparam) ? VC_SURVIVE : VC_INVALID;
	case VC_RESOURCE:
		return (m_Minerals.quant[(MINERAL_TYPE)vc.fparam] >= vc.sparam) ? VC_RESOURCE : VC_INVALID;
	case VC_ARTIFACT:
		for (iPlayer::iCHLIt hit = HeroFirst(); hit != HeroEnd(); ++hit) 
			if((*hit)->Artifacts().HasArtifact(vc.fparam))
				return VC_ARTIFACT;
		return VC_INVALID;
	case VC_MONSTER:
		{
		uint32 count = 0;

		// sum up all the heroes, castles and owncnsts guard creatures of the required type
		for (iPlayer::iCHLIt hit = HeroFirst(); hit != HeroEnd(); ++hit) 
			for (sint32 xx=0; xx<7; ++xx)
				if (hit->Army().At(xx).Type() == vc.fparam)
					count += hit->Army().At(xx).Count();
			for (iPlayer::iCCtLIt ctlit = CastleFirst(); ctlit != CastleEnd(); ++ctlit)
				for (sint32 xx=0; xx<7; ++xx)
   					if (ctlit->Army().At(xx).Type() == vc.fparam)
						count += ctlit->Army().At(xx).Count();
			for (iGameMap::iOwnCnstList::ConstIterator ocit = gGame.Map().OwnCnstFirst(); ocit != gGame.Map().OwnCnstEnd(); ++ocit)
				if(ocit->Owner() == PlayerId())
					for (sint32 xx=0; xx<7; ++xx)
						if (ocit->Army().At(xx).Type() == vc.fparam)
							count += ocit->Army().At(xx).Count();
			return (count >= vc.sparam) ?VC_MONSTER : VC_INVALID;
		}
	case VC_SPELL:
		for (iPlayer::iCHLIt hit = HeroFirst(); hit != HeroEnd(); ++hit) 
			if(hit->HasSpell(vc.fparam))
				return VC_SPELL;
		return VC_INVALID;
	case VC_ULTIMATE_ART:
		for (iPlayer::iCHLIt hit = HeroFirst(); hit != HeroEnd(); ++hit) 
			if(hit->Artifacts().HasArtType(ARTT_ULTIMATE))
				return VC_ULTIMATE_ART;
		return VC_INVALID;
	}
	return VC_INVALID;
}

LOSE_CONDITION_TYPE iPlayer::MeetsLoseCondition() const
{
	if(!DefeatState()) return LC_DEFAULT;
	if(!gGame.Started()) return LC_INVALID;
	const iVictoryCondition& vc = gGame.Map().VictoryCondition();
	const iLoseCondition& lc = gGame.Map().LoseCondition();
	
	if(PlayerType() != PT_COMPUTER && vc.time != 0 && vc.time < gGame.Map().GameDay()) // out of time
		return LC_INTIME; // we've LOST
	
	switch(gGame.Map().LoseCondition().type) {
	case LC_DEFAULT:
		return LC_INVALID;
	case LC_LOSE_HERO:
		for (iPlayer::iCHLIt hit = HeroFirst(); hit != HeroEnd(); ++hit) 
			if(hit->Proto()->m_protoId == lc.fparam && m_PlayerId == lc.sparam)
				return LC_INVALID;
		return (m_PlayerId == lc.sparam) ? LC_LOSE_HERO : LC_INVALID;	
	case LC_LOSE_CASTLE:
		for (iPlayer::iCCtLIt ctlit = CastleFirst(); ctlit != CastleEnd(); ++ctlit)
			if(ctlit->Pos().x == (sint32) (lc.fparam & 0xFFFF) && (sint32)(ctlit->Pos().y == lc.fparam >> 16))
				return LC_INVALID;
		return (m_PlayerId == lc.sparam) ? LC_LOSE_CASTLE : LC_INVALID;		
	}

	return LC_INVALID;
}
