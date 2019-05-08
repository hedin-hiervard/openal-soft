#ifndef PHEROES_GAME_PATHFINDING_H_
#define PHEROES_GAME_PATHFINDING_H_

const uint8 CNST_PASS_MASK = 0x38;
const uint8 CSTL_PASS_MASK = 0x28;
extern const sint16 nOffs[8][2];


/*  
 *	
 */
class iPath
{
public:
	enum Action {
		None = 0,	// Invalid action
		MoveTo,		// Move to specified point
		Attack,		// Attack guard at specified point
		Touch,		// touch item at specified point
		Enter,		// Enter into construction
		Meet,		// Hero meets friendly heroes
        Leave,      // Hero leaves ship		
        Occupy      // Hero occupy ship
	};

	struct Target {
		Target() : action(MoveTo), epmask(0xFF) {}
		sint32	action;
		uint32	epmask;
	}; // 4 + 4  = 8 bytes

	struct iStep {
		iStep()
		: m_Cost(0) {}
		iStep(const iPoint& pos, sint32 cost, Action act)
		: m_Pos(pos), m_Cost(cost), m_action(act) {}
		iPoint	m_Pos;
		sint32	m_Cost;
		sint32	m_action;
	}; //  8 + 4 + 4 = 16 bytes

	typedef iSimpleArray<iStep>			iStepArray;

	// c-tor, d-tor
	iPath(const iPoint& pos) { SetEmpty(pos); }
	~iPath() {}

	void SetEmpty(const iPoint& pos)
	{
		m_Start = pos;
		m_Path.RemoveAll();
	}

	inline bool IsEmpty() const
	{ return m_Path.GetSize() == 0; }

	inline void SetTargetAction(Action act)
	{ m_Path.GetLast().m_action = act; }

	inline sint32 TargetAction() const
	{ return m_Path.GetLast().m_action; }

	inline const iPoint& DestPoint() const
	{ return m_Path.GetSize() ? m_Path.GetLast().m_Pos : m_Start; }

	inline bool IsDestPoint(const iPoint& pos)
	{ return m_Path.GetSize() && m_Path.GetLast().m_Pos == pos; }
	
	inline bool BelongToPath(const iPoint& pos)
	{
		for (uint32 xx=0; xx<m_Path.GetSize(); ++xx) if (m_Path[xx].m_Pos == pos) return true;
		return false;
	}

	inline sint32 PathCost() const
	{
		sint32 result=0;
		for (uint32 xx=0; xx<m_Path.GetSize(); ++xx)  result += m_Path[xx].m_Cost;
		return result;
	}

	inline iStep GetStep() const
	{ check(!IsEmpty()); return m_Path[0]; }

	inline iPoint GetStepPos() const
	{ check(!IsEmpty()); return m_Path[0].m_Pos; }

	inline sint32 GetStepCost() const
	{ check(!IsEmpty()); return m_Path[0].m_Cost; }

	sint32 GetStepCostTillValid() const;

	
	inline fix32 GetStepLength() const
	{
		if (IsEmpty()) return fix32();
		iPoint p = m_Path[0].m_Pos;
		sint32 x_diff = p.x - m_Start.x;
		sint32 y_diff = p.y - m_Start.y;
		if (x_diff == 0 || y_diff ==0) return fix32(0.5f);
		else return fix32(0.75f);
	}

	inline iStep Step()
	{
		check(m_Path.GetSize());
		iStep res = m_Path.PopFirst();
		m_Start = res.m_Pos;
		return res;
	}

	uint8 GetAngle() const
	{
		if (IsEmpty()) return 0;
		iPoint p = m_Path[0].m_Pos;
		// dumb ass lookup		
		sint32 idx = 4 + iSIGN( p.x - m_Start.x )*3 + iSIGN( p.y - m_Start.y );
		check( idx >= 0 );
		static const uint8 dirs[9] = { 0, 7, 6, 1, 0, 5, 2, 3, 4 };
		return dirs[idx];
/*
		sint32 x_diff = p.x - m_Start.x;
		sint32 y_diff = p.y - m_Start.y;
		if (x_diff<0 && y_diff<0) return 0;
		else if (x_diff==0 && y_diff<0) return 1;
		else if (x_diff>0 && y_diff<0) return 2;
		else if (x_diff>0 && y_diff==0) return 3;
		else if (x_diff>0 && y_diff>0) return 4;
		else if (x_diff==0 && y_diff>0) return 5;
		else if (x_diff<0 && y_diff>0) return 6;
		else if (x_diff<0 && y_diff==0) return 7;
		else return 0;
*/
	}

