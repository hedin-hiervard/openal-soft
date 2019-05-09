#include "stdafx.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif

//#include "xxc/wce.detects.h"
#include "Dlg_LeaveGuards.h"
 

/*
 *	Battle Member
 */
iBattleMember::iBattleMember(Type type, Side side, const iFurtSkills& furtSkills)
: m_Type(type), m_Side(side), m_furtSkills(furtSkills), m_Experience(0), m_bCastFlag(false)
{}

#ifdef MULTIPLAYER
void iBattleMember::Update() {}
#endif

//////////////////////////////////////////////////////////////////////////
iBattleMember_Hero::iBattleMember_Hero(iHero* pHero, Side side)
: iBattleMember(Hero, side, pHero->GetFullFurtSkills()), m_pHero(pHero)
{}

void iBattleMember_Hero::OnWin(iBattleResult br)
{
	// transfer artifacts from loser
	if (br.m_defCause == DC_DEFEAT) {
		iHero* pHero = NULL;
		if (iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pLoser)) pHero = pHeroMember->GetHero();
		else if (iBattleMember_Castle* pCastleMember = DynamicCast<iBattleMember_Castle*>(br.m_pLoser)) pHero = pCastleMember->GetVisitor();
		if (pHero) {
			iArtList artList;
			pHero->Artifacts().PopAll(artList);
			if (artList.GetSize()) m_pHero->CaptureArtifacts(artList);
		}
	}

	// Raise skeletons
	m_pHero->RiseSkeletons(br.m_losCas, br.m_losHpCas);
	
	// receive experience
	if (br.m_experience) m_pHero->ReceiveExperience(br.m_experience);

	m_pHero->OnEndBattle(true);
}

void iBattleMember_Hero::OnLose(iBattleResult br)
{
	m_pHero->OnEndBattle(false);
	//iPlayer* pOwner = m_pHero->Owner();
	if (br.m_defCause == DC_SURRENDER || br.m_defCause == DC_RETREAT) {
		// Check destination castle
		check(br.m_pDest && !br.m_pDest->Visitor());
		iPoint oldPos = m_pHero->Pos();
		if (br.m_defCause == DC_RETREAT) {
			// Reset army
			m_pHero->ResetArmy();
			m_pHero->AddGroupToArmy((CREATURE_TYPE)INITIAL_HERO_ARMY[m_pHero->Proto()->m_hType][0][0],1);
		} else {
			// Transfer surrender fee
			check(br.m_cost && m_pHero->Owner()->Mineral(MINERAL_GOLD) >= br.m_cost);
			iBattleMember_Hero* pEnemyHero = DynamicCast<iBattleMember_Hero*>(br.m_pWinner);
			check(pEnemyHero);
			m_pHero->Owner()->AddMineral(MINERAL_GOLD, -(sint32)br.m_cost);
			pEnemyHero->GetHero()->Owner()->AddMineral(MINERAL_GOLD, br.m_cost);
		}
		// Move hero in specified castle
		gGame.Map().MoveHero(m_pHero->Pos(), br.m_pDest->Pos());
		m_pHero->SetPos(br.m_pDest->Pos());
		m_pHero->Angle() = HERO_ORIENT[br.m_pDest->Proto()->Orient()];
		m_pHero->ResetPath();
		//m_pHero->Init(pOwner, br.m_pDest->Pos(), HERO_ORIENT[br.m_pDest->Proto()->Orient()]);
		//gGame.Map().MoveHero(oldPos, br.m_pDest->Pos());
		if (m_Side == Assaulter) gGame.OnHeroChanged(m_pHero);
	} else {
		// Reset army
		gGame.Map().RemoveHero(m_pHero, true);		
		iPlayer* winner = gGame.Map().FindPlayer(br.m_pWinner->Owner());		
		
		// winner got the victory condition 'defeat hero' accomplished and therefore immediately wins the game
		gGame.CheckPlayerWinAndLose(winner);

		// loser is check automatically in iPlayer::RemoveHero
	}
}

#ifdef MULTIPLAYER
void iBattleMember_Hero::Update()
{
	gMPMgr.OnHeroUpdate(m_pHero);
}
#endif

//////////////////////////////////////////////////////////////////////////
#define SAFE_CALL(pObj, func, defVal) (pObj)?pObj->func():defVal

