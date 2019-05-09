#ifdef MULTIPLAYER

#include "stdafx.h"
#include "Remote_Player.h"

iRemote_Player::iRemote_Player(PLAYER_TYPE_MASK playerTypeMask, PLAYER_ID playerId, NATION_TYPE nation, const iSize& map_siz)
: iPlayer(playerTypeMask, playerId, nation, map_siz)
{	
}


iRemote_Player::~iRemote_Player()
{	
}

void iRemote_Player::OnSelect(bool bNewWeek, bool bAfterLoad)
{

}

void iRemote_Player::Process(fix32 t) {

}

bool iRemote_Player::NeedStopHero() const {
	return false;
}

iRemote_Player::iRemote_Player(iPlayer& other): 
iPlayer(other.m_playerTypeMask, other.m_PlayerId, other.m_Nation, iSize(gGame.Map().GetWidth(),
		gGame.Map().GetHeight()))
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

#endif //MULTIPLAYER