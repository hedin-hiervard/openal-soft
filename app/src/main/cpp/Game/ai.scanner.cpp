#include "stdafx.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif

#include "ai.scanner.h"
#include "ai.explore.h"
#include "dbg.h"

#if defined( OS_APPLE ) || defined(OS_ANDROID)
void DebugOutput(const char * str);
#endif
void DebugOutputT(iStringT str) {
	const char * s = CvtT2A<>(str.CStr());

#if defined( OS_APPLE ) || defined(OS_ANDROID)
	DebugOutput(s);
#endif
}

//#define DEBUG_AISCANNER
#undef _LOG
#ifdef DEBUG_AISCANNER
#define _LOG(x) DebugOutputT(x)
#else
#define _LOG(x)
#endif //DEBUG_OVERLAND_AI


enum TSP_ENTRIES {
	tspTargetScannerCnst = 0,
	tspDistMapProcessorCnst,
	tspUpdatePassMap,
	tspMakeDistMap,
	tspSelectTarget,
	tspProcessObject,
	tspDiscoverPoint,
	tspMakePath,
	tspDefineTask,
	tspCount
};

//#define PROFILE_TARGET_SCANNER
#ifdef PROFILE_TARGET_SCANNER
static TCHAR* TSPROFILE_ENTRIES_TEXT[tspCount] = {
	_T("TargetScannerCnst"),
	_T("DistMapProcessorCnst"),
	_T("UpdatePassMap"),
	_T("MakeDistMap"),
	_T("SelectTarget"),
	_T("ProcessObject"),
	_T("DiscoverPoint"),
	_T("MakePath"),
	_T("DefineTask")
};

struct TSP_ENTRY {
	TSP_ENTRY() : count(0), time(0) { }

	uint32	count;
	uint32	time;
};

static TSP_ENTRY tspEntries[tspCount];

struct BoundTimer {
	BoundTimer(TSP_ENTRIES _entry) : entry(_entry) {}

	~BoundTimer() { tspEntries[entry].time += timer.GetStep(); }

	TSP_ENTRIES		entry;
	iTimer			timer;
};

void DumpTargetScannerResults()
{
	TCHAR buff[256];
	_LOG(_T("Target scanner profiling results\n------------------------------------\n"));
	for (uint32 xx=0; xx<tspCount; ++xx) {
		_stprintf(buff, _T("%22s: %5d times, %10d msec\n"), TSPROFILE_ENTRIES_TEXT[xx], tspEntries[xx].count, tspEntries[xx].time);
		_LOG(buff);
	}
	_LOG(_T("------------------------------------\n"));
}

struct iDumpDummy{
	~iDumpDummy() {
		DumpTargetScannerResults();
	}
};
static iDumpDummy sumpDummy;

#define TSP_INCCOUNT(x) (tspEntries[x].count++)
#define TSP_BOUNDTIMER(x) BoundTimer btimer(x)
#else //PROFILE_TARGET_SCANNER
#define TSP_INCCOUNT(x)
#define TSP_BOUNDTIMER(x)
#endif //PROFILE_TARGET_SCANNER

inline sint32 StepCost(sint32 mbonus, sint32 cost)
{
	if(cost == PASSMAP_PWATER || cost == PASSMAP_SWATER) return cost;
	if (mbonus && cost > SURF_MOVE_COST[STYPE_DIRT]) cost -= (cost-SURF_MOVE_COST[STYPE_DIRT]) * mbonus / 100;
	return cost;
}

/*
*	Hero distribution map
*/
iDistMapProcessor::iDistMapProcessor(const iSize& metrics)
: m_distMap(metrics.w, metrics.h)
, m_cellCfgMap(metrics.w, metrics.h)
, m_tobjMap(metrics.w, metrics.h)
, m_passMap(metrics.w, metrics.h)
{
	TSP_INCCOUNT(tspDistMapProcessorCnst);
}

