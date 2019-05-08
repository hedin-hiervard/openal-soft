#ifndef PHEROES_GAME_AI_SCANNER_H_
#define PHEROES_GAME_AI_SCANNER_H_

/*
 *	Hero distribution map
 */

class iDistMapProcessor
{
protected:
	struct iMark {
		unsigned int	dir:4;
		unsigned int	move:14;
		unsigned int	touch:14;
		unsigned int    ship:1;
	};

	struct iOpenCell {
		iOpenCell(sint16 _posX, sint16 _posY, sint32 _val, bool _ship) : posX(_posX), posY(_posY), val(_val), ship(_ship) {}
		sint16 posX;
		sint16 posY;
		sint32 val;
		bool   ship; // means current cell needs ship to reach		
	};

	typedef iSortArray<iOpenCell>	iOpenCells;
	typedef iMapT<iMark>			iDistMap;
	typedef iMapT<sint8>			iPassMap;
	typedef iMapT<uint8>			iCellCfgMap;
	typedef iMapT<uint8>			iTouchMap;

public:
	friend class iDangerMap;

	iDistMapProcessor(const iSize& metrics);
	
	void MakeDistMap(const iPoint& pos, sint32 armyPower, sint32 moveBonus, bool bInShip, PLAYER_ID owner, sint32 totalMoves);

	inline uint16 MoveCost(sint16 px, sint16 py) const { return m_distMap.GetAt(px, py).move; }	
	inline uint16 TouchCost(sint16 px, sint16 py) const { return m_distMap.GetAt(px, py).touch; }
	inline bool NeedShip(sint16 px, sint16 py) const { return m_distMap.GetAt(px, py).ship; }
	inline uint8 Dir(sint16 px, sint16 py) const { return m_distMap.GetAt(px, py).dir; }
	inline bool IsTouchPoint(sint16 px, sint16 py) const { return (m_tobjMap.GetAt(px, py) & 0x7) != 0 /*&& !(m_tobjMap.GetAt(px, py) & 0xF8)*/; }

protected:
	void UpdatePassMap(const iPoint& pos, sint32 armyPower, sint32 moveBonus, PLAYER_ID owner);

protected:
	iDistMap		m_distMap;
	iPassMap		m_passMap;
	iCellCfgMap		m_cellCfgMap;
	iTouchMap		m_tobjMap;
	iOpenCells		m_openCells;
};

/*
 *	Target scanner
 *  used to define one task for one hero at a turn
 *  doesn't have any memory
 */
class iTargetScanner
{
public:
	enum ProcessFlags {
		PRF_NORMAL			= 0x00,
		PRF_IGN_ARMYPOWER	= 0x01,
		PRF_IGN_CURLOC		= 0x02,
		PRF_IGN_VISITABLES	= 0x04,
		PRF_IGN_FCASTLES	= 0x08,
		PRF_IGN_DISTANCE	= 0x10
	};

	enum Result {
		Process,
		Wait,
		Reject
	};

public:
	iTargetScanner(iHero* pActHero, const iDangerMap& dangerMap);
	iTargetScanner(const iHero* pHero, const iDangerMap& dangerMap, iAI_Player* pOwner, const iPoint& origin, sint32 armyPower);
	bool DefineTask(const iVisItemList& mapItems);
	fix32 LookAround(const iVisItemList& mapItems, iSortArray<iBaseMapObject*>& surObjects);

private:
	iPoint SelectTarget(const iVisItemList& mapItems, const iShPointList& discoverPoints, sint32 moves, uint32 flags, sint32& priority, bool &bNeedShip);
	Result ProcessObject(iBaseMapObject* pObj, sint32& priority, uint32 flags);
	iPoint MakePath(const iPoint& target, iPath& path);

private:
	iDistMapProcessor	m_distMap;
	const iDangerMap&	m_dangerMap;
	const iHero*		m_pHero;
	iHero*				m_pActHero;
	iAI_Player*			m_pOwner;
	iPoint				m_origin;
	sint32				m_armyPower;
};



#endif //PHEROES_GAME_AI_SCANNER_H_
