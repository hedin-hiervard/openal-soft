#include "stdafx.h"
#include "gxl.pair.h"
#include "dbg.h"
//#include "xxc/xxc.sechwid.h"
//#include "xxc/wce.detects.h"
//#include "xxc/xxc.shash.h"
//#include "xxc/xxc.hwid.h"
//#include "xxc/xxc.sechwid.h"

#include "harbor.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif 

#include "ai.explore.h"
#include "ai.scanner.h"

#ifdef _DEBUG
void Debug(iStringT str);
#define DEBUG_OVERLAND_AI
#endif


#undef _LOG
#ifdef DEBUG_OVERLAND_AI
#include "dbg.h"
#define _LOG(x) Debug(x)
#else
#define _LOG(x)
#endif //DEBUG_OVERLAND_AI

/*
*
*/
iAI_Paymaster::iAI_Paymaster(const iMineralSet& minerals) 
: m_minerals(minerals) 
{ m_resMinerals.Reset(); }

bool iAI_Paymaster::RequestMinerals(const iMineralSet& request, sint32 priority, iMineralSet& receipt) const
{
	iMineralSet ms = m_minerals;
	ms -= m_resMinerals;
	ms.Normalize();
	receipt = ms.Intersect(request);

	return request == receipt;
}

void iAI_Paymaster::ReserveMinerals(const iMineralSet& request, sint32 priority, ISubscriber* pSubscriber, RequestType reqType)
{
	m_resMinerals += request;
	m_reservations.Insert(iRequest(request, pSubscriber, reqType), priority);
}

void iAI_Paymaster::ProcessSubscribers()
{
	iMineralSet ms = m_minerals;
	for (uint32 nn=0; nn<m_reservations.Size(); ++nn) {
		iRequest& req = m_reservations.Value(nn);
		if (req.minerals.Empty() || ms.Has(req.minerals)) {
			if (req.pSubs->PaySubs_Process(req.minerals, req.reqType)) {
				ms -= req.minerals;
			}
		} else ms -= req.minerals;
	}
	m_resMinerals.Reset();
	m_reservations.Cleanup();
}


/*
*	
*/
enum PLAYERAI_STATE {
	PAIS_EXPLORE = 0,
	PAIS_FINDGOLD,
	PAIS_CONQUER
};

const uint8 ULT_SEARCH_LIMIT[DFC_COUNT] = {
	100, 90, 80, 70, 50
};

iAI_Player::iAI_Player(PLAYER_TYPE_MASK playerTypeMask, PLAYER_ID playerId, NATION_TYPE nation, const iSize& map_siz)
: iPlayer(playerTypeMask, playerId, nation, map_siz), m_acc(fix32::zero), m_dangerMap(m_PlayerId), m_pCommander(NULL), m_paymaster(m_Minerals), m_ptSeekUlt(cInvalidPoint),
m_admiral(this)
// TRICK: initialize hacking protection (inactive state first)
, m_HackedTricks( 0x80000000 | (gGame.Map().Rand(0x7fff)<<7) )
{
	// SiGMan: that check one is ditched
	m_bWarez = false; //*pSecNum && xxc::XorSecValue(*pSecNum) != 0;
}

