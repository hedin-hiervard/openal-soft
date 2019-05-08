#include "stdafx.h"
#include "Dlg_Level.h"
#include "ArtifactList.h"
#include "Dlg_CapArt.h"
#include "Dlg_RiseSkeletons.h"
#include "Dlg_ArmyRoom.h"
#include "Dlg_Rewards.h"
#include "serialize.h"
#include "helpers.h"

#include "OverlandView.h"
#include "OverlandView.touch.h"
#include "CastleView.h"

/*
*  Secondary skills container
*/
iSecSkillsCtr::iSecSkillsCtr(iFurtSkills& furtSkills) 
: m_furtSkills(furtSkills)
{}

SECSKILL_LEVEL iSecSkillsCtr::SkillLevel(SECONDARY_SKILLS secSkill) const
{
	for (uint32 xx=0; xx<m_secSkills.GetSize(); ++xx) if (m_secSkills[xx].m_skill == secSkill) return (SECSKILL_LEVEL)m_secSkills[xx].m_level;
	return SSLVL_NONE;
}

void iSecSkillsCtr::Inc(SECONDARY_SKILLS secSkill)
{
	for (uint32 xx=0; xx<m_secSkills.GetSize(); ++xx){
		if (m_secSkills[xx].m_skill == secSkill){
			check(m_secSkills[xx].m_level < SSLVL_EXPERT && m_secSkills[xx].m_level > SSLVL_NONE);
			m_furtSkills.Value((FURTHER_SKILLS)SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][0]) -= SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][1];
			m_secSkills[xx].m_level++;
			m_furtSkills.Value((FURTHER_SKILLS)SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][0]) += SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][1];
			return;
		}
	}

	Set(secSkill,SSLVL_BASIC);
}


void iSecSkillsCtr::Inc(SECONDARY_SKILLS secSkill, sint32 change)
{
	for (uint32 xx=0; xx<m_secSkills.GetSize(); ++xx){
		if (m_secSkills[xx].m_skill == secSkill){
			//check(m_secSkills[xx].m_level < SSLVL_EXPERT && m_secSkills[xx].m_level > SSLVL_NONE);
			m_furtSkills.Value((FURTHER_SKILLS)SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][0]) -= SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][1];
			sint8 res = iCLAMP<sint8>((sint8)SSLVL_NONE, (sint8)SSLVL_EXPERT, m_secSkills[xx].m_level + change);            
			if(res <= 0) {
				m_secSkills.RemoveAt(xx);
				return;
			} 
			m_secSkills[xx].m_level = res;
			m_furtSkills.Value((FURTHER_SKILLS)SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][0]) += SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][1];
			return;
		}
	}
	if(m_secSkills.GetSize() < 8 && change > 0)
		Set(secSkill, (SECSKILL_LEVEL)(change + (sint32)SSLVL_NONE));
}


void iSecSkillsCtr::Set(SECONDARY_SKILLS secSkill, SECSKILL_LEVEL lvl)
{
	check(SkillLevel(secSkill) == SSLVL_NONE && m_secSkills.GetSize() < 8);
	m_secSkills.Add(iSecSkillEntry(secSkill,lvl));
	m_furtSkills.Value((FURTHER_SKILLS)SEKSKILL_DESC[secSkill][lvl][0]) += SEKSKILL_DESC[secSkill][lvl][1];
}

void iSecSkillsCtr::Set(const iSecSkills& secSkills)
{
	check(!m_secSkills.GetSize());
	for (uint32 xx=0; xx<secSkills.GetSize(); ++xx) 
		Set((SECONDARY_SKILLS)secSkills[xx].m_skill, (SECSKILL_LEVEL)secSkills[xx].m_level);
}

void iSecSkillsCtr::Reset()
{
	for (uint32 xx=0; xx<m_secSkills.GetSize(); ++xx) {
		m_furtSkills.Value((FURTHER_SKILLS)SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][0]) -= SEKSKILL_DESC[m_secSkills[xx].m_skill][m_secSkills[xx].m_level][1];
	}
	m_secSkills.RemoveAll();
}

/*
*  Hero
*/
iHero::iHero(iHeroT* pProto, sint32 fake, bool bAfterLoad)
: iBaseMapObject(cInvalidPoint, true)
, m_pProto(pProto), m_bDead(true), m_Path(cInvalidPoint), m_Enchs(m_varFurtSkills), m_SecSkills(m_varFurtSkills)
, m_Artifacts(this), m_pShip(NULL), m_bNoExping(false)
{    
	Init(NULL, cInvalidPoint, -1, bAfterLoad);
}

// Serialize / Deserialize hero object
void iHero::Serialize(iDynamicBuffer& dbuff) const
{
	// Proto Idf
	dbuff.Write((uint16)m_pProto->m_protoId);
	// Owner Id
	if (m_pOwner) dbuff.Write((sint8)m_pOwner->PlayerId());
	else dbuff.Write((sint8)PID_NEUTRAL);
	// Position
	::Serialize(dbuff, m_Pos);
	// Angle
	dbuff.Write((sint8)m_Angle);
	// Target
	if (m_Path.IsEmpty()) ::Serialize(dbuff, m_Pos);
	else ::Serialize(dbuff, m_Path.DestPoint());
	// Custom Name
	::Serialize(dbuff, m_CustomName);
	// Level
	dbuff.Write(m_Level);
	// Experience
	dbuff.Write((uint32)m_Experience);
	//Secondary skills
	::Serialize(dbuff, m_SecSkills.SecSkills());
	// Artifact list
	iArtifactList artList;
	for (uint32 artIdx=0; artIdx<m_Artifacts.BackPackCount(); ++artIdx) 
		artList.Add(m_Artifacts.BackPackItem(artIdx), AC_UNDEFINED);
	for (uint32 artIdx=0; artIdx<AC_COUNT; ++artIdx) 
		if (!m_Artifacts.DressedItem((HERO_ART_CELL)artIdx).Empty()) 
			artList.Add(m_Artifacts.DressedItem((HERO_ART_CELL)artIdx).artId, (HERO_ART_CELL)artIdx);
	::Serialize(dbuff, artList);
	// Enchancement list
	dbuff.Write((uint16)m_Enchs.Count());
	for (uint32 xx=0; xx<m_Enchs.Count(); ++xx) {
		// provider
		//int a1=m_Enchs[xx].provider;
		dbuff.Write((sint32)m_Enchs[xx].provider);
		// type (FURTHER_SKILLS)
		//  int a2=m_Enchs[xx].type;
		dbuff.Write((sint16)m_Enchs[xx].type);
		// duration
		dbuff.Write((uint16)m_Enchs[xx].dur);
		// mod
		dbuff.Write((sint32)m_Enchs[xx].mod);
	}
	// FurtSkills
	::Serialize(dbuff, m_constFurtSkills);
	// Army
	::Serialize(dbuff, Army());
	// Spellbook
	dbuff.Write((uint8)1);
	::Serialize(dbuff, m_spellBook.Spells());
	dbuff.Write((uint8)m_spellBook.FavoritesCount());
	for (uint32 xx=0; xx<m_spellBook.FavoritesCount(); ++xx) dbuff.Write(m_spellBook.Favorite(xx));
	// Mana points
	dbuff.Write(m_manaPts);
	// Action points
	dbuff.Write(m_Moves);
	// Can dig flag
	dbuff.Write((uint8)m_bCanDig);
	// initial pos
	::Serialize(dbuff, m_initialPos);

	// +ship save/load code start
	// save in ship
	uint8 in_ship = m_pShip ? 1 : 0;
	dbuff.Write(in_ship);
	if (in_ship)    
	{
		m_pShip->Serialize(dbuff);
	}
	// -ship save/load code end

	// noexping flag
	dbuff.Write((uint8)m_bNoExping);
}

iStringT MakeTextEntryName(TextEntryType t, const void* pObj);

