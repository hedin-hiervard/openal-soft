#ifndef PHEROES_GAME_CONSTRUCTIONS_H_
#define PHEROES_GAME_CONSTRUCTIONS_H_

/*
 *  Common construction type
 */
class iMapCnst : public iIListNode, public iBaseMapObject
{
public:
    IMPL_TYPEAWARE( iMapCnst );
    iMapCnst(const iPoint& pos) : iBaseMapObject(pos, false) {}
    virtual ~iMapCnst() {}
    iHero* Visitor() const;
    virtual bool Activate(iHero* pVisitor, bool bActive, bool &bDisappear) { return true; }	
};

/*
 *  Visitables
 */
class iHero;
class iVisCnst : public iMapCnst, public iArmyOwner
{
public:
    typedef iSimpleArray<uint16> iVisitors;

public:
    IMPL_TYPEAWARE( iVisCnst );
    iVisCnst(iVisCnstT* pProto, const iPoint& pos, bool bCustomGUI) : iMapCnst(pos), m_pProto(pProto), m_bCustomGUI(bCustomGUI), m_bDisabled(false), m_seenIntro(0) {}
    virtual ~iVisCnst() {}

    inline const iVisCnstT* Proto() const { return m_pProto; }
    inline const iArmy& Guard() const { return Army(); }
    inline const iStringT& CustomMessage() const { return m_customMsg; }
    inline iStringT& CustomMessage() { return m_customMsg; }

	inline iReqsCtr& Reqs() { return m_reqs; }
	inline iStringT& CurFailMessage() { return m_curFailMessage; }
	inline iRewardsCtr& OKRewards() { return m_okRewards; }
	inline iStringT& OKRewardMessage() { return m_okMessage; }	
	inline iRewardsCtr& PermFailRewards() { return m_permFailRewards; }
	inline iStringT& PermFailRewardMessage() { return m_permFailMessage; }
	inline CNST_REW_BEH& OkRewBehaviour() { return m_okRewBeh; }
	inline CNST_REW_BEH& PermFailRewBehaviour() { return m_permFailRewBeh; }

    virtual void NewDay();
    virtual void NewWeek(const iWeekDesc& weekDesk);
    bool Activate(iHero* pVisitor, bool bActive, bool& bDisappear);
    virtual void Unvisit(const iHero *pVisitor);
    virtual void OnActivate(iHero* pVisitor, bool bActive);
    virtual bool Visited(const iHero* pVisitor) const;
    virtual void Serialize(iDynamicBuffer& dbuff);
    virtual uint8 Unserialize(iDynamicBuffer& dbuff);
    virtual bool CustomPopupText(const iHero* pVisitor, iStringT& ctext) const { return false; }
    virtual bool ShowVisitedLabel() const { return true; }  	

	virtual bool CanBeEmpty() const { return true; }
protected:
    bool        m_bCustomGUI;    
    iStringT    m_customMsg;

    iVisitors   m_Visitors;
    iVisCnstT*  m_pProto;

	iReqsCtr	m_reqs;
	iStringT	m_curFailMessage;
		
	iRewardsCtr	m_okRewards;
	iStringT	m_okMessage;
	CNST_REW_BEH   m_okRewBeh;