void iDistMapProcessor::UpdatePassMap(const iPoint& pos, sint32 armyPower, sint32 moveBonus, PLAYER_ID owner)
{
	TSP_INCCOUNT(tspUpdatePassMap);
	TSP_BOUNDTIMER(tspUpdatePassMap);
	const iGameWorld& map = gGame.Map();
	m_passMap.CopyFrom(map.m_PassMap);
	m_cellCfgMap.ZeroMem(0xFF);
	m_tobjMap.ZeroMem();

	// Prepare pass map
	m_passMap.CopyFrom(map.m_PassMap);
	for (sint32 yy=0; yy<m_passMap.GetHeight(); ++yy) {
		for (sint32 xx=0; xx<m_passMap.GetWidth(); ++xx){
			sint32 res;
			iBaseMapObject* pObj = gGame.Map().m_CoverMap.GetAt(xx,yy);

			if (pObj && pObj->Pos() == iPoint(xx,yy)) {
				if (iCastle* pCastle = DynamicCast<iCastle*>(pObj)) {
					if (pCastle->Owner() != owner && pCastle->Garrison().ArmyPower() > armyPower) res = -1;
					else res = 5;
					m_cellCfgMap.GetAt(xx,yy) = CSTL_PASS_MASK;
				} else if (iOwnCnst* pOwnCnt = DynamicCast<iOwnCnst*>(pObj)) {
					if (pOwnCnt->Owner() != owner && pOwnCnt->Guard().ArmyPower() > armyPower) res = -1;
					else res = 5;
					m_cellCfgMap.GetAt(xx,yy) = CNST_PASS_MASK;
				} else if (iVisCnst* pVisCnst = DynamicCast<iVisCnst*>(pObj)) {
					if (pVisCnst->Guard().ArmyPower() > armyPower) res = -1;
					else res = 5;
					m_cellCfgMap.GetAt(xx,yy) = CNST_PASS_MASK;
					if (pVisCnst->Proto()->Type() == VCNST_TELEPORT) {
						// Teleport
						iVisCnst_Teleport* pTeleport = (iVisCnst_Teleport*)pVisCnst;
						sint32 tgtIdx = pTeleport->GetTarget() + 1;

						if (tgtIdx) {
							iVisCnst_Teleport* pTgtTeleport = gGame.Map().GetTeleport(tgtIdx-1);
							// check for target visitor
							if (iHero* pTgtVisitor = DynamicCast<iHero*>(gGame.Map().m_CoverMap.GetAt(pTgtTeleport->Pos().x,pTgtTeleport->Pos().y))) {
								// target teleport has visitor
								check(pTgtVisitor->GetLocation() == pTgtTeleport);
								if (pTgtVisitor->Pos() == pos) {
									// myself
									m_tobjMap.GetAt(xx,yy) = (tgtIdx << 3);
								} else if (pTgtVisitor->Owner()->PlayerId() == owner) {
									// friendly hero
								} else {
									// enemy hero
									if (pTgtVisitor->Army().ArmyPower() > armyPower) {
										// Enemy hero army is too strong, so stay away from this teleport
										res = -1;
									} else {
										// hero at target teleport
										m_tobjMap.GetAt(xx,yy) = (tgtIdx << 3);
									}
								}
							} else {
								// target teleport is clear
								m_tobjMap.GetAt(xx,yy) = (tgtIdx << 3);
							}
						}
					} else if (pVisCnst->Proto()->Type() == VCNST_NEWTELEPORT) {
						// New Teleport
						iVisCnst_NewTeleport* pTeleport = (iVisCnst_NewTeleport*)pVisCnst;

						if (pTeleport->OutIndex() != 0) {
							iSimpleArray<iBaseMapObject*> list = gGame.Map().FindTeleportDestinations(pTeleport, pTeleport->OutIndex());

							for(uint32 zz=0; zz<list.GetSize(); zz++) {
								iBaseMapObject* pTgt = list[zz];
								// check for target visitor
								uint8 tgtIdx = pTeleport->OutIndex();
								if (iHero* pTgtVisitor = DynamicCast<iHero*>(gGame.Map().m_CoverMap.GetAt(pTgt->Pos().x,pTgt->Pos().y))) {
									// target teleport has visitor
									if (pTgtVisitor->Pos() == pos) {
										// myself
										m_tobjMap.GetAt(xx,yy) = (tgtIdx << 4);
									} else if (pTgtVisitor->Owner()->PlayerId() == owner) {
										// friendly hero
									} else {
										// enemy hero
										if (pTgtVisitor->Army().ArmyPower() > armyPower) {
											// Enemy hero army is too strong, so stay away from this teleport
											res = -1;
										} else {
											// hero at target teleport
											m_tobjMap.GetAt(xx,yy) = (tgtIdx << 4);
										}
									}
								} else {
									// target teleport is clear
									m_tobjMap.GetAt(xx,yy) = (tgtIdx << 4);
								}
							}
						}
					}
				} else if (DynamicCast<iMapItem_Mineral*>(pObj) || DynamicCast<iMapItem_ManaCrystal*>(pObj) || DynamicCast<iMapItem_CampFire*>(pObj) || DynamicCast<iMapItem_Lamp*>(pObj) || DynamicCast<iMapItem_Chest*>(pObj) || DynamicCast<iMapItem_Artifact*>(pObj) || DynamicCast<iMapItem_SpellScroll*>(pObj)) {
					iMapItem* pMapItem = (iMapItem*)pObj;
					if (pMapItem->Guard().ArmyPower() > armyPower) res = -1;
					else {
						res = StepCost(moveBonus, m_passMap.GetAt(xx,yy));
						m_tobjMap.GetAt(xx,yy) = 1;
					}
				} else if (iMapItem_KeyGuard* pKeyGuard = DynamicCast<iMapItem_KeyGuard*>(pObj)) {
					if (!gGame.Map().FindPlayer(owner)->HasKey(pKeyGuard->GetKeyType())) res = -1;
					else {
						res = StepCost(moveBonus, m_passMap.GetAt(xx,yy));
						m_tobjMap.GetAt(xx,yy) = 1;
					}
				} else if (iMapGuard* pMapGuard = DynamicCast<iMapGuard*>(pObj)) {
					if (pMapGuard->Creatures().GroupPower() > armyPower) res = -1;
					else
					{
						res = StepCost(moveBonus, m_passMap.GetAt(xx,yy));
						m_tobjMap.GetAt(xx,yy) = 1;
					}
				} else if (iShip* pShip = DynamicCast<iShip*>(pObj)) {
					// CtllTODO: finish ship code here
					res = -1;
				} else if (iHero* pHero = DynamicCast<iHero*>(pObj)) {
					if (iMapCnst* pCnst = pHero->GetLocation()) {
						// Setup cell pass mask
						if (DynamicCast<iOwnCnst*>(pCnst) || DynamicCast<iVisCnst*>(pCnst)) {
							m_cellCfgMap.GetAt(xx,yy) = CNST_PASS_MASK;
						} else if (DynamicCast<iCastle*>(pCnst)) {
							m_cellCfgMap.GetAt(xx,yy) = CSTL_PASS_MASK;
						}

						iVisCnst_NewTeleport* pTeleport = NULL;
						if (iVisCnst* pVisCnst = DynamicCast<iVisCnst*>(pCnst)) {
							if (pVisCnst->Proto()->Type() == VCNST_NEWTELEPORT) pTeleport = (iVisCnst_NewTeleport*)pVisCnst;
						}

						if (pTeleport) {
							// The target is teleport
							sint32 tgtIdx = pTeleport->OutIndex();

							if (pHero->Pos() == pos) {
								// myself
								m_tobjMap.GetAt(xx,yy) = (tgtIdx << 4);
								res = 5;
							} else if (pHero->Owner()->PlayerId() == owner) {
								// Friendly hero
								res = 5;
							} else if (pHero->Army().ArmyPower() > armyPower) {
								// Enemy hero with stronger army
								res = -1;
							} else {
								// Enemy hero with weaker army
								m_tobjMap.GetAt(xx,yy) = (tgtIdx << 4) | 1;
								res = 5;
							}

							/*if (tgtIdx) {
								// check for target visitor
								iVisCnst_Teleport* pTgtTeleport = gGame.Map().GetTeleport(tgtIdx-1);
								iHero* pTgtVisitor = DynamicCast<iHero*>(gGame.Map().m_CoverMap.GetAt(pTgtTeleport->Pos().x,pTgtTeleport->Pos().y));
								if (pTgtVisitor) {
									// target teleport has visitor
									check(pTgtVisitor->GetLocation() == pTgtTeleport);
									if (pTgtVisitor->Pos() == pos) {
										// myself
									} else if (pTgtVisitor->Owner()->PlayerId() == owner) {
										// friendly hero
										m_tobjMap.GetAt(xx,yy) = 0;
									} else {
										// enemy hero
										if (pTgtVisitor->Army().ArmyPower() > armyPower) {
											// Enemy hero army is too strong, so stay away from this teleport
											m_tobjMap.GetAt(xx,yy) = 0;
											res = -1;
										} else {
											// weak hero at target teleport
										}
									}
								} else {
									// target teleport is clear
								}
							}*/
						} else {
							// The target is (not teleport) visitable with hero
							if (pHero->Pos() == pos) {
								// myself
								res = 5;
							} else if (pHero->Owner()->PlayerId() == owner) {
								// Friendly hero
								m_tobjMap.GetAt(xx,yy) = 2;
								res = 5;
							} else if (pHero->Army().ArmyPower() > armyPower) {
								// Enemy hero with stronger army
								res = -1;
							} else {
								// Enemy hero with weaker army
								m_tobjMap.GetAt(xx,yy) = 1;
								res = 5;
							}
						}
					} else {
						// Target is standalone hero
						if (pHero->Pos() == pos) {
							// myself
							res = -1;
						} else if (pHero->Owner()->PlayerId() == owner) {
							// Friendly hero
							m_tobjMap.GetAt(xx,yy) = 2;
							res = 5;
						} else if (pHero->Army().ArmyPower() > armyPower) {
							// Enemy hero with stronger army
							res = -1;
						} else {
							// Enemy hero with weaker army
							m_tobjMap.GetAt(xx,yy) = 1;
							res = StepCost(moveBonus, m_passMap.GetAt(xx,yy));
						}
					}
				} else {
					check(0);
				}
			} else {
				res = StepCost(moveBonus, m_passMap.GetAt(xx,yy));
			}
			// CHECK:!!!
			// VC reports usage of the uninitialized variable - obviously no previous
			// conditions are satisfied
			m_passMap.GetAt(xx,yy) = (sint8)res;
		}
	}
}

const sint32 inv_dir[8] = {4, 5, 6, 7, 0, 1, 2, 3};