void iHero::Unserialize(iDynamicBuffer& dbuff, bool bInit, bool bAfterLoad, bool bSetOwner)
{
	//DbgDumpFurtSkills(Name() + _T(" (initial constant)"), m_constFurtSkills);
	//DbgDumpFurtSkills(Name() + _T(" (initial var)"), m_varFurtSkills);

	// CtllTODO: why?
	//  check(m_bDead);
	if (bInit) m_bDead = false;

	// Owner
	sint8 owner; dbuff.Read(owner);
	if (bSetOwner && owner != PID_NEUTRAL) {
		m_pOwner = gGame.Map().FindPlayer((PLAYER_ID)owner);
		check(m_pOwner);
		m_pOwner->AddHero(this, false);
	}

	// Position
	iPoint pos; ::Unserialize(dbuff, pos); 
	SetPos(pos);
	// Angles
	sint8 angle; dbuff.Read(angle);
	if (angle < 0) m_Angle = gGame.Map().Rand(8); else m_Angle = (uint8)angle;
	// Target
	iPoint target; ::Unserialize(dbuff, target);
#ifdef MULTIPLAYER
	if(!gMPMgr.IsPRA()) // fix for the 'unserialize hero with path set -- he begins moving' bug
#endif
		if (target != pos) SetPath(target);
	// Custom name
	::Unserialize(dbuff, m_CustomName);
	
	// if this is first launch and custom name is set, write the text key instead
	if(!bAfterLoad && !m_CustomName.Empty())
		m_CustomName = MakeTextEntryName(TET_HERO_CNAME, (void*)this);
		
	// Level
	uint32 level; dbuff.Read(level);
	if (level) m_Level = level;
	// Experience
	uint32 exp; dbuff.Read(exp);
	if (exp) 
		m_Experience = exp;
	else if(m_Level != 1) {
		m_Experience = LevelBound(m_Level);
	}
	// Secondary skills
	iSecSkills secSkills;   
	::Unserialize(dbuff, secSkills);
	m_SecSkills.Reset();
	m_SecSkills.Set(secSkills);
	// Artifact list
	iArtifactList artList; ::Unserialize(dbuff, artList);

	// clear old artifacts
	for (uint32 artIdx=0; artIdx<AC_COUNT; ++artIdx) 
		if(!m_Artifacts.DressedItem((HERO_ART_CELL)artIdx).Empty()) {
			gGame.ItemMgr().m_artMgr[m_Artifacts.DressedItem((HERO_ART_CELL)artIdx).artId].Dettach(this);
			m_Artifacts.DressedItem((HERO_ART_CELL)artIdx).Reset();
		}
		m_Artifacts.BackPack().RemoveAll();

		for (uint32 artIdx=0; artIdx<artList.Count(); ++artIdx) {
			if (artList.At(artIdx).assign == AC_UNDEFINED) 
				m_Artifacts.PushToBackPack(artList.At(artIdx).id);
			else  if(artList.At(artIdx).assign != AC_SMARTPUSH)
				m_Artifacts.PushDressed(artList.At(artIdx).id, (HERO_ART_CELL)artList.At(artIdx).assign);
			else
				m_Artifacts.SmartPush(artList.At(artIdx).id);
		}
		// Enchancement list
		m_Enchs.RemoveAll();
		m_Enchs.Clear();
		uint16 enCnt; dbuff.Read(enCnt);
		for (uint32 xx=0; xx<enCnt; ++xx) {
			// provider
			sint32 provider; dbuff.Read(provider);
			// type (FURTHER_SKILLS)
			sint16 type; dbuff.Read(type);
			// duration
			uint16 dur; dbuff.Read(dur);
			// mod
			sint32 mod; dbuff.Read(mod);
			// Add enchancement item
			m_Enchs.Add(provider, (FURTHER_SKILLS)type, (ENCH_DURATION)dur, mod);
		}
		// FurtSkills
		iFurtSkills furtSkills; ::Unserialize(dbuff, furtSkills);
		m_constFurtSkills = furtSkills;
		// Army
		iArmy army; ::Unserialize(dbuff, army);
		SetArmy(army);
	
		// we are starting the game
		if (!bAfterLoad) {
			// if the army is completely empty, reset it to default values
			if(Army().Empty()) 
				ResetHeroArmy(bAfterLoad);
							
			// init the possible random values of creature numbers
			InitArmy();
		}
		//}
		// Spellbook
		uint8 spflag; dbuff.Read(spflag);   
		if (spflag != 0) {
			iSpellList spellList; ::Unserialize(dbuff, spellList);
			m_spellBook.Reset();
			if (spellList.GetSize()) m_spellBook.Add(spellList);
		}
		// Favorite spells
		uint8 fspcnt; dbuff.Read(fspcnt);   
		while (fspcnt--) {
			uint16 spellId;
			dbuff.Read(spellId);
			m_spellBook.AddFavorite(spellId);
		}

		// Mana points
		sint32 mana;
		dbuff.Read(mana);
		if (mana >= 0) m_manaPts = mana;
		else m_manaPts = MaxManaPts();

		// Action points
		sint32 action; dbuff.Read(action);
		if (action >= 0) m_Moves = action;
		else m_Moves = TotalMoves();

		// Can dig flag
		uint8 bCanDig;
		dbuff.Read(bCanDig);
		m_bCanDig = (bCanDig==0)?false:true;

		// initial pos
		::Unserialize(dbuff, m_initialPos);


		// +ship save/load code start
		// load ship
		uint8 in_ship;
		dbuff.Read(in_ship);        
		if (in_ship)    
		{
			m_pShip = new iShip();
			gGame.Map().m_Ships.Add(m_pShip);
			m_pShip->Unserialize(dbuff, bInit, bAfterLoad, bSetOwner, this);
			// gGame.Map().SetShipPos(m_pShip, m_pShip->Pos());
		}

		// -ship save/load code end

		// no exping flag
		uint8 noexping; dbuff.Read(noexping);
		m_bNoExping = (bool)noexping;

		//DbgDumpFurtSkills(Name() + _T(" (unserialized constant)"), m_constFurtSkills);
		//DbgDumpFurtSkills(Name() + _T(" (unserialized var)"), m_varFurtSkills);
}

void iHero::Deinit(bool bResetArmy)
{
	const iHeroT *pProto = this->Proto();
	check(!m_bDead);
	m_bDead = true;
	m_pOwner = NULL;
	SetPos(cInvalidPoint);
	m_Path.SetEmpty(m_Pos);
	if (m_pLocation) SetLocation(NULL);
	if (bResetArmy) {
		ResetHeroArmy(false);
		InitArmy();
	}
	m_Artifacts.Update(this);
	m_Enchs.Update(m_varFurtSkills), 
		m_Enchs.Clear();
	//m_SecSkills.Set(m_varFurtSkills);
	m_Experience = InitialExpPts();
	m_Level = 1, 
		m_spellSetMode = SSM_ALL;
	m_bCanDig = true;

	// Init FurtSkills
	m_constFurtSkills = pProto->m_furtSkills;
	// Init secondary skills
	m_SecSkills.Reset();
	m_SecSkills.Set(pProto->m_secSkills);
	// Init Spellbook
	m_spellBook.Add(pProto->m_spells);
	// Mana points
	m_manaPts = MaxManaPts();
	// Action points
	m_Moves = TotalMoves();

}

void iHero::Init(iPlayer* pOwner, const iPoint& pos, sint8 angle, bool bAfterLoad)
{
	const iHeroT *pProto = this->Proto();

	m_bDead = false;

	m_bStop = false;
	m_bMoving = false;
	m_Path = cInvalidPoint;
	m_pLocation = NULL;
	m_Artifacts.Update(this);
	m_Enchs.Update(m_varFurtSkills);
		m_Enchs.Clear();
	//m_SecSkills.Set(m_varFurtSkills);
	m_Experience = InitialExpPts();
	m_Level = 1, 
		m_spellSetMode = SSM_ALL;
	m_bCanDig = true;

	m_pOwner = pOwner;

	//  useless now
	//  check(m_bDead && !m_pOwner && m_Pos == cInvalidPoint); 

	// Init FurtSkills
	m_constFurtSkills = pProto->m_furtSkills;
	// Init secondary skills
	m_SecSkills.Reset();
	m_SecSkills.Set(pProto->m_secSkills);
	// Init Army
	ResetHeroArmy(bAfterLoad);
	InitArmy();
	// Init Spellbook
	m_spellBook.Add(pProto->m_spells);
	// Mana points
	m_manaPts = MaxManaPts();
	// Action points
	m_Moves = TotalMoves();

	// unvisit all visitables
	for(iGameMap::iVisCnstList::Iterator cnst = gGame.Map().m_VisCnsts.First(); cnst != gGame.Map().m_VisCnsts.End(); cnst++)
		cnst->Unvisit(this);

	SetPos(pos);
	if (angle < 0) m_Angle = gGame.Map().Rand(8); else m_Angle = (uint8)angle;

	m_bNoExping = false;
}

void iHero::StartMoving()
{
	m_bMoving = true;
}

void iHero::StopMoving()
{
    if(m_Moves < m_Path.GetStepCostTillValid()) {
        gTutorial.Trigger(TUT_HERO_TIRED);
    }

	if (m_bMoving) {
		m_bMoving = false;
		gGame.OnHeroStopMoving(this);
	}
}