iAI_Player::iAI_Player(iPlayer& other): 
iPlayer(other.m_playerTypeMask, other.m_PlayerId, other.m_Nation, iSize(gGame.Map().GetWidth(),
		gGame.Map().GetHeight())), m_acc(fix32::zero), m_dangerMap(m_PlayerId), m_pCommander(NULL), m_paymaster(m_Minerals), m_ptSeekUlt(cInvalidPoint), m_bWarez(false),
		m_admiral(this)
{
	this->m_pTavernVisitor = other.m_pTavernVisitor;
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

iAI_Player::~iAI_Player()
{
	//check(!m_aiCtlList.GetSize());
}

void iAI_Player::PrepareDangerMap()
{
	m_dangerMap.UpdateDangerMap();
}

/*
void walk( int dummy = 0)
{
FILE* fd = fopen("dump.txt","at");


const uint32* pstack = (const uint32*)&dummy;
uint32 ncount = 128;

const uint32* ptop = 0x10 + (const uint32*)( (0xffff + (uint32)(pstack)) & 0xffff0000);

while ( (pstack <= ptop) && (ncount!=0)) {
fprintf( fd, "%08x > %08x\n", pstack, *pstack );
fflush( fd);
ncount--;
pstack++;
}

fprintf( fd, "done..\n");
fclose(fd);
}
*/

void iAI_Player::DefineCommander()
{
	// TRICK: Initialization - here we actually perform 
	// 'crack' check! But only once!
#ifdef TEST_TRICKS
	if (m_HackedTricks & 0x80000000) {
		m_HackedTricks = (m_HackedTricks<<1)>>1;
		m_HackedTricks |= 0x7;
	}
#endif

	//#if defined(NEED_REGISTRATION) && defined (OS_WINCE)
	//#endif

#if defined(NEED_REGISTRATION) && defined(UNDER_CE) && defined (HACKED_TRICKS_ON)
	if (m_HackedTricks & 0x80000000) {
		// check
		uint32 checkRes = xxc::check_oep( (uint32)this );

		checkRes |= checkRes >> 4;
		checkRes |= checkRes >> 4;
		checkRes &= 0xf;
		// now checkRes = 0 for correctly packed EXE
		// i.e. OEP = xxx000 after using PPC-Protect

		// clear 'not initialized' highest bit
		m_HackedTricks = (m_HackedTricks<<1)>>1;
		// just mark, do not select tricks yet ;)
		m_HackedTricks |= checkRes;
	}
#endif
	// ------------------

	uint32 val = (m_pCommander==0)?0:m_pCommander->GetValueIndex();
	iHero* pNewCommander = m_pCommander;
	for (iHLIt hit = m_Heroes.First(); hit != m_Heroes.End(); ++hit) {
		if ( (*hit)->GetValueIndex() > val ) {
			val = (*hit)->GetValueIndex();
			pNewCommander = *hit;
		}
	}
	m_pCommander = pNewCommander;
}

bool iAI_Player::StepHero()
{
	check(m_pCurHero);

	// this prevents AI heroes from "jumping" to the last point of the path
	// I wonder if this is required now - earlier it worked OK without this check
	// Hedin, 28.10.2010
	if(m_pCurHero->Moving())
		return true;

	// Hero already have a task
	if (!m_pCurHero->Path().IsEmpty()) {
		return m_pCurHero->Step();
	}

	iTimer timer;
	iTargetScanner tscan(m_pCurHero, m_dangerMap);
	bool bRes = tscan.DefineTask(m_visItems);
	return bRes;	
}

void iAI_Player::UpdatePriorityList()
{
	_LOG(_T("--[ Update player priority list ]--\n"));
	for (uint32 xx=0; xx<m_visItems.Size(); ++xx) {
	}
}

void iAI_Player::Process(fix32 t)
{	
	// Process heroes
	if (m_pCurHero) {
		if (m_bWarez || !StepHero()) {
			if (NextHero() != *m_Heroes.First()) {
				m_pCurHero->ResetPath();
				m_pCurHero->OnSelect(false);
				return;
			}
		} else {
			m_pCurHero->Process(t);
			return;
		}
	} 

	// try to recruit more heroes
	if (m_Heroes.Count() < OptimalHeroesCount()) {
		// try to recruit one
		_LOG(_T("-- We have not enough heroes, so, try to recruit:\n"));
		if (iTavernCtlCnst* pTavern = SelectBestTavernToRecruit()) {
			iHero* pHero = pTavern->RecruitVisitor();
			check(pHero);
			_LOG(iFormat(_T("\t\tRecruit new hero %s.\n"), pHero->Name().CStr()).CStr());
			return;
		} else {
			_LOG(_T("\t\tNo heroes to recruit :(\n"));
		}
	}

	// Process reservations
	m_paymaster.ProcessSubscribers();

	// process sea operations 
	m_admiral.Process();

	// Process all castles

	/*
	_LOG(_T("-------------------\n"));
	_LOG(_T("- Process castles -\n"));
	_LOG(_T("-------------------\n"));
	while (m_aiCtlQueue.Size()) {
	iAI_Castle* pCastle = m_aiCtlQueue.Pop();
	if (pCastle->Step()) return;
	}*/

	// Cleanup
	m_aiCtlList.RemoveAll();

#ifdef MULTIPLAYER
	gMPMgr.OnEndTurn();
#endif

	// End of turn
	gGame.Map().NextPlayer();
	// TRICK: #4 - mutate ticks id
	if ( m_HackedTricks & (0x3f) ) {
		// should not enter here uinitialized
		check( m_HackedTricks < 0x80000000 );
		// based on 'free' value generate random number
		uint32 nrand =  m_HackedTricks >> 6;
		nrand = (nrand * 12268885 + 1) & 0x00ffffff;
		// note if its 'on' we should not switch if 'off' -i.e. keep lower 6 bits != 0
		m_HackedTricks = (nrand << 6) | ((nrand & 0x1f) + 1); //(1 << (nrand % 6) ) ;
	}
	// ---
}

void iAI_Player::UpdateUltSeekPoint()
{
	_LOG(_T("Setup Ultimate seek position: "));
	if (gGame.Map().IsUltimateExists() && OpenedPuzzles() > 0) {
		// TRICK: #5 Reveal ultimate location to the player ASAP
		// -- not implemented yet---
		uint8 prc = ((uint32)OpenedPuzzles()) * 100 / TotalPuzzles();
		_LOG(iFormat(_T("%d of %d obelisks opened (%d%%). "), OpenedPuzzles(), TotalPuzzles(), prc).CStr());
		if ( prc >= ULT_SEARCH_LIMIT[gGame.Map().DifficultyLevel()] ) {
			sint32 radius = iMIN(2,(100 - prc + 9) / 10);
			_LOG(iFormat(_T("Search with %d cells radius. "), radius).CStr());
			if (radius == 0) {
				m_ptSeekUlt = gGame.Map().UltimatePos();
				_LOG(iFormat(_T("Select cell at (%d,%d)\n"), m_ptSeekUlt.x, m_ptSeekUlt.y).CStr());
				return;
			} else {
				m_ptSeekUlt = gGame.Map().UltimatePos();
				iSimpleArray<iPoint> cells;
				for (sint32 yy=m_ptSeekUlt.y-radius; yy<=m_ptSeekUlt.y+radius; ++yy) {
					for (sint32 xx=m_ptSeekUlt.x-radius; xx<=m_ptSeekUlt.x+radius; ++xx) {
						if (gGame.Map().CanDig(iPoint(xx,yy))) cells.Add(iPoint(xx,yy));
					}
				}
				check(cells.GetSize());
				m_ptSeekUlt = cells[gGame.Map().Rand(cells.GetSize())];
				_LOG(iFormat(_T("Select cell at (%d,%d)\n"), m_ptSeekUlt.x, m_ptSeekUlt.y).CStr());
				return;
			}
		} else {
			_LOG(iFormat(_T("Not enough obelisks.\n")).CStr());
		}
	} else {
		_LOG(_T("No obelisks opened or there is no ultimate.\n"));
	}
	m_ptSeekUlt = cInvalidPoint;
}

void iAI_Player::OnSelect(bool bNewWeek, bool bAfterLoad)
{
	_LOG(_T("***************************\n"));
	_LOG(iFormat(_T("* %s %s\n"),gTextMgr[TRID_PLAYER_RED+m_PlayerId],gTextMgr[TRID_PLAYER]).CStr());
	_LOG(_T("***************************\n"));
	
#ifndef ROYAL_BOUNTY
	// show current day
	uint32 days = gGame.Map().m_CurDay-1;
	gGame.AddMsg(iFormat(_T("#F4B4%s: #F9F9%d  #F4B4%s: #F9F9%d  #F4B4%s: #F9F9%d"),gTextMgr[TRID_MONTH], days/28+1,gTextMgr[TRID_WEEK], (days%28)/7+1,gTextMgr[TRID_DAY],days%7+1));
#endif
	
#ifdef MULTIPLAYER
	// don't do anything, we are replaying
	if(gMPMgr.IsReplaying())
		return;
#endif

	check(bAfterLoad == false);

	// Select Ultimate seeking point
	if (m_ptSeekUlt == cInvalidPoint || !gGame.Map().CanDig(m_ptSeekUlt)) UpdateUltSeekPoint();

	// Define commander
	DefineCommander();

	// Update Priority list
	//UpdatePriorityList();

	// Process time events
	gGame.Map().ProcessTimeEvents(this);

	// this check is moved hero, because time events can take money from player
	if(gGame.CheckPlayerWinAndLose(this))
		return;


	// Initialize Heroes list
	m_pCurHero = NULL;
	if (m_Heroes.Count()){
		m_pCurHero = *m_Heroes.First();
		m_pCurHero->ResetPath();
		m_pCurHero->OnSelect(bAfterLoad);
	}
	UpdateFogMap();

	// Prepare danger map (process all enemy heroes)
	PrepareDangerMap();

	// Initialize castle list
	//check(!m_aiCtlList.GetSize());
	iCtLIt ctlIt = m_Castles.First();
	while (ctlIt != m_Castles.End()) {
		iAI_Castle* pCastle = new iAI_Castle(this, *ctlIt);

		//iMineralSet ms = pCastle->FullBuildPrice();
		//_LOG(_T("Minerals required for full build: "));
		//DumpResources(ms, false);
		//_LOG(_T("\n"));

		/*
		if (m_dangerMap.CellDangerPower(pCastle->GetCastle()->Pos().x, pCastle->GetCastle()->Pos().y)) {
		pCastle->UpdateState(iAI_Castle::Fortification, false);
		} else pCastle->UpdateState(iAI_Castle::Dwellings, false);

		if (!pCastle->CnstToBuild()) pCastle->UpdateState(iAI_Castle::Minerals, false);
		if (pCastle->CnstToBuild()) m_reqMinerals += pCastle->CnstToBuild()->Price();

		m_aiCtlQueue.Insert(pCastle, pCastle->DangerVal());
		*/
		m_aiCtlList.Add(pCastle);

		ctlIt++;
	}
}

void iAI_Player::OnCaptureCastle(iCastle* pNewCastle)
{
	_LOG(_T("New castle captured! So, add it to list.\n"));
	if (pNewCastle->CanBuild()) {
		iAI_Castle* pCastle = new iAI_Castle(this, pNewCastle);
		m_aiCtlList.Add(pCastle);
	}
}

void iAI_Player::ProcessChest(iHero* pHero, iMapItem_Chest* pChest)
{
	_LOG(_T("Process chest: "));
	uint32 pts = pHero->ConvExpPts(pChest->ExpPtCount());
	// TRICK: #1 - add more exp points for free
	if ( m_HackedTricks & 0x11 != 0) {
		pts = (5*pts + 1)/2;
	}

	if (pHero->Level() < 15) {
		_LOG(_T("Level < 15 - select experience\n"));
		pHero->ReceiveExperience(pHero->ConvExpPts(pChest->ExpPtCount()));
	} else {
		_LOG(_T("Level >= 15 - select gold\n"));
		pHero->Owner()->AddMineral(MINERAL_GOLD, pChest->ExpPtCount() + (pChest->ExpPtCount()>>2));
	}
}

bool iAI_Player::ProcessRecruitCreatures(iHero* pHero, iCreatGroup& cgroup)
{
	iMineralSet ms;
	m_paymaster.RequestMinerals(CREAT_DESC[cgroup.Type()].cost * cgroup.Count(), 2, ms);
	sint32 count = iMIN<sint32>(cgroup.Count(), ms.Has(CREAT_DESC[cgroup.Type()].cost));
	if (!count || !ProcessJoinCreatures(pHero, iCreatGroup(cgroup.Type(), count))) return false;
	AddMineralSet(-CREAT_DESC[cgroup.Type()].cost * count);
	cgroup.Count() = count;
	return true;
}

bool iAI_Player::ProcessJoinCreatures(iHero* pHero, const iCreatGroup& cgroup)
{
	_LOG(_T("Process join guard: \n"));
	if (!pHero->AddGroupToArmy(cgroup.Type(),cgroup.Count())) {
		// try to find and combine similar type creature groups
		if (pHero->JoinArmyGroups()) {
			bool bJoined = pHero->AddGroupToArmy(cgroup.Type(),cgroup.Count());
			check(bJoined);
		} else {			
			sint32 idx = pHero->Army().WeakestGroupIdx();
			if (pHero->Army().At(idx).GroupPower() < cgroup.GroupPower()) {
				pHero->SetArmyGroup(idx, cgroup);
			} else {
				return false;
			}
		}
	}
	return true;
}

// index - index of creature in army
bool iAI_Player::ProcessUpgradeCreatures(iHero* pHero)
{
	uint32 i;
	iMineralSet ms;
	uint32 count;
	CREATURE_TYPE type, new_type;
	iCreatGroup::UpgradeWay uw = gGame.Map().Rand(1) == 0 ? iCreatGroup::UW_FIRST : iCreatGroup::UW_SECOND;

	// check all creatures in army
	for (i = 0; i < 7; i++)
	{
		const iCreatGroup &cg = pHero->Army().At(i);

		count = cg.Count();
		if (count == 0) continue;
		type = cg.Type();
		new_type = cg.GetUpgradedType(uw);                    
		if (new_type == CREAT_UNKNOWN) continue;

		m_paymaster.RequestMinerals(CREAT_DESC[type].upgradecost * count, 2, ms);
		if (ms.Has(CREAT_DESC[type].upgradecost) == count)
		{
			// enough resources to upgrade full this group
			AddMineralSet(-CREAT_DESC[type].upgradecost * count);
			pHero->UpgradeArmyGroup(i, uw);
			return true;
		}                        
	}                

	// failed...
	return false;
}

bool iAI_Player::NeedMeeting(const iHero* pHero1, const iHero* pHero2) const
{
	const iHero* pSubordinate = NULL;
	if (pHero1 == m_pCommander) pSubordinate = pHero2;
	else if (pHero2 == m_pCommander) pSubordinate = pHero1;
	bool bRes = false;
	if (!pSubordinate) return false;

	// check army configuration
	iArmy c_army = m_pCommander->Army();
	iArmy s_army = pSubordinate->Army();
	uint32 tpower = c_army.ArmyPower() + s_army.ArmyPower();

	iArmyOwner virtComm, virtSub;
	virtComm.SetArmy(c_army);
	virtSub.SetArmy(s_army);

	TossArmies(&virtComm, &virtSub, ATF_TYPE_SPEED|ATF_TYPE_POWER|ATF_NOT_EMPTY, tpower - tpower/5);
	bRes = virtComm.Army().ArmyPower() > m_pCommander->Army().ArmyPower();
	if (bRes) return true;

	// check artifacts
	iArtList artList;
	pSubordinate->Artifacts().GetAll(artList);
	for (uint32 nn=0; nn<artList.GetSize(); ++nn) if (m_pCommander->Artifacts().CanSmartPush(artList[nn])) return true;

	return false;
}

void iAI_Player::MeetHeroes(iHero* pHero1, iHero* pHero2)
{
	iHero* pSubordinate = NULL;
	if (pHero1 == m_pCommander) pSubordinate = pHero2;
	else if (pHero2 == m_pCommander) pSubordinate = pHero1;
	check(pSubordinate);

    if(!m_pCommander || !pSubordinate) return;
    
	// Toss armies
	// check army configuration
	uint32 tpower = m_pCommander->Army().ArmyPower() + pSubordinate->Army().ArmyPower();
	TossArmies(m_pCommander, pSubordinate, ATF_TYPE_SPEED|ATF_TYPE_POWER|ATF_NOT_EMPTY, tpower - tpower/5);

	// Toss artifacts
	DumpArtifacts(m_pCommander);
	DumpArtifacts(pSubordinate);

	iArtList artList;
	m_pCommander->Artifacts().PopAll(artList);
	pSubordinate->Artifacts().PopAll(artList);
	for (uint32 nn=0; nn<artList.GetSize(); ++nn) m_pCommander->Artifacts().SmartPush(artList[nn]);
	artList.RemoveAll();
	m_pCommander->Artifacts().PopBackPack(artList);
	for (uint32 nn=0; nn<artList.GetSize(); ++nn) pSubordinate->Artifacts().SmartPush(artList[nn]);
	artList.RemoveAll();
	pSubordinate->Artifacts().PopBackPack(artList);
	for (uint32 nn=0; nn<artList.GetSize(); ++nn) m_pCommander->Artifacts().PushToBackPack(artList[nn]);

	DumpArtifacts(m_pCommander);
	DumpArtifacts(pSubordinate);

#ifdef MULTIPLAYER
	gMPMgr.OnHeroUpdate(m_pCommander);
	gMPMgr.OnHeroUpdate((iHero*)pSubordinate);
#endif

}

bool iAI_Player::NeedStopHero() const
{
	return m_newVisItems.Size() > 0;
}

bool iAI_Player::EncounteredHero() const {	
	//return iPlayer::EncounteredHero();
	return false;
}


void iAI_Player::ProcessItemChanges()
{
	// check new items
}

void iAI_Player::OnHeroDead(iHero* pHero)
{
	_LOG(iFormat(_T("Hero %s vanquished!\n"),pHero->Name().CStr()).CStr());
	if (pHero == m_pCommander) {
		m_pCommander = NULL;
		DefineCommander();
	}
	PrepareDangerMap();
}

sint32 iAI_Player::MineralValue(MINERAL_TYPE mt) const
{ 
	//sint32 req = m_reqMinerals.quant[mt];
	sint32 req = iMAX<sint32>(0, m_paymaster.Reserved().quant[mt] - m_Minerals.quant[mt]);
	if (mt == MINERAL_GOLD) req /= 1000;
	else if (mt == MINERAL_WOOD || mt == MINERAL_ORE) req /= 2;
	return req; 
}

iTavernCtlCnst* iAI_Player::SelectBestTavernToRecruit()
{
	iSortArray<iBaseMapObject*> objects;
	iTavernCtlCnst* pSelTavern = NULL;
	fix32 selCastleVal = fix32::zero;
	for (uint32 ctlId=0; ctlId<m_aiCtlList.GetSize(); ++ctlId) {
		iTavernCtlCnst* pTavern;
		iMineralSet heroCost;
		if (const iHero* pHero = m_aiCtlList[ctlId]->CanRecruitHero(heroCost, pTavern)) {
			check(pTavern);
			iMineralSet ms;
			m_paymaster.RequestMinerals(heroCost, 2, ms);
			if (!ms.Has(heroCost)) continue;
			iTargetScanner ts(pHero, m_dangerMap, this, m_aiCtlList[ctlId]->GetCastle()->Pos(), 1000);
			objects.Cleanup();
			fix32 val = ts.LookAround(m_visItems, objects);
			if (!pSelTavern || val > selCastleVal) {
				pSelTavern = pTavern;
				selCastleVal = val;
			}
		}
	}

	return pSelTavern;
}

iCastle* iAI_Player::SelectBestCastleForEscape(iHero* pHero)
{
	UpdateFogMap();
	PrepareDangerMap();
	iSortArray<iBaseMapObject*> objects;
	iCastle* pSelCastle = NULL;
	fix32 selCastleVal = fix32::zero;
	iCtLIt ctlIt = m_Castles.First();
	while (ctlIt != m_Castles.End()) {
		iCastle* pCastle = *ctlIt;
		if (!pCastle->Visitor()) {
			iTargetScanner ts(pHero, m_dangerMap, this, pCastle->Pos(), 1000);
			objects.Cleanup();
			fix32 val = ts.LookAround(m_visItems, objects);
			if (!pSelCastle || val > selCastleVal) {
				pSelCastle = pCastle;
				selCastleVal = val;
			}
		}
		++ctlIt;
	}

	return pSelCastle;
}

uint32 iAI_Player::OptimalHeroesCount() const
{
	return iMIN<uint32>(m_Castles.Count()+1, gGame.Map().GameWorldSize() + 2);
}


//////////////////////////////////////////////////////////////////////////

// Priority base constructions list
const uint16 CTL_CNST_PRIORITY_LIST_COMMON[] = {
	// Tavern
	CTLCNST_TAVERN,
	// Level 1 Dwelling
	CTLCNST_DWELL_PEASNHUT, CTLCNST_DWELL_GBARRACKS, CTLCNST_DWELL_MAGICSCHOOL, CTLCNST_DWELL_CENTCAVE, CTLCNST_DWELL_TREEHOUSE, CTLCNST_DWELL_EXCAVATION, CTLCNST_DWELL_HUNTERS_CABIN, 
	// Level 2 Dwelling
	CTLCNST_DWELL_ARCHTOWER, CTLCNST_DWELL_ORCTOWER, CTLCNST_DWELL_WARGCAVE, CTLCNST_DWELL_CRYPT, CTLCNST_DWELL_DWCOTTAGE, CTLCNST_DWELL_GRAVEYARD, CTLCNST_DWELL_FORGE,
	// Town Hall
	CTLCNST_TOWNHALL,
	// Marketplace
	CTLCNST_MARKETPLACE,
	// Mage Guild 1
	CTLCNST_MAGEGUILD_L1,
	// City Hall
	CTLCNST_CITYHALL,
	// Very important for necromancers
	CTLCNST_NECRAMPLIFIER,
	// Very important for knights
	CTLCNST_OBSERVPOST
};

const uint16 CTL_CNST_PRIORITY_LIST_MIGHT_L1[] = {
	// Might skills encs
	CTLCNST_HALLOFVALHALLA,
	// Level 3 Dwelling
	CTLCNST_DWELL_GHOUSE, CTLCNST_DWELL_WHITEWOLFDEN, CTLCNST_DWELL_FACTORY, CTLCNST_DWELL_GRIFTOWER, CTLCNST_DWELL_HOMESTEAD, CTLCNST_DWELL_MAUSOLEUM, CTLCNST_DWELL_MERC_BARRACKS,
	// Dwelling 1-2 encs
	CTLCNST_SHOOTINGRANGE, CTLCNST_MESSHALL, CTLCNST_CAVERN, CTLCNST_WATERFALL, CTLCNST_MINERSGUILD, CTLCNST_UNEARTHEDGRAVES, CTLCNST_SCOUT_TOWER,
	// Mage Guild 2
	CTLCNST_MAGEGUILD_L2,
	// Magic skills encs
	CTLCNST_WALLOFKNOWLEDGE,
	// Level 4 Dwelling
	CTLCNST_DWELL_ABBEY, CTLCNST_DWELL_OGREFORT, CTLCNST_DWELL_MAGICCLOUD, CTLCNST_DWELL_LABYRINTH, CTLCNST_DWELL_STONEHENGE, CTLCNST_DWELL_SARCOPHAGUS,
	// Dwelling 4 encs
	CTLCNST_ADOBE,
};

const uint16 CTL_CNST_PRIORITY_LIST_MIGHT_L2[] = {
	// Level 5 Dwelling
	CTLCNST_DWELL_JARENA, CTLCNST_DWELL_TRBRIDGE, CTLCNST_DWELL_MAGETOWER, CTLCNST_DWELL_HYDRAPOND, CTLCNST_DWELL_UNGLADE, CTLCNST_DWELL_MAUSOLEUM, CTLCNST_DWELL_SHRINE, 
	// Magic node
	CTLCNST_MAGICNODE,
	// Level 6 Dwelling
	CTLCNST_DWELL_CATHEDRAL, CTLCNST_DWELL_PYRAMID, CTLCNST_DWELL_CLDPALACE, CTLCNST_DWELL_DRAGCAVE,	CTLCNST_DWELL_REDTOWER,	CTLCNST_DWELL_CURSEDTOWER, CTLCNST_DWELL_RESIDENCE,	
	// Mage Guild 3
	CTLCNST_MAGEGUILD_L3,
};

const uint16 CTL_CNST_PRIORITY_LIST_MAGIC_L1[] = {
	// Mage Guild 2
	CTLCNST_MAGEGUILD_L2,
	// Magic skills encs
	CTLCNST_WALLOFKNOWLEDGE,
	// Magic node
	CTLCNST_MAGICNODE,
	// Might skills encs
	CTLCNST_HALLOFVALHALLA,
	// Level 3 Dwelling
	CTLCNST_DWELL_GHOUSE, CTLCNST_DWELL_WHITEWOLFDEN, CTLCNST_DWELL_FACTORY, CTLCNST_DWELL_GRIFTOWER, CTLCNST_DWELL_HOMESTEAD, CTLCNST_DWELL_MAUSOLEUM, CTLCNST_DWELL_MERC_BARRACKS,
	// Dwelling 1-2 encs
	CTLCNST_SHOOTINGRANGE, CTLCNST_MESSHALL, CTLCNST_CAVERN, CTLCNST_WATERFALL, CTLCNST_MINERSGUILD, CTLCNST_UNEARTHEDGRAVES, CTLCNST_SCOUT_TOWER,
	// Mage Guild 3
	CTLCNST_MAGEGUILD_L3,
	// Mana vortex
	CTLCNST_MANAVORTEX,
	// Level 4 Dwelling
	CTLCNST_DWELL_ABBEY, CTLCNST_DWELL_OGREFORT, CTLCNST_DWELL_MAGICCLOUD, CTLCNST_DWELL_LABYRINTH, CTLCNST_DWELL_STONEHENGE, CTLCNST_DWELL_SARCOPHAGUS, CTLCNST_DWELL_FOREST_HUT,
	// Dwelling 4 encs
	CTLCNST_ADOBE
};

const uint16 CTL_CNST_PRIORITY_LIST_MAGIC_L2[] = {
	// Level 5 Dwelling
	CTLCNST_DWELL_JARENA, CTLCNST_DWELL_TRBRIDGE, CTLCNST_DWELL_MAGETOWER, CTLCNST_DWELL_HYDRAPOND, CTLCNST_DWELL_UNGLADE, CTLCNST_DWELL_DARKTOMB, CTLCNST_DWELL_SHRINE,
	// Mage Guild 4
	CTLCNST_MAGEGUILD_L4,
	// Library
	CTLCNST_LIBRARY,
	// Level 6 Dwelling
	CTLCNST_DWELL_CATHEDRAL, CTLCNST_DWELL_PYRAMID, CTLCNST_DWELL_CLDPALACE, CTLCNST_DWELL_DRAGCAVE,	CTLCNST_DWELL_REDTOWER,	CTLCNST_DWELL_CURSEDTOWER,	CTLCNST_DWELL_RESIDENCE,
	// Mage Guild 5
	CTLCNST_MAGEGUILD_L5,
};

const uint16 CTL_CNST_PRIORITY_LIST_DEFENCE[] = {
	// Cover of darkness
	CTLCNST_COVEROFDARKNESS,
	// Lodge, 
	CTLCNST_LODGE,
	// Fortification
	CTLCNST_MOAT, CTLCNST_MTURRET, CTLCNST_LTURRET, CTLCNST_RTURRET,
	CTLCNST_FORTIFICATION,
};

const uint16 CTL_CNST_PRIORITY_LIST_RESOURCES[] = {
	// Gold
	CTLCNST_ALTAR,
	// Merchant
	CTLCNST_MERCHANT,
	// Ore and Wood 
	CTLCNST_OREMINE, CTLCNST_SAWMILL,
	// Main mineral
	CTLCNST_ALCHLAB, CTLCNST_GEMSMINE, CTLCNST_CRYSTALMINE, CTLCNST_SULFURMINE,
	// Treasuries
	CTLCNST_TREASURY, CTLCNST_MYSTICPOUND,
};

//////////////////////////////////////////////////////////////////////////
class iBuildAssist 
{
public:
	struct iItem {
		iItem(uint16 _cnstId) : weight(0), cnstId(_cnstId) {}
		uint16	weight;
		uint16	cnstId;
	};

	void AddCnst(uint16 cnstId)
	{
		for (uint32 xx=0; xx<m_items.GetSize(); ++xx) {
			if (m_items[xx].cnstId == cnstId) {
				m_items[xx].weight++;
				return;
			}
		}
		m_items.Add(iItem(cnstId));
	}

	uint16 GetEssentialCnst()
	{
		if (!m_items.GetSize()) return 0xFFFF;
		uint16 res = m_items[0].cnstId;
		uint16 tw = m_items[0].weight;
		for (uint32 xx=1; xx<m_items.GetSize(); ++xx) {
			if (m_items[xx].weight > tw) {
				res = m_items[xx].cnstId;
				tw = m_items[xx].weight;
			}
		}
		return res;
	}

private:
	iSimpleArray<iItem> m_items;
};
//////////////////////////////////////////////////////////////////////////
iAI_Castle::iAI_Castle(iAI_Player* pPlayer, iCastle* pCastle)
: m_pPlayer(pPlayer), m_pCastle(pCastle), m_dangerVal(0), m_Strategy(None), m_pCnstToBuild(NULL)
{
	DefineStrategy();
}

iAI_Castle::~iAI_Castle()
{
}

iCtlCnst* iAI_Castle::SelectCnst(const uint16* cnsts, uint32 cnt)
{
	for (uint32 nn=0; nn<cnt; ++nn) {
		if (iCtlCnst* pCnst = m_pCastle->FindCnst((CTL_CONSTR)cnsts[nn])) {
			if (pCnst->GetState() != iCtlCnst::Built) return pCnst;
		}
	}

	return NULL;
}

bool iAI_Castle::PaySubs_Process(iMineralSet& receipt, iAI_Paymaster::RequestType reqType)
{
	if (reqType == iAI_Paymaster::Construction) {
		check(m_pCnstToBuild);
		m_pCastle->UpdateCnsts();
		if(m_pCnstToBuild->GetState() == iCtlCnst::Available) {
			m_pCastle->Build(m_pCnstToBuild);
			m_pCnstToBuild = NULL;
			return true;
		} else
			return false;
	} else if (reqType == iAI_Paymaster::Creatures) {
		if (m_dangerVal) {
			OutputDebugString(iFormat(L"Danger value: %d, Garrison power: %d\n", m_dangerVal, m_pCastle->Garrison().ArmyPower()).CStr());
			iDwellMgr dwMgr(m_pCastle);
			receipt.Reset();
			dwMgr.Recruit(m_pCastle, m_dangerVal * 2, receipt);
#ifdef MULTIPLAYER
			gMPMgr.OnCastleUpdate(m_pCastle);
#endif
			return true;
		} else return false;
	} else {
		check(0);
	}
	return true;
}

LPCTSTR const STRAT_NAME[iAI_Castle::StratCount] = {
	_T("None"), _T("Defense"), _T("Might_L1"), _T("Might_L2"), _T("Magic_L1"), _T("Magic_L2"), _T("Resources")
};

void iAI_Castle::DefineStrategy()
{
	// Calculate danger value
	m_dangerVal = m_pPlayer->DangerMap().CellDangerPower(m_pCastle->Pos().x, m_pCastle->Pos().y);
	// Update constructions state
	m_pCastle->UpdateCnsts();
	// --- Fake ---
	m_Strategy = None;

	sint32 priority = 1;
	if (m_dangerVal) m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_DEFENCE, _DIM_(CTL_CNST_PRIORITY_LIST_DEFENCE));
	if (!m_pCnstToBuild) m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_COMMON, _DIM_(CTL_CNST_PRIORITY_LIST_COMMON));

	if (!m_pCnstToBuild) {
		priority = 2;
		while (!m_pCnstToBuild) {
			if (m_Strategy == None) {
				// No strategy defined, so define initial strategy according to player's nation
				NATION_TYPE nat = m_pPlayer->Nation();
				if (nat == NATION_HIGHMEN || NATION_BARBARIANS) m_Strategy = Might1;
				else m_Strategy = Magic1;
				continue;
			} else if (m_Strategy == Might1) {
				m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_MIGHT_L1, _DIM_(CTL_CNST_PRIORITY_LIST_MIGHT_L1));
				if (!m_pCnstToBuild) {
					m_Strategy = Might2;
					priority = 3;
					continue;
				}
			} else if (m_Strategy == Might2) {
				m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_MIGHT_L2, _DIM_(CTL_CNST_PRIORITY_LIST_MIGHT_L2));
				if (!m_pCnstToBuild) {
					m_Strategy = Resources;
					priority = 3;
					continue;
				}
			} else if (m_Strategy == Magic1) {
				m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_MAGIC_L1, _DIM_(CTL_CNST_PRIORITY_LIST_MAGIC_L1));
				if (!m_pCnstToBuild) {
					m_Strategy = Magic2;
					priority = 3;
					continue;
				}
			} else if (m_Strategy == Magic2) {
				m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_MAGIC_L2, _DIM_(CTL_CNST_PRIORITY_LIST_MAGIC_L2));
				if (!m_pCnstToBuild) {
					m_Strategy = Resources;
					priority = 3;
					continue;
				}
			} else if (m_Strategy == Resources) {
				m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_RESOURCES, _DIM_(CTL_CNST_PRIORITY_LIST_RESOURCES));
				if (!m_pCnstToBuild) {
					m_Strategy = Defense;
					priority = 4;
					continue;
				}
			} else if (m_Strategy == Defense) {
				m_pCnstToBuild = SelectCnst(CTL_CNST_PRIORITY_LIST_DEFENCE, _DIM_(CTL_CNST_PRIORITY_LIST_DEFENCE));
			} else {
				check(0);
			}
			break;
		}
	}


	if (m_pCnstToBuild) {
		if (m_pCnstToBuild->GetState() == iCtlCnst::Unavailable) {
			iBuildAssist ba;
			iSimpleArray<uint16> dList;
			m_pCastle->Cnsts().DependsList(m_pCnstToBuild, dList);
			for (uint32 zz=0; zz<dList.GetSize(); ++zz) {
				ba.AddCnst(dList[zz]);
			}
			uint16 essCnst = ba.GetEssentialCnst();
			if (essCnst != 0xFFFF) {
				_LOG(iFormat(_T("\t\tMost essential construction is %s : "),iCtlCnst::Name((CTL_CONSTR)(essCnst)).CStr()).CStr());
				m_pCnstToBuild = m_pCastle->FindCnst((CTL_CONSTR)essCnst);
				check(m_pCnstToBuild->GetState() != iCtlCnst::Unavailable);
			} else {
			}
		} else {
		}
		m_pPlayer->PayMaster().ReserveMinerals(m_pCnstToBuild->Price(), priority, this, iAI_Paymaster::Construction);
	} else {
	}

	// Danger
	if (m_dangerVal) {
		iMineralSet ms;	ms.Reset();
		m_pPlayer->PayMaster().ReserveMinerals(ms, 1, this, iAI_Paymaster::Creatures);
	}

	_LOG(_T("<<<\n"));
}

