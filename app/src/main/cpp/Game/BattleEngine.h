#ifndef PHEROES_GAME_BATTLEENGINE_H_
#define PHEROES_GAME_BATTLEENGINE_H_

class iBattleWrapper;
//////////////////////////////////////////////////////////////////////////
enum MELEE_DIR {
	MDIR_NORTHWEST = 0,
	MDIR_NORTH,
	MDIR_NORTHEAST,
	MDIR_RSPTOP,
	MDIR_EAST,
	MDIR_RSPBOTTOM,
	MDIR_SOUTHEAST,
	MDIR_SOUTH,
	MDIR_SOUTHWEST,
	MDIR_LSPBOTTOM,
	MDIR_WEST,
	MDIR_LSPTOP,
	MDIR_COUNT
};

//////////////////////////////////////////////////////////////////////////
enum CELL_ACT_MASK {
	CAM_NONE	= 0x00,	// no action
	CAM_MOVE	= 0x01,	// can move
	CAM_ATTACK	= 0x02,	// can attack
	CAM_SHOOT	= 0x04,	// can shoot
	CAM_FSPELL	= 0x08,	// can cast friendly spell
	CAM_ESPELL	= 0x10,	// can cast enemy spell
	CAM_INFO	= 0x20	// creature information
};

/*
 *	Action list
 */
struct iBattleAct : public iIListNode, public TypeAware {
	IMPL_TYPEAWARE( iBattleAct );

	enum Type {
		Move = 0,		// Move to specified cell (async)
		Rotate,			// Change group's orientation (async)
		Attack,			// Melee attack specified cell
		Shoot,			// Shoot to specified cell
		Delay,			// Delay
		GoodMorale,		// Good morale 
		BadMorale,		// Bad morale 
		GoodLuck,		// Good luck
		BadLuck,		// Bad luck
		CastSpell,		// Casting spell to specified cell
		Catapult,		// Catapult shot
		Turret,			// Turret shot
		Gate,			// Open/Close gate
	};

	iBattleAct(fix32 delay);
	virtual ~iBattleAct() {}
	
	virtual bool IsValid() const  =0;
	virtual bool Normalize() { return true; }
	virtual void BeginAct()  =0;
	virtual void EndAct()  =0;

	fix32			m_delay;
};
//
struct iBattleAct_Move : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Move );
	iBattleAct_Move(iBattleGroup* pActor, const iPoint& target, sint32 frame);
	bool IsValid() const { return m_pActor->IsAlive(); }

	void BeginAct();
	void EndAct();

	uint8			m_bridgeFlag;
	iBattleGroup*	m_pActor;
	iPoint			m_target;	
	sint32			m_frame;
};
//
struct iBattleAct_Anim : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Anim );
	iBattleAct_Anim(iBattleGroup* pActor, const iPoint& target, sint32 frame);
	bool IsValid() const { return m_pActor->IsAlive(); }	
	void BeginAct();
	void EndAct();
	
	iBattleGroup*	m_pActor;
	iPoint			m_target;
	sint32			m_frame;
	sint32			m_snd;
};

struct iBattleAct_BeginMoving : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_BeginMoving );
	iBattleAct_BeginMoving(iBattleGroup* pActor);
	bool IsValid() const { return m_pActor->IsAlive(); }	
	void BeginAct();
	void EndAct();
	
	iBattleGroup*	m_pActor;	
};

struct iBattleAct_EndMoving : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_EndMoving );
	iBattleAct_EndMoving(iBattleGroup* pActor);
	~iBattleAct_EndMoving();
	bool IsValid() const { return m_pActor->IsAlive(); }	
	void BeginAct();
	void EndAct();
	
	iBattleGroup*	m_pActor;	
};


//

//