bool iHero::Step()
{	
#ifndef MULTIPLAYER
	if (/*gGame.Map().m_PassMap.GetAt(m_Pos.x, m_Pos.y) != PASSMAP_PWATER && */(m_bDead || m_Path.IsEmpty() || m_Moves < m_Path.GetStepCostTillValid())) {
#else
	if (m_bDead || m_Path.IsEmpty() || (!gMPMgr.IsPRA() && m_Moves < m_Path.GetStepCostTillValid())) {
#endif
		StopMoving();
#ifdef ROYAL_BOUNTY
		if(gGame.Map().ActPlayer() == m_pOwner)
			gGame.AutoNextTurn();
#endif
		return false;
	}

	m_pOwner->SetHeroMovedFlag();

	bool bNeedAction = false;

	if (m_NextPoint == m_Path.GetStepPos()) {
		m_bCanDig = false;
		m_Moves -= m_Path.GetStepCost();
		iPath::iStep step = m_Path.GetStep();
		if (step.m_action == iPath::MoveTo || step.m_action == iPath::Enter) {
			iPoint oldPos = Pos();
			iPath::iStep nStep = m_Path.Step();

			// if hero is in ship and entering some construction we obviously leave the ship
			bool bWasShip = m_pShip;
				
			
			if (step.m_action ==iPath::Enter && m_pShip)
			{				
				m_pShip->Leave(this, nStep.m_Pos);				
			} else {	
				SetPos(nStep.m_Pos);
				gGame.Map().MoveHero(oldPos,nStep.m_Pos);
			}
			m_pOwner->UpdateFogMap();
			gGame.OnHeroPosChanged(this, nStep.m_Pos); 

			if (!m_Path.IsEmpty()){
				// Check target
				if (m_Path.TargetAction() != iPath::MoveTo) {
					uint8 act = GetTarget((sint16)m_Path.DestPoint().x,(sint16)m_Path.DestPoint().y).action;
					if (act != m_Path.TargetAction()) {
						StopMoving();
						m_Path.SetEmpty(m_Pos);
						return false;
					}
				}
			} else if (step.m_action == iPath::Enter && !bWasShip) { // don't activate harbor when leaving ship
				// process action (if end point)
				check(m_Path.IsEmpty() && m_pLocation);
				StopMoving();
				if (iCastle* pCastle = DynamicCast<iCastle*>(m_pLocation)) {
					if (pCastle->Owner() == m_pOwner->PlayerId()) 
                        EnterCastle(pCastle);
					else CaptureCastle(pCastle, true);
				} else 
					ActivateConstruction(m_pLocation);
			}
		} else if (step.m_action == iPath::Touch || step.m_action == iPath::Attack || step.m_action == iPath::Meet){
			iPath::iStep nStep = m_Path.Step();
			//
			iBaseMapObject* pMapObj = gGame.Map().m_CoverMap.GetAt(nStep.m_Pos.x, nStep.m_Pos.y);
			check(pMapObj);

			if (iHero* pHero = DynamicCast<iHero*>(pMapObj)){
				if (step.m_action == iPath::Meet) {
					check(pHero->Owner() == Owner());
					StopMoving();
					bNeedAction = true;
					MeetHero(pHero);
				} else if (step.m_action == iPath::Attack){
					check(pHero->Owner() != Owner());
					if (iCastle* pCastle = DynamicCast<iCastle*>(pHero->GetLocation())){
						StopMoving();
						bNeedAction = true;
#ifdef MULTIPLAYER
						if(!gMPMgr.IsReplaying())
#endif
							SiegeCastle(pCastle);
					} else {
						StopMoving();
						bNeedAction = true;
#ifdef MULTIPLAYER
						if(!gMPMgr.IsReplaying())
#endif
							AttackHero(pHero);
					}
				} else {
					check(0);
				}
			} else if (iMapGuard* pGuard = DynamicCast<iMapGuard*>(pMapObj)) {
				check(step.m_action == iPath::Attack);
				StopMoving();
				bNeedAction = true;
				TouchMapGuard(pGuard, false);
			} else if (DynamicCast<iMapItem_Artifact*>(pMapObj) || DynamicCast<iMapItem_CampFire*>(pMapObj) || DynamicCast<iMapItem_Chest*>(pMapObj) || DynamicCast<iMapItem_Lamp*>(pMapObj) || DynamicCast<iMapItem_ManaCrystal*>(pMapObj) || DynamicCast<iMapItem_Mineral*>(pMapObj) || DynamicCast<iMapItem_SpellScroll*>(pMapObj) || DynamicCast<iMapItem_KeyGuard*>(pMapObj)) {
				check(step.m_action == iPath::Touch);
				StopMoving();
				bNeedAction = true;
				iMapItem* pMapItem = (iMapItem*)pMapObj;
				TouchMapItem(pMapItem, false);
			} else if (iCastle* pCastle = DynamicCast<iCastle*>(pMapObj)) {
				StopMoving();
				bNeedAction = true;
#ifdef MULTIPLAYER
				if(!gMPMgr.IsReplaying())
#endif
					SiegeCastle(pCastle);
			} else {
				// unknown object
				check(0);
			}
		} 
		else if (step.m_action == iPath::Occupy)
		{
			// occupy ship
			iPath::iStep nStep = m_Path.Step();
			iBaseMapObject* pObj = gGame.Map().m_CoverMap.GetAt(nStep.m_Pos.x, nStep.m_Pos.y);
			check(m_Path.IsEmpty());
			StopMoving();
			bNeedAction = true;            
			if (iShip* pShip = DynamicCast<iShip*>(pObj))
			{
				if (!pShip->IsLoaded())
				{
					pShip->Occupy(this);
					// setup hero angle same as ship
					m_Angle = pShip->Angle();
				}
				else
				{
					check(0);
				}
			}
		}
		else if (step.m_action == iPath::Leave)
		{   
			// leave ship
			m_Path.DumpPath();
			iPath::iStep nStep = m_Path.Step();
			if (m_pShip)
			{				
				m_pShip->Leave(this, nStep.m_Pos);				
			}			
		}
		else 
		{
			// unknown action
			check(0);
		}
		if (m_bDead) return false;

		if (bNeedAction) return false;
		if (iMapEvent* pEvent = gGame.Map().FindMapEvent(m_NextPoint, m_pOwner->PlayerId())) {
			if((pEvent->Flags() & MEF_HUMAN_ENABLED && m_pOwner->PlayerType() == PT_HUMAN) || 
				pEvent->Flags() & MEF_AI_ENABLED && m_pOwner->PlayerType() == PT_COMPUTER) {

					StopMoving();
					bool stopMoving = false;
					if (pEvent->Activate(this, m_pOwner->PlayerType() == PT_HUMAN, true, stopMoving)) {             
						if(!(pEvent->Flags() & MEF_REUSABLE))
							gGame.Map().RemoveMapEvent(pEvent);                 
						return false;
					} 
					if(!stopMoving) {
						if(!NeedStop()) {
							if(!gGame.BattleWrapper())
								StartMoving();              
							else 
								return false;
						} else {
							m_bStop = false;
							StopMoving(); 			
							return false;
						}
					} else
						return false;
			} else {
				if (NeedStop()) {
					m_bStop = false;
					StopMoving();
					return false;
				}
			}       
		} else if (NeedStop()) {
			m_bStop = false;
			StopMoving();
			return false;
		}
	} else {
		/*if (NeedStop()) {
		m_bStop = false;
		StopMoving();
		return false; 
		} 
		 */
		// This is commented out to prevent AI from stupidly losing turns when encountering new objects (and thus returning false). Hedin, 20.10.2010 */
		StartMoving();
	}

	m_Angle = m_Path.GetAngle();
	m_NextPoint = m_Path.GetStepPos();

	// update angle in ship
	// if hero leave ship
	if (InShip())
	{
		m_pShip->SetAngle(m_Angle);
	}


	gGame.OnHeroMoveTo(this, m_Path.GetStep());
	m_bStop = false;

	return true;
}

void iHero::Stop()
{
	m_bStop = true;
}

bool iHero::NeedStop()
{
	if(gGame.Map().m_PassMap.GetAt(m_Pos.x, m_Pos.y) == PASSMAP_PWATER)
		return false;

	if (m_pOwner->NeedStopHero()) {
		if(m_pOwner->EncounteredHero()) {
			Stop();
			ResetPath();
		}

		ProcessItemChanges();
		m_pOwner->ResetVistItemChanges();
	}
	
	
#ifdef ROYAL_BOUNTY
	if(gGame.Map().ActPlayer() == m_pOwner &&
	   !m_Path.IsEmpty() &&
	   m_Moves < m_Path.GetStepCost())
		gGame.AutoNextTurn();
#endif
		
#ifndef MULTIPLAYER
	return m_bStop || m_Path.IsEmpty() || m_Moves < m_Path.GetStepCost();
#else
	return m_bStop || m_Path.IsEmpty() || (!gMPMgr.IsPRA() && m_Moves < m_Path.GetStepCost());
#endif

}

void iHero::ProcessItemChanges()
{
	if (iAI_Player* pAIPlayer =  DynamicCast<iAI_Player*>(m_pOwner)){
		Stop();
		m_Path.SetEmpty(m_Pos);
		pAIPlayer->ProcessItemChanges();
	} else {
		/*for (uint32 xx=0; xx<m_pOwner->NewVisItems().Size(); ++xx){
			if (m_Path.BelongToPath(m_pOwner->NewVisItems()[xx].value->Pos())) {
				// Stop hero if new items belong to path
				Stop();
				m_Path.SetEmpty(m_Pos);
			}
		}*/
	}
}

iNewLevelInfo iHero::NextLevelInfo()
{
	iNewLevelInfo result;

	// Primary skill
	sint32 pskill = CalcRandValue(HERO_PRIM_SKILL[Type()],PRSKILL_COUNT);
	check(pskill != -1);
	result.prSkillType = (PRSKILL_TYPE)pskill;

	// Secondary skill
	uint8 frSkills[SECSK_COUNT];
	memcpy(frSkills,HERO_SEC_SKILL[Type()],sizeof(frSkills));
	uint8 upSkills[SECSK_COUNT];
	memset(upSkills,0,sizeof(upSkills));

	sint32 frCnt = 8-m_SecSkills.Count();
	sint32 upCnt = 0;

	for (uint32 xx=0; xx<m_SecSkills.Count(); ++xx) {
		if (m_SecSkills.At(xx).m_level < SSLVL_EXPERT) {
			upSkills[m_SecSkills.At(xx).m_skill] = (uint8)iMAX<sint32>(1,frSkills[m_SecSkills.At(xx).m_skill]);
			upCnt++;
		}
		frSkills[m_SecSkills.At(xx).m_skill] = 0;
	}

	if (upCnt) {
		sint32 sskill = CalcRandValue(upSkills, SECSK_COUNT);
		result.secSkill[0] = (SECONDARY_SKILLS)sskill;
		upSkills[result.secSkill[0]] = 0;
		upCnt--;
	} else {
		if (frCnt) {
			result.secSkill[0] = (SECONDARY_SKILLS)CalcRandValue(frSkills, SECSK_COUNT);
			frSkills[result.secSkill[0]] = 0;
		} else {
			// nothing to learn
			result.secSkill[0] = result.secSkill[1] = SECSK_NONE;
			return result;
		}
	}

	if (frCnt){
		result.secSkill[1] = (SECONDARY_SKILLS)CalcRandValue(frSkills, SECSK_COUNT);
		frSkills[result.secSkill[1]] = 0;
	} else {
		if (upCnt){
			result.secSkill[1] = (SECONDARY_SKILLS)CalcRandValue(upSkills, SECSK_COUNT);
			upSkills[result.secSkill[1]] = 0;
			upCnt--;
		} else {
			result.secSkill[1] = SECSK_NONE;
		}
	}

	return result;
}

void iHero::Process(fix32 t)
{
}

bool iHero::NeedArtifact(uint16 artId) const
{
	// Process negative case for special artifacts
	if (gGame.ItemMgr().m_artMgr[artId].Type() == ARTT_NEGSPHERE && Owner()->PlayerType() == PT_COMPUTER) {
		sint32 mval = FurtSkill(FSK_POWER) + FurtSkill(FSK_KNOWLEDGE);
		sint32 sval = FurtSkill(FSK_ATTACK) + FurtSkill(FSK_DEFENCE);
		return mval > 20 || (mval > 3 && mval > (sval/2));
	}
	return true;
}

void iHero::CaptureArtifacts(const iArtList& artList)
{
	// show dialog with captured artifacts (for active player)
	if (m_pOwner->PlayerType() == PT_HUMAN) {
		iDlg_CapArt cadlg(&gApp.ViewMgr(), this, artList);
		cadlg.DoModal();
		// Add artifacts to heroe's inventory
		for (uint32 xx=0; xx<artList.GetSize(); ++xx) m_Artifacts.Push(artList[xx]);

		// got the victory artifact ?
		if(gGame.CheckPlayerWinAndLose(Owner()))		
			return;		
	} else {
		// Add artifacts to heroe's inventory using SmartPush method
		for (uint32 xx=0; xx<artList.GetSize(); ++xx) m_Artifacts.SmartPush(artList[xx]);
	}

}

bool iHero::HasSpecFlag(SPEC_HERO_FLAGS flag) const
{
	return m_Artifacts.HasArtType(ARTT_ULTIMATE) && ULTART_SPECFLAGS[m_pProto->m_hType] == flag;
}

sint32 iHero::RiseSkeletons(sint32 enCas, sint32 enHpCas)
{
	sint32 val = FurtSkill(FSK_NECROMANCY);
	if (!val || !enCas || m_SecSkills.SkillLevel(SECSK_NECROMANCY) == SSLVL_NONE) return 0;

	CREATURE_TYPE ct = HasSpecFlag(SHF_NECRBONUS)?CREAT_VAMPIRE:CREAT_SKELETON;

	sint32 quant = iCLAMP<sint32> (1, enCas,(enHpCas * val) / (CREAT_DESC[ct].hits * 100)); 

	// show dialog with raised skeletons
	if (m_pOwner == gGame.Map().ActPlayer()) {
		iDlg_RiseSkeletons rsdlg(&gApp.ViewMgr(), m_pOwner->PlayerId(), ct, quant);
		rsdlg.DoModal();
	}

	// Add Skeletons to army
	if(!AddGroupToArmy(ct, quant) && m_pOwner == gGame.Map().ActPlayer()) {
		iCreatGroup cg(ct, quant);
		iDlg_ArmyRoom arDlg(&gApp.ViewMgr(), *this, cg);
		arDlg.DoModal();
	}

	return quant;
}

void iHero::FinalizeReward(const iRewardItem* item) {
	if(item->m_type == RIT_EXPERIENCE)
		ReceiveExperience(item->m_sParam);
	else if(item->m_type == RIT_TELEPORT) {
		iBaseMapObject* pTarget = gGame.Map().FindTeleportDestination(NULL, item->m_fParam);
		if(!pTarget) return;
		if (iHero* pTeleportVisitor = DynamicCast<iHero*>(gGame.Map().m_CoverMap.GetAt(pTarget->Pos().x,pTarget->Pos().y)))
			return;

		if(m_pShip) {
			iShip* ship = m_pShip;
			ship->Leave(this, pTarget->Pos());
			gGame.Map().RemoveShip(ship);
		}
		else {
			gGame.Map().MoveHero(Pos(), pTarget->Pos());
			SetPos(pTarget->Pos());
		}		
		
		Owner()->UpdateFogMap();
		m_Path.SetEmpty(Pos());
		gGame.OnHeroPosChanged(this, Pos());
		gGame.OnHeroChanged(this);
		gGame.OnHeroTeleport(this, Pos(), pTarget->Pos());
	}
}

iRewardItem* iHero::ReceiveReward(const iRewardItem& reward)
{
	iRewardItem *res = new iRewardItem(reward.m_type, reward.m_fParam, reward.m_sParam);
	switch(reward.m_type) {
	case RIT_MINERAL:       
		if(m_pOwner->GetMineral((MINERAL_TYPE)reward.m_fParam) + reward.m_sParam < 0)
			res->m_sParam = - m_pOwner->GetMineral((MINERAL_TYPE)reward.m_fParam);
		m_pOwner->AddMineral((MINERAL_TYPE)reward.m_fParam, res->m_sParam); 
		return res->m_sParam != 0 ? res : NULL;
	case RIT_EXPERIENCE:
		if(reward.m_sParam > 0) {            		
			return res;
		} 
		delete res;
		return NULL;        
	case RIT_MANAPTS:   
		if(m_manaPts + reward.m_sParam< 0)
			res->m_sParam = -m_manaPts;
		m_manaPts += res->m_sParam;
		return res->m_sParam != 0 ? res : NULL;
	case RIT_TRAVELPTS:
		if(m_Moves + reward.m_sParam < 0)
			res->m_sParam = -m_Moves;
		m_Moves += res->m_sParam;
		return res->m_sParam != 0 ? res : NULL;     
	case RIT_MORALE:
		m_Enchs.Add((intptr_t)(&reward), FSK_MORALE,ENDUR_NEXTBATTLE,reward.m_sParam);
		return res;
	case RIT_LUCK:
		m_Enchs.Add((intptr_t)(&reward), FSK_LUCK,ENDUR_NEXTBATTLE,reward.m_sParam);
		return res;
	case RIT_FURTSKILL:
		check(reward.m_fParam >= FSK_ATTACK && reward.m_fParam <= FSK_KNOWLEDGE);
		if(m_constFurtSkills.Value((FURTHER_SKILLS)reward.m_fParam) + reward.m_sParam < 0)
			res->m_sParam = -m_constFurtSkills.Value((FURTHER_SKILLS)reward.m_fParam);
		m_constFurtSkills.Value((FURTHER_SKILLS)reward.m_fParam) += res->m_sParam;
		return res->m_sParam != 0 ? res : NULL; 
	case RIT_SECSKILL:
		{
			check(reward.m_fParam >= SECSK_ESTATES && reward.m_fParam < SECSK_COUNT);
			SECSKILL_LEVEL before = m_SecSkills.SkillLevel((SECONDARY_SKILLS)reward.m_fParam);
			m_SecSkills.Inc((SECONDARY_SKILLS)reward.m_fParam, reward.m_sParam);
			if(before == m_SecSkills.SkillLevel((SECONDARY_SKILLS)reward.m_fParam)) return NULL;
			return res->m_sParam != 0 ? res : NULL;     
		}
	case RIT_TELEPORT:
		return res;
	case RIT_ARTIFACT:
		if(!reward.m_sParam) {
			if(reward.m_fParam == ARTT_RAND)
				res->m_fParam = gGame.ItemMgr().m_artMgr.SelectRandomArtifact(ART_LEVEL_NONE);
			else if(reward.m_fParam == ARTT_RAND_L1)
				res->m_fParam = gGame.ItemMgr().m_artMgr.SelectRandomArtifact(ART_LEVEL_TREASURE);
			else if(reward.m_fParam == ARTT_RAND_L2)
				res->m_fParam = gGame.ItemMgr().m_artMgr.SelectRandomArtifact(ART_LEVEL_MINOR);
			else if(reward.m_fParam == ARTT_RAND_L3)
				res->m_fParam = gGame.ItemMgr().m_artMgr.SelectRandomArtifact(ART_LEVEL_MAJOR);
			else if(reward.m_fParam == ARTT_RAND_L4)
				res->m_fParam = gGame.ItemMgr().m_artMgr.SelectRandomArtifact(ART_LEVEL_RELICT);

			if (m_pOwner->PlayerType() == PT_HUMAN) m_Artifacts.Push(res->m_fParam);
			else m_Artifacts.SmartPush(res->m_fParam);
			res->m_sParam = 1;
		} else {            
			res->m_sParam = -m_Artifacts.SmartPop(reward.m_fParam);             
		}
		return res;
	case RIT_MAGICSPELL:
		if (CanLearnSpell(reward.m_fParam)) { LearnSpell(reward.m_fParam); return res; }
		return NULL;        
	case RIT_CREATGROUP: 
		{
			if(Army().CreatureCount((CREATURE_TYPE)reward.m_fParam) + reward.m_sParam < 0)
				res->m_sParam = -Army().CreatureCount((CREATURE_TYPE)reward.m_fParam);

			iCreatGroup cg((CREATURE_TYPE)reward.m_fParam,res->m_sParam);
			if (m_pOwner->PlayerType() == PT_COMPUTER) {
				iAI_Player* pAIOwner = DynamicCast<iAI_Player*>(m_pOwner);
				check(pAIOwner);
				pAIOwner->ProcessJoinCreatures(this, cg);
			} else {                
				if (!AddGroupToArmy(cg.Type(), cg.Count())) {
					iDlg_ArmyRoom arDlg(&gApp.ViewMgr(), *this, cg);
					arDlg.DoModal();
				}
			}
			return res->m_sParam != 0 ? res : NULL;
		}
		break;
	default:
		check(0);
	}
	return NULL;
}

bool iHero::Dig()
{
	if (m_pOwner->PlayerType() == PT_HUMAN) {
#ifndef ROYAL_BOUNTY
		if (!m_bCanDig) {
			iTextDlg tdlg(&gApp.ViewMgr(), _T(""), gTextMgr[TRID_MSG_DIG_TRY_TOMORROW], m_pOwner->PlayerId());
			tdlg.DoModal();
		} else 
#endif
		if (!gGame.Map().CanDig(m_Pos)) {
			iTextDlg tdlg(&gApp.ViewMgr(), _T(""), gTextMgr[TRID_MSG_DIG_TRY_ON_CLEAR_LAND], m_pOwner->PlayerId());
			tdlg.DoModal();
		} else {
			// dig here
			gSfxMgr.PlaySound(CSND_DIG);
			m_Moves = 0;
			if (gGame.Map().Dig(m_Pos)) {
				iIconDlg idlg(&gApp.ViewMgr(), _T(""), gTextMgr[TRID_MSG_DIG_FOUND], PDGG(ULTIMATE_ART), m_pOwner->PlayerId());
				idlg.DoModal();

				m_Artifacts.SmartPush(ARTT_ULTIMATE);				
#ifdef MULTIPLAYER
				FIXME
					gMPMgr.OnHeroUpdate(this);
#endif
				return true;
			} else {
				iTextDlg tdlg(&gApp.ViewMgr(), _T(""), gTextMgr[TRID_MSG_DIG_NOTHING], m_pOwner->PlayerId());
				tdlg.DoModal();
			}
		}
	} else {
		if (!m_bCanDig || !gGame.Map().CanDig(m_Pos)) {
			check(0);
		} else {
			// dig here
			if (gGame.Map().Dig(m_Pos)) {
				m_Artifacts.SmartPush(ARTT_ULTIMATE);
				m_Moves = 0;
#ifdef MULTIPLAYER
				gMPMgr.OnHeroUpdate(this);
#endif

				return true;
			}
			m_Moves = 0;
		}
	}
	return false;
}
    
iStringT iHero::Name() const
{
	if (!m_CustomName.Empty()) return gGame.Map().GetText(m_CustomName);
	return gTextMgr[m_pProto->m_NameKey];
}
    
iStringT iHero::EnglishName() const{

    if (!m_CustomName.Empty()) return gGame.Map().GetEnglishText(m_CustomName);
	return gTextMgr.English( m_pProto->m_NameKey );
}

const HERO_TYPE iHero::Type() const
{
	return m_pProto->m_hType;
}

uint8 iHero::Scouting() const
{ 
	return (uint8)iCLAMP<sint32>(1,128,DEF_HERO_SCOUTING+FurtSkill(FSK_SCOUTING));
}

sint32 iHero::TotalMoves() const
{ 
	// initial speed according to slowest creature
	sint32 tmoves = (DEF_HERO_MOVES + (Army().SlowestSpeed()*2));

	if (!InShip())
		tmoves += (tmoves * FurtSkill(FSK_LOGISTICS)) / 100;
	else
		tmoves += (tmoves * FurtSkill(FSK_SEAFARING)) / 100;

	if (InShip() && HasSpecFlag(SHF_SEAFARING))    
		tmoves += (tmoves * 40) / 100;


	tmoves += FurtSkill(FSK_ACTPTS);

	// TRICK!
	// make twice as much move points for computer
#if defined(NEED_REGISTRATION) && defined(UNDER_CE)
	iAI_Player* pOwner = DynamicCast<iAI_Player*>( m_pOwner );
	if ( pOwner && (pOwner->PlayerType() == PT_COMPUTER) && (pOwner->m_HackedTricks & 0x0f) ) {
		tmoves = ((tmoves<<1) + (tmoves<<2)) / 3;
	}

#endif  

	return tmoves;
}

sint32 iHero::MoveCostBonus() const
{
	// 0 - 100;
	return FurtSkill(FSK_PATHFINDING);
}

const sint32 DEF_LEVEL_BOUNDS[12] = { 0, 1000, 2000, 3200, 4600, 6200, 8000, 10000, 12200, 14700, 17500, 20600 };

sint32 iHero::LevelBound(uint32 level) 
{ 
	// old formula: return (level-1)*(level-1)*500; 
	if (level < 2) return 0;
	if (level <= 12) return DEF_LEVEL_BOUNDS[level-1];

	sint32 cdif = (DEF_LEVEL_BOUNDS[11] - DEF_LEVEL_BOUNDS[10]);
	sint32 val = DEF_LEVEL_BOUNDS[11];

	for (uint32 nn=12; nn<level; ++nn) {    
		cdif += cdif / 5;
		val += cdif;
	}
	return val;
}

sint32 iHero::FurtSkill(FURTHER_SKILLS furtSkill) const 
{ 
	sint32 res = m_constFurtSkills.Value(furtSkill);
	res += m_varFurtSkills.Value(furtSkill);
	if (m_pOwner) res += m_pOwner->FurtSkills().Value(furtSkill);
	return  res;
}

iFurtSkills iHero::GetFullFurtSkills() const 
{ 
	iFurtSkills res = m_constFurtSkills + m_varFurtSkills;
	if (m_pOwner) res += m_pOwner->FurtSkills();
	return  res; 
}

	
sint32 iHero::TNLPerc() const
{
	sint32 needed = LevelBound(Level()+1) -  LevelBound(Level()); 
	sint32 acq = Experience() - LevelBound(Level());

	return iCLAMP<sint32>(0, 100, acq * 100 / needed);
}
sint32 iHero::ReceiveExperience(sint32 points)
{	
	if(m_bNoExping) return 0;

	sint32 oldperc = TNLPerc();
	
	m_Experience += points;
	
	while (ReachNextLevel()) {
		// show the progress until end
		gGame.MainView()->OnHeroReceivedExperience(this, oldperc, 100);	
		
		m_Level++;
		iNewLevelInfo nli = NextLevelInfo();
		m_constFurtSkills.Value((FURTHER_SKILLS)(FSK_ATTACK+nli.prSkillType))++;
		if (m_pOwner == gGame.Map().ActPlayer()) {
			gSfxMgr.PlaySound(CSND_EXP);
			iDlg_NewLevel dlg(&gApp.ViewMgr(), this, nli);
			dlg.DoModal();
			SECONDARY_SKILLS ss = dlg.SecSkill();
			if (ss != SECSK_NONE) m_SecSkills.Inc(ss);
		} else {
			check(Owner()->PlayerType() == PT_COMPUTER);
			if ( 1 /* difficulty <= normal */ ) {
				if (nli.secSkill[0] != SECSK_NONE && nli.secSkill[1] != SECSK_NONE) m_SecSkills.Inc(nli.secSkill[gGame.Map().Rand(2)]);
				else if (nli.secSkill[0] != SECSK_NONE) m_SecSkills.Inc(nli.secSkill[0]);
				else if (nli.secSkill[1] != SECSK_NONE) m_SecSkills.Inc(nli.secSkill[1]);
			} else {
				// more smart upgrade policy
				// sort offered skills according the order of preference
				// than by upgrade/add status
				// pick the first
			}
		}
		
		oldperc = 0;
	}

	// just move the bar a bit
	gGame.MainView()->OnHeroReceivedExperience(this, oldperc, TNLPerc());
	gGame.MainView()->OnHeroFinishedReceivingExperience(this);
	
	if (m_pOwner == gGame.Map().ActPlayer()) gGame.AddCellMsg(iFormat(_T("#I%04X+%d"),PDGG(DGLYPH_EXP),points), m_Pos);    
	return points;
}

sint32 iHero::MaxManaPts() const
{
	return (FurtSkill(FSK_KNOWLEDGE) * 10) + ((FurtSkill(FSK_KNOWLEDGE) * 10 * FurtSkill(FSK_INTELLIGENCE))/100);
}

void iHero::SetLocation(iMapCnst* pLocation) 
{
	if (m_pLocation == pLocation) return;
	iCastle* pCastle;

	if (pCastle = DynamicCast<iCastle*>(m_pLocation)) {
		pCastle->OnVisitorLeave(this);
	}

	m_pLocation = pLocation;

	if (pCastle = DynamicCast<iCastle*>(m_pLocation)) {
		pCastle->OnVisitorEnter(this);
	}
}

void iHero::CastSpell(MAGIC_SPELL spell)
{
	check(spell != MSP_INVALID && SPELL_DESCRIPTORS[spell].type == SPT_OVERLAND);

	iOverlandSpell* pSpell =  (iOverlandSpell*)gGame.ItemMgr().m_spellMgr.Spell(spell);

	uint8 cost = iBaseSpell::GetSpellCost(pSpell->Id(),this);
	if (m_Moves < cost
#ifdef MULTIPLAYER
		&& !gMPMgr.IsPRA() 
#endif
		) {
			iTextDlg tdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_MSG_TIRED_TO_CAST],m_pOwner->PlayerId());
			tdlg.DoModal();
			return;
	}

	if (pSpell->Cast(this)) {
		m_manaPts -= cost;      
		m_pOwner->SetHeroMovedFlag();

		m_Moves -= cost;

#ifdef MULTIPLAYER      
		//if(!gMPMgr.IsPRA())
		gMPMgr.OnHeroUpdate(this);
#endif

	}
}