iBattleMember_Castle::iBattleMember_Castle(iCastle* pCastle)
: iBattleMember(Castle, Defender, SAFE_CALL(pCastle->Visitor(), GetFullFurtSkills, iFurtSkills())), m_pCastle(pCastle)
{}

void iBattleMember_Castle::OnPrepare(iBattleMember* pEnemy)
{
	if (m_pCastle->Visitor() && !m_pCastle->Garrison().Empty()) {
		iPlayer *pPlayer = gGame.Map().FindPlayer(m_pCastle->Owner());
		check(pPlayer);
#ifdef MULTIPLAYER
			toss?
#endif
		// Combine hero army and castle's garrison for AI player
		if(pPlayer->PlayerType() == PT_COMPUTER)
			TossArmies(m_pCastle->Visitor(), m_pCastle, ATF_TYPE_SPEED|ATF_TYPE_POWER, 0);
		// or popup a dialog for human
		else if(pPlayer->PlayerType() == PT_HUMAN) {
			iDlg_LeaveGuards dlg(&gApp.ViewMgr(), m_pCastle, m_pCastle->Visitor(), TRID_CHOOSE_CASTLE_DEFENDERS);
			dlg.DoModal();
		}
	}
}

void iBattleMember_Castle::OnWin(iBattleResult br)
{
	iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pLoser);
	check(pHeroMember);

	if (iHero* pVisitor = GetVisitor()) {
		// transfer artifacts from loser
		if (br.m_defCause == DC_DEFEAT) {
			iArtList artList;
			pHeroMember->GetHero()->Artifacts().PopAll(artList);
			if (artList.GetSize()) pVisitor->CaptureArtifacts(artList);
		}

		// Raise skeletons
		pVisitor->RiseSkeletons(br.m_losCas, br.m_losHpCas);
		
		// receive experience
		if (br.m_experience) pVisitor->ReceiveExperience(br.m_experience);

		pVisitor->OnEndBattle(true);
	}
}

void iBattleMember_Castle::OnLose(iBattleResult br)
{
	check(br.m_defCause == DC_DEFEAT);
	iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pWinner);
	check(pHeroMember);

	if (iHero* pVisitor = GetVisitor()) {
		pVisitor->OnEndBattle(false);
		gGame.Map().RemoveHero(pVisitor, true);
		// Reset castle's garrison
		m_pCastle->ResetArmy();
	}

	iHero* pHero = pHeroMember->GetHero();
	// flag the castle with winner's playerId
	pHero->CaptureCastle(GetCastle(), false);	
}

#ifdef MULTIPLAYER
void iBattleMember_Castle::Update()
{
	gMPMgr.OnCastleUpdate(m_pCastle);
}
#endif


//////////////////////////////////////////////////////////////////////////
iBattleMember_OwnCnst::iBattleMember_OwnCnst(iOwnCnst* pOwnCnst)
: iBattleMember(OwnCnst, Defender, iFurtSkills()), m_pOwnCnst(pOwnCnst)
{}

void iBattleMember_OwnCnst::OnLose(iBattleResult br)
{
	iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pWinner);
	check(pHeroMember);
	iHero* pHero = pHeroMember->GetHero();
	// flag the ownerable construction with winner's playerId	
	pHero->ActivateConstruction(GetOwnCnst());
}

#ifdef MULTIPLAYER
void iBattleMember_OwnCnst::Update()
{
	gMPMgr.OnOwnCnstUpdate(m_pOwnCnst);
}
#endif

//////////////////////////////////////////////////////////////////////////
iBattleMember_VisCnst::iBattleMember_VisCnst(iVisCnst* pVisCnst)
: iBattleMember(VisCnst, Defender, iFurtSkills()), m_pVisCnst(pVisCnst)
{}

void iBattleMember_VisCnst::OnLose(iBattleResult br)
{
	iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pWinner);
	check(pHeroMember);
	iHero* pHero = pHeroMember->GetHero();
	// Activate visitable construction
	pHero->ActivateConstruction(GetVisCnst());
}

#ifdef MULTIPLAYER
void iBattleMember_VisCnst::Update()
{
	gMPMgr.OnVisCnstUpdate(m_pVisCnst);
}
#endif