void iDistMapProcessor::MakeDistMap(const iPoint& pos, sint32 armyPower, sint32 moveBonus, bool bInShip, PLAYER_ID owner, sint32 totalMoves)
{
	TSP_INCCOUNT(tspMakeDistMap);
	TSP_BOUNDTIMER(tspMakeDistMap);
	// Update pass map

	UpdatePassMap(pos, armyPower, moveBonus, owner);

	//_LOG(iFormat(_T("Params: (%d, %d), %d, %d, %d, %d, %d"), pos.x, pos.y, armyPower, moveBonus, bInShip, owner, totalMoves).CStr());

	// Reset dist map
	m_distMap.ZeroMem(0xFF);

	// Prepare dist map
	//uint32 pwr = armyPower;

	// Add first cell
	bool bFirstShip = bInShip || m_passMap.GetAt(pos.x,pos.y) == PASSMAP_SWATER || m_passMap.GetAt(pos.x,pos.y) == PASSMAP_PWATER;
	m_openCells.Insert(iOpenCell((sint16)pos.x, (sint16)pos.y, 0, bFirstShip), 0);
	m_distMap.GetAt((sint16)pos.x, (sint16)pos.y).move = 0;
	m_distMap.GetAt((sint16)pos.x, (sint16)pos.y).ship = bFirstShip;


	// Propagate
	while (m_openCells.Size()) {
		iOpenCell cell = m_openCells.Pop();
		// Check for teleport
		if (m_tobjMap.GetAt(cell.posX,cell.posY) & 0xF8) {
			// Retreive target teleport index
			sint32 tidx = (m_tobjMap.GetAt(cell.posX,cell.posY) >> 3) - 1;
			iVisCnst_Teleport* pTarget = gGame.Map().GetTeleport(tidx);
			if(pTarget) {
				iOpenCell nCell(pTarget->Pos().x, pTarget->Pos().y, cell.val + 5, cell.ship);
				if (nCell.val <= totalMoves && m_distMap.GetAt(nCell.posX, nCell.posY).move > nCell.val) {
					m_distMap.GetAt(nCell.posX,nCell.posY).move = (uint16)nCell.val;
					m_distMap.GetAt(nCell.posX,nCell.posY).touch = (uint16)nCell.val;
					m_distMap.GetAt(nCell.posX, nCell.posY).dir = 0x8;
					m_openCells.Insert(nCell, -nCell.val);
				}
			}
		}

		// Check for new teleport
		if (m_tobjMap.GetAt(cell.posX,cell.posY) & 0xF0) {
			// Retreive target teleport index
			sint32 tidx = (m_tobjMap.GetAt(cell.posX,cell.posY) >> 4);
			iSimpleArray<iBaseMapObject*> list = gGame.Map().FindTeleportDestinations(NULL, (uint8)tidx);
			for(uint32 xx=0; xx < list.GetSize(); xx++) {
				iBaseMapObject* pTarget = list[xx];
				if(pTarget->Pos().x != cell.posX || pTarget->Pos().y != cell.posY) {
					iOpenCell nCell(pTarget->Pos().x, pTarget->Pos().y, cell.val + 5, cell.ship);
					if (nCell.val <= totalMoves && m_distMap.GetAt(nCell.posX, nCell.posY).move > nCell.val) {
						m_distMap.GetAt(nCell.posX,nCell.posY).move = (uint16)nCell.val;
						m_distMap.GetAt(nCell.posX,nCell.posY).touch = (uint16)nCell.val;
						m_distMap.GetAt(nCell.posX, nCell.posY).dir = 0x8;
						m_openCells.Insert(nCell, -nCell.val);
					}
				}
			}
		}

		// Propagate 8 directions
		uint8 pm = m_cellCfgMap.GetAt(cell.posX,cell.posY);
		for (uint32 nid = 0; nid<8; ++nid) {
			// We can't propagate in this direction (no exit)
			if ( !(pm & (1<<nid)) ) {
				continue;
			}
			iOpenCell nCell(cell.posX + nOffs[nid][0], cell.posY + nOffs[nid][1], cell.val, cell.ship);
			// Valid and passable cell (>= 0 means land, -128 means full water, -127 means partial water
			if (m_passMap.IsValidPos(nCell.posX, nCell.posY) && m_passMap.GetAt(nCell.posX,nCell.posY) != -1) {

				// if you need ship to get to some land, you can't build your path FURTHER to sea, to avoid cyclic boardings/landings
				// i.e. ship changing during dist map build is prohibited
				if(cell.ship && m_passMap.GetAt(cell.posX, cell.posY) > 0 && m_passMap.GetAt(nCell.posX,nCell.posY) < 0)
					continue;

				// if you landed from ship, you can't just continue to sea (no Jesus allowed)
				if(cell.ship && m_passMap.GetAt(cell.posX, cell.posY) == PASSMAP_PWATER && m_passMap.GetAt(nCell.posX,nCell.posY) == PASSMAP_SWATER)
					continue;

				// if you didn't come by ship, you can't get from PWATER to land to avoid 'beachwalking'
				if(!cell.ship && m_passMap.GetAt(cell.posX, cell.posY) == PASSMAP_PWATER && m_passMap.GetAt(nCell.posX,nCell.posY) > 0)
					continue;



				// We can't propagate in this direction (no enter)
				if ( !(m_cellCfgMap.GetAt(nCell.posX, nCell.posY) & (1<<inv_dir[nid])) ) {
					continue;
				}

				iMark& dmark = m_distMap.GetAt(nCell.posX,nCell.posY);
				iMark& oldmark = m_distMap.GetAt(cell.posX,cell.posY);
				// this is in order to prevent ships from passing through PWATER (near the shore)
				// check the previous TWO cells

				uint8 dir = oldmark.dir & 0x7;
				iPoint beforeold = iPoint(cell.posX + nOffs[dir][0], cell.posY + nOffs[dir][1]);

				if(m_passMap.GetAt(beforeold.x, beforeold.y) == PASSMAP_SWATER &&
					m_passMap.GetAt(cell.posX,cell.posY) == PASSMAP_PWATER &&
					m_passMap.GetAt(nCell.posX,nCell.posY) == PASSMAP_SWATER)
					continue;

				if(m_passMap.GetAt(beforeold.x, beforeold.y) == PASSMAP_PWATER &&
					m_passMap.GetAt(cell.posX,cell.posY) == PASSMAP_SWATER &&
					m_passMap.GetAt(nCell.posX,nCell.posY) == PASSMAP_PWATER)
					continue;

				if(m_passMap.GetAt(beforeold.x, beforeold.y) == PASSMAP_PWATER &&
					m_passMap.GetAt(cell.posX,cell.posY) == PASSMAP_PWATER &&
					m_passMap.GetAt(nCell.posX,nCell.posY) == PASSMAP_PWATER)
					continue;


				if(m_passMap.GetAt(beforeold.x, beforeold.y) == PASSMAP_SWATER &&
					m_passMap.GetAt(cell.posX,cell.posY) == PASSMAP_PWATER &&
					m_passMap.GetAt(nCell.posX,nCell.posY) == PASSMAP_PWATER)
					continue;

				// Touch object first
				sint32 touchCost = 0;
				uint8 tobj = m_tobjMap.GetAt(nCell.posX, nCell.posY) & 0x7;
				if (tobj > 0) {

					// we can't touch from the ship - skip that way, only exception - touch ship from ship (attack)
					if(m_passMap.GetAt(nCell.posX, nCell.posY) != PASSMAP_SWATER &&
						(m_passMap.GetAt(cell.posX,cell.posY) == PASSMAP_SWATER ||
						m_passMap.GetAt(cell.posX,cell.posY) == PASSMAP_PWATER))
						continue;

					touchCost += 5;
					if (!(nid&0x1)) touchCost += 2;
				}

				// enough points to touch and new cell is not marked by more optimal index
				uint16 nval = nCell.val + 5;
				if (!(nid&0x1)) nval += 2;
				if ( tobj && nval <= totalMoves && dmark.touch > nval) {
					dmark.touch = nval;
					dmark.dir = (nid+4) & 0x7;
					// if water or partial water, to pass further we need ship
					dmark.ship = nCell.ship = dmark.ship = cell.ship || m_passMap.GetAt(nCell.posX,nCell.posY) == PASSMAP_SWATER;

				}

				// Not disapearable touch object (Friendly hero)
				if (tobj == 2) {
					continue;
				}

				// Calculate move cost
				sint32 moveCost = (m_passMap.GetAt(nCell.posX,nCell.posY));

				// to replace -128 and -127 with actual move cost over the water
				if(moveCost == PASSMAP_SWATER || moveCost == PASSMAP_PWATER) moveCost = SURF_MOVE_COST[STYPE_WATER];

				// diagonal direction * 1.5
				if (!(nid&0x1)) moveCost += (moveCost>>1);

				// enough points to move and new cell is not marked by more optimal index
				// upd: OR the old way used ship and new way doesn't!
				// upd: AND no teleport involved!
				nCell.val += moveCost + touchCost;
				if ( dmark.dir != 0x8 && ((nCell.val <= totalMoves && (sint32)dmark.move > nCell.val ) ||
					(!cell.ship && dmark.ship) )) {
						dmark.move = (uint16)nCell.val;
						dmark.dir = ((nid+4) & 0x7);
						// if water or partial water, to pass further we need ship
						sint8 pm =  m_passMap.GetAt(nCell.posX,nCell.posY);
						nCell.ship = dmark.ship = cell.ship || pm == PASSMAP_SWATER;
						//_LOG(iFormat(_T("Updating path to (%d, %d) from (%d, %d), need ship %d\n"), nCell.posX, nCell.posY, cell.posX, cell.posY, dmark.ship).CStr());
						m_openCells.Insert(nCell, -nCell.val);
				}
			}
		}
	}
}

/*
*	Target scanner
*/
iTargetScanner::iTargetScanner(iHero* pActHero, const iDangerMap& dangerMap)
: m_pActHero(pActHero)
, m_pHero(pActHero)
, m_dangerMap(dangerMap)
, m_distMap(iSize(gGame.Map().m_PassMap.GetWidth(),gGame.Map().m_PassMap.GetHeight()))
, m_pOwner(DynamicCast<iAI_Player*>(pActHero->Owner()))
, m_origin(pActHero->Pos())
, m_armyPower(pActHero->Army().ArmyPower())
{
	TSP_INCCOUNT(tspTargetScannerCnst);
	check(m_pOwner);
}

iTargetScanner::iTargetScanner(const iHero* pHero, const iDangerMap& dangerMap, iAI_Player* pOwner, const iPoint& origin, sint32 armyPower)
: m_pActHero(NULL)
, m_pHero(pHero)
, m_dangerMap(dangerMap)
, m_distMap(iSize(gGame.Map().m_PassMap.GetWidth(),gGame.Map().m_PassMap.GetHeight()))
, m_pOwner(pOwner)
, m_origin(origin)
, m_armyPower(armyPower)
{
	TSP_INCCOUNT(tspTargetScannerCnst);
	check(m_pOwner);
}