void iHero::AddCharm(MAGIC_SPELL spell, sint32 flag)
{
	m_charms.Add(iCharmEntry(spell,flag));
#ifdef MULTIPLAYER
	gMPMgr.OnCharmAdded(this, spell, flag);
#endif
}

void iHero::NewDay()
{
	// Remove "next day" enchancers
	m_Enchs.RemoveDur(ENDUR_NEXTDAY);

	// Restore moves
	m_Moves = TotalMoves();

	// Reset "can dig" flag
	m_bCanDig = true;

	// Restore mana points
	if (m_manaPts < MaxManaPts()) {
		if (HasSpecFlag(SHF_MANARESTORE)) {
			m_manaPts = MaxManaPts();
		} else {
			m_manaPts = iMIN<sint32>(MaxManaPts(),m_manaPts + DEF_HERO_MYSTICISM + FurtSkill(FSK_MANAPTS));
		}
	}

	// Increase minerals without owner's income
	for (uint32 xx=0; xx<MINERAL_COUNT; ++xx) m_pOwner->AddMineral((MINERAL_TYPE)xx, m_constFurtSkills.Value((FURTHER_SKILLS)(FSK_MIN_GOLD+xx)) + m_varFurtSkills.Value((FURTHER_SKILLS)(FSK_MIN_GOLD+xx)));

	// Remove all charms
	for (uint32 xx=0; xx<m_charms.GetSize(); ++xx) {
		MAGIC_SPELL spell = m_charms[xx].spellId;
		check(spell != MSP_INVALID && SPELL_DESCRIPTORS[spell].type == SPT_OVERLAND);
		iOverlandSpell* pSpell =  (iOverlandSpell*)gGame.ItemMgr().m_spellMgr.Spell(spell);
		pSpell->OnRemove(this, m_charms[xx].flag);
	}
	m_charms.RemoveAll();

	// CPERK_MISER (+1$ each day)
	uint32 total_add = 0; 
	for (uint32 i = 0; i < 7; i++)
	{
		const iCreatGroup &cg = Army().At(i);
		if (CREAT_DESC[cg.Type()].perks & CPERK_MISER)
		{            
			total_add += cg.Count();
		}        
	}
	if (total_add) m_pOwner->AddMineral(MINERAL_GOLD, total_add, false);
}