//////////////////////////////////////////////////////////////////////////
iBattleMember_MapItem::iBattleMember_MapItem(iMapItem* pMapItem)
: iBattleMember(MapItem, Defender, iFurtSkills()), m_pMapItem(pMapItem)
{}

void iBattleMember_MapItem::OnLose(iBattleResult br)
{
	iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pWinner);
	check(pHeroMember);
	iHero* pHero = pHeroMember->GetHero();
	//
	pHero->TouchMapItem(GetMapItem(), true);
}

#ifdef MULTIPLAYER
void iBattleMember_MapItem::Update()
{
	gMPMgr.OnMapItemUpdate(m_pMapItem);
}
#endif

//////////////////////////////////////////////////////////////////////////
iBattleMember_MapEvent::iBattleMember_MapEvent(iMapEvent* pMapEvent)
: iBattleMember(MapEvent, Defender, iFurtSkills()), m_pMapEvent(pMapEvent)
{}

void iBattleMember_MapEvent::OnLose(iBattleResult br)
{
	iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pWinner);
	check(pHeroMember);
	iHero* pHero = pHeroMember->GetHero();
	
	iArmy tmp = Army();
	m_pMapEvent->ResetArmy();

	bool stopMoving = false;
	if (m_pMapEvent->Activate(pHero, pHero->Owner()->PlayerType() == PT_HUMAN, false, stopMoving)) {
		if(!(m_pMapEvent->Flags() & MEF_REUSABLE)) 
			gGame.Map().RemoveMapEvent(m_pMapEvent);
		else
			m_pMapEvent->SetArmy(tmp);
	}
}

#ifdef MULTIPLAYER
void iBattleMember_MapEvent::Update()
{	
}
#endif

//////////////////////////////////////////////////////////////////////////
iBattleMember_MapGuard::iBattleMember_MapGuard(iMapGuard* pMapGuard)
: iBattleMember(MapGuard, Defender, iFurtSkills()), m_pMapGuard(pMapGuard)
{
	// TRICK: double number of guards as a time goes by
	
	SetArmyGroup(0, pMapGuard->Creatures());
	// TRICK: initialize hacking protection (inactive state first)
/*#if defined(NEED_REGISTRATION) && defined(UNDER_CE)
	uint32 dayParam = (1 + gGame.Map().GameDay() / 11);
	if ( (dayParam > 1) 
			&& (gGame.Map().Rand(7) < 4)  // note - this is a sections number
			&& ((113 * xxc::check_sections_pe() + 1) < 679) ) {

		m_Army.At(0).Count() *= dayParam;
	}
#endif
	*/
}

void iBattleMember_MapGuard::OnPrepare(iBattleMember* pEnemy)
{
	// can make a trick here - multiply army here at random
	iCreatGroup cg = ArmyOwner()->Army().At(0);
	ResetArmy();
	check(cg.IsValid());
	sint32 ePwr = iMAX<uint32>(pEnemy->ArmyOwner()->Army().ArmyPower(), 1);
	sint32 fPwr = cg.GroupPower();
	// Distribute creatures group
	uint32 gcnt = iMIN<uint32>(cg.Count(), iCLAMP<sint32>(1,6,(fPwr*4)/ePwr));
	uint32 gqnt = cg.Count() / gcnt;
	uint32 ovr = cg.Count() % gcnt;
	for (uint32 xx=0; xx<gcnt; ++xx) {
		SetArmyGroupType(xx, cg.Type());
		SetArmyGroupCount(xx, (sint32)gqnt);
		if (xx<ovr) SetArmyGroupCount(xx , Army().At(xx).Count() + 1);
	}
}

void iBattleMember_MapGuard::OnWin(iBattleResult br)
{
	// Combine creatures
	sint32 ncnt = 0;
	for (uint32 xx=0; xx<7; ++xx) if (Army().At(xx).IsValid()) ncnt += Army().At(xx).Count();
	check(ncnt > 0);
	m_pMapGuard->Creatures().Count() = ncnt;
}

void iBattleMember_MapGuard::OnLose(iBattleResult br)
{
	iBattleMember_Hero* pHeroMember = DynamicCast<iBattleMember_Hero*>(br.m_pWinner);
	check(pHeroMember);
	iHero* pHero = pHeroMember->GetHero();
	pHero->TouchMapGuard(GetMapGuard(), true);
}

