#ifndef PHEROES_GAME_BATTLEINFO_H_
#define PHEROES_GAME_BATTLEINFO_H_

/*
 *	Battle result
 */
enum BATTLE_RESULT {
	BR_NA,
	BR_ASSAULTER_WIN,
	BR_DEFENDER_WIN
};

enum DEFEAT_CAUSE {
	DC_DEFEAT,
	DC_RETREAT,
	DC_SURRENDER
};

class iBattleMember;
struct iBattleResult {
	iBattleResult() : m_result(BR_NA), m_defCause(DC_DEFEAT), m_pDest(NULL), m_pLoser(NULL), m_pWinner(NULL), m_experience(0) {}
	iBattleResult(BATTLE_RESULT result, DEFEAT_CAUSE defCause, iCastle* pDest = NULL, uint32 cost = 0) : m_result(result), m_defCause(defCause), m_pDest(pDest), m_cost(cost), m_pLoser(NULL), m_pWinner(NULL), m_experience(0) {}

	BATTLE_RESULT	m_result;
	DEFEAT_CAUSE	m_defCause;
	iCastle*		m_pDest;
	uint32			m_cost;
	iBattleMember*	m_pLoser;
	iBattleMember*	m_pWinner;
	sint32			m_winCas;
	sint32			m_losCas;
	sint32          m_losHpCas;
	sint32			m_experience;
};

/*
 *	Battle Side
 */
class iBattleMember : public TypeAware
{
public:
	enum Side {
		Assaulter = 0,
		Defender
	};

	enum Type {
		Undefined = 0,
		Hero,
		Castle,
		OwnCnst,
		VisCnst,
		MapItem,
		MapEvent,
		MapGuard
	};

public:
	IMPL_TYPEAWARE( iBattleMember );
	// c-tor
	iBattleMember(Type type, Side side, const iFurtSkills& furtSkills);
	virtual ~iBattleMember() {}

	// inlines
	inline Side GetSide() const { return m_Side; }
	inline Type MemberType() const { return m_Type; }
	inline const iFurtSkills& FurtSkills() const { return m_furtSkills; }
	inline sint32 GetExperience() const { return m_Experience; }
	inline void AddExperience(sint32 exp) { m_Experience += exp; }
	inline bool CanCastSpell() { return (SpellCaster() && !m_bCastFlag); }
	inline void SetCastFlag(bool bCastFlag = true) { m_bCastFlag = bCastFlag; }

	// virtuals
	virtual inline PLAYER_ID Owner() const { return PID_NEUTRAL; }
	virtual void OnPrepare(iBattleMember* pEnemy) {};
	virtual void OnWin(iBattleResult br) { }
	virtual void OnLose(iBattleResult br) { }

	virtual inline bool IsValid() const =0;
#ifdef MULTIPLAYER
	virtual void Update(); // send the updated information to MP server, used after combat
#endif
	// pure virtuals
	virtual const iArmy& Army() const  =0;	
	virtual iArmyOwner* ArmyOwner() const  =0;	
	virtual const iBaseMapObject* BaseMapObject() const  =0;
	virtual iHero* SpellCaster()  =0;

protected:
	bool		m_bCastFlag;
	sint32		m_Experience;
	Type		m_Type;
	Side		m_Side;
	iFurtSkills	m_furtSkills;
};
//////////////////////////////////////////////////////////////////////////
class iBattleMember_Hero : public iBattleMember
{
public:
	IMPL_TYPEAWARE( iBattleMember_Hero );
	iBattleMember_Hero(iHero* pHero, Side side);

	void OnWin(iBattleResult br);
	void OnLose(iBattleResult br);
	inline PLAYER_ID Owner() const { return m_pHero->Owner() ? m_pHero->Owner()->PlayerId() : PID_NEUTRAL; }
	inline const iArmy& Army() const { return m_pHero->Army(); }	
	inline virtual iArmyOwner* ArmyOwner() const  { return m_pHero; }
	inline const iHero* GetHero() const { return m_pHero; }
	inline iHero* GetHero() { return m_pHero; }
	inline const iBaseMapObject* BaseMapObject() const { return m_pHero; }
	inline iHero* SpellCaster() { return m_pHero; }
	inline virtual bool IsValid() const { return m_pHero->Owner() != NULL; }
#ifdef MULTIPLAYER
	virtual void Update();


#endif
private:
	iHero*	m_pHero;
};
//////////////////////////////////////////////////////////////////////////
class iBattleMember_Castle : public iBattleMember
{
public:
	IMPL_TYPEAWARE( iBattleMember_Castle );
	iBattleMember_Castle(iCastle* pCastle);