void iHero::NewWeek(const iWeekDesc& weekDesk)
{
	m_Enchs.RemoveDur(ENDUR_NEXTWEEK);

	// this is a fix for 'Stables bonus move' problem #0000203
	// its caused by wrong order of NewDay / NewWeek calls
	// where NewWeek cancels stables effect but does not recalculate move points
	// normally I would refactor NewDay/NewWeek into one function with parameter
	// or factor out effects/updates separately but for now simply...
	// NB: Calling it twice also affects anti-hack trick in a bad way:(
	m_Moves = TotalMoves();
}

void iHero::OnEndBattle(bool bWin)
{
	m_Enchs.RemoveDur(ENDUR_NEXTBATTLE);
	
	if (bWin && m_pOwner->PlayerType() == PT_HUMAN) {
		m_pOwner->SetCurHero(this);
		gGame.OnHeroChanged(this);
	}
}

void iHero::OnSelect(bool bAfterLoad)
{
	//
	//if (m_pOwner == gGame.Map().ActPlayer()) gGame.AddMsg(iFormat(gTextMgr[TRID_MSG_HEROSELECTED].CStr(),Name().CStr(),gTextMgr[TRID_HERO_TYPE_KNIGHT+Type()].CStr(),Level()));
	gGame.OnHeroChanged(this);
	// Recalculate path
	if (m_pOwner->PlayerType() == PT_COMPUTER) {
		m_Path.SetEmpty(Pos());
	} else if (!m_Path.IsEmpty()) {
		iPoint dpos = m_Path.DestPoint();
		m_Path.SetEmpty(Pos());
		SetPath(dpos);
#ifdef ROYAL_BOUNTY
		if(!bAfterLoad)
			SetPath();
		//StartMoving();
#endif
	}
}

