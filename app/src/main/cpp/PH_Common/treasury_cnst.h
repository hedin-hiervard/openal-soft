#ifndef PH_COMMON_TREASURY_CNST_H_
#define PH_COMMON_TREASURY_CNST_H_

// Treasury variants container
class iTrVarCtr
{
public:
	struct Item {
		Item(uint32 _probability) : probability(_probability) {}
		uint32		probability;
		iArmy		guards;
		iRewardsCtr	rewards;
	};

public:
	void AddVariant(uint32 probability);
	Item& GetLastVariant() { return m_items.GetLast(); }
	Item& GetVariant(uint32 idx) { return m_items[idx]; }
	const Item& GetVariant(uint32 idx) const { return m_items[idx]; }
	uint32 VariantsCount() const { return m_items.GetSize(); }
	void Serialize(iDynamicBuffer& dbuff) const;
	void Unserialize(iDynamicBuffer& dbuff);

private:
	iSimpleArray<Item>	m_items;
};


#endif //PH_COMMON_TREASURY_CNST_H_