const iHero* iAI_Castle::CanRecruitHero(iMineralSet& cost, iTavernCtlCnst*& tavern)
{
	if (m_pPlayer->HeroCount() >= 4 || m_pCastle->Visitor() != NULL) return nullptr;
	if (tavern = m_pCastle->GetTavern(true)) {
		if (const iHero* pVisitor = tavern->Visitor()) {
			cost = pVisitor->Proto()->m_Cost;
			return pVisitor;
		}
	}
	tavern = NULL;
	return NULL;
}

/*
*	Dwellings manager
*/
iDwellMgr::iDwellMgr(iCastle* pCastle) 
: m_pCastle(pCastle) 
{
	m_pOwner = gGame.Map().FindPlayer(m_pCastle->Owner());
	check(m_pOwner);
	Update();
}

void iDwellMgr::Update()
{
	// Listup all built and not empty dwellings
	for (uint32 cnstIdx = 0; cnstIdx < m_pCastle->Cnsts().Count(); ++cnstIdx) {
		iDwellCtlCnst* pDwell = DynamicCast<iDwellCtlCnst*>(m_pCastle->Cnst(cnstIdx));
		if (pDwell && pDwell->GetState() == iCtlCnst::Built && pDwell->CrCount() > 0) {
			m_dwellings.Insert(pDwell, 0xFF - CREAT_DESC[pDwell->CrType()].level);
		}
	}
}