iPath::Target iHero::GetTarget(sint16 x, sint16 y, bool bIgnoreFog)
{
	iPath::Target res;
	iBaseMapObject* pObj = gGame.Map().m_CoverMap.GetAt(x,y);
	check(pObj);
	if (DynamicCast<iHero*>(pObj)) {
		if (bIgnoreFog || !Owner()->FogMap().IsHidden(iPoint(x,y))) {
			if (iHero* pHero = DynamicCast<iHero*>(pObj)) {
				if (pHero->Owner() == Owner()) res.action = iPath::Meet;
				else res.action = iPath::Attack;
				if (iMapCnst* pCnst = pHero->GetLocation()) {
					if (DynamicCast<iCastle*>(pCnst)) {
						res.epmask = 0x82;
					} else {
						res.epmask = 0x83;
					}
				}
			}
		}
	}
	else if(DynamicCast<iMapGuard*>(pObj) || DynamicCast<iMapItem_Mineral*>(pObj) || DynamicCast<iMapItem_ManaCrystal*>(pObj) || DynamicCast<iMapItem_CampFire*>(pObj) || DynamicCast<iMapItem_Chest*>(pObj) || DynamicCast<iMapItem_Lamp*>(pObj) || DynamicCast<iMapItem_Artifact*>(pObj) || DynamicCast<iMapItem_SpellScroll*>(pObj)  || DynamicCast<iMapItem_KeyGuard*>(pObj)) {
	 	res.epmask = 0xFF;
		if (DynamicCast<iMapGuard*>(pObj)) {
			res.action = iPath::Attack;
		} else {
			res.action = iPath::Touch;
		}
		
	} else if (DynamicCast<iVisCnst*>(pObj) ){
		res.action = iPath::Enter;
		res.epmask = 0x83;
	} else if (DynamicCast<iOwnCnst*>(pObj)) {
		res.action = iPath::Enter;
		res.epmask = 0x83;
	} else if (iCastle* pCastle = DynamicCast<iCastle*>(pObj)) {
		if (pCastle->Owner() != Owner()->PlayerId() && !pCastle->Garrison().Empty()) res.action = iPath::Attack;
		else res.action = iPath::Enter;
		res.epmask = 0x82;
	} 
	else if (iShip* pSnip = DynamicCast<iShip*>(pObj))
	{    
		if (!InShip())
		{
			// if hero isn't in ship, we can occupy ship
			// we can occupy it
			res.epmask = 0xFF;
			res.action = iPath::Occupy;
		}
		else
		{
			// otherwise action denied
			res.epmask = 0xFF;
			res.action = iPath::None;
		}
	}
	else
	{
		check(0);
	}

	return res;
}