// bShip as an input parameters means if we have a theoretical access to ship
// bShip as an output parameter means if we need a ship to access selected target
iPoint iTargetScanner::SelectTarget(const iVisItemList& mapItems, const iShPointList& discoverPoints, sint32 moves, uint32 flags, sint32& priority, bool &bShip)
{
	TSP_INCCOUNT(tspSelectTarget);
	TSP_BOUNDTIMER(tspSelectTarget);
	check(m_pActHero);

	priority = 0xFFFF;
	iPoint target = cInvalidPoint;
	uint32 xx;
	sint32 nPriority = 0;

	bool bHasShip = bShip;

	// Prepare dist map
	m_distMap.MakeDistMap(m_origin, m_armyPower, m_pActHero->MoveCostBonus(), m_pActHero->InShip(), m_pOwner->PlayerId(), moves);


	// Process objects
	{
		TSP_BOUNDTIMER(tspProcessObject);
		for (xx=0; xx<mapItems.Size(); ++xx) {
			if (ProcessObject(mapItems[xx].value, nPriority, flags) == Process &&
				nPriority < priority &&
				(bHasShip || !m_distMap.NeedShip(mapItems[xx].value->Pos().x, mapItems[xx].value->Pos().y))) {
					priority = nPriority;
					target = mapItems[xx].value->Pos();
					bShip = m_distMap.NeedShip(mapItems[xx].value->Pos().x, mapItems[xx].value->Pos().y);

			}
		}
	}

	// Process discover points
	{
		TSP_BOUNDTIMER(tspDiscoverPoint);
		for (xx=0; xx<discoverPoints.GetSize(); ++xx) {
			TSP_INCCOUNT(tspDiscoverPoint);
			sint16 px = SHPOINT_X(discoverPoints[xx]), py = SHPOINT_Y(discoverPoints[xx]);
			if (gGame.Map().GetCellObj(iPoint(px, py))) continue;
			if (m_distMap.MoveCost(px, py) == 0x3FFF) continue;
			nPriority = m_distMap.MoveCost(px, py);
			if(!bHasShip && m_distMap.NeedShip(px, py)) continue;
			// this increases the discover area on land, but breaks Royal bounty map logics
			//if(m_distMap.NeedShip(px, py)) nPriority *= 10;
			if(m_distMap.NeedShip(px, py)) nPriority /= 10;

			uint16 pe = gGame.Map().GetPathEl(px, py);
			if (pe != 0xFFFF && gGame.ItemMgr().m_PathElProts[pe]->Type() == PET_ROAD) nPriority /= 2;

			if ( nPriority < priority &&
				//m_distMap.NeedShip(px, py) &&
				(bHasShip || !m_distMap.NeedShip(px, py))) {
					priority = nPriority;

					target.x = px;
					target.y = py;
					bShip = m_distMap.NeedShip(px, py);

			}
		}
	}

	// Process ultimate seeking cell
	iPoint uspt = m_pOwner->UltSeekPoint();
	if (uspt != cInvalidPoint) {
		iBaseMapObject* pCellObj = gGame.Map().GetCellObj(uspt);
		if (!pCellObj || pCellObj == m_pActHero) {
			nPriority = m_distMap.MoveCost(uspt.x, uspt.y);
			check(m_pOwner->TotalPuzzles());
			if ( nPriority != 0x3FFF ) {
				sint32 prc = ((sint32)m_pOwner->OpenedPuzzles()) * 100 / m_pOwner->TotalPuzzles();
				if (prc < 70) nPriority /= 2;
				else if (prc < 80) nPriority /= 3;
				else if (prc < 90) nPriority /= 4;
				else if (prc < 100) nPriority /= 5;
				else if (prc == 100) nPriority /= 15;
				if (nPriority < priority
					&& (bHasShip || !m_distMap.NeedShip(uspt.x, uspt.y))) {
						priority = nPriority;
						target = uspt;
						bShip = m_distMap.NeedShip(uspt.x, uspt.y);
				}
			}
		}
	}
	return target;
}