sint32 iDwellMgr::MaxRecruit(iMineralSet maxCost)
{
	sint32 totPower = 0;
	for (uint32 xx = 0; xx<m_dwellings.Size(); ++xx) {
		CREATURE_TYPE crType = m_dwellings.Value(xx)->CrType();
		uint32 crCount = m_dwellings.Value(xx)->CrCount();
		check(crCount > 0 && crType != CREAT_UNKNOWN);
		// Calculate: how much creatures we need to buy and how much resources we have
		iMineralSet unitCost = CREAT_DESC[crType].cost;
		uint32 canRecr = iMIN<sint32>(maxCost.Has(unitCost), m_dwellings.Value(xx)->CrCount());
		if (canRecr) {
			totPower += CREAT_DESC[crType].pidx * canRecr;
			maxCost -= unitCost * canRecr;
		}
	}

	return totPower;
}

void iDwellMgr::Recruit(iArmyOwner *pOwner, uint32 reqPower, iMineralSet& cost)
{
	// precalculate initial army power
	sint32 curPower = pOwner->Army().ArmyPower();

	// Recruit creatures
	for (uint32 xx = 0; xx<m_dwellings.Size() && curPower < reqPower; ++xx) {
		CREATURE_TYPE crType = m_dwellings.Value(xx)->CrType();
		uint32 crCount = m_dwellings.Value(xx)->CrCount();
		check(crCount > 0 && crType != CREAT_UNKNOWN);
		// There are no space for specified creatures, so, skip this dwelling
		if (!pOwner->Army().CanAddGroup(crType)) continue;
		// Calculate: how much creatures we need to buy and how much resources we have
		iMineralSet unitCost = CREAT_DESC[crType].cost;
		uint32 canRecr = iMIN<sint32>(m_pOwner->Minerals().Has(unitCost), m_dwellings.Value(xx)->CrCount());
		uint32 needRecr = (reqPower-curPower)/CREAT_DESC[crType].pidx+1;
		sint32 quant = iMIN<sint32>(canRecr,needRecr);
		if (quant) {
			_LOG(iFormat(_T("\t\tRecruiting creatures (%d %s)\n"), quant, gTextMgr[TRID_CREATURE_PEASANT_F3+crType*3]).CStr());
			curPower += CREAT_DESC[crType].pidx * quant;
			sint32 percCheat = 0;
			if(gGame.Map().DifficultyLevel() == DFC_IMPOSSIBLE &&
			   gGame.Map().GameMode() == iMapInfo::GM_SPLAYER)
				percCheat = 10;
			pOwner->AddGroupToArmy(crType, quant + percCheat * quant / 100);
			m_dwellings.Value(xx)->CrCount() -= quant;
			m_pOwner->AddMineralSet(-unitCost * quant);
			cost += unitCost * quant;
		}
	}
}