struct iBattleAct_Rotate : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Rotate );
	iBattleAct_Rotate(iBattleGroup* pActor);
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
	void EndAct();
	
	iBattleGroup*	m_pActor;
};
//
struct iBattleAct_Attack : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Attack );
	iBattleAct_Attack(iBattleGroup* pActor, const iPoint& cell, iBattleGroup* pTarget, sint32 penalty, uint32 joustBonus, bool bRetail, iBattleGroup::MORLUCK_MOD luck);
	bool Normalize();
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
    void BeginAct_Estimate(bool maxDamage, sint32& damage, sint32& deaths);
	void EndAct();

	iBattleGroup::MORLUCK_MOD	m_luck;
	bool						m_bRetail;
	sint32						m_penalty;
	iBattleGroup*				m_pActor;
	iPoint						m_cell;
	iSimpleArray<iPair<iBattleGroup*, bool> >	m_targetList;
	uint32						m_joustBonus;
};
//
struct iBattleAct_ShootingStand : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_ShootingStand );
	iBattleAct_ShootingStand(iBattleGroup* pActor, iBattleGroup* pTarget, bool start);
	bool IsValid() const { return m_pActor->IsAlive(); }
	bool Normalize();
	void BeginAct();
	void EndAct();

	iBattleGroup*				m_pActor;
	bool						m_start;
	iSimpleArray<iBattleGroup*>	m_targetList;
};
//
struct iBattleAct_Shoot : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Shoot );
	iBattleAct_Shoot(iBattleGroup* pActor, const iPoint& cell, iBattleGroup* pTarget, sint32 penalty, iBattleGroup::MORLUCK_MOD luck);
	bool Normalize();
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
    void BeginAct_Estimate(bool maxDamage, sint32& damage, sint32& deaths);	
    void EndAct();

	iBattleGroup::MORLUCK_MOD	m_luck;
	sint32						m_penalty;
	iBattleGroup*				m_pActor;
	iPoint						m_cell;
	iSimpleArray<iPair<iBattleGroup*, bool> >	m_targetList;
};
//

struct iAmmo {
	sint16 sid;
	iPoint m_pos;
	bool reversed;
};


struct iBattleAct_Ammo : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Ammo );
	iBattleAct_Ammo(iAmmo& m_ammo, iPoint pos, sint16 sid, fix32 mult, iBattleGroup* pTarget);
	bool IsValid() const { return true; }
	void BeginAct();
	void EndAct();
	bool Normalize();

	iPoint m_pos;
	sint16 m_sid;
	iAmmo&  m_ammo;
	iSimpleArray<iBattleGroup*>	m_targetList;
	bool m_bHasTarget;
};

//
struct iBattleAct_Delay : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Delay );
	iBattleAct_Delay();
	bool IsValid() const { return true; }
	void BeginAct();
	void EndAct();
};
//
struct iBattleAct_GoodMorale : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_GoodMorale );
	iBattleAct_GoodMorale(iBattleGroup* pActor);
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
	void EndAct();
	iBattleGroup*				m_pActor;
};
//
struct iBattleAct_BadMorale : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_BadMorale );
	iBattleAct_BadMorale(iBattleGroup* pActor);
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
	void EndAct();
	iBattleGroup*				m_pActor;
};
//
struct iBattleAct_GoodLuck : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_GoodLuck );
	iBattleAct_GoodLuck(iBattleGroup* pActor);
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
	void EndAct();
	iBattleGroup*				m_pActor;
};
//
struct iBattleAct_BadLuck : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_BadLuck );
	iBattleAct_BadLuck(iBattleGroup* pActor);
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
	void EndAct();
	iBattleGroup*				m_pActor;
};
//
struct iBattleAct_CastSpell : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_CastSpell );
	iBattleAct_CastSpell(iBattleUnit_Hero* pCaster, iCombatSpell* pSpell, const iPoint& cell);
	bool IsValid() const { return m_pCaster->IsAlive(); }
	void BeginAct();
	void EndAct();

	iBattleUnit_Hero*	m_pCaster;
	iCombatSpell*		m_pSpell;
	iPoint				m_cell;
	iSimpleArray<iBattleGroup*>				m_targetList;
	iSimpleArray<iBattleGroup*>				m_resistList;
	iSimpleArray<iCastleFort::iElement*>	m_affectedConstrList;	// Used for EarthQuake
};
//
struct iBattleAct_FortDmg : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_FortDmg );
	iBattleAct_FortDmg(iBattleUnit_Catapult* pActor, iCastleFort* pFort, iCastleFort::ElementType target);
	bool IsValid() const { return true; }
	void BeginAct();
	void EndAct();

	iCastleFort*				m_pFort;
	iBattleUnit_Catapult*		m_pActor;
	iCastleFort::ElementType	m_target;
};

struct iBattleAct_Catapult : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Catapult );
	iBattleAct_Catapult(iBattleUnit_Catapult* pActor, bool start);
	bool IsValid() const { return true; }
	void BeginAct();
	void EndAct();

	iBattleUnit_Catapult*		m_pActor;
	bool						m_start;
};