	// used solely by the path finder
	inline void AddMoveToPoint( const iPoint& pt, sint32 cost, bool bLeaveShip = false )
	{
		if (!m_Path.GetSize()) m_Path.Add(iStep( pt, cost, bLeaveShip ? Leave : MoveTo ) );
		else m_Path.InsertBefore(0, iStep( pt, cost, bLeaveShip ? Leave : MoveTo ) );
	}

    // used solely by the path finder
    // version for FindPathForShip
    inline void AddMoveToPointShip( const iPoint& pt, sint32 cost, bool leave_ship)
    {
        m_Path.Add(iStep( pt, cost, leave_ship ? Leave : MoveTo ) );        
    }

#if 1
	void DumpPath() const;

	/// transforms path adding draining all movement points when boarding or leaving ship
	void ProcessShipOperations(sint32 initialMoves);
#endif
	iPoint		m_Start;
	iStepArray	m_Path;
};

//////////////////////////////////////////////////////////////////////////

/*
 *	Since the new pathfinder uses statically allocated data
 *	It couldn't be called recursively.
 *	Additionally that statically allocated data could be purged
 *	to free up memory. That could be done in high-level functions
 *	after successive calls to the pathfinder.
 */
class iPathFinder 
{
private:
	// disable copy construction & assignment
	iPathFinder( const iPathFinder& );
	iPathFinder& operator=( const iPathFinder& );

public:

	enum {
		SUnknown	= 0,
		SOpen		= 1,
		SClosed		= 2,
		SImpassable	= 3,
	};

	typedef uint16 PYX;
		
	struct Mark {
		uint8	state:3;// cached cell state /unknown/closed/open/impassable/
        uint8	flag:1;// ground -> water flag
		uint8	dir:4;	// stored direction (valid only for closed cells)
		uint8	pass;	// edge passability flags
		uint16	g;
	}; // 4 bytes * mW * mH

	struct Node {
		PYX		yx;		// packed position
		uint16	g;		// sum of g
	}; // 4 bytes * mW * mH

	// NB: 'nodes' and 'heap' data structures use element 0 as a sentinel, it doesnt belong to the actual data

	// heap structure - made static
	typedef uint32 HeapVal;
	//	HeapVal*	heap;	
	//	size_t		lastHeapNode;

	// world map 
	Mark*	world;		// mW * mH

	// tracked nodes - made static
	//	Node*	nodes;		
	//	size_t	maxNodesNum;	

	size_t	mapW,mapH;

	const sint8* passMap;

	typedef iMapT<sint8>		iPassMap;

public:
	iPathFinder(const iPassMap& pmap);
	~iPathFinder();

	// returns path length or -1 if failed
	sint32 FindPath(const iPoint& srcPos, const iPoint& dstPos, iPath& path, uint8 spMask, uint8 epMask, bool is_water = false);
    // special version when hero in the ship
    sint32 FindPathForShip(const iPoint& srcPos, const iPoint& dstPos, iPath& path, uint8 spMask, uint8 epMask);
    // special version when hero on land and target is ship
    sint32 FindPathToShip(const iPoint& srcPos, const iPoint& dstPos, iPath& path, uint8 spMask, uint8 epMask);

	// Free up internal static structures
	static void Purge();

private:

	static	size_t		entryCount;
	static	size_t		maxNodesNum;
	static	Node*		nodes;
	static	HeapVal*	heap;
	static 	size_t		lastHeapNode;

	// internal heap functions
	void	HeapRemoveRoot();
	size_t	HeapInsert( HeapVal n );
	size_t	HeapRebuild( size_t index );

	void	SetupEntryExitFlags( uint16 sp, uint16 ep, uint8 exitMask, uint8 entryMask );
	void	PrepareEmptyWorld();
};




#endif //PHEROES_GAME_PATHFINDING_H_