#ifdef MULTIPLAYER
void iBattleMember_MapGuard::Update()
{
	gMPMgr.OnMapGuardUpdate(m_pMapGuard);
}
#endif


/*
 *	Battle Info
 */
iBattleInfo::iBattleInfo()
: m_pAssaulter(NULL), m_pDefender(NULL)
{}

iBattleInfo::iBattleInfo(iHero* pAssaulter, iHero* pDefender) 
{
	m_pAssaulter = new iBattleMember_Hero(pAssaulter, iBattleMember::Assaulter);
	m_pDefender = new iBattleMember_Hero(pDefender, iBattleMember::Defender);
	m_pAssaulter->OnPrepare(m_pDefender);
	m_pDefender->OnPrepare(m_pAssaulter);
}

iBattleInfo::iBattleInfo(iHero* pAssaulter, iCastle* pDefender)
{
	m_pAssaulter = new iBattleMember_Hero(pAssaulter, iBattleMember::Assaulter);
	m_pDefender = new iBattleMember_Castle(pDefender);
	m_pAssaulter->OnPrepare(m_pDefender);
	m_pDefender->OnPrepare(m_pAssaulter);
}

iBattleInfo::iBattleInfo(iHero* pAssaulter, iOwnCnst* pDefender) 
{
	m_pAssaulter = new iBattleMember_Hero(pAssaulter, iBattleMember::Assaulter);
	m_pDefender = new iBattleMember_OwnCnst(pDefender);
	m_pAssaulter->OnPrepare(m_pDefender);
	m_pDefender->OnPrepare(m_pAssaulter);
}

iBattleInfo::iBattleInfo(iHero* pAssaulter, iVisCnst* pDefender) 
{
	m_pAssaulter = new iBattleMember_Hero(pAssaulter, iBattleMember::Assaulter);
	m_pDefender = new iBattleMember_VisCnst(pDefender);
	m_pAssaulter->OnPrepare(m_pDefender);
	m_pDefender->OnPrepare(m_pAssaulter);
}

iBattleInfo::iBattleInfo(iHero* pAssaulter, iMapItem* pDefender) 
{
	m_pAssaulter = new iBattleMember_Hero(pAssaulter, iBattleMember::Assaulter);
	m_pDefender = new iBattleMember_MapItem(pDefender);
	m_pAssaulter->OnPrepare(m_pDefender);
	m_pDefender->OnPrepare(m_pAssaulter);
}

iBattleInfo::iBattleInfo(iHero* pAssaulter, iMapEvent* pDefender) 
{
	m_pAssaulter = new iBattleMember_Hero(pAssaulter, iBattleMember::Assaulter);
	m_pDefender = new iBattleMember_MapEvent(pDefender);
	m_pAssaulter->OnPrepare(m_pDefender);
	m_pDefender->OnPrepare(m_pAssaulter);
}

iBattleInfo::iBattleInfo(iHero* pAssaulter, iMapGuard* pDefender) 
{
	m_pAssaulter = new iBattleMember_Hero(pAssaulter, iBattleMember::Assaulter);
	m_pDefender = new iBattleMember_MapGuard(pDefender);
	m_pAssaulter->OnPrepare(m_pDefender);
	m_pDefender->OnPrepare(m_pAssaulter);
}

void iBattleInfo::Cleanup()
{
	delete m_pAssaulter;
	m_pAssaulter = NULL;
	delete m_pDefender;
	m_pDefender = NULL;
}

iBattleMember::Side iBattleInfo::InteractorSide() const
{
	if (m_pAssaulter->Owner() != PID_NEUTRAL && gGame.Map().FindPlayer(m_pAssaulter->Owner())->PlayerType() == PT_HUMAN) return iBattleMember::Assaulter;
	check( m_pDefender->Owner() != PID_NEUTRAL && gGame.Map().FindPlayer(m_pDefender->Owner())->PlayerType() == PT_HUMAN);
	return iBattleMember::Defender;
}

iHero* iBattleInfo::HasArtInBattle(ART_TYPE artt) const
{
	iHero* pAst = m_pAssaulter->SpellCaster();
	iHero* pDef = m_pDefender->SpellCaster();
	if (pAst && pAst->Artifacts().HasArtType(artt)) return pAst;
	if (pDef && pDef->Artifacts().HasArtType(artt)) return pDef;
	return NULL;
}

