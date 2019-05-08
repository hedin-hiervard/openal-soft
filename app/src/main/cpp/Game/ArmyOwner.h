#ifndef PHEROES_GAME_ARMYOWNER_H_
#define PHEROES_GAME_ARMYOWNER_H_
class iCreatGroup;
class iPlayer;
class iHero;
class iArmy;
class iCastle;

class iArmyOwner
{
private:
	iArmy		m_Army;
	sint32		m_changedCell;
public:	
	iArmyOwner(): m_changedCell(-1) {}
	inline const iArmy& Army() const { return m_Army; }
	inline void ResetChangedCell() { m_changedCell = -1; }
	sint32 ChangedCell() const { return m_changedCell; }
	bool AddGroupToArmy(CREATURE_TYPE ct, sint32 count);
	bool JoinArmyGroups();
	void UpgradeArmyGroup(uint32 idx, iCreatGroup::UpgradeWay uw); 
	void ResetArmy();
	void SetArmyGroup(sint32 idx, const iCreatGroup& cg);
	void ResetArmyGroup(sint32 idx);
	void SetArmyGroupCount(sint32 idx, sint32 count);
	void AddArmyGroupCount(sint32 idx, sint32 add);
	void SetArmyGroupType(sint32 idx, CREATURE_TYPE ct);
	void InitArmy();
	void InitArmyRandoms();
	void GrowArmyGroup(sint32 idx, uint32 weeks = 1);
	void UnserializeArmy(iDynamicBuffer& dbuff);
	void SetArmy(const iArmy& anotherArmy);
	void SwapArmyGroups(sint32 idx1, sint32 idx2, iArmyOwner* other);

	uint32 TotalCreatures() const;

	virtual iHero* GetArmyOwnerHero() { return NULL; }
	virtual iCastle* GetArmyOwnerCastle() { return NULL; }
	virtual iPlayer* GetArmyOwnerPlayer() { return NULL; }

	virtual bool CanBeEmpty() const { return true; }

	void CheckVictory();
};
#endif //PHEROES_GAME_ARMYOWNER_H_