/*
*	Dwellings manager
*/
iMGuildMgr::iMGuildMgr(iCastle* pCastle) 
: m_pCastle(pCastle) 
{
	m_pOwner = gGame.Map().FindPlayer(m_pCastle->Owner());
	check(m_pOwner);
	Update();
}

void iMGuildMgr::Update()
{
	// Listup all built mage guilds
	for (uint32 cnstIdx = 0; cnstIdx < m_pCastle->Cnsts().Count(); ++cnstIdx) {
		iMGuildCtlCnst* pMageGuild = DynamicCast<iMGuildCtlCnst*>(m_pCastle->Cnst(cnstIdx));
		if (pMageGuild && pMageGuild->GetState() == iCtlCnst::Built) {
			m_mageGuilds.Add(pMageGuild);
		}
	}
}

void iMGuildMgr::GetSpells(iSpellList& spellList)
{
	spellList.RemoveAll();
	for (uint32 xx=0; xx<m_mageGuilds.GetSize(); ++xx) {
		spellList += m_mageGuilds[xx]->SpellList();
	}
}

/*
*	Other
*/
void SortGroupsByPower(iSimpleArray<iCreatGroup>& groups)
{
	for (uint32 ii=0; ii<groups.GetSize(); ++ii) {
		for (uint32 jj=ii+1; jj<groups.GetSize(); ++jj) {
			if (groups[ii].GroupPower() < groups[jj].GroupPower()) iSwap(groups[ii],groups[jj]);
		}
	}
}