iTargetScanner::Result iTargetScanner::ProcessObject(iBaseMapObject* pObj, sint32& priority, uint32 flags)
{
	TSP_INCCOUNT(tspProcessObject);
	uint32 estArmyPower =  m_armyPower - (m_armyPower>>2);
	uint32 dVal = m_dangerMap.CellDangerPower(pObj->Pos().x, pObj->Pos().y);
	bool bInDanger = dVal && dVal > m_armyPower;
	sint32 moveCost  = m_distMap.MoveCost((sint16)pObj->Pos().x, (sint16)pObj->Pos().y);
	sint32 touchCost  = m_distMap.TouchCost((sint16)pObj->Pos().x, (sint16)pObj->Pos().y);
	bool bNeedShip = m_distMap.NeedShip((sint16)pObj->Pos().x, (sint16)pObj->Pos().y);

	const iCharT* ships[2] = { _T("no"), _T("yes") };

	if (flags & PRF_IGN_DISTANCE) {
		if (moveCost != 0x3FFF) moveCost = 100;
		if (touchCost != 0x3FFF) touchCost = 100;
	}

	if ( (flags & PRF_IGN_CURLOC) && m_origin == pObj->Pos() ) {
		_LOG(iFormat(_T("\t\t- Ignoring current location at (%d,%d)\n"),pObj->Pos().x, pObj->Pos().y).CStr());
		return Reject;
	}

	if (iOwnCnst* pOwnCnst = DynamicCast<iOwnCnst*>(pObj)) {
		// OwnCnst
		_LOG(iFormat(_T("\t\tOwnCnst %s at (%d,%d), ship: %s: "),gTextMgr[pOwnCnst->Proto()->NameKey()], pOwnCnst->Pos().x, pOwnCnst->Pos().y, ships[bNeedShip]).CStr());
		if (moveCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		} else if (pOwnCnst->Owner() == m_pOwner->PlayerId()) {
			// friendly
			_LOG(_T("Declined (already captured)\n"));
			return Reject;
		} else {
			// enemy
			if (pOwnCnst->Guard().ArmyPower() > estArmyPower) {
				// guard is too strong
				_LOG(_T("Declined (guard is too strong)\n"));
				return Reject;
			}
			if (bInDanger) {
				// cell in danger
				_LOG(_T("Declined (cell in danger)\n"));
				return Reject;
			}
		}

		if (iOwnCnstT_FurtSkill* pFSKProto = DynamicCast<iOwnCnstT_FurtSkill*>(pOwnCnst->Proto())) {
			// FurtSkill ownerable
			sint32 val;
			if (pFSKProto->FurtSkills().Value(FSK_MIN_GOLD) > 0) {
				// Gold Mine
				val = 5 + m_pOwner->MineralValue(MINERAL_GOLD);
			} else if (pFSKProto->FurtSkills().Value(FSK_MIN_ORE) > 0) {
				// Ore mine
				val = 5 + m_pOwner->MineralValue(MINERAL_ORE);
			} else if (pFSKProto->FurtSkills().Value(FSK_MIN_WOOD) > 0) {
				// Sawmill
				val = 5 + m_pOwner->MineralValue(MINERAL_WOOD);
			} else if (pFSKProto->FurtSkills().Value(FSK_MIN_MERCURY) > 0) {
				// Alchemist's lab
				val = 5 + m_pOwner->MineralValue(MINERAL_MERCURY);
			} else if (pFSKProto->FurtSkills().Value(FSK_MIN_GEMS) > 0) {
				// Gems mine
				val = 5 + m_pOwner->MineralValue(MINERAL_GEMS);
			} else if (pFSKProto->FurtSkills().Value(FSK_MIN_CRYSTAL) > 0) {
				// Crystal mine
				val = 5 + m_pOwner->MineralValue(MINERAL_CRYSTAL);
			} else if (pFSKProto->FurtSkills().Value(FSK_MIN_SULFUR) > 0) {
				// Sulfur mine
				val = 5 + m_pOwner->MineralValue(MINERAL_SULFUR);
			} else {
				// Other furt skills ownerables
				val = 5;
			}
			check(val);
			priority = moveCost/val;
		} else {
			// Other ownerable
			check(0);
		}

		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), moveCost, priority).CStr());
		return Process;
	} else if (iCastle* pCastle = DynamicCast<iCastle*>(pObj)) {
		// Castle
		_LOG(iFormat(_T("\t\tCastle %s at (%d,%d), ship: %s: "),gTextMgr[pCastle->Proto()->NameKey()], pCastle->Pos().x, pCastle->Pos().y, ships[bNeedShip]).CStr());

		if (moveCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		} else if (pCastle->Owner() == m_pOwner->PlayerId()) {
			// friendly
			if (pCastle->Visitor()) {
				_LOG(_T("Declined (has visitor)\n"));
				return Reject;
			}

			const iHero* pDangerActor = m_dangerMap.CellDangerActor((sint16)pCastle->Pos().x, (sint16)pCastle->Pos().y);
			//dmp.MakeDistMap(pCastle->Pos(), pCastle->Garrison().ArmyPower(), 0, false, m_pHero->Owner()->PlayerId(), 50);

			// Need protection ?
			bool bNeedProtection = (dVal && dVal > pCastle->Garrison().ArmyPower());

			uint16 fightcost = 1000;
			if(pDangerActor)
				fightcost = m_distMap.TouchCost(pDangerActor->Pos().x, pDangerActor->Pos().y);

			if (bNeedProtection && moveCost <= m_pHero->Moves() && fightcost <= moveCost) {
				priority = 1;
				_LOG(iFormat(_T("Accepted: (to protect from %s) (Distance = %d, Index = %d)\n"), pDangerActor->Name().CStr(), moveCost, priority).CStr());
				return Process;
			}

			uint32 value = 0;
			// Can learn new spells ?
			iSpellList spells;
			iMGuildMgr(pCastle).GetSpells(spells);
			bool bFound = false;
			for (uint32 spid=0; spid<spells.GetSize(); ++spid) {
				if (m_pHero->CanLearnSpell(spells[spid]) && !m_pHero->HasSpell(spells[spid])) {
					bFound = true;
					break;
				}
			}
			if (bFound) {
				_LOG(iFormat(_T("Accepted: (to learn new spells)")).CStr());
				value += 3;
			}

			// Need army ???
			sint32 canRecr = iDwellMgr(pCastle).MaxRecruit(m_pOwner->Minerals());
			sint32 recrVal = (canRecr) / iMAX(m_armyPower, 1);
			if (recrVal) {
				value += recrVal * 5;
				_LOG(iFormat(_T("Accepted: (to recruit army: (Current poser: %d, Can recruit: %d, Index = %d)"), m_armyPower, canRecr, recrVal).CStr());
			} else if (canRecr > (m_armyPower/2)) {
				value += 1;
				_LOG(iFormat(_T("Accepted: (to recruit army: (Current poser: %d, Can recruit: %d, Index = %d)"), m_armyPower, canRecr, recrVal).CStr());
			}


			if (value) {
				priority = moveCost / value;
				_LOG(iFormat(_T(" (Distance = %d, Index = %d)\n"), moveCost, priority).CStr());
				return Process;
			} else {
				_LOG(_T("Declined (nothing to do)\n"));
				return Reject;
			}
		} else {
			// enemy
			uint32 gPower = pCastle->GarrisonPower();
			if (gPower > estArmyPower) {
				// guard is too strong
				_LOG(_T("Declined (guard is too strong)\n"));
				return Reject;
			}

			if (pCastle->Owner() == PID_NEUTRAL) {
				priority = moveCost / 8;
				_LOG(_T("(Neutral) "));
			} else {
				priority = moveCost / 12;
				_LOG(_T("(Enemy) "));
			}
			return Process;
		}

		_LOG(_T("Declined (nothing to do)\n"));
		return Reject;
	} else if (iVisCnst* pVisCnst = DynamicCast<iVisCnst*>(pObj)) {
		// VisCnst
		_LOG(iFormat(_T("\t\tVisCnst %s at (%d,%d), ship: %s: "),gTextMgr[pVisCnst->Proto()->NameKey()], pVisCnst->Pos().x, pVisCnst->Pos().y, ships[bNeedShip]).CStr());

		if (moveCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pVisCnst->Visitor() && pVisCnst->Visitor() != m_pHero) {
			_LOG(_T("Declined (has visitor)\n"));
			return Reject;
		}
		if (pVisCnst->Visited(m_pHero)) {
			_LOG(_T("Declined (already visited)\n"));
			return Reject;
		}
		if (pVisCnst->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (guard is too strong)\n"));
			return Reject;
		}

		if (pVisCnst->Proto()->Type() == VCNST_DWELLING) {
			// Dwelling
			iVisCnst_Dwelling* pDwelling = (iVisCnst_Dwelling*)pVisCnst;
			iCreatGroup cg(pDwelling->CreatType(),pDwelling->CreatCount());
			if (!m_pHero->Army().CanAddGroup(cg.Type())) {
				_LOG(_T("Declined (not enough space in army to recruit creatures)\n"));
				return Reject;
			} else if (pDwelling->JoinForFree()) {
				// Free dwelling
				uint32 pval = m_armyPower / cg.GroupPower();
				if (!pval) {
					// dwellings creatures is very powerfull, so its good idea to join
					priority = moveCost / 4;
				} else if (pval < 3) {
					priority = moveCost / 3;
				} else if (pval < 5 || (pval < 10 && moveCost <= m_pHero->Moves())){
					priority = moveCost / 2;
				} else {
					_LOG(_T("Declined (dweling creatures is to weak or its too far)\n"));
					return Reject;
				}
			} else {
				// Not free dwelling
				iMineralSet ms;
				m_pOwner->PayMaster().RequestMinerals(CREAT_DESC[cg.Type()].cost * cg.Count(), 2, ms);
				cg.Count() = iMIN<sint32>(cg.Count(), ms.Has(CREAT_DESC[cg.Type()].cost));
				if (!cg.Count()) {
					_LOG(_T("Declined (not enough resources to recruit)\n"));
					return Reject;
				}
				uint32 pval = m_armyPower / cg.GroupPower();
				if (!pval) {
					// dwellings creatures is very powerful, so its good idea to recruit
					priority = moveCost / 4;
				} else if (pval < 3) {
					priority = moveCost / 3;
				} else if (pval < 5 || (pval < 10 && moveCost <= m_pHero->Moves())){
					priority = moveCost / 2;
				} else {
					_LOG(_T("Declined (dweling creatures is to weak or its too far)\n"));
					return Reject;
				}
			}
		}
		else if (pVisCnst->Proto()->Type() == VCNST_WARACADEMY)
		{
			uint32 i;
			// War academy (upgrade for creatures)
			iVisCnst_WarAcademy* pDwelling = (iVisCnst_WarAcademy*)pVisCnst;
			iMineralSet ms;
			uint32 count;
			CREATURE_TYPE type, new_type;

			// check all creatures in army
			for (i = 0; i < 7; i++)
			{
				const iCreatGroup &cg = m_pHero->Army().At(i);

				count = cg.Count();
				if (count == 0) continue;
				type = cg.Type();
				new_type = cg.GetUpgradedType(iCreatGroup::UW_FIRST);
				if (new_type == CREAT_UNKNOWN) continue;

				m_pOwner->PayMaster().RequestMinerals(CREAT_DESC[type].upgradecost * count, 2, ms);
				if (ms.Has(CREAT_DESC[type].upgradecost) == count)
				{
					// enough money to upgrade full this group
					break;
				}
			}

			if (i == 7)
			{
				_LOG(_T("Declined (no creatures in the army or no resources)\n"));
				return Reject;
			}

			uint32 new_power = count * CREAT_DESC[new_type].pidx;

			// check range
			uint32 pval = m_armyPower / new_power;
			if (!pval) {
				// dwellings creatures is very powerfull, so its good idea to recruit
				priority = moveCost / 4;
			} else if (pval < 3) {
				priority = moveCost / 3;
			} else if (pval < 5 || (pval < 10 && moveCost <= m_pHero->Moves())){
				priority = moveCost / 2;
			} else {
				_LOG(_T("Declined (dweling creatures is to weak or its too far)\n"));
				return Reject;
			}

		}
		else if (pVisCnst->Proto()->Type() == VCNST_HARBOR) {
			if(!m_pOwner->Admiral().NeedProceedToHarbor(dynamic_cast<IHarbor*>(pVisCnst))) {
				_LOG(_T("Declined (no order from Admiral)\n"));
				return Reject;
			} else {
				_LOG(_T("Accepted (Admiral ordered to proceed to harbor)\n"));
				return Process;
			}
		}
		else if (pVisCnst->Proto()->Type() == VCNST_SHRINE) {
			// Shrine
			iVisCnst_Shrine* pShrine = (iVisCnst_Shrine*)pVisCnst;
			if (m_pHero->HasSpell(pShrine->Spell())) {
				_LOG(_T("Declined (already have this spell)\n"));
				return Reject;
			} else if (!m_pHero->CanLearnSpell(pShrine->Spell())) {
				_LOG(_T("Declined (unable to learn this spell)\n"));
				return Reject;
			}
			priority = moveCost / 2;
		} else if (pVisCnst->Proto()->Type() == VCNST_MANASOURCE) {
			// Mana source
			iVisCnst_ManaSource* pManaSource = (iVisCnst_ManaSource*)pVisCnst;
			sint32 mana = m_pHero->MaxManaPts() * pManaSource->ManaPts();
			if (m_pHero->ManaPts() >= mana || moveCost > m_pHero->Moves()) {
				_LOG(_T("Declined (Mana source is too far)\n"));
				return Reject;
			}
			priority = moveCost / (pManaSource->ManaPts() * 2);
		} else if (pVisCnst->Proto()->Type() == VCNST_STABLE) {
			// Stable
			if (moveCost > m_pHero->Moves()) {
				_LOG(_T("Declined (Stables is too far)\n"));
				return Reject;
			}
			priority = moveCost / 4;
		} else if (pVisCnst->Proto()->Type() == VCNST_TREASURY) {
			// Treasury
			if (moveCost > (m_pHero->TotalMoves() + m_pHero->Moves())) {
				_LOG(_T("Declined (Treasury is too far)\n"));
				return Reject;
			}
			priority = moveCost / 3;
		} else if (pVisCnst->Proto()->Type() == VCNST_KEYMASTER) {
			// Keymaster tent
			priority = moveCost / 5;
		} else if (pVisCnst->Proto()->Type() == VCNST_TELEPORT) {
			// Teleport
			iVisCnst_Teleport* pTeleport = (iVisCnst_Teleport*)pVisCnst;
			sint32 tidx = pTeleport->GetTarget();

			if (tidx != -1) {
				const iFogMap& fm = m_pOwner->FogMap();
				iVisCnst_Teleport* pTarget = gGame.Map().GetTeleport(tidx);
				check(pTarget);
				// if teleport with hidden target, then process it as cell to explore
				if (fm.IsInvisible(pTarget->Pos())) {
					priority = moveCost / 5;
				} else {
					_LOG(_T("Declined (We already process teleport as space wrapper)\n"));
					return Reject;
				}
			} else {
				_LOG(_T("Declined (We already process teleport as space wrapper)\n"));
				return Reject;
			}
		}  else if (pVisCnst->Proto()->Type() == VCNST_NEWTELEPORT) {
			// New Teleport
			iVisCnst_NewTeleport* pTeleport = (iVisCnst_NewTeleport*)pVisCnst;
			sint32 tidx = pTeleport->OutIndex();

			if (tidx != 0) {
				const iFogMap& fm = m_pOwner->FogMap();
				iSimpleArray<iBaseMapObject*> list = gGame.Map().FindTeleportDestinations(DynamicCast<iVisCnst_NewTeleport*>(pVisCnst), tidx);
				bool ok = false;

				for(uint32 zz=0; zz<list.GetSize(); zz++) {
					iBaseMapObject *pTarget = list[zz];
					// if teleport with hidden target, then process it as cell to explore
					if (fm.IsInvisible(pTarget->Pos())) {
						priority = moveCost / 5;
						ok = true;
					}
				}
				if(!ok) {
					_LOG(_T("Declined (We already process new teleport as space wrapper)\n"));
					return Reject;
				}
			} else {
				_LOG(_T("Declined (New Teleport leads to nowhere)\n"));
				return Reject;
			}
		} else {
			priority = moveCost / 2;
		}

		/*if (priority < 0) {
		sint32 a = 10;
		*/

	//	_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), moveCost, priority).CStr());

		// if this is our current position, return Reject, otherwise we will get stuck in Wooden sign without any sense
		if(m_pHero->Pos() == pObj->Pos())
			return Reject;
		else
			return Process;
	}
	else if (iMapGuard* pGuard = DynamicCast<iMapGuard*>(pObj)) {
		// MapGuard
		_LOG(iFormat(_T("\t\tMapGuard %d of %s at (%d,%d), ship: %s: "),pGuard->Creatures().Count(), gTextMgr[TRID_CREATURE_PEASANT_F3 + pGuard->Creatures().Type()*3], pGuard->Pos().x, pGuard->Pos().y, ships[bNeedShip]).CStr());

		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}

		if (bInDanger) {
			// cell in danger
			_LOG(_T("Declined (cell in danger)\n"));
			return Reject;
		}
		if (pGuard->Creatures().GroupPower() > estArmyPower) {
			// guard is too strong
			_LOG(_T("Declined (guard is too strong)\n"));
			return Reject;
		}

		priority = touchCost / 2;

		uint16 pe = gGame.Map().GetPathEl(pObj->Pos().x, pObj->Pos().y);
		if (pe != 0xFFFF && gGame.ItemMgr().m_PathElProts[pe]->Type() == PET_ROAD) priority /= 2;

		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), moveCost, priority).CStr());
		return Process;
	} else if (iHero* pHero = DynamicCast<iHero*>(pObj)) {
		// Hero
		_LOG(iFormat(_T("\t\tHero %s at (%d,%d), ship: %s: "),pHero->Name().CStr(), pHero->Pos().x, pHero->Pos().y, ships[bNeedShip]).CStr());
		if (pHero->Proto()->m_protoId == m_pHero->Proto()->m_protoId) {
			_LOG(_T("it's me :)\n"));
			return Reject;
		}

		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}

		if (m_pHero->Owner() && pHero->Owner()->PlayerId() == m_pHero->Owner()->PlayerId()) {
			// Friendly hero
			_LOG(_T("(Friendly hero) \n"));
			sint32 max_range = m_pHero->TotalMoves() * 3;
			if (!m_pOwner->NeedMeeting(m_pHero, pHero)){
				_LOG(_T("Declined (we're not intrested to meet this hero)\n"));
				return Reject;
			} else if (touchCost > max_range) {
				_LOG(_T("Declined (hero is too far)\n"));
				return Reject;
			}

			if (touchCost <= m_pHero->Moves()) {
				priority = touchCost / 6;
			} else {
				priority = touchCost / 2;
			}

		} else {
			// Enemy hero
			_LOG(_T("(Enemy hero) \n"));
			if (pHero->Army().ArmyPower() > estArmyPower) {
				_LOG(_T("Declined (army is too strong)\n"));
				return Reject;
			}

			if (m_dangerMap.IsDangerActor(pHero) && touchCost <= m_pHero->Moves()) {
				_LOG(iFormat(_T("Accepted: Danger actor! (Distance = %d, Index = 0)\n"), touchCost).CStr());
				priority = 0;
				return Process;
			}

			priority = touchCost / 10;
			if (DynamicCast<iCastle*>(pHero->GetLocation())) priority /= 2;
		}

		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	}
	else if (iMapItem_Mineral* pMineral = DynamicCast<iMapItem_Mineral*>(pObj)) {
		// Mineral
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_GOLD+pMineral->MineralType()], pMineral->Pos().x, pMineral->Pos().y, ships[bNeedShip]).CStr());
		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pMineral->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (army is too strong)\n"));
			return Reject;
		}

		priority = touchCost / (3 + m_pOwner->MineralValue(pMineral->MineralType()));
		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	} else if (iMapItem_ManaCrystal* pManaCrystal = DynamicCast<iMapItem_ManaCrystal*>(pObj)) {
		// Mana crystal
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_MANACRYSTAL], pManaCrystal->Pos().x, pManaCrystal->Pos().y, ships[bNeedShip]).CStr());
		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pManaCrystal->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (army is too strong)\n"));
			return Reject;
		}
		priority = touchCost / 2;
		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	} else if (iMapItem_CampFire* pCampFire = DynamicCast<iMapItem_CampFire*>(pObj)) {
		// Camp fire
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_CAMPFIRE], pCampFire->Pos().x, pCampFire->Pos().y, ships[bNeedShip]).CStr());
		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pCampFire->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (army is too strong)\n"));
			return Reject;
		}
		priority = touchCost / 3;
		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	} else if (iMapItem_Chest* pChest = DynamicCast<iMapItem_Chest*>(pObj)) {
		// Chest
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_CHEST], pChest->Pos().x, pChest->Pos().y, ships[bNeedShip]).CStr());
		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pChest->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (army is too strong)\n"));
			return Reject;
		}
		priority = touchCost / 4;
		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	} else if (iMapItem_Lamp* pLamp = DynamicCast<iMapItem_Lamp*>(pObj)) {
		// Lamp
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_LAMP], pLamp->Pos().x, pLamp->Pos().y, ships[bNeedShip]).CStr());
		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pLamp->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (army is too strong)\n"));
			return Reject;
		}

		if (!m_pHero->Army().CanAddGroup(CREAT_GENIE)) {
			_LOG(_T("Declined (not enough space in army to recruit genies)\n"));
			return Reject;
		}

		iCreatGroup cg(CREAT_GENIE, pLamp->Count());
		iMineralSet ms;
		m_pOwner->PayMaster().RequestMinerals(CREAT_DESC[cg.Type()].cost * cg.Count(), 2, ms);
		cg.Count() = iMIN<sint32>(cg.Count(), ms.Has(CREAT_DESC[cg.Type()].cost));
		if (!cg.Count()) {
			_LOG(_T("Declined (not enough resources to recruit)\n"));
			return Reject;
		}		uint32 pval = m_armyPower / cg.GroupPower();
		if (!pval) {
			priority = moveCost / 4;
		} else if (pval < 3) {
			priority = moveCost / 3;
		} else if (pval < 5 || (pval < 10 && moveCost <= m_pHero->Moves())){
			priority = moveCost / 2;
		} else {
			_LOG(_T("Declined (dweling creatures is to weak or its too far)\n"));
			return Reject;
		}

		priority = touchCost / 4;
		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	} else if (iMapItem_Artifact* pArtifact = DynamicCast<iMapItem_Artifact*>(pObj)) {
		// Artifact
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_ARTIFACT], pArtifact->Pos().x, pArtifact->Pos().y, ships[bNeedShip]).CStr());
		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pArtifact->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (army is too strong)\n"));
			return Reject;
		}
		priority = touchCost / (3 * (gGame.ItemMgr().m_artMgr[pArtifact->ArtifactId()].Level()+1));
		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	} else if (iMapItem_SpellScroll* pSpellScroll = DynamicCast<iMapItem_SpellScroll*>(pObj)) {
		// Spell Scroll
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_SPELLSCROLL], pSpellScroll->Pos().x, pSpellScroll->Pos().y, ships[bNeedShip]).CStr());
		if (touchCost == 0x3FFF){
			_LOG(_T("Declined (not accessible)\n"));
			return Reject;
		}
		if (pSpellScroll->Guard().ArmyPower() > estArmyPower) {
			_LOG(_T("Declined (army is too strong)\n"));
			return Reject;
		}
		priority = touchCost / 3;
		_LOG(iFormat(_T("Accepted: (Distance = %d, Index = %d)\n"), touchCost, priority).CStr());
		return Process;
	} else if (DynamicCast<iMapItem_KeyGuard*>(pObj)) {
		// KeyGuard
		_LOG(iFormat(_T("\t\t%s at (%d,%d), ship: %s: "),gTextMgr[TRID_MAPRES_KEYGUARD], pObj->Pos().x, pObj->Pos().y, ships[bNeedShip]).CStr());
		_LOG(_T("There is no reason to parse KeyGuard.\n"));
		return Reject;
	}
	else if (iShip* pShip = DynamicCast<iShip*>(pObj))
	{
		if(!pShip->IsLoaded()) {
			_LOG(iFormat(_T("\t\t%s at (%d,%d): "),gTextMgr[TRID_OBJ_SHIP], pObj->Pos().x, pObj->Pos().y).CStr());
			_LOG(_T("There is no order from Admiral to board this ship!\n"));
		} else {
			return ProcessObject(pShip->Hero(), priority, flags); // repeat with the passenger
		}
		return Reject;
	}

	check(0);
	return Reject;
}