	void OnPrepare(iBattleMember* pEnemy);
	void OnWin(iBattleResult br);
	void OnLose(iBattleResult br);
	inline PLAYER_ID Owner() const { return m_pCastle->Owner(); }
	inline const iArmy& Army() const { return (GetVisitor())?GetVisitor()->Army():m_pCastle->Garrison(); }
	inline virtual iArmyOwner* ArmyOwner() const  { return GetVisitor()?(iArmyOwner*)GetVisitor():(iArmyOwner*)m_pCastle; }
	inline const iCastle* GetCastle() const { return m_pCastle; }
	inline iCastle* GetCastle() { return m_pCastle; }
	inline const iHero* GetVisitor() const { return m_pCastle->Visitor(); }
	inline iHero* GetVisitor() { return m_pCastle->Visitor(); }
	inline const iBaseMapObject* BaseMapObject() const { return m_pCastle; }
	inline iHero* SpellCaster() { return m_pCastle->Visitor(); }
	inline virtual bool IsValid() const { return true; }
#ifdef MULTIPLAYER
	virtual void Update();

#endif
private:
	iCastle*	m_pCastle;
};
//////////////////////////////////////////////////////////////////////////
class iBattleMember_OwnCnst : public iBattleMember
{
public:
	IMPL_TYPEAWARE( iBattleMember_OwnCnst );
	iBattleMember_OwnCnst(iOwnCnst* pOwnCnst);

	void OnLose(iBattleResult br);
	inline PLAYER_ID Owner() const { return m_pOwnCnst->Owner(); }
	inline const iArmy& Army() const { return m_pOwnCnst->Guard(); }	
	inline virtual iArmyOwner* ArmyOwner() const  { return m_pOwnCnst; }
	inline const iOwnCnst* GetOwnCnst() const { return m_pOwnCnst; }
	inline iOwnCnst* GetOwnCnst() { return m_pOwnCnst; }
	inline const iBaseMapObject* BaseMapObject() const { return m_pOwnCnst; }
	inline iHero* SpellCaster() { return NULL; }
	inline virtual bool IsValid() const { return true; }
#ifdef MULTIPLAYER
	virtual void Update();

#endif
private:
	iOwnCnst*	m_pOwnCnst;
};
//////////////////////////////////////////////////////////////////////////
class iBattleMember_VisCnst : public iBattleMember
{
public:
	IMPL_TYPEAWARE( iBattleMember_VisCnst );
	iBattleMember_VisCnst(iVisCnst* pVisCnst);

	void OnLose(iBattleResult br);
	inline const iArmy& Army() const { return m_pVisCnst->Guard(); }	
	inline virtual iArmyOwner* ArmyOwner() const  { return m_pVisCnst; }
	inline const iVisCnst* GetVisCnst() const { return m_pVisCnst; }
	inline iVisCnst* GetVisCnst() { return m_pVisCnst; }
	inline const iBaseMapObject* BaseMapObject() const { return m_pVisCnst; }
	inline iHero* SpellCaster() { return NULL; }
	inline virtual bool IsValid() const { return true; }
#ifdef MULTIPLAYER
	virtual void Update();


#endif
private:
	iVisCnst*	m_pVisCnst;
};
//////////////////////////////////////////////////////////////////////////
class iBattleMember_MapItem : public iBattleMember
{
public:
	IMPL_TYPEAWARE( iBattleMember_MapItem );
	iBattleMember_MapItem(iMapItem* pMapItem);