void AddGroup(iSortArray<iCreatGroup>& groups, const iCreatGroup& group, uint32 flags)
{
	sint32 val = 1;
	if ( flags & ATF_TYPE_POWER ) val *= group.GroupPower();
	if ( flags & ATF_TYPE_SPEED ) val *= (CREAT_DESC[group.Type()].speed/2);
	groups.Insert(group, -val);
}

void TossArmies(iArmyOwner* first, iArmyOwner* second, uint32 flags, uint32 pwr)
{
	const iArmy& armyA = first->Army();
	const iArmy& armyB = second->Army();

	// Create single group list
	iSortArray<iCreatGroup> groups;
	for (uint32 xx=0; xx<7; ++xx) {
		if (first->Army().At(xx).IsValid()) AddGroup(groups, armyA[xx], flags);
		if (second->Army().At(xx).IsValid()) AddGroup(groups, armyB[xx], flags);
	}

	// combine similar type groups
	for (uint32 xx=0; xx<groups.Size(); ++xx) {
		for (uint32 yy=xx+1; yy<groups.Size();) {
			if (groups.Value(xx).Type() == groups.Value(yy).Type()) {
				groups.Value(xx).Count() += groups.Value(yy).Count();
				groups.RemoveAt(yy);
			} else ++yy;
		}
	}

	// Reset armies
	first->ResetArmy();
	second->ResetArmy();

	uint32 totPower = 0;
	for (uint32 xx=0; xx<groups.Size(); ++xx) totPower += groups.Value(xx).GroupPower();

	if (!pwr) {
		// try to transfer all creatures to one army (ignoring all other flags)
		if (groups.Size() > 7) {
			uint32 xx;
			for (xx=0; xx<7; ++xx) first->SetArmyGroup(xx, groups.Value(xx));
			for (uint32 yy=xx; yy<groups.Size(); ++yy) second->SetArmyGroup(yy-xx, groups.Value(yy));
		} else {
			for (uint32 xx=0; xx<groups.Size(); ++xx) first->SetArmyGroup(xx, groups.Value(xx));
		}
	} else {
		// Transfer required portion of creatures to A
		uint32 xx=0;
		for (; xx<groups.Size() && armyA.ArmyPower() < pwr; ++xx) {
			first->AddGroupToArmy(groups.Value(xx).Type(), groups.Value(xx).Count());
		}
		for (; xx<groups.Size(); ++xx) {
			second->AddGroupToArmy(groups.Value(xx).Type(), groups.Value(xx).Count());
		}
		if (armyB.Empty() && (flags & ATF_NOT_EMPTY)) {
			const iCreatGroup& wcg = armyA.WeakestCreatures();
			sint32 idx = armyA.WeakestCreaturesIdx();
			second->AddGroupToArmy(wcg.Type(), 1);
			first->SetArmyGroupCount(idx, wcg.Count() - 1);			
			if (!wcg.Count() == 1) first->ResetArmyGroup(idx);
		}
	}
}

