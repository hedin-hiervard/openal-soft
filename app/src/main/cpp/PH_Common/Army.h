#ifndef PH_COMMON_ARMY_H_
#define PH_COMMON_ARMY_H_

/////////////////////////////////////////////////////////////////////////
class iCreatGroup
{
private:
	CREATURE_TYPE	m_creatType;
	sint32			m_count;

public:
	enum UpgradeWay {
		UW_FIRST = 0,
		UW_SECOND
	};
public:
	iCreatGroup(CREATURE_TYPE ct = CREAT_UNKNOWN, sint32 count = RANDOM_QUANTITY);

	inline void Reset(CREATURE_TYPE ct = CREAT_UNKNOWN, sint32 count = RANDOM_QUANTITY) { m_creatType = ct; m_count = count; }
	inline bool IsValid() const { return m_creatType != CREAT_UNKNOWN; }
	inline CREATURE_TYPE& Type() { return m_creatType; }
	inline CREATURE_TYPE Type() const { return m_creatType; }
	inline sint32& Count() { return m_count; }
	inline sint32 Count() const { return m_count; }
	inline uint32 GroupPower() const { return m_count*CREAT_DESC[m_creatType].pidx; }
	void Grow(uint32 weeks = 1);
    void Upgrade(UpgradeWay way);
	CREATURE_TYPE GetUpgradedType(UpgradeWay way) const;
	bool inline Upgradeable() const { return GetUpgradedType(UW_FIRST) != CREAT_UNKNOWN || GetUpgradedType(UW_SECOND) != CREAT_UNKNOWN; }
	void Init();
	void InitRandoms();
};

//////////////////////////////////////////////////////////////////////////
class iArmy
{
private:
	iCreatGroup		m_creatGroups[7];

public:	

	bool CanAddGroup(CREATURE_TYPE ct) const;
	sint32 AddGroup(CREATURE_TYPE ct, sint32 count);
	sint32 CreatureCount(CREATURE_TYPE ct) const;
	sint32 SlowestSpeed() const;
	const iCreatGroup& WeakestCreatures() const;
	sint32 WeakestCreaturesIdx() const;
	const iCreatGroup& WeakestGroup() const;
	sint32 WeakestGroupIdx() const;
	bool JoinGroups();
	void ResetGroup(uint32 idx);
	void ResetArmy();
	void SetGroup(uint32 idx, CREATURE_TYPE ct, sint32 count);
	CREATURE_TYPE StrongestCreature() const;
	sint32 MoraleModifier() const;
	sint32 FearFactor() const;

	inline bool Empty() const 
	{ 
		for (uint32 xx=0; xx<7; ++xx) 
			if (m_creatGroups[xx].Type() != CREAT_UNKNOWN || m_creatGroups[xx].Count() != 0) 
				return false; 
		return true; 
	}

	inline bool HasCreatures(CREATURE_TYPE ct) const { for (uint32 xx=0; xx<7; ++xx) if (m_creatGroups[xx].Type() == ct) return true; return false; }
	inline iCreatGroup& FirstGroup() { for (uint32 xx=0; xx<7; ++xx) if (m_creatGroups[xx].Type() != CREAT_UNKNOWN) return m_creatGroups[xx]; check(0); return m_creatGroups[0]; }
	inline sint32 GroupCount() const { sint32 cnt=0; for (uint32 xx=0; xx<7; ++xx) if (m_creatGroups[xx].Type() != CREAT_UNKNOWN) ++cnt; return cnt; }
	inline uint32 ArmyPower() const  { uint32 res=0; for (uint32 xx=0; xx<7; ++xx) if (m_creatGroups[xx].Type() != CREAT_UNKNOWN) res += m_creatGroups[xx].GroupPower(); return res; }
	
	inline const iCreatGroup& At(uint32 idx) const { check(idx<7); return m_creatGroups[idx]; }
	inline iCreatGroup& At(uint32 idx) { check(idx<7); return m_creatGroups[idx]; }
	inline const iCreatGroup& operator[](uint32 idx) const { return At(idx); }
	inline iCreatGroup& operator[](uint32 idx) { return At(idx); }

	void operator= (const iArmy& other)	{ memcpy(m_creatGroups, other.m_creatGroups, sizeof(m_creatGroups)); }
};



#endif //PH_COMMON_ARMY_H_