//
struct iBattleAct_Turret : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Turret );
	iBattleAct_Turret(iBattleUnit_Turret* pActor, iBattleGroup* pTarget);
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
	void EndAct();

	iBattleUnit_Turret*		m_pActor;
	iBattleGroup*			m_pTarget;
};
//
struct iBattleAct_Gate : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_Gate );
	iBattleAct_Gate(iBattleGroup* pActor, iCastleFort::iElement_Bridge* pGate, bool bEnter);
	bool IsValid() const { return m_pActor->IsAlive(); }
	void BeginAct();
	void EndAct();

	iBattleGroup*					m_pActor;
	iCastleFort::iElement_Bridge*	m_pGate;
	bool							m_bEnter;
};
//
struct iBattleAct_MoatDmg : public iBattleAct {
	IMPL_TYPEAWARE( iBattleAct_MoatDmg );
	iBattleAct_MoatDmg(iBattleUnit_Moat* pActor, iBattleGroup* pTarget);
	bool IsValid() const { return m_pTarget->IsAlive(); }
	void BeginAct();
	void EndAct();

	iBattleUnit_Moat*		m_pActor;
	iBattleGroup*			m_pTarget;
};
//////////////////////////////////////////////////////////////////////////
class iBattleActList
{
public:
	typedef iIList<iBattleAct>		iActList;
	typedef iActList::ConstIterator	cbIt;
	typedef iActList::Iterator		bIt;

public:
	iBattleActList() {}

	inline uint8 EndDir(iBattleGroup* pActor, uint8 dir)
	{ 
		for (bIt it=m_ActList.First(); it!=m_ActList.End(); ++it) {
			if (iBattleAct_Rotate* pActRotate = DynamicCast<iBattleAct_Rotate*>(*it)) {
				if (pActRotate->m_pActor == pActor)	dir = !dir; 
			}
		}
		return dir; 
	}

	inline void EndDirPos(iBattleGroup* pActor, iPoint& pos, uint8& dir) 
	{ 
		for (bIt it=m_ActList.First(); it!=m_ActList.End(); ++it) {
			if (iBattleAct_Rotate* pActRotate = DynamicCast<iBattleAct_Rotate*>(*it)) {
				if (pActor->Size() == 2 && pActRotate->m_pActor == pActor){
					pos.x += TAIL_OFFSET[dir];
					dir = !dir; 
				}
			} else if (iBattleAct_Move* pActMove= DynamicCast<iBattleAct_Move*>(*it)) {
				if (pActMove->m_pActor == pActor){
					pos = pActMove->m_target;
				}
			}
		}
	}


	inline void Reset() { m_ActList.DeleteAll(); }
	inline uint32 Count() const { return m_ActList.Count(); }
	inline void AddAction(iBattleAct* pAct) { m_ActList.Add(pAct); }
	inline void PushAction(iBattleAct* pAct) { m_ActList.Push(pAct); }
	inline void InsertActionBefore(bIt it, iBattleAct* pAct) { m_ActList.InsertBefore(it, pAct); }
	inline void InsertActionAfter(bIt it, iBattleAct* pAct) { m_ActList.InsertAfter(it, pAct); }
	inline iBattleAct* StepAction() { return m_ActList.Pop(); }
	inline bIt First() { return m_ActList.First(); }
	inline cbIt First() const { return m_ActList.First(); }
	inline cbIt Last() const { return m_ActList.Last(); }
	inline cbIt End() const { return m_ActList.End(); }

private:
	iActList		m_ActList;
};

/*
 *	Battle Army
 */
class iBattleArmy
{
public:
	// c-tor, d-tor
	iBattleArmy(iBattleWrapper* pWrapper);
	~iBattleArmy();

	// inlines
	inline uint32 AliveGroups() const { uint32 res=0; for (uint32 xx=0; xx<m_groups.GetSize(); ++xx) if (m_groups[xx]->IsAlive()) ++res; return res; }
	inline uint32 Count() const { return m_groups.GetSize(); }
	inline const iSimpleArray<iBattleGroup*>& Groups() { return m_groups; }
	inline const iBattleGroup* operator[] (uint32 idx) const { return m_groups[idx]; }
	inline iBattleGroup* operator[] (uint32 idx) { return m_groups[idx]; }
	inline const iBattleGroup* FindGroupByIdx(uint32 idx) const { for (uint32 aidx=0; aidx<m_groups.GetSize(); ++aidx) if (m_groups[aidx]->ArmyGroupIndex() == idx) return m_groups[aidx]; return NULL; }

	// operations
	void Init(const iArmy& army, iBattleMember* pOwner, sint32 moraleModifier);
	iBattleGroup* Summon(const iCreatGroup& cg, const iPoint& pos);
	void Synchronize(iArmyOwner* pOwner);
	sint32 Casualties() const { sint32 res=0; for (uint32 xx=0; xx<m_groups.GetSize(); ++xx) res += m_groups[xx]->Casualties(); return res;}
	sint32 HpCasualties() const { sint32 res=0; for (uint32 xx=0; xx<m_groups.GetSize(); ++xx) res += m_groups[xx]->Casualties() * m_groups[xx]->HitPoints(); return res;}
	// Army cost (according with diplomacy level)
	sint32 Cost() const;