	void OnLose(iBattleResult br);
	inline const iArmy& Army() const { return m_pMapItem->Guard(); }	
	inline virtual iArmyOwner* ArmyOwner() const  { return m_pMapItem; }
	inline const iMapItem* GetMapItem() const { return m_pMapItem; }
	inline iMapItem* GetMapItem() { return m_pMapItem; }
	inline const iBaseMapObject* BaseMapObject() const { return m_pMapItem; }
	inline iHero* SpellCaster() { return NULL; }
	inline virtual bool IsValid() const { return true; }
#ifdef MULTIPLAYER
	virtual void Update();


#endif
private:
	iMapItem*	m_pMapItem;
};
//////////////////////////////////////////////////////////////////////////
class iBattleMember_MapEvent : public iBattleMember
{
public:
	IMPL_TYPEAWARE( iBattleMember_MapEvent );
	iBattleMember_MapEvent(iMapEvent* pMapEvent);

	void OnLose(iBattleResult br);
	inline const iArmy& Army() const { return m_pMapEvent->Guard(); }	
	inline virtual iArmyOwner* ArmyOwner() const  { return m_pMapEvent; }
	inline const iMapEvent* GetMapEvent() const { return m_pMapEvent; }
	inline iMapEvent* GetMapEvent() { return m_pMapEvent; }
	inline const iBaseMapObject* BaseMapObject() const { return m_pMapEvent; }
	inline iHero* SpellCaster() { return NULL; }
	inline virtual bool IsValid() const { return true; }
#ifdef MULTIPLAYER
	virtual void Update();

#endif
private:
	iMapEvent*	m_pMapEvent;
};
//////////////////////////////////////////////////////////////////////////
class iBattleMember_MapGuard : public iBattleMember, public iArmyOwner
{
public:
	IMPL_TYPEAWARE( iBattleMember_MapGuard );
	iBattleMember_MapGuard(iMapGuard* pMapGuard);

	void OnPrepare(iBattleMember* pEnemy);
	void OnWin(iBattleResult br);
	void OnLose(iBattleResult br);
	inline const iArmy& Army() const { return iArmyOwner::Army(); }	
	inline virtual iArmyOwner* ArmyOwner() const { return (iArmyOwner*)this; }
	inline const iMapGuard* GetMapGuard() const { return m_pMapGuard; }
	inline iMapGuard* GetMapGuard() { return m_pMapGuard; }
	inline const iBaseMapObject* BaseMapObject() const { return m_pMapGuard; }
	inline iHero* SpellCaster() { return NULL; }
	inline virtual bool IsValid() const { return true; }

	virtual bool CanBeEmpty() const { return true; }
#ifdef MULTIPLAYER
	virtual void Update();

#endif
private:	
	iMapGuard*	m_pMapGuard;
};

/*
 *	Battle info
 */
struct iBattleInfo {
	// c-tors
	iBattleInfo();
	iBattleInfo(iHero* pAssaulter, iHero* pDefender);
	iBattleInfo(iHero* pAssaulter, iCastle* pDefender);
	iBattleInfo(iHero* pAssaulter, iOwnCnst* pDefender);
	iBattleInfo(iHero* pAssaulter, iVisCnst* pDefender);
	iBattleInfo(iHero* pAssaulter, iMapItem* pDefender);
	iBattleInfo(iHero* pAssaulter, iMapEvent* pDefender);
	iBattleInfo(iHero* pAssaulter, iMapGuard* pDefender);

	// Cleanup
	void Cleanup();

	//
	const iBattleMember* Member(iBattleMember::Side side) const { return (side==iBattleMember::Assaulter)?m_pAssaulter:m_pDefender; }
	iBattleMember* Member(iBattleMember::Side side) { return (side==iBattleMember::Assaulter)?m_pAssaulter:m_pDefender; }
	iBattleMember::Side InteractorSide() const;
	inline PLAYER_ID InteractorPlayer() const
	{ iBattleMember::Side is = InteractorSide(); return (is==iBattleMember::Assaulter)?m_pAssaulter->Owner():m_pDefender->Owner(); }

	inline const iBaseMapObject* Defender() const { return m_pDefender->BaseMapObject(); }

	iHero* HasArtInBattle(ART_TYPE artt) const;

	//
	iBattleMember* m_pAssaulter;
	iBattleMember* m_pDefender;
};


#endif //PHEROES_GAME_BATTLEINFO_H_