iPoint iTargetScanner::MakePath(const iPoint& target, iPath& path)
{
	TSP_INCCOUNT(tspMakePath);
	TSP_BOUNDTIMER(tspMakePath);
	check(path.IsEmpty());
	iPath tpath(path.m_Start);

	// Make path
	sint16 px = (sint16)target.x;
	sint16 py = (sint16)target.y;
	if (m_distMap.IsTouchPoint(px, py)) {
		sint32 cost = m_distMap.TouchCost(px, py);
		if(cost == 0x3fff)
			return cInvalidPoint;
		iPoint npos(px, py);
		uint8 dir = m_distMap.Dir(px,py);
		bool bInShip = gGame.Map().m_PassMap.GetAt(px, py) == PASSMAP_SWATER;
		px += nOffs[dir][0];
		py += nOffs[dir][1];
		// the point previous to touch point can't be on partial water, or we'll get stuck
		if(m_pOwner->PassMap().GetAt(px, py) == PASSMAP_PWATER)
			return cInvalidPoint;
		sint32 ncost = cost - m_distMap.MoveCost(px,py);
		tpath.AddMoveToPoint(npos, ncost, m_pOwner->PassMap().GetAt(px, py) == PASSMAP_SWATER && !bInShip);
	}
	bool bStartFromTeleport = false;
	uint32 iter = 0;

	while (px != path.m_Start.x || py != path.m_Start.y) {
		if(iter ++ > 10000)
			return cInvalidPoint;
		//! point might be inaccessible
		sint32 cost = m_distMap.MoveCost(px,py);
		iPoint npos(px, py);
		uint8 dir = m_distMap.Dir(px,py);
		bool bInShip = gGame.Map().m_PassMap.GetAt(px, py) == PASSMAP_SWATER;

		if (dir & 0x8) {
			// teleport
			//! teleport == NULL
			if(iVisCnst_Teleport* pTeleport = gGame.Map().FindTeleport(px, py)) {
               check(pTeleport && pTeleport->GetOrigin() != -1);
               iVisCnst_Teleport* pOrigin = gGame.Map().GetTeleport(pTeleport->GetOrigin());
               check(pOrigin);
               px = pOrigin->Pos().x;
               py = pOrigin->Pos().y;
            } else if(iVisCnst_NewTeleport* pTeleport = gGame.Map().FindNewTeleport(px, py)) {

				iSimpleArray<iBaseMapObject*> list = gGame.Map().FindTeleportOrigins(pTeleport, pTeleport->InIndex());
				for(uint32 xx=0; xx<list.GetSize(); xx++)
					if(m_distMap.MoveCost(list[xx]->Pos().x, list[xx]->Pos().y) != 0x3FFF) {
						px = list[xx]->Pos().x;
						py = list[xx]->Pos().y;
					}
			} else { // this is map marker, so there is no path
				return cInvalidPoint;
			}
			bStartFromTeleport = true;
		} else

		{
			px += nOffs[dir][0];
			py += nOffs[dir][1];
			bStartFromTeleport = false;
		}

		sint32 ncost = cost - m_distMap.MoveCost(px,py);
		tpath.AddMoveToPoint(npos, ncost, gGame.Map().m_PassMap.GetAt(px, py) == PASSMAP_SWATER && !bInShip);
	}

	// If first step is teleport, then add origin point
	if (bStartFromTeleport) {
		return path.m_Start;
	}

	// check for boarding and landing
	tpath.ProcessShipOperations(m_pHero->Moves());

	// Walk through the path and search for obstacles
	iPoint opos = cInvalidPoint;
	for (uint32 xx=0; xx<tpath.m_Path.GetSize(); ++xx) {
		iPoint pos = tpath.m_Path[xx].m_Pos;
		// if next step is teleport, then stop
		if ( m_distMap.Dir(pos.x, pos.y) & 0x8) {
			check(opos != cInvalidPoint);
			if (opos == cInvalidPoint) return pos; else return opos;
		} else if ( m_distMap.Dir(pos.x, pos.y) & 0x10) {
			check(opos != cInvalidPoint);
			if (opos == cInvalidPoint) return pos; else return opos;
		}

		// add next step
		path.m_Path.Add(tpath.m_Path[xx]);
		// if current step is object to touch, then stop
		if ( m_distMap.IsTouchPoint(pos.x, pos.y)){
			return pos;
		}
		opos = pos;
	}

	return target;
}