	iRewardsCtr	m_permFailRewards;
	iStringT	m_permFailMessage;
	CNST_REW_BEH   m_permFailRewBeh;
	bool		m_bDisabled;
	uint16	m_seenIntro;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_Stable : public iVisCnst
{
public:
    iVisCnst_Stable(iVisCnstT_Stable* pProto, const iPoint& pos) : iVisCnst(pProto, pos, false) {}
    void NewWeek(const iWeekDesc& weekDesk);
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_Gazebo : public iVisCnst
{
public:
    iVisCnst_Gazebo(iVisCnstT_Gazebo* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true) {}
    void OnActivate(iHero* pVisitor, bool bActive);
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_ManaSource : public iVisCnst
{
public:
    iVisCnst_ManaSource(iVisCnstT_ManaSource* pProto, const iPoint& pos) : iVisCnst(pProto, pos, false) {}
    void NewWeek(const iWeekDesc& weekDesk);
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;
    sint32 ManaPts() const;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_MLModifier : public iVisCnst
{
public:
    iVisCnst_MLModifier(iVisCnstT_MLModifier* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true) {}
    void NewWeek(const iWeekDesc& weekDesk);
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_PSModifier : public iVisCnst
{
public:
    iVisCnst_PSModifier(iVisCnstT_PSModifier* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true) {}
    void OnActivate(iHero* pVisitor, bool bActive);
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_Obelisk : public iVisCnst
{
public:
    iVisCnst_Obelisk(iVisCnstT_Obelisk* pProto, const iPoint& pos) : iVisCnst(pProto, pos, false) {}
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;
    void Unvisit(const iHero* pVisitor) {}
};
//////////////////////////////////////////////////////////////////////////

#include "harbor.h"

class iVisCnst_Harbor : public iVisCnst, public IHarbor
{
public:	
    iVisCnst_Harbor(iVisCnstT_Harbor* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true)    
    {
        m_shipsCount[0] = 0;
        m_shipsCount[1] = 0;
    }
    void OnActivate(iHero* pVisitor, bool bActive);
    void NewWeek(const iWeekDesc& weekDesk);
    bool Visited(const iHero* pVisitor) const;
    void Unvisit(const iHero* pVisitor) {}    
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);    

	// implementation of IHarbor
	virtual void ShipBought(uint8 dock);  // informs the structure that ship was bought in a dock
	virtual uint8 ShipsAvailable(uint8 dock); // returns the number of ships available in the dock
	virtual iShip* GetShip( uint8 dockType ); // returns the ship in the selected dock or NULL if none
	virtual iPoint GetShipPos( uint8 dockType ); // returns the ship position for the selected dock
	virtual bool IsShipBuildSupported(); // returns if the structure can build ships
	virtual uint8 NumOfDocks(); // returns number of docks available
	virtual iPlayer* GetOwner(); // get owner of the structure
	virtual bool IsBoardable(); // returns if the harbor has the ability to board hero directly from it
private:   
    // Harbor have two separate docks, each dock can produce one ship per week
    uint32 m_shipsCount[2]; // an array of ships remaining in each docks 
};

//////////////////////////////////////////////////////////////////////////
class iVisCnst_WarAcademy: public iVisCnst
{
public:
    iVisCnst_WarAcademy(iVisCnstT_WarAcademy* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true)    
    {
    }
    void OnActivate(iHero* pVisitor, bool bActive);
    void NewWeek(const iWeekDesc& weekDesk);
    void NewDay();
    bool Visited(const iHero* pVisitor) const;
    void Unvisit(const iHero* pVisitor) {}    
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);

    void UpgradeBought(uint32 dock);
    bool IsUpgradeAvailable(uint32 dock);
private:   
    ;
};

//////////////////////////////////////////////////////////////////////////
class iVisCnst_Sign : public iVisCnst
{
public:
    iVisCnst_Sign(iVisCnstT_Sign* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true) {}    
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_Dwelling : public iVisCnst
{
public:
    iVisCnst_Dwelling(iVisCnstT_Dwelling* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_creatType(pProto->CreatType()), m_growth(CREAT_DESC[pProto->CreatType()].growth), m_creatCount(0) {}
    void NewWeek(const iWeekDesc& weekDesk);
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;  
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);
    inline CREATURE_TYPE CreatType() const { return m_creatType; }
    inline uint32 CreatCount() const { return m_creatCount; }
    inline bool JoinForFree() const { return CREAT_DESC[m_creatType].level < 3; }

private:
    uint32              m_growth;
    CREATURE_TYPE       m_creatType;
    uint32              m_creatCount;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_WitchHut : public iVisCnst
{
public:
    iVisCnst_WitchHut(iVisCnstT_WitchHut* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_skill(SECSK_NONE) {}
    void OnActivate(iHero* pVisitor, bool bActive);
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);
    bool CustomPopupText(const iHero* pVisitor, iStringT& ctext) const;

private:
    SECONDARY_SKILLS            m_skill;
    iSimpleArray<PLAYER_ID>     m_visPlayers;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_Shrine : public iVisCnst
{
public:
    iVisCnst_Shrine(iVisCnstT_Shrine* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_spell(MSP_INVALID) {}
    void OnActivate(iHero* pVisitor, bool bActive);
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);
    bool CustomPopupText(const iHero* pVisitor, iStringT& ctext) const;
    MAGIC_SPELL Spell() const { return m_spell; }

private:
    MAGIC_SPELL m_spell;
    iSimpleArray<PLAYER_ID>     m_visPlayers;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_Treasury : public iVisCnst
{
public:
    iVisCnst_Treasury(iVisCnstT_Treasury* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_bVisited(false) {}
    void OnActivate(iHero* pVisitor, bool bActive);
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);
    bool Visited(const iHero* pVisitor) const { return m_bVisited; }
    void Unvisit(const iHero *pVisitor) const {};

private:
    bool        m_bVisited;
    iRewardsCtr m_rewards;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_Teleport : public iVisCnst
{
public:
    iVisCnst_Teleport(iVisCnstT_Teleport* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_tgtIdx(-1) {}
    void OnActivate(iHero* pVisitor, bool bActive);
    sint32 GetTarget() const { return (IsTwoWay() || IsEntrance())?m_tgtIdx:-1; }
    sint32 GetOrigin() const { return (IsTwoWay() || !IsEntrance())?m_tgtIdx:-1; }
    uint8 Index() const;
    bool IsEntrance() const;
    bool IsTwoWay() const;
    void SetTarget(sint32 tgtIdx) { m_tgtIdx = tgtIdx; }
    bool CustomPopupText(const iHero* pVisitor, iStringT& ctext) const;
    bool ShowVisitedLabel() const { return false; }

private:
    sint32  m_tgtIdx;
};

//////////////////////////////////////////////////////////////////////////
class iVisCnst_NewTeleport : public iVisCnst
{
public:
    iVisCnst_NewTeleport(iVisCnstT_NewTeleport* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_inIdx(-1), m_outIdx(-1) {}
    void OnActivate(iHero* pVisitor, bool bActive);
    inline uint8 InIndex() const { return m_inIdx; }
    inline uint8 OutIndex() const { return m_outIdx; }
    bool CustomPopupText(const iHero* pVisitor, iStringT& ctext) const;
    bool ShowVisitedLabel() const { return false; }
    virtual void Serialize(iDynamicBuffer& dbuff);
    virtual uint8 Unserialize(iDynamicBuffer& dbuff);

private:
    uint8   m_inIdx;
    uint8   m_outIdx;
};


//////////////////////////////////////////////////////////////////////////
class iVisCnst_Keymaster : public iVisCnst
{
public:
    iVisCnst_Keymaster(iVisCnstT_Keymaster* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true) {}
    uint8 GetKeyType() const;
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;
};

//////////////////////////////////////////////////////////////////////////
class iVisCnst_TreeOfKnowledge : public iVisCnst
{
public:
    iVisCnst_TreeOfKnowledge(iVisCnstT_TreeOfKnowledge* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true) {}
    void OnActivate(iHero* pVisitor, bool bActive);
};

//////////////////////////////////////////////////////////////////////////
class iVisCnst_Windmill : public iVisCnst
{
public:
    iVisCnst_Windmill(iVisCnstT_Windmill* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_bVisited(false) {}
    void NewWeek(const iWeekDesc& weekDesk);
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);
    void Unvisit(const iHero *pVisitor) const {};

private:
    void UpdateMinerals();

private:
    MINERAL_TYPE    m_mtype;
    sint32          m_mcnt;
    bool            m_bVisited;
};
//////////////////////////////////////////////////////////////////////////
class iVisCnst_WeeklyMin : public iVisCnst
{
public:
    iVisCnst_WeeklyMin(iVisCnstT_WeeklyMin* pProto, const iPoint& pos) : iVisCnst(pProto, pos, true), m_bVisited(false) {}
    void NewWeek(const iWeekDesc& weekDesk);
    void OnActivate(iHero* pVisitor, bool bActive);
    bool Visited(const iHero* pVisitor) const;
    void Serialize(iDynamicBuffer& dbuff);
    uint8 Unserialize(iDynamicBuffer& dbuff);
    void Unvisit(const iHero *pVisitor) const {};

private:
    bool            m_bVisited;
};

/*
 *  Ownerables
 */
class iOwnCnst : public iMapCnst, public iArmyOwner
{
public:
    IMPL_TYPEAWARE( iOwnCnst );
    iOwnCnst(iOwnCnstT* pProto, const iPoint& pos);
    virtual ~iOwnCnst() {}

    virtual iPlayer* GetArmyOwnerPlayer();

	inline const iOwnCnstT* Proto() const { return m_pProto; }
    inline iOwnCnstT* Proto() { return m_pProto; }
    inline PLAYER_ID Owner() const { return m_Owner; }   
    inline const iArmy& Guard() const { return Army(); }
    void SetOwner(PLAYER_ID newOwner);

	virtual void Unserialize(iDynamicBuffer& dbuff);
	virtual void Serialize(iDynamicBuffer& dbuff);
    virtual void OnOwnerChanged(PLAYER_ID newOwner, PLAYER_ID oldOwner);
    virtual void NewDay();
    virtual void NewWeek(const iWeekDesc& weekDesk);
    bool Activate(iHero* pVisitor, bool bActive, bool& bDisappear);
    virtual void OnActivate(iHero* pVisitor, bool bActive);

	inline iStringT& IntroMessage() { return m_introMessage; }
	inline iReqsCtr& Reqs() { return m_reqs; }
	inline iStringT& CurFailMessage() { return m_curFailMessage; }
	inline iRewardsCtr& OKRewards() { return m_okRewards; }
	inline iStringT& OKRewardMessage() { return m_okMessage; }	
	inline iRewardsCtr& PermFailRewards() { return m_permFailRewards; }
	inline iStringT& PermFailRewardMessage() { return m_permFailMessage; }
	inline CNST_REW_BEH& OkRewBehaviour() { return m_okRewBeh; }
	inline CNST_REW_BEH& PermFailRewBehaviour() { return m_permFailRewBeh; }	


	virtual bool CanBeEmpty() const { return true; }
protected:    
    iOwnCnstT*  m_pProto;
    PLAYER_ID   m_Owner;
	
	iStringT	m_introMessage;
	iReqsCtr	m_reqs;
	iStringT	m_curFailMessage;
		
	iRewardsCtr	m_okRewards;
	iStringT	m_okMessage;
	CNST_REW_BEH   m_okRewBeh;

	iRewardsCtr	m_permFailRewards;
	iStringT	m_permFailMessage;
	CNST_REW_BEH   m_permFailRewBeh;  
	bool		m_bDisabled;
	uint16		m_seenIntro;
};

//////////////////////////////////////////////////////////////////////////
class iOwnCnst_FurtSkill : public iOwnCnst
{
public:
    iOwnCnst_FurtSkill(iOwnCnstT_FurtSkill* pProto, const iPoint& pos) : iOwnCnst(pProto, pos) {}
    void OnOwnerChanged(PLAYER_ID newOwner, PLAYER_ID oldOwner);
};


#endif //PHEROES_GAME_CONSTRUCTIONS_H_
