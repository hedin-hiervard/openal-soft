#ifndef PHEROES_GAME_CASTLES_H_
#define PHEROES_GAME_CASTLES_H_

/*
 *	Castle
 */

#include "harbor.h"

class iPlayer;
class iCastle : public iMapCnst, public iArmyOwner, public IHarbor
{
friend class iGameWorld;
public:
	IMPL_TYPEAWARE( iCastle );
	iCastle(iCastleT* pProto, const iPoint& pos, const iStringT& cname, bool bCreateHero);

	inline const iCastleT* Proto() const { return m_pProto; }
	inline PLAYER_ID Owner() const { return m_Owner; }
	virtual iPlayer* GetArmyOwnerPlayer();
	
	void SetOwner(PLAYER_ID newOwner);
	//const iStringT& Name() const;
	iStringT& Name();
	inline TextResId NameKey() const { return m_NameKey; };

	void Init();
	void OnSelect();
	void OnVisitorEnter(iHero* pHero);
	void OnVisitorLeave(iHero* pHero);

	void NewDay();
	void NewWeek(const iWeekDesc& weekDesk);

	uint32 GetConstrCaps();

	inline iCtlCnst* FindCnst(CTL_CONSTR cnst) { return m_Cnsts.GetCnst(cnst); }
	inline iCtlCnst* Cnst(uint32 idx) { return m_Cnsts[idx]; }
		inline iCtlCnsts& Cnsts() { return m_Cnsts; }
	iTavernCtlCnst* GetTavern(bool bOnlyBuilt);
	inline const iArmy& Garrison() const { return Army(); }
	uint32 GarrisonPower(bool bOnlyFort = false) const;
	
	sint32 Scouting() const;
	inline sint32 ScoutMod() const { return m_scoutMod; }
	inline sint32& ScoutMod() { return m_scoutMod; }

	inline bool CanBuild() const { 		
		return m_bCanBuild; 
	}
	inline void SetBuilt(bool val)  { m_bCanBuild = val; }
	void Build(iCtlCnst* pCnst);
	void Build(uint32 cnstIdx);
	void UpdateCnsts();
	iDwellCtlCnst* GetDwelling( CREATURE_TYPE ct );

	bool CheckPathToShip(iPlayer* pPlayer, const iPoint &pt);

	virtual iCastle* GetArmyOwnerCastle() { return this; }
	bool IsMGuildBuilt();

	// implementation of IHarbor
	virtual void ShipBought(uint8 dock); // informs the structure that ship was bought in a dock
	virtual uint8 ShipsAvailable(uint8 dock); // returns the number of ships available in the dock
	virtual iShip* GetShip( uint8 dockType ); // returns the ship in the selected dock or NULL if none
	virtual iPoint GetShipPos( uint8 dockType ); // returns the ship position for the selected dock
	virtual bool IsShipBuildSupported(); // returns if the structure can build ships
    virtual uint8 NumOfDocks(); // returns number of docks available
	virtual iPlayer* GetOwner(); // get owner of the structure
	virtual bool IsBoardable(); // returns if the harbor has the ability to board hero directly from it	

	virtual bool CanBeEmpty() const { return true; }	
private:
	void GenerateGuards();		
protected:
	sint32		m_scoutMod;	
	iStringT	m_Name;	
	iCtlCnsts	m_Cnsts;
	bool		m_bCanBuild;
	iCastleT*	m_pProto;
	PLAYER_ID	m_Owner;
	bool		m_bCreateHero;
	bool		m_bIsCustomName;
	TextResId	m_NameKey;
    uint8       m_shipsCount;
};



#endif //PHEROES_GAME_CASTLES_H_