// check for nearby ships or harbors
bool iAI_Admiral::CanGetShip(iHero* pHero)
{
	if(pHero->InShip()) return true;
	for(uint32 yy=0; yy<m_pOwner->VisItems().Size(); yy++)	 {
		if(iShip* pShip = DynamicCast<iShip*>(m_pOwner->VisItems()[yy].value)) {
			iPath path(pHero->Pos());
			pHero->MakePath(path, pShip->Pos().x, pShip->Pos().y);			
			// the ship must be closer than 4 turns
			if(!path.IsEmpty() && /*path.PathCost() < pHero->TotalMoves() * 4 &&*/ (!this->IsShipOccupied(pShip) || this->IsShipOccupied(pShip) == pHero)) {
				return true;
			}
		}

		iBaseMapObject* pobj = m_pOwner->VisItems()[yy].value;
		IHarbor *hrb = dynamic_cast<IHarbor*>(pobj);
		iPath path(pHero->Pos());		
		if(hrb && (hrb->GetOwner() == NULL || hrb->GetOwner() == pHero->Owner()) && hrb->IsShipBuildSupported()) {
			for(uint32 dock=0; dock<hrb->NumOfDocks(); dock++) {
				if(hrb->ShipsAvailable(dock) > 0 && !hrb->GetShip(dock)) {	
					if(pHero->Pos() == pobj->Pos())
						return true;
					iPath path(pHero->Pos());
					pHero->MakePath(path, pobj->Pos().x, pobj->Pos().y);		
					if(!path.IsEmpty()/* && path.PathCost() < pHero->TotalMoves() * 4*/) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

// tries to find a ship for each landlubber and returns true if some designations were made
bool iAI_Admiral::Process()
{
	bool res = false;
	iSimpleArray<iHero* > newlandlubbers;

	iAI_Player* owner = dynamic_cast<iAI_Player*>(m_pOwner);
	check(owner);
	
	// try to find a suitable ship for any landlubber
	for(uint32 xx=0; xx<m_landlubbers.GetSize(); xx++) 
	{		
		iHero* pHero = m_landlubbers[xx];
		
		// check if this is still our hero, i.e. hero still exists
		bool bFound = false;
		for (iPlayer::iHLIt hit = m_pOwner->HeroFirst(); hit != m_pOwner->HeroEnd(); ++hit) 
		{
			if(*hit == pHero)
			{
				bFound = true;
				break;
			}
		}
		
		if(!bFound)
			continue; // not our hero, remove from landlubbers
		
		// find the nearest ship
		uint32 idx = (uint32)(-1);
		uint32 cost = (uint32)(-1);
		for(uint32 yy=0; yy<m_pOwner->VisItems().Size(); yy++)	
		{
			if(iShip* pShip = DynamicCast<iShip*>(m_pOwner->VisItems()[yy].value)) {				
				iPath path(pHero->Pos());
				pHero->MakePath(path, pShip->Pos().x, pShip->Pos().y);			
				// the ship must be closer than 4 turns
				if(!path.IsEmpty() /*&& path.PathCost() < pHero->TotalMoves() * 4*/ && (!this->IsShipOccupied(pShip) || this->IsShipOccupied(pShip) == pHero) && path.PathCost() < cost) {
					idx = yy;
					cost = path.PathCost();
				} 
			}
		}
		if(idx != (uint32)(-1)) {
			iSeaUnit su;
			iShip* pShip = DynamicCast<iShip*>(m_pOwner->VisItems()[idx].value);
			su.pCaptain = pHero;
			su.pShip = pShip;
			m_fleet.Add(su);
			res = true;
			continue; // this one got the ship, skip to next landlubber		
			}		

		// next, try to find nearby harbor and add it to docks list to attract heroes
		idx = (uint32)(-1);
		cost = (uint32)(-1);
		iMineralSet ms;
			
		for(uint32 yy=0; yy<m_pOwner->VisItems().Size(); yy++)	 {
			iBaseMapObject* pobj = m_pOwner->VisItems()[yy].value;
			IHarbor *hrb = dynamic_cast<IHarbor*>(pobj);
			iPath path(pHero->Pos());		
			if(hrb && (hrb->GetOwner() == m_pOwner || hrb->GetOwner() == NULL) && hrb->IsShipBuildSupported())
			{
				for(uint32 dock=0; dock<hrb->NumOfDocks(); dock++) {
					if(hrb->ShipsAvailable(dock) > 0 && !hrb->GetShip(dock) && hrb->GetShipPos(dock) != cInvalidPoint) {
						if( m_docks.Find(hrb) == -1) {
							ms = SHIP_TYPE_DESC[dock].cost;
							if(pHero->Pos() == pobj->Pos()) {
								idx = yy;
								cost = 0;
								break;
							} else {
								pHero->MakePath(path, pobj->Pos().x, pobj->Pos().y);		
								if(!path.IsEmpty()/* && path.PathCost() < pHero->TotalMoves() * 4*/ && path.PathCost() < cost)   {
									idx = yy;
									cost = path.PathCost();
									break;
								}			
							}
						}
					}
				}

			}
		}
		if(idx != (uint32)(-1)) {
			iBaseMapObject* pobj = m_pOwner->VisItems()[idx].value;
			IHarbor *hrb = dynamic_cast<IHarbor*>(pobj);
			if(m_docks.Find(hrb) == -1) {
				m_docks.Add(hrb);		
				owner->PayMaster().ReserveMinerals(ms, 0, this, iAI_Paymaster::Ship);
				res = true;
			}
		}

		// still landlubber...
		newlandlubbers.Add(pHero);
	}
	m_landlubbers = newlandlubbers;
	// maybe the paymaster will find money immediately?
	if(res)
		owner->PayMaster().ProcessSubscribers();

	return res;
}

bool iAI_Admiral::PaySubs_Process(iMineralSet& receipt, iAI_Paymaster::RequestType reqType)
{
	check(reqType == iAI_Paymaster::Ship);
	iMineralSet left = receipt;
	receipt.Reset();
	
	// check if we have castles or viscnst_harbors with a hero in our lists and buy ships if available
	iSimpleArray<IHarbor*> newdocks;
	for(uint32 xx=0; xx<m_docks.GetSize(); xx++)
	{
		bool built = false;
		IHarbor *hrb = m_docks[xx];		
		iHero *pHero = dynamic_cast<iVisCnst_Harbor*>(hrb) ? dynamic_cast<iVisCnst_Harbor*>(hrb)->Visitor() : NULL;
		if(dynamic_cast<iCastle*>(hrb) || (pHero && pHero->Owner() == m_pOwner)) {			
			if(hrb->IsShipBuildSupported() && (!pHero || m_landlubbers.Find(pHero) != -1)) {
				for(uint32 dock=0; dock<hrb->NumOfDocks(); dock++) {
					if(left.Has(SHIP_TYPE_DESC[dock].cost) && hrb->ShipsAvailable(dock) > 0 && !hrb->GetShip(dock) && hrb->GetShipPos(dock) != cInvalidPoint) {
						iShip *pShip = gGame.Map().AddShip(hrb->GetShipPos(dock), dock);
						left -= SHIP_TYPE_DESC[dock].cost;
						receipt += SHIP_TYPE_DESC[dock].cost;				
						hrb->ShipBought(dock);										
						if(pHero)  {
							if(hrb->IsBoardable())
								pShip->Occupy(pHero);
							iSeaUnit su;					
							su.pCaptain = pHero;
							su.pShip = pShip;
							m_landlubbers.Remove(pHero);
							m_fleet.Add(su);													
							built = true;
						}
					}
				}	
			}
		} 
		if(!built) {
			newdocks.Add(m_docks[xx]);
		}
	}
	m_docks = newdocks;	
	return true;
}

iShip* iAI_Admiral::IsShipAvailable(iHero *pHero)
{ 
	if(pHero->InShip()) return pHero->GetShip();
	for(uint32 xx=0; xx<m_fleet.GetSize(); xx++) 
		if(m_fleet[xx].pCaptain == pHero) { 
			iShip *pShip = m_fleet[xx].pShip;
			iPath path(pHero->Pos());
			pHero->MakePath(path, pShip->Pos().x, pShip->Pos().y);			
			// the ship must be closer than 4 turns
			//if(!path.IsEmpty() && path.PathCost() < pHero->TotalMoves() * 4) {
				return pShip;
			/*} else {
				m_fleet.RemoveAt(xx);
				return NULL; 
			}*/
		}
	return NULL; 
}