bool iTargetScanner::DefineTask(const iVisItemList& mapItems)
{
	TSP_INCCOUNT(tspDefineTask);
	TSP_BOUNDTIMER(tspDefineTask);
	check(m_pActHero);
	_LOG(iFormat(_T(">>> %s (%d,%d):\n"),m_pActHero->Name().CStr(), m_pActHero->Pos().x, m_pActHero->Pos().y).CStr());

	// Process current location
	if (iMapCnst* pLocation = m_pActHero->GetLocation()) {
		_LOG(_T("\tCheck current location:\n"));
		if (iCastle* pCastle = DynamicCast<iCastle*>(pLocation)) {
			_LOG(iFormat(_T("\t\tCastle %s: "), pCastle->Name().CStr()).CStr());
			if (uint32 dval = m_dangerMap.CellDangerPower((sint16)pCastle->Pos().x, (sint16)pCastle->Pos().y)) {
				_LOG(_T("(In danger) : "));
				// Toss armies
				if (!pCastle->Garrison().Empty()) {
					TossArmies(m_pActHero, pCastle, ATF_TYPE_SPEED|ATF_TYPE_POWER, 0);
				}
				// Recruit creatures (MAX for now)
				iDwellMgr dwMgr(pCastle);
				iMineralSet cost; cost.Reset();
				dwMgr.Recruit(pCastle, 1000000, cost);
				// Toss armies again
				if (!pCastle->Garrison().Empty()) {
					TossArmies(m_pActHero, pCastle, ATF_TYPE_SPEED|ATF_TYPE_POWER, 0);
				}

				// Compare power
				uint32 ap = m_pActHero->Army().ArmyPower();
				if (ap >= (dval+(dval>>1))) {
					// our army is stronger
					const iHero* pDangerActor = m_dangerMap.CellDangerActor((sint16)pCastle->Pos().x, (sint16)pCastle->Pos().y);
					check(pDangerActor);
					m_distMap.MakeDistMap(m_pActHero->Pos(), m_pActHero->Army().ArmyPower(), m_pActHero->MoveCostBonus(), m_pActHero->InShip(), m_pActHero->Owner()->PlayerId(), m_pActHero->Moves());

					if (m_distMap.TouchCost((sint16)pDangerActor->Pos().x, (sint16)pDangerActor->Pos().y) <= m_pActHero->Moves()) {
						// we shouldn't attack danger hero if we need ship, it causes 'water movement' bug; Hedin, 18.11.2010
						if(!m_distMap.NeedShip((sint16)pDangerActor->Pos().x, (sint16)pDangerActor->Pos().y))
						{
							// we can attack danger actor, so, do it
							_LOG(_T("Try to attack danger hero\n"));
							iPath path(m_pActHero->Pos());
							iPoint dst = MakePath(pDangerActor->Pos(), path);
							if(dst == cInvalidPoint) { // oops
								return false;
							}
							path.SetTargetAction((iPath::Action)m_pActHero->GetTarget(dst.x, dst.y).action);
							m_pActHero->SetPath(path);
							return m_pActHero->Step();
						} else {
							_LOG(_T("Ship required to attack danger hero, so, lets wait\n"));
							return false;
						}
					} else {
						_LOG(_T("Danger hero is too far, so, we have more important tasks\n"));
					}
				} else if (ap >= dval) {
					// our army is similar to enemy, so, lets wait
					_LOG(_T("Danger hero has army similar to ours, so, lets wait\n"));
					return false;
				} else {
					// our army is too weak, wait
					_LOG(_T("Our army is too weak to protect castle, so, do nothing!!!\n"));
					return false;
				}
			} else {
				_LOG(_T("(Normal)\n"));
				// Toss armies
				if (!pCastle->Garrison().Empty()) {
					_LOG(_T("Tossing armies with castle:\n"));
#ifdef DEBUG_AISCANNER
					DumpArmy(m_pActHero->Army(), _T("Hero army before toss"));
					DumpArmy(pCastle->Garrison(), _T("Castle garrison before toss"));
#endif //DEBUG_AISCANNER
					TossArmies(m_pActHero, pCastle, ATF_TYPE_SPEED|ATF_TYPE_POWER, 0);

#ifdef DEBUG_AISCANNER
					DumpArmy(m_pActHero->Army(), _T("Hero army after toss"));
					DumpArmy(pCastle->Garrison(), _T("Castle garrison after toss"));
#endif //DEBUG_AISCANNER
				}
				// Recruit creatures (MAX for now)
				_LOG(_T("Recruiting creatures:\n"));
				iDwellMgr dwMgr(pCastle);
				iMineralSet cost; cost.Reset();
				dwMgr.Recruit(pCastle, 1000000, cost);
#ifdef DEBUG_AISCANNER
				DumpArmy(pCastle->Garrison(), _T("Recruited army"));
#endif //DEBUG_AISCANNER
				_LOG(_T("Tossing armies with castle:\n"));
#ifdef DEBUG_AISCANNER
				DumpArmy(m_pActHero->Army(), _T("Hero army before toss"));
				DumpArmy(pCastle->Garrison(), _T("Castle garrison before toss"));
#endif //DEBUG_AISCANNER
				// Toss armies again
				if (!pCastle->Garrison().Empty()) {
					TossArmies(m_pActHero, pCastle, ATF_TYPE_SPEED|ATF_TYPE_POWER, 0);
				}
#ifdef DEBUG_AISCANNER
				DumpArmy(m_pActHero->Army(), _T("Hero army after toss"));
				DumpArmy(pCastle->Garrison(), _T("Castle garrison after toss"));
#endif //DEBUG_AISCANNER

			}
		}
	}

	sint32 priority;

	// Prepare Discover point list
	// IN: iPassMap, iFogMpap, iPoint(heropos)
	// OUT: iShPointList - discoverPoints
	_LOG(_T("\tPrepare list of points for discover: "));
	iShPointList discoverPoints;
	m_pActHero->Owner()->UpdatePassMap();
	iPlayer::iPassMap& pm = m_pActHero->Owner()->PassMap();
	const iFogMap& fm = m_pActHero->Owner()->FogMap();
#if 1
	for (sint32 yy=0; yy<pm.GetHeight(); ++yy ) {
		for (sint32 xx=0; xx<pm.GetWidth(); ++xx ) {
			if (pm.GetAt(xx,yy) != -1 && fm.IsVisBound(xx,yy) && pm.GetAt(xx,yy) != PASSMAP_PWATER)
				discoverPoints.Add(MAKE_SHPOINT(xx,yy));
		}
	}
#else
	DiscoverMap( pm, fm,
#endif
		_LOG(iFormat(_T("Found %d point(s)\n"),discoverPoints.GetSize()).CStr());

	//
	bool bShip = m_pOwner->Admiral().CanGetShip(m_pActHero); // the input parameter to SelectTarget means if we have access to a ship
	_LOG(iFormat(_T("\tParse target (%s ship available):\n"), bShip ? _T("with") : _T("without")).CStr());
	_LOG(_T("\t\t:::Scan objects (1 turn):::\n"));

    iPoint target = SelectTarget(mapItems, discoverPoints, m_pActHero->Moves() + m_pActHero->TotalMoves(), PRF_NORMAL, priority, bShip);

	if (target == cInvalidPoint) {
		_LOG(_T("\t\t:::Scan objects (6 turns):::\n"));
		target = SelectTarget(mapItems, discoverPoints, m_pActHero->Moves() + m_pActHero->TotalMoves()*6, PRF_NORMAL,priority, bShip);
		if (target == cInvalidPoint) {
			_LOG(_T("\t\t:::Scan objects (whole map):::\n"));
			target = SelectTarget(mapItems, discoverPoints, 10000, PRF_NORMAL,priority, bShip);
		}
	}

	bool bNeedShip = bShip; // need ship to get to target

	if (target != cInvalidPoint) {
		// we need ship and don't have one
		if(!m_pActHero->InShip() && bNeedShip && !m_pOwner->Admiral().IsShipAvailable(m_pActHero)) {
			_LOG(iFormat(_T("\t\tReport to admiral: ship required to get to (%d, %d)\n"), target.x, target.y).CStr());
			m_pOwner->Admiral().DoHeroRequiresShip(m_pActHero);
			return m_pOwner->Admiral().Process(); // true means continue hero iteration this turns, makes sense only if Admiral made some designations
		}
		const iShip* pShip;
		// target requires ship to get to: acquire one from admiral
		if((pShip = m_pOwner->Admiral().IsShipAvailable(m_pActHero)) && !pShip->IsLoaded() && !m_pActHero->InShip() && bNeedShip) {
			target = pShip->Pos();
			_LOG(iFormat(_T("\tProceeding to board ship at (%d,%d)\n"), pShip->Pos().x, pShip->Pos().y).CStr());
			iPath path(m_pActHero->Pos());
			m_pActHero->MakePath(path, pShip->Pos().x, pShip->Pos().y);
			m_pActHero->SetPath(path);
			return m_pActHero->Step();
		}

		// we are finishing our sail, report ship no more needed
		if(m_pActHero->InShip() && m_pOwner->PassMap().GetAt(target.x, target.y) > 0) {
			m_pOwner->Admiral().RetireToCoast(m_pActHero);
		}

		// we found suitable object
		if (target != m_pActHero->Pos()) {
			iPath path(m_pActHero->Pos());
			iPoint dst = MakePath(target, path);
			if(dst == cInvalidPoint) { // oops
				return false;
			}

			/*iPoint dst = target;
			m_pActHero->MakePath(path, target.x, target.y);
			if(path.IsEmpty())
			return false; // oops
			*/
			if (iBaseMapObject* pMapObj =gGame.Map().m_CoverMap.GetAt(target.x,target.y)) {
				_LOG(iFormat(_T("\tSelect object at (%d,%d), cost: %d\n"), target.x, target.y, path.PathCost()).CStr());
				if (path.IsEmpty()) {
					check(m_pActHero->GetLocation());

					// Activate current location
					_LOG(iFormat(_T("\tActivate current location at (%d,%d)\n"), target.x, target.y).CStr());
					m_pActHero->SetPath();
					return true;
					//path.m_Path.Add(iPath::iStep(m_pActHero->Pos(), 5, iPath::Enter));
				} else {
					path.SetTargetAction((iPath::Action)m_pActHero->GetTarget(dst.x, dst.y, true).action);
				}
				m_pActHero->SetPath(path);
				check(!m_pActHero->Path().IsEmpty());
			} else {
				_LOG(iFormat(_T("\tSelect cell to discover at (%d,%d), cost: %d\n"), target.x, target.y, path.PathCost()).CStr());
				if (dst == m_pActHero->Pos()) {
					// We're on teleport
					m_pActHero->SetPath();
					return true;
				} else if (dst != target) {
					path.SetTargetAction((iPath::Action)m_pActHero->GetTarget(dst.x, dst.y, true).action);
				} else {
					path.SetTargetAction(iPath::MoveTo);
				}
				m_pActHero->SetPath(path);
			}
		} else {
			if (target == m_pOwner->UltSeekPoint()) {
				if (m_pActHero->CanDig()) {
					bool bFound = m_pActHero->Dig();
					_LOG(iFormat(_T("\tDigging for ultimate artifact at (%d,%d): %s\n"), target.x, target.y, (bFound)?_T("Found!"):_T("Nope :(")).CStr());
				} else {
					_LOG(iFormat(_T("\tWait for ultimate artifact digging at (%d,%d)\n"), target.x, target.y).CStr());
				}
				return false;
			} else {
				// if we are at harbor object need to ask admiral first
				IHarbor *hrb = dynamic_cast<IHarbor*>(m_pActHero->GetLocation());
				if(hrb && m_pOwner->Admiral().Process())
					return true;

				_LOG(iFormat(_T("\tActivate current location at (%d,%d)\n"), target.x, target.y).CStr());
				return m_pActHero->SetPath();
			}
		}
	} else {
		_LOG(_T("\tFound nothing, so, lets wait ...\n"));
		return false;
	}

	_LOG(_T("<<<\n"));

	// Start action
	return m_pActHero->Step();
}


fix32 iTargetScanner::LookAround(const iVisItemList& mapItems, iSortArray<iBaseMapObject*>& surObjects)
{
	fix32 result = fix32::zero;
	surObjects.Cleanup();
	m_distMap.MakeDistMap(m_origin, m_armyPower, m_pHero->MoveCostBonus(), m_pHero->InShip(), m_pOwner->PlayerId(), 300);
	sint32 nPriority;
	for (uint32 xx=0; xx<mapItems.Size(); ++xx) {
		iBaseMapObject* pObj = mapItems.Value(xx);
		if (ProcessObject(pObj, nPriority, PRF_IGN_CURLOC | PRF_IGN_DISTANCE) == Process) {
			surObjects.Insert(pObj, nPriority);
			result += fix32((sint32)1) / fix32(iMAX<sint32>(1,nPriority));
		}
	}

	return result;
}


