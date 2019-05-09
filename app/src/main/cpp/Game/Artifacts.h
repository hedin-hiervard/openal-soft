#ifndef PHEROES_GAME_ARTIFACTS_H_
#define PHEROES_GAME_ARTIFACTS_H_

/*
 *	Artifacts container
 */
struct iArtCell {
	iArtCell(HERO_ART_CELL _cell, uint16 _artId = 0xFFFF) : cell(_cell), artId(_artId) {}
	inline bool Empty() const { return artId == 0xFFFF;}
	inline void Reset() { artId = 0xFFFF; }
	ART_ASSIGN_TYPE AssignType();

	HERO_ART_CELL	cell;
	uint16			artId;
};

typedef iSimpleArray<uint16>	iArtList;
typedef iSimpleArray<iArtCell>	iArtCellList;
//
class iArtCtr
{
public:	
	iArtCtr(iHero *pOwner);
	void Update(iHero *pOwner);

	//
	uint16 Pop();

	// 
	bool HasArtType(ART_TYPE artt) const;
	bool HasCreatBonus(CREATURE_TYPE ct, iCreatBonusList &cbl) const;
	bool HasMagicSphere(MAGIC_SCHOOL sch) const;
	bool CanAttach(uint16 artId) const;
	void Push(uint16 artId);
	bool CanSmartPush(uint16 artId) const;
	void SmartPush(uint16 artId);
	void PushDressed(uint16 artId, HERO_ART_CELL cell);
	void PushToBackPack(uint16 artId);
	uint16 Pop(bool bDressed, uint32 idx);
	void PopBackPack(iArtList& artList);
	void PopAll(iArtList& artList);
	void GetAll(iArtList& artList) const;

	bool HasArtifact(uint16 artId) const;
	sint32 SmartPop(uint16 artId);

	// Backpack
	inline uint32 BackPackCount() const { return m_BackPack.GetSize(); }
	inline uint16 BackPackItem(uint32 idx) const { check(idx<m_BackPack.GetSize()); return m_BackPack[idx]; }
	inline const iArtCell& DressedItem(HERO_ART_CELL cell) const { return m_Dressed[cell]; }
	inline iArtCell& DressedItem(HERO_ART_CELL cell) { return m_Dressed[cell]; }
	inline iArtList& BackPack() { return m_BackPack; }

	uint32 TotalCount() const;

private:
	iArtCellList	m_Dressed;
	iArtList		m_BackPack;
	iHero*			m_pOwner;
};


#endif //PHEROES_GAME_ARTIFACTS_H_