	// Army power
	uint32 ArmyPower() const;

	void AddGroup(iBattleGroup *pGroup);

	// 
	inline iBattleMember* Owner() { return m_pOwner; }

private:
	iBattleMember*	m_pOwner;
	iBattleWrapper*	m_pWrapper;
	iSimpleArray<iBattleGroup*>	m_groups;
};

/*
 *	Turn sequence
 */
class iTurnSeq
{
public:
	enum UpdateSequenceFlags {
		USF_ACTIVES	= 0x1,
		USF_DEADS	= 0x2
	};

public:
	iTurnSeq(iBattleWrapper* pWrapper);
	~iTurnSeq();
	void NewRoundSequence(iBattleArmy& aa, iBattleArmy& da, iCastleFort* pFort);
	iBattleUnit* CurUnit() const;
	inline iBattleUnit_Catapult* Catapult() const { return m_catapult; }
	iBattleUnit* NextGroup();
	iBattleUnit* Wait();
	iBattleUnit* Defend();
	void SetSpellCaster(iBattleUnit_Hero* pSpellCaster);
	inline iBattleUnit_Hero* GetSpellCaster() const { return m_pSpellCaster; }
	void UpdateSequence(uint32 flags);
	void AddNewGroup(iBattleGroup* pNewGroup);

	inline uint32 GetTurnSeqSize() const { return m_turnSeq.GetSize(); }
	inline iBattleUnit* GetTurnSeqItem(uint32 idx) const { return m_turnSeq[idx]; }

private:
	bool StepSequence();
	void Cleanup();
	typedef iSimpleArray<iCastleFort::ElementType> iTowerList;

private:
	iBattleWrapper*					m_pWrapper;
	iBattleUnit*					m_pCurUnit;		// Current/Active unit
	iBattleUnit_Hero*				m_pSpellCaster;	// Spell caster
	iSimpleArray<iBattleUnit*>		m_turnSeq;		// Prepared turn sequence
	iSimpleArray<iBattleGroup*>		m_dead;			// Dead not moved units (can be resurrected to active or waiting list)
	iSortArray<iBattleGroup*>		m_active[2];	// Active units (not moved)
	iSortArray<iBattleGroup*>		m_waiting[2];	// Waiting units
	uint32							m_LastSide;		// Last acting side
	bool							m_bHasCatapult;	// Need catapult shoot
	iTowerList						m_turrets;		// Turrets
	iBattleUnit_Catapult*			m_catapult;		// the catapult unit
};


class iDamageEstimate {
public:
    iDamageEstimate() {
        minDamage = maxDamage = minDeaths = maxDeaths = 0;
    }
    
    sint32 minDamage;
    sint32 maxDamage;
    sint32 minDeaths;
    sint32 maxDeaths;
    
    iSimpleArray<CREAT_PERK> perks;
};

/*
 *	Battle Engine
 */

class iBattleEngine
{
public:
	iBattleEngine(iBattleWrapper* pWrapper, bool bFakeMode);
	~iBattleEngine();

	bool BeginBattle(const iBattleInfo& bi);
	void Start();
	void EndBattle(iBattleResult br);
	void NextGroup();

	inline bool IsFakeMode() const { return m_bFakeMode; }

	void SelectSpellTargets(iCombatSpell* pSpell, iSimpleArray<iBattleGroup*>& targetList);
	void GetValidSpellTargets(iCombatSpell* pSpell, MAGIC_SCHOOL_LEVEL msl, iSimpleArray<iPoint>& targetList);
	void GetSpellCovers(iCombatSpell* pSpell, MAGIC_SCHOOL_LEVEL msl, const iPoint& pos, iSimpleArray<iPoint>& targetList);

	iPoint GetThroughHex(const iPoint& pos, uint16 mdir);

	uint32 GetSurrenderCost(iBattleMember_Hero* pActor);
	BATTLE_RESULT CheckBattleState();
	void PrepareBattleResult(iBattleResult& br);
	iSimpleArray<iBattleGroup*> FindAllGroups(const iPoint& pos, iBattleMember *owner=NULL, bool bIncDead=false, bool bUndeadOnly=false);
	iBattleGroup* FindGroup(const iPoint& pos, bool bIncDead=false);
    iBattleGroup* GetCurrentGroup();    
	iBattleAct* StepAction();
	inline iBattleAct* CurAction() { return m_pCurAct; }
	