bool iHero::MakePath(iPath& path, sint16 x, sint16 y)
{
	iHero* pOldHero = m_pOwner->m_pCurHero;
	m_pOwner->m_pCurHero = this;
	m_pOwner->UpdatePassMap();
	m_pOwner->m_pCurHero = pOldHero;

	// if we are in place, just activate the construction we are visiting
	if (m_Pos.x == x && m_Pos.y == y && !InShip()) {		
		check(m_pLocation);
		ActivateConstruction(m_pLocation);
		return true;
	}

	sint8 oval = m_pOwner->PassMap().GetAt(x,y);	

	// first, check if target is principally unreachable
	if (oval == PASSMAP_IMPASSABLE) return false;
	if (oval == PASSMAP_PWATER) return false; // disable target point on partial water (you can't stand on water: no Jesus allowed)

	iPath::Target tgt;

	// if cell is occupied, set target action
	if (oval == PASSMAP_OCCUPIED) 
	{
		tgt = GetTarget(x,y);

		// this can happen only if hero already in ship and target points to empty ship
		if (tgt.action == iPath::None)
		{			
			return false;
		}
	}

	uint8 spMask = 0xFF;
	if (m_pLocation)
	{
		if (DynamicCast<iVisCnst*>(m_pLocation) || DynamicCast<iOwnCnst*>(m_pLocation)) spMask = CNST_PASS_MASK;
		else if (DynamicCast<iCastle*>(m_pLocation)) spMask = CSTL_PASS_MASK;
	}

	bool bRes = false;
	iPathFinder pf(m_pOwner->PassMap());
	path.SetEmpty(Pos());	

	// an object at the target point
	iBaseMapObject* pMapObj = gGame.Map().m_CoverMap.GetAt(x, y);		

	if (InShip() && pMapObj && dynamic_cast<iVisCnst_Harbor*>(pMapObj) && gGame.Map().m_CoverMap.GetAt(pMapObj->Pos().x, pMapObj->Pos().y) == pMapObj)
	{		
		// first case: we are sailing and going to harbor, check the ability to land quick way
		// i.e. park ship in harbor's dock and go out without loosing turn	
		IHarbor* hrb = dynamic_cast<IHarbor*>(pMapObj);		
		bool found = false;
		iPoint dockPnt = cInvalidPoint; // the parking point of the dock found

		// first try to find dock, we are parked at
		for(uint32 xx=0; xx<hrb->NumOfDocks(); xx++) {
			if(hrb->GetShipPos(xx) == GetShip()->Pos()) {				
				dockPnt = hrb->GetShipPos(xx);
				found = true;
				break;
			}
		}

		// if not, try to find free dock on that harbor
		if(!found) {
			for(uint32 xx=0; xx<hrb->NumOfDocks(); xx++) {
				if(!hrb->GetShip(xx)) {		
					dockPnt = hrb->GetShipPos(xx);
					found = true;
					break;
				}
			}
		}

		// no docks to park at, cease operation
		if(!found) 
			return false;

		// try to make path to dock, if no way - fail the global idea
		if(Pos() != dockPnt) {
			sint8 oldpm = m_pOwner->PassMap().GetAt(dockPnt.x, dockPnt.y);
			m_pOwner->PassMap().GetAt(dockPnt.x, dockPnt.y) = PASSMAP_SWATER;			
			bool bPathToDock = pf.FindPathForShip(Pos(), dockPnt, path, spMask, tgt.epmask) != -1;
			m_pOwner->PassMap().GetAt(dockPnt.x, dockPnt.y) = oldpm;
			if(!bPathToDock) {
				path.SetEmpty(Pos());
				return false;
			}
			path.SetTargetAction(iPath::MoveTo);
		}
				
		// add harbor entrance path point
		path.AddMoveToPointShip(pMapObj->Pos(), SURF_MOVE_COST[STYPE_WATER], false);
		path.SetTargetAction(iPath::Enter);

		bRes = true;
	} else if(InShip()) { 		
		// second case: we are just sailing and want to do simple pathfinding

		// we can only Leave or MoveTo if target is on land, to prevent Jesuses
		if(gGame.Map().GetAt(x, y).surf != STYPE_WATER && tgt.action != iPath::MoveTo && tgt.action != iPath::Leave)
			return false;

		sint8 oldpm = m_pOwner->PassMap().GetAt(x, y);
		bool bReset = m_pOwner->PassMap().GetAt(x,y) == PASSMAP_OCCUPIED;
		if(bReset)
			m_pOwner->PassMap().GetAt(x,y) = PASSMAP_SWATER;		 
		bRes = pf.FindPathForShip(Pos(),iPoint(x,y),path,spMask, tgt.epmask) != -1;        				
		if(bReset)
			m_pOwner->PassMap().GetAt(x,y) = oldpm;

		if(bRes)
			path.SetTargetAction(iPath::Action(tgt.action));

		// drain all moves from a hero when leaving or boarding a ship																
		path.ProcessShipOperations(m_Moves);					

	} else if (tgt.action == iPath::Occupy) {				
		iShip *pShip = dynamic_cast<iShip*>(gGame.Map().m_CoverMap.GetAt(x, y));
		check(pShip);

		// try to find nearby harbor, this ship is parked at
		uint32 dock;
		iVisCnst_Harbor* hrb = pShip->GetHarborCnst(dock);

		// if harbor found and not occupied, try 'quick boarding' through it
		if(hrb && (Pos() == hrb->Pos() || gGame.Map().m_CoverMap.GetAt(hrb->Pos().x, hrb->Pos().y) == hrb)) {
		// if we can make path to its entrance (or in place), first go to harbor, then jump
			if(hrb->Pos() != Pos()) {
				tgt = GetTarget(hrb->Pos().x, hrb->Pos().y);
				sint8 oldpm = m_pOwner->PassMap().GetAt(hrb->Pos().x, hrb->Pos().y);
				bool bReset = m_pOwner->PassMap().GetAt(hrb->Pos().x, hrb->Pos().y) == PASSMAP_OCCUPIED;
				if(bReset)
					m_pOwner->PassMap().GetAt(hrb->Pos().x, hrb->Pos().y) = 5;		 

				bool bPathToHrb = pf.FindPath(Pos(), hrb->Pos(), path, spMask, tgt.epmask, false) != -1;
				
				if(bReset)
					m_pOwner->PassMap().GetAt(hrb->Pos().x, hrb->Pos().y) = oldpm;
				if(!bPathToHrb) {
					path.SetEmpty(Pos());
					return false;
				}
			}

			// path to harbor now set, add path point to ship
			path.AddMoveToPointShip(pShip->Pos(), SURF_MOVE_COST[STYPE_WATER], false);				
			path.SetTargetAction(iPath::Occupy);		
			bRes = true;	
		} else { // harbor not found, try usual boarding
			sint8 oldpm = m_pOwner->PassMap().GetAt(x, y);
			m_pOwner->PassMap().GetAt(x,y) = PASSMAP_SWATER;		 
			bRes = pf.FindPathToShip(Pos(),iPoint(x,y), path,spMask, tgt.epmask) != -1;
			m_pOwner->PassMap().GetAt(x,y) = oldpm;

			if(bRes) {
				path.SetTargetAction(iPath::Action(tgt.action));
				// drain all moves from a hero when leaving or boarding a ship																
				path.ProcessShipOperations(m_Moves);					
			}
		}
	}
	else {
		// just land pathfinding, no ships
		sint8 oldpm = m_pOwner->PassMap().GetAt(x, y);
		bool bReset = m_pOwner->PassMap().GetAt(x,y) == PASSMAP_OCCUPIED;
		if(bReset)
			m_pOwner->PassMap().GetAt(x,y) = 5;		 
		bRes = pf.FindPath(Pos(),iPoint(x,y),path,spMask, tgt.epmask, false) != -1;
		if(bReset)
			m_pOwner->PassMap().GetAt(x,y) = oldpm;
		if(bRes)
			path.SetTargetAction(iPath::Action(tgt.action));
	}
		
	// set the next path point to current pos
	m_NextPoint = Pos();

	return bRes;
}

bool iHero::SetPath()
{
	if (!m_Path.m_Path.IsEmpty()) { SetPath( m_Path.DestPoint(), true ); return true;}
	else if (m_pLocation && 
#ifndef MULTIPLAYER     
		m_Moves > 0
#else
		(!gMPMgr.IsPRA() && m_Moves > 0)
#endif      
		) { 
			m_pOwner->SetHeroMovedFlag();
			ActivateConstruction(m_pLocation);
			m_Moves--;
			return true;
	} else {
		m_Path.SetEmpty(m_Path.DestPoint());
		return false;
	}
}

void iHero::SetPath(const iPoint& cell, bool moveAllowed, uint32 radius)
{
	if (m_bMoving) {
		Stop();
	} else if ((m_Path.IsDestPoint(cell) && gSettings.GetInterfaceType() == IT_STYLUS) ||
			   (gSettings.GetInterfaceType() == IT_TOUCH && !m_Path.IsEmpty() && m_Path.DestPoint().GetDelta(cell) < radius && moveAllowed)) {        
		Step();
	} else if (gGame.Map().IsValidPos(cell.x,cell.y) && Pos() != cell) {
		//
		MakePath(m_Path, (sint16)cell.x, (sint16)cell.y);
	} else
		ResetPath();
}

void iHero::SetPath(const iPath& path)
{
	check(!m_bMoving);
	m_Path = path;
	m_NextPoint = Pos();
}

void iHero::ResetPath()
{
	m_Path.SetEmpty(Pos());
}

VISION_LEVEL iHero::VisionLevel(sint32& radius) const
{
	radius = 0;
	VISION_LEVEL vl = VL_NONE;
	if (CharmedBy(MSP_VISIONS, radius)) {
		vl = VL_EXPERT;
	} else if (Army().HasCreatures(CREAT_ROGUE)) {
		radius = 5;
		vl =  VL_EXPERT;
	} else if (m_pOwner->CastleCount()) {
		vl = VL_BASIC;
	}
	if((vl - VL_NONE) < SecSkills().SkillLevel(SECSK_SCOUTING) - SSLVL_BASIC) {
		vl = (VISION_LEVEL)(VL_NONE + (SecSkills().SkillLevel(SECSK_SCOUTING) - SSLVL_BASIC));
		radius = 5;
	}
	return vl;
}

void iHero::ResetHeroArmy(bool bAfterLoad)
{
	ResetArmy();

	if(!bAfterLoad) {
		sint8 ht = m_pProto->m_hType;
		sint32 rcnt = INITIAL_HERO_ARMY[ht][0][1] + gGame.Map().Rand(INITIAL_HERO_ARMY[ht][0][2]-INITIAL_HERO_ARMY[ht][0][1]);
		AddGroupToArmy((CREATURE_TYPE)INITIAL_HERO_ARMY[ht][0][0], rcnt);
		rcnt = INITIAL_HERO_ARMY[ht][1][1] + gGame.Map().Rand(INITIAL_HERO_ARMY[ht][1][2]-INITIAL_HERO_ARMY[ht][1][1]);
		if (rcnt) AddGroupToArmy((CREATURE_TYPE)INITIAL_HERO_ARMY[ht][1][0], rcnt);
	}
}

void iHero::ActivateConstruction(iMapCnst* pCnst)
{
	bool bDisappear = false;
	pCnst->Activate(this, m_pOwner == gGame.Map().ActPlayer(), bDisappear);
	if (m_bDead) return;
	if(bDisappear) {
#ifdef MULTIPLAYER
FIXME: there should be MP command to remove objects as well
#endif
		   //gGame.OnDisapObject(pCnst->Sprite(),pCnst->Pos());
		   if(iOwnCnst* pOwnCnst = DynamicCast<iOwnCnst*>(pCnst))
			   gGame.Map().RemoveOwnCnst(pOwnCnst);
		   else if(iVisCnst* pVisCnst = DynamicCast<iVisCnst*>(pCnst))
			   gGame.Map().RemoveVisCnst(pVisCnst);		
	   gGame.Map().InitPassMap();
	   m_pOwner->UpdatePassMap();
	} 	
	m_pOwner->UpdateVisItems();
}