	sint32 Summon(iBattleMember::Side side, const iCreatGroup& cg, const iPoint& pos);
	bool CastSpell(iCombatSpell* pSpell, const iPoint& pos, iBattleGroup *pSpellTarget);
	void Move(const iPoint& pos, uint8 orient);
	
    void Melee(const iPoint& pos, uint16 mdir);
	void Shot(const iPoint& pos, sint32 penalty);
    
	void MeleeEstimate(const iPoint& pos, uint16 mdir, iDamageEstimate* estimate);
	void ShotEstimate(const iPoint& pos, sint32 penalty, iDamageEstimate* estimate);
	
    void BadMorale();
	bool CanWait() const;
	void Wait();
	bool Defend();
	void CatapultShot(iCastleFort::ElementType target);
	void TurretShot(iBattleGroup* pTarget);

	inline bool Inited() const { return m_bInited; }
	inline const iBattleInfo& GetBattleInfo() const { return m_bi; }
	inline iBattleInfo& GetBattleInfo() { return m_bi; }
	inline iBattleMap& ObstaclesMap() { return m_obsMap; }

	inline const iBattleArmy& AArmy() const { return m_aArmy; }
	inline iBattleArmy& AArmy() { return m_aArmy; }
	inline const iBattleArmy& DArmy() const { return m_dArmy; }
	inline iBattleArmy& DArmy() { return m_dArmy; }
	inline uint32 ActionCount() const { return m_ActList.Count(); }
	inline const iTurnSeq& TurnSeq() const { return m_turnSeq; } 
	inline const iCastleFort* CastleFort() const { return m_pCastleFort; }
    const bool OnShip() const;

	sint32 CalcShotPenalty(iBattleGroup* pRanger, const iPoint& ptTarget);

    void SetAutobattle(bool bAutoBattle);
    bool IsAutoBattle();
    bool IsAnyAutoBattle();

	
//	inline void SetAutobattle(bool bAutoBattle) { m_bAutoBattle = bAutoBattle; }
//	inline bool IsAutoBattle() const { return m_bAutoBattle; }

	inline iAmmo& Ammo() { return m_ammo; }

	void MakePassMap(iBattleMap& pmap);
	

private:
	void ProcessMoat(iBattleGroup* pCurGroup);
	void ProcessGate(iBattleGroup* pCurGroup);
	void NextRound();
	void PreparePassMap();
	void PrepareDistMap();

	void MakeMeleeSequence(iBattleGroup* pActor, iBattleGroup* pTarget, const iPoint& pos, sint32 penalty, uint16 mdir, bool bRetail, iBattleActList& actList, iPoint meleePoint);
	void MakeShootSequence(iBattleGroup* pActor, iBattleGroup* pTarget, const iPoint& pos, sint32 penalty, iBattleActList& actList);
	
private:
	bool			m_bFakeMode;	
	iCastleFort*	m_pCastleFort;
	bool			m_bInited;
	iBattleInfo		m_bi;
	
	uint32			m_iRound;
	iTurnSeq		m_turnSeq;
	iBattleActList	m_ActList;
	iBattleAct*		m_pCurAct;

	iBattleArmy		m_aArmy;
	iBattleArmy		m_dArmy;
	
	iBattleMap		m_obsMap;
	iBattleWrapper*	m_pWrapper;
	iAmmo			m_ammo;

    bool			m_bAutoBattleForA;
    bool			m_bAutoBattleForD;
};

/*
 *	
 */
class iBattleWrapper
{
public:
	iBattleWrapper(bool bFakeMode);
	virtual ~iBattleWrapper() {}

	void BeginBattle(iBattleInfo bi);
	void EndBattle(iBattleResult br);
	inline iBattleEngine& Engine() { return m_engine; }
	inline iBattleResult Result() { return m_result; }

	// pure virtuals
	virtual void OnBeginBattle()  =0;
	virtual void OnStart()  =0;
	virtual void OnEndBattle()  =0;
	virtual void AddLogEvent(const iStringT& msg)  =0;
	virtual void AddCellEvent(const iStringT& msg, const iPoint& pos)  =0;
	virtual void OnNextGroup() = 0;
	virtual void PlayAnim() = 0;
	inline virtual sint16 PlaySound(sint16 sid, bool loop = false) const =0;
	inline virtual void StopSound(sint16 streamid) const =0;
	virtual void Block() =0;
	virtual void Unblock() = 0;
	virtual bool IsBlocked() const= 0;
protected:
	iBattleResult	m_result;
	iBattleEngine	m_engine;
};


#endif //PHEROES_GAME_BATTLEENGINE_H_