void iHero::SiegeCastle(iCastle* pCastle)
{
	check(pCastle->Owner() != m_pOwner->PlayerId());
	check(!pCastle->Garrison().Empty() || pCastle->Visitor());
	iBattleInfo bi(this, pCastle);
#ifdef MULTIPLAYER
	gMPMgr.OnSiegeCastle(this, pCastle);
#endif
	gGame.BeginBattle(bi);
}

void iHero::CaptureCastle(iCastle* pCastle, bool bEnter)
{
	iPlayer* oldPlayer = NULL;	
	if (pCastle->Owner() != PID_NEUTRAL) {
		oldPlayer = gGame.Map().FindPlayer(pCastle->Owner());
		check(oldPlayer);
		pCastle->SetOwner(PID_NEUTRAL);
		oldPlayer->RemoveCastle(pCastle);		
	} else {
		gGame.Map().m_Castles.Remove(iGameWorld::iCtIt(pCastle));
	}
	
	pCastle->SetOwner(m_pOwner->PlayerId());	
	bool bGameResult;
	m_pOwner->AddCastle(pCastle, bGameResult, true);
	m_pOwner->OnCaptureCastle(pCastle);

#ifdef MULTIPLAYER
	gMPMgr.OnCastleCaptured(pCastle, m_pOwner->PlayerId());
#endif
	
	if (bEnter && !bGameResult) {
		EnterCastle(pCastle);
	}
}

void iHero::EnterCastle(iCastle* pCastle)
{
	if (Owner() == gGame.Map().ActPlayer()) {
		/*if (m_pOwner->CurCastle() != pCastle) {
			m_pOwner->SetCurCastle(pCastle);
		}*/
#ifdef MULTIPLAYER
		if(!gMPMgr.IsReplaying())
#endif
		{
			gGame.CastleView()->SetCastle(pCastle);
			gGame.ShowView(iChildGameView::CV_CASTLE);
		}

	} 
}

void iHero::TouchMapItem(iMapItem* pMapItem, bool bPostBattle)
{
	if (pMapItem->Activate(this, m_pOwner == gGame.Map().ActPlayer(), !bPostBattle)){
		gGame.OnDisapObject(pMapItem->Sprite(), pMapItem->Pos());
		gGame.Map().RemoveMapItem(pMapItem);
		m_pOwner->UpdateVisItems();
	}
}

void iHero::TouchMapGuard(iMapGuard* pMapGuard, bool bPostBattle)
{
	if (bPostBattle || pMapGuard->Activate(this, m_pOwner == gGame.Map().ActPlayer())){
		if (m_pOwner == gGame.Map().ActPlayer()) {
			gSfxMgr.PlaySound(CSND_DEL_GUARD);
		}
		gGame.OnDisapObject(pMapGuard->Sprite(),pMapGuard->Pos());
		gGame.Map().RemoveMapGuard(pMapGuard);
		m_pOwner->UpdateVisItems();
	}
}

void iHero::AttackHero(iHero* pHero)
{
	iBattleInfo bi(this, pHero);
#ifdef MULTIPLAYER
	gMPMgr.OnAttackHero(this, pHero);
#endif
	gGame.BeginBattle(bi);
}

void iHero::MeetHero(iHero* pHero)
{
	check(m_pOwner->PlayerId() == pHero->Owner()->PlayerId());
	if (m_pOwner->PlayerType() == PT_HUMAN) {
		gGame.MeetHeroes(this, pHero, true);
	} else if(m_pOwner->PlayerType() == PT_COMPUTER) {
		iAI_Player* pAIPlyer = DynamicCast<iAI_Player*>(m_pOwner);
		// check(pAIPlyer && pAIPlyer->NeedMeeting(this, pHero));
		pAIPlyer->MeetHeroes(this, pHero);
	}
}

void iHero::SetPos(const iPoint& pos)
{
	if (m_pShip) m_pShip->SetPos(pos);
	iBaseMapObject::SetPos(pos);
}

bool iHero::MeetsReqs(iReqsCtr& reqs)
{
	bool reqfail = false;
	for(uint32 xx=0; xx<reqs.GetSize(); xx++)
	{
		switch(reqs[xx].m_type) {
			case RQT_MINERAL:
				if(Owner()->GetMineral((MINERAL_TYPE)reqs[xx].m_fParam) < reqs[xx].m_sParam)
					reqfail = true;
				break;
			case RQT_MANAPTS: 
				if(ManaPts() < reqs[xx].m_sParam)
					reqfail = true;
				break;
			case RQT_TRAVELPTS: 
				if(Moves() < reqs[xx].m_sParam)
					reqfail = true;
				break;
			case RQT_ARTIFACT: 
				if(reqs[xx].m_fParam == ARTT_ULTIMATE) {
					if(!Artifacts().HasArtType(ARTT_ULTIMATE))
						reqfail = true;
				}
				else if(!Artifacts().HasArtifact(reqs[xx].m_fParam))
					reqfail = true;
				break;
			case RQT_MORALE: 
				if(FurtSkill(FSK_MORALE) < reqs[xx].m_sParam)
					reqfail = true;
				break;
			case RQT_DATE: 
				if((reqs[xx].m_sParam == 0 && gGame.Map().GameDay() < reqs[xx].m_fParam) || 
					(reqs[xx].m_sParam == 1 && gGame.Map().GameDay() > reqs[xx].m_fParam))
					reqfail = true;
				break;
			case RQT_LUCK:
				if(FurtSkill(FSK_LUCK) < reqs[xx].m_sParam)
					reqfail = true;
				break;
			case RQT_FURTSKILL:
				if(FurtSkill((FURTHER_SKILLS)reqs[xx].m_fParam) < reqs[xx].m_sParam)
					reqfail = true;
				break;
			case RQT_SECSKILL: 
				if(SecSkills().SkillLevel((SECONDARY_SKILLS)reqs[xx].m_fParam) < reqs[xx].m_sParam)
					reqfail = true;				
				break;	
			case RQT_MAGICSPELL: 
				if(!HasSpell(reqs[xx].m_fParam))
					reqfail = true;
				break;
			case RQT_CREATGROUP:
				if(Army().CreatureCount((CREATURE_TYPE)reqs[xx].m_fParam) < reqs[xx].m_sParam)
					reqfail = true;
				break;
			case RQT_HERO: 
				if(Proto()->m_protoId != reqs[xx].m_fParam)
					reqfail = true;
				break;
			case RQT_LEVEL: 
				if(Level() < reqs[xx].m_fParam)
					reqfail = true;
				break;
		}
	}
	return !reqfail;
}

bool iHero::PermanentlyFailedReqs(iReqsCtr& reqs)
{
	for(uint32 xx=0; xx<reqs.GetSize(); xx++)
	{
		if(reqs[xx].m_type == RQT_DATE && reqs[xx].m_sParam == 1 && gGame.Map().GameDay() > reqs[xx].m_fParam)
			return true;
	}
	return false;
}

uint32 iHero::ReceiveRewards(const iRewardsCtr& rewards, bool showMsg, const iCharT* title, const iCharT* text)
{
	if(rewards.IsEmpty()) return 0;

	iRewardsCtr received;
	iRewardItem *temp;		
	for (uint32 xx=0; xx<rewards.GetSize(); ++xx) {
		if((temp = ReceiveReward(rewards[xx]))) {
			if((temp->m_type != RIT_ARTIFACT || temp->m_sParam != 0)) {
				received.Add(*temp);									
			}							
		}
	}

	// Show Reward message
	if (showMsg) {			
		iStringT test(text);
		if(!test.Empty()) {
			iDlg_Rewards dlg(&gApp.ViewMgr(), title, text, this, received);
			dlg.DoModal();			
		}
	}

	for(uint32 xx=0; xx<received.GetSize(); xx++)
		FinalizeReward(&received[xx]);	
	return received.GetSize();
}

bool iHero::ReachNextLevel() const 
{ 
	return LevelBound(m_Level+1) <= m_Experience && (gGame.Map().LevelCap() == 0 || m_Level < gGame.Map().LevelCap()); 
}

void iHero::LearnSpell(uint16 spellId, bool bForce) 
{ 
	check(bForce || CanLearnSpell(spellId)); 
	m_spellBook.Add(spellId); 
#ifdef MULTIPLAYER
	gMPMgr.OnHeroUpdate(pHero);
#endif

	gGame.CheckPlayerWinAndLose(m_pOwner);	
}

bool iHero::IsCrucial()
{
	const iLoseCondition& lc = gGame.Map().LoseCondition();
    bool bOnlyHero = m_pOwner->HeroCount() == 1 && m_pOwner->CastleCount() == 0;
	return bOnlyHero || (lc.type == LC_LOSE_HERO && Proto()->m_protoId == lc.fparam && m_pOwner->PlayerId() == lc.sparam);
}

uint16 GetMoveCost(uint16 pm, uint8 dir);
iPoint GetDirOffset(uint8 dir);


bool iHero::Exhausted()
{
	if(m_bMoving) return false;

	for(sint32 xx=-2; xx<=2; xx++)
		for(sint32 yy=-2; yy<=2; yy++) 
			if((xx != 0 || yy != 0) && gGame.Map().IsValidPos(Pos().x + xx, Pos().y + yy))
			{
				iPath path(Pos());
				if(MakePath(path, Pos().x + xx, Pos().y + yy) && path.GetStepCost() <= m_Moves)
					return false;
			}
			
	return true;
}


// END OF FILE
