#include "stdafx.h"
#include "BattleView.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif 

#include "BattlePF.h"

const sint16 HEX_NEBS[2][2][3][2] = {
	{
		{{1,-1},{1,0},{1,1}},
		{{0,-1},{-1,0},{0,1}}
	},{
		{{0,-1},{1,0},{0,1}},
		{{-1,-1},{-1,0},{-1,1}}
	}
};

//////////////////////////////////////////////////////////////////////////
inline uint32 CellsDelta(sint32 px1, sint32 py1, sint32 px2, sint32 py2) 
{ 
	uint32 dx = iABS(px1 - px2); 
	uint32 dy = iABS(py1 - py2);
	if ( (dx & 1) == 0 ) ++dy;
	return dx+(dy>>1); 
}

//////////////////////////////////////////////////////////////////////////
void CalculatePath(iBattleGroup* pGroup, iPoint& dstPoint, uint8& orient)
{
	check (dstPoint.x >= 0 && dstPoint.x < 13 && dstPoint.y >= 0 && dstPoint.y < 11);
	sint16 px = (sint16)dstPoint.x;
	sint16 py = (sint16)dstPoint.y;
	const iBattleGroup::iDistMap& dm = pGroup->DistMap();
	uint8 cost = dm.GetAt(px,py).dirs[orient].val;
	sint32 speed = pGroup->Speed();

	if (pGroup->TransType() == TRANS_FLY) {
		sint16 tgtX, tgtY;
		uint32 tgtIdx = 0xFFFF;
		ODS(iFormat(_T("--Processing unit at %d,%d:\n"), pGroup->Pos().x, pGroup->Pos().y).CStr());
		// Locate nearest acceptable cell
		iPoint np;
		for (sint16 yy=0; yy<dm.GetHeight(); ++yy) {
			np.y = yy;
			for (sint16 xx=0; xx<dm.GetWidth(); ++xx) {
				np.x = xx;
				for (uint8 dd = 0; dd<2; ++dd) {
					if (pGroup->Pos() != np && dm.GetAt(xx,yy).dirs[dd].val <= pGroup->Speed() && pGroup->CanMove(xx,yy) && (pGroup->PassMap().GetAt(xx,yy) & 0x7F) != CT_MOAT){
						uint32 nidx = CellsDelta(xx, yy, px, py);
						ODS(iFormat(_T("\tCan move at %d,%d with idx %d: "), xx, yy, nidx).CStr());
						if (nidx < tgtIdx) {
							tgtX = xx;
							tgtY = yy;
							orient = dd;
							tgtIdx = nidx;
							ODS(_T("Ok\n"));
						} else {
							ODS(_T("No\n"));
						}
					}
				}
			}
		}
		ODS(iFormat(_T("--Found cell at %d,%d.\n"), tgtX, tgtY).CStr());
		//check(pGroup->Pos().x != tgtX || pGroup->Pos().y != tgtY);
		px = tgtX;
		py = tgtY;
	} else if (pGroup->TransType() == TRANS_WALK) {
		if (pGroup->Size() == 1) {
			while (cost > speed && pGroup->CanMove(px,py)) {
				sint16 sv = py&1;
				uint8 dd = dm.GetAt(px,py).dirs[orient].dir;
				uint8 d = dd >> 1;
				uint8 o = !(dd & 0x1);
				sint16 npx = px + HEX_NEBS[sv][o][d][0];
				sint16 npy = py + HEX_NEBS[sv][o][d][1];
				if (orient == o) orient = !orient;
				px = npx;
				py = npy;
				cost = dm.GetAt(px,py).dirs[o].val;
			}
		} else {
			while (cost > speed && pGroup->CanMove(px,py)) {
				sint16 sv = py&1;
				uint8 dd = dm.GetAt(px,py).dirs[orient].dir;
				uint8 d = dd >> 1;
				uint8 o = !(dd & 0x1);
				sint16 npx = px + HEX_NEBS[sv][o][d][0];
				sint16 npy = py + HEX_NEBS[sv][o][d][1];
				uint8 ncost = dm.GetAt(npx,npy).dirs[!o].val;

				sint16 rx = px + TAIL_OFFSET[!o];
				sint16 ry = py;
				if (dm.IsValidPos(rx,ry)) {
					//uint8 rdd = dm.GetAt(rx,ry).dirs[!orient].dir;
					//uint8 rd = rdd >> 1;
					//uint8 ro = !(rdd & 0x1);
					sint16 rnpx = px + HEX_NEBS[sv][!o][d][0];
					sint16 rnpy = py + HEX_NEBS[sv][!o][d][1];
					if (dm.IsValidPos(rnpx,rnpy) && dm.GetAt(rnpx,rnpy).dirs[o].val < ncost) {
						npx = rnpx;
						npy = rnpy;
						ncost = dm.GetAt(rnpx,rnpy).dirs[o].val;
						orient = !orient;
					}
				}
				px = npx;
				py = npy;
				cost = ncost;
			}
		}
	} else {
		// unknown transportation type
		check(0);
	}
	dstPoint.x = px;
	dstPoint.y = py;

}

/*
 *	Path finding
 */
iBattlePF::iBattlePF(iBattleGroup* pGroup)
: m_pGroup(pGroup)
{ }

//static int counter = 0;
static int tcounter = 0;


struct iOpenCell {
	iOpenCell(sint16 _px, sint16 _py, uint8 _dir, uint32 _step)
	: px(_px), py(_py), dir(_dir), step(_step) {}
	sint16 px, py;
	uint32 step;
	uint8 dir;
};

void iBattlePF::PropagateDown(sint16 px, sint16 py, uint8 dir, uint32 step, bool bFull)
{
	iSimpleArray<iOpenCell> openCells;
	uint32 curCell = 0;

	sint32 speed = m_pGroup->Speed();
	bool bFlying = m_pGroup->TransType() == TRANS_FLY;
	
	// Add origin cell
	openCells.Add(iOpenCell(px, py, dir, step));

	while (curCell < openCells.GetSize()) {
		iOpenCell cell = openCells[curCell++];

		// process current cell
		uint8 qdir = (cell.dir & 0x1);
		// 1
		if ( (!bFull && cell.step > speed) ) continue; 
		// 2
		uint8& pmc = m_pGroup->PassMap().GetAt(cell.px,cell.py);
		uint8 tt = (pmc&0x7F);
		// 3
		if ( cell.step && (tt != CT_PASSABLE && tt != CT_BRIDGE && tt != CT_MOAT && !bFlying)) continue;
		// 4
		if ( cell.step && (!bFlying || (tt == CT_PASSABLE || tt == CT_BRIDGE || tt == CT_MOAT)) && (cell.step <= speed || bFull) ) pmc |= 0x80;

		sint16 sv = cell.py&1;
		iBattleGroup::iDistCell& dc = m_pGroup->DistMap().GetAt(cell.px,cell.py);

		// add nebs to open list
		for (uint32 yy=0; yy<2; ++yy) {
			if (dc.dirs[qdir].val > cell.step) {
				dc.dirs[qdir].val = cell.step;
				dc.dirs[qdir].dir = cell.dir;
				uint32 nstep = cell.step+1;
				bool bhpass = (cell.step && tt == CT_MOAT && bFull && !bFlying);
				if (bhpass) nstep = cell.step + 20;
				if (nstep < 0xFF && (!cell.step || tt != CT_MOAT || bFlying || bhpass)) {
					for (uint32 xx=0; xx<3; ++xx) {
						sint16 npx = cell.px+HEX_NEBS[sv][qdir][xx][0];
						sint16 npy = cell.py+HEX_NEBS[sv][qdir][xx][1];
						if (m_pGroup->PassMap().IsValidPos(npx,npy)) openCells.Add(iOpenCell(npx, npy, ((2-xx)<<1) | qdir, nstep));
					}
				}
			}
			qdir = !qdir;
		}
	}

	/*
	iters_cur++;
	uint8 qdir = (dir & 0x1);
	bool bFlying = m_pGroup->TransType() == TRANS_FLY;
	counter++;
	// 1
	if ( (!bFull && step > m_pGroup->Speed()) ) return; 
	// 2
	uint8& pmc = m_pGroup->PassMap().GetAt(px,py);
	uint8 tt = (pmc&0x7F);
	// 3
	if ( step && (tt != CT_PASSABLE && tt != CT_BRIDGE && tt != CT_MOAT && !bFlying)) return;
	// 4
	if (step && (!bFlying || (tt == CT_PASSABLE || tt == CT_BRIDGE || tt == CT_MOAT)) && (step <= m_pGroup->Speed() || bFull)) pmc |= 0x80;

	sint16 sv = py&1;

	iBattleGroup::iDistCell& dc = m_pGroup->DistMap().GetAt(px,py);

	for (uint32 yy=0; yy<2; ++yy) {
		if (dc.dirs[qdir].val > step) {
			dc.dirs[qdir].val = step;
			dc.dirs[qdir].dir = dir;
			uint32 nstep = step+1;
			bool bhpass = (step && tt == CT_MOAT && bFull && !bFlying);
			if (bhpass) nstep = step + 20;
			if (nstep < 0xFF && (!step || tt != CT_MOAT || bFlying || bhpass)) {
				for (uint32 xx=0; xx<3; ++xx) {
					sint16 npx = px+HEX_NEBS[sv][qdir][xx][0];
					sint16 npy = py+HEX_NEBS[sv][qdir][xx][1];
					if (m_pGroup->PassMap().IsValidPos(npx,npy)) PropagateDown(npx,npy, ((2-xx)<<1) | qdir,nstep, bFull);
				}
			}
		}
		qdir = !qdir;
	}*/
}

void iBattlePF::PropagateDown2(sint16 px, sint16 py, uint8 dir, uint32 step, bool bFull)
{
	iSimpleArray<iOpenCell> openCells;
	uint32 curCell = 0;

	sint32 speed = m_pGroup->Speed();
	bool bFlying = m_pGroup->TransType() == TRANS_FLY;
	
	// Add origin cell
	openCells.Add(iOpenCell(px, py, dir, step));

	while (curCell < openCells.GetSize()) {
		iOpenCell cell = openCells[curCell++];
		// process current cell
		uint8 qdir = (cell.dir & 0x1);
		sint16 tpx = cell.px+TAIL_OFFSET[qdir];
		sint16 tpy = cell.py;
		if (!m_pGroup->PassMap().IsValidPos(cell.px,cell.py) || !m_pGroup->PassMap().IsValidPos(tpx,tpy) || (!bFull && cell.step > speed)) continue; 
		// 2
		uint8 c = (m_pGroup->PassMap().GetAt(cell.px,cell.py)&0x7F);
		uint8 tc = (m_pGroup->PassMap().GetAt(tpx,tpy)&0x7F);
		// 3
		if ( cell.step && (( (c != CT_PASSABLE && c != CT_BRIDGE && c != CT_MOAT) || (tc != CT_PASSABLE && tc != CT_BRIDGE && tc != CT_MOAT)) && !bFlying) ) continue;
		// 5
		sint16 sv = cell.py&1;
		// add nebs to open list
		for (uint32 yy=0; yy<2; ++yy) {
			iBattleGroup::iDistCell& dc = m_pGroup->DistMap().GetAt(cell.px,cell.py);
			if (dc.dirs[qdir].val > cell.step) {
				dc.dirs[qdir].val = cell.step;
				dc.dirs[qdir].dir = cell.dir;
				if ((cell.step <= speed || bFull) && (!bFlying || ((c == CT_PASSABLE || c == CT_BRIDGE || c == CT_MOAT) && (tc == CT_PASSABLE || tc == CT_BRIDGE || tc == CT_MOAT)))) m_pGroup->PassMap().GetAt(cell.px,cell.py) |= 0x80;
				if (!cell.step || c != CT_MOAT || bFlying) {
					for (uint32 xx=0; xx<3; ++xx) {
						sint16 npx = cell.px+HEX_NEBS[sv][qdir][xx][0];
						sint16 npy = cell.py+HEX_NEBS[sv][qdir][xx][1];
						if (m_pGroup->PassMap().IsValidPos(npx,npy)) openCells.Add(iOpenCell(npx, npy, ((2-xx)<<1) | qdir, cell.step+1));
					}
				}
			}
			cell.px = tpx;
			qdir = !qdir;
		}
	}

	/*
	iters_cur++;
	// 1
	uint8 qdir = (dir & 0x1);
	sint16 tpx = px+TAIL_OFFSET[qdir];
	sint16 tpy = py;
	bool bFlying = m_pGroup->TransType() == TRANS_FLY;

	if (!m_pGroup->PassMap().IsValidPos(px,py) || !m_pGroup->PassMap().IsValidPos(tpx,tpy) || (!bFull && step > m_pGroup->Speed())) return; 
	// 2
	uint8 c = (m_pGroup->PassMap().GetAt(px,py)&0x7F);
	uint8 tc = (m_pGroup->PassMap().GetAt(tpx,tpy)&0x7F);
	// 3
	if ( step && (( (c != CT_PASSABLE && c != CT_BRIDGE && c != CT_MOAT) || (tc != CT_PASSABLE && tc != CT_BRIDGE && tc != CT_MOAT)) && !bFlying) ) return;

	// 5
	sint16 sv = py&1;

	for (uint32 yy=0; yy<2; ++yy) {
		iBattleGroup::iDistCell& dc = m_pGroup->DistMap().GetAt(px,py);
		if (dc.dirs[qdir].val > step) {
			dc.dirs[qdir].val = step;
			dc.dirs[qdir].dir = dir;
			if ((step <= m_pGroup->Speed() || bFull) && (!bFlying || ((c == CT_PASSABLE || c == CT_BRIDGE || c == CT_MOAT) && (tc == CT_PASSABLE || tc == CT_BRIDGE || tc == CT_MOAT)))) m_pGroup->PassMap().GetAt(px,py) |= 0x80;
			if (!step || c != CT_MOAT || bFlying) {
				for (uint32 xx=0; xx<3; ++xx) {
					sint16 npx = px+HEX_NEBS[sv][qdir][xx][0];
					sint16 npy = py+HEX_NEBS[sv][qdir][xx][1];
					if (m_pGroup->PassMap().IsValidPos(npx,npy)) PropagateDown2(npx,npy, ((2-xx)<<1) | qdir,step+1, bFull);
				}
			}
		}
		px = tpx;
		qdir = !qdir;
	}*/
}

void iBattlePF::MakeDistMap(bool bFull)
{
	// move and melee
	m_pGroup->DistMap().FillMem(iBattleGroup::iDistCell());

	tcounter++;
	if (m_pGroup->Size() == 1) {
		PropagateDown((sint16)m_pGroup->Pos().x,(sint16)m_pGroup->Pos().y,m_pGroup->Orient(), 0, bFull);
	} else {
		PropagateDown2((sint16)m_pGroup->Pos().x,(sint16)m_pGroup->Pos().y,m_pGroup->Orient(), 0, bFull);
	}
}

const sint32 mult = 2;

void iBattlePF::PushAnimation(iBattleActList& actList, iPoint cur, iPoint old, sint8 tailOffset) {		
		//animation frames to show in the following animation sequence
		sint32 to;		
				
		// usual 6-frame walking			
		to = 6;  // last frame (first frame is (to-5) == 1)			
		old.x += tailOffset;
		actList.PushAction(new iBattleAct_Move(m_pGroup, old, to));			
		// push animation			
		
		cur = iBattleView::Map2Screen(cur);
		old = iBattleView::Map2Screen(old);
		sint32 dx = cur.x - old.x, dy = cur.y - old.y;
		for(sint32 x=1; x< 6 * mult; x++) {
			actList.PushAction(new iBattleAct_Anim(m_pGroup, iPoint(old.x + dx * x / (6 * mult), old.y + dy * x / (6 * mult)), to - x%6));
		}											
}

void iBattlePF::MakePath1(const iPoint& pos, uint8 dir, iBattleActList& actList, ANIM_TYPE tt)
{
	sint16 px = (sint16)pos.x;
	sint16 py = (sint16)pos.y;
	iPoint old = iPoint(px, py), cur = cInvalidPoint;
	const iBattleGroup::iDistMap& dm = m_pGroup->DistMap();
	uint8 cost = dm.GetAt(px,py).dirs[dir].val;
	
	// stop sound
	actList.PushAction(new iBattleAct_EndMoving(m_pGroup));

	// last animation frame: static
	actList.PushAction(new iBattleAct_Move(m_pGroup, iPoint(px,py), 0));			

	while (cost) {
		sint16 sv = py&1;
		uint8 dd = dm.GetAt(px,py).dirs[dir].dir;
		uint8 d = dd >> 1;
		uint8 o = !(dd & 0x1);
		sint16 npx = px + HEX_NEBS[sv][o][d][0];
		sint16 npy = py + HEX_NEBS[sv][o][d][1];

		if (dir == o) {
			actList.PushAction(new iBattleAct_Rotate(m_pGroup));
			dir = !dir;
		}			
				
		cur = iPoint(npx, npy);
		PushAnimation(actList, cur, old, 0);

		old = cur;
		px = npx;
		py = npy;
		cost = dm.GetAt(px,py).dirs[0].val;
	}
	// Initial (start point) rotate
	if (dir != m_pGroup->Orient()) actList.PushAction(new iBattleAct_Rotate(m_pGroup));

	// start sound
	actList.PushAction(new iBattleAct_BeginMoving(m_pGroup));

}

void iBattlePF::MakePath2(const iPoint& pos, uint8 dir, bool bMove, iBattleActList& actList, ANIM_TYPE tt)
{
	sint16 px = (sint16)pos.x;
	sint16 py = (sint16)pos.y;
	iPoint old = iPoint(px, py), cur = cInvalidPoint;
	const iBattleGroup::iDistMap& dm = m_pGroup->DistMap();
	uint8 cost = dm.GetAt(px,py).dirs[dir].val;
		
	if ( cost == 0xFF ) {
		dir = !dir;
		if (bMove) {
			actList.PushAction(new iBattleAct_Rotate(m_pGroup));
			cost = dm.GetAt(px,py).dirs[dir].val;
		}
	}
	check(cost != 0xFF);
	
	// stop sound
	actList.PushAction(new iBattleAct_EndMoving(m_pGroup));


	// last animation frame: static
	actList.PushAction(new iBattleAct_Move(m_pGroup, iPoint(px,py), 0));			

	while (cost) {
		sint16 sv = py&1;

		uint8 dd = dm.GetAt(px,py).dirs[dir].dir;
		uint8 d = dd >> 1;
		uint8 o = !(dd & 0x1);

		sint16 npx, npy;
		bool bRotate = false;
		if ( dir == o ){
			npx = px + HEX_NEBS[sv][!o][d][0];
			npy = py + HEX_NEBS[sv][!o][d][1];
			cur = iPoint(npx, npy);				
			dir = !dir;
			bRotate = true;
			actList.PushAction(new iBattleAct_Rotate(m_pGroup));
		} else {
			npx = px + HEX_NEBS[sv][o][d][0];
			npy = py + HEX_NEBS[sv][o][d][1];

		}

		cur = iPoint(npx, npy);											
		// push animation		
		PushAnimation(actList, cur, old, bRotate ? TAIL_OFFSET[o] : 0);
		bRotate = false;

		bRotate = false;

		old = cur;
				
		px = npx;
		py = npy;
		cost = dm.GetAt(px,py).dirs[!o].val;
	}
	// Initial (start point) rotate
	if (dir != m_pGroup->Orient()) actList.PushAction(new iBattleAct_Rotate(m_pGroup));

	// start sound
	actList.PushAction(new iBattleAct_BeginMoving(m_pGroup));

}

bool iBattlePF::MakePath(const iPoint& pos, uint8 dir, bool bMove, iBattleActList& actList, ANIM_TYPE tt)
{
	if (m_pGroup->DistMap().GetAt(pos.x,pos.y).data == 0xFFFF) return false;
	if (m_pGroup->Size() == 1) MakePath1(pos, dir, actList, tt);
	else MakePath2(pos, dir, bMove, actList, tt);

	return true;
}

bool iBattlePF::MakeDirectPath(const iPoint& ipos, const iPoint& pos, uint8 dir, bool bMove, iBattleActList& actList, ANIM_TYPE tt)
{
	bool bRotate = false;	
	uint8 orient = m_pGroup->Orient();
	//iPoint ipos = m_pGroup->Pos();
	uint8 fly;
	const sint32 coef = 4;
	
	
	// check actual moving direction (rotate in case of backward movement)	
	if ((fly = BattleCellsOrient(ipos.x,ipos.y, pos.x,pos.y,orient)) != (uint8)orient) {						
		bRotate = true;
	}

	// stop sound
	actList.PushAction(new iBattleAct_EndMoving(m_pGroup));


	//return to normal
	if(fly != dir) {
		actList.PushAction(new iBattleAct_Rotate(m_pGroup));
	}

	// try to move at tail cell in case of backward movement and size == 2
	iPoint mpos = pos;
	/*if (m_pGroup->Size() == 2) {
	    if (bRotate) {
	        if (m_pGroup->CanMove(pos.x + TAIL_OFFSET[dir],pos.y)) {
                mpos.x += TAIL_OFFSET[m_pGroup->Orient()];
			}
			//ipos.x += TAIL_OFFSET[dir];
        } else if (!m_pGroup->CanMove(pos.x + TAIL_OFFSET[dir],pos.y))
			  mpos.x += TAIL_OFFSET[!dir];
		} 		
*/
	iPoint dst = iBattleView::Map2Screen(mpos);	
	iPoint src = iBattleView::Map2Screen(ipos);

	//sint16 px = dst.x, py = dst.y;	
	float dx, dy;
	sint16 steps;
	sint16 xsteps = iABS<sint16>(dst.x - src.x) / coef; // 1 frame every (coef) px	
	sint16 ysteps = iABS<sint16>(dst.y - src.y) / coef; // 1 frame every (coef) px	;
	if(xsteps > ysteps) {
		steps = xsteps;
		dx = (sint16)(dst.x > src.x ? (-coef) : coef);
		dy = (dst.y > src.y ? (-1.0f) : 1.0f) * iABS<sint16>(dst.y - src.y) / steps;				
	} else {
		steps = ysteps;
		dy = (sint16)(dst.y > src.y ? (-coef) : coef);
		dx = (dst.x > src.x ? (-1.0f) : 1.0f) * iABS<sint16>(dst.x - src.x) /steps;			
	}
		
	// last animation frame: static
	actList.PushAction(new iBattleAct_Move(m_pGroup, mpos, 0));			
	
	if(tt == ANIM_18_TELEPORT) {
		for(sint16 x=0; x<6; ++x) {
			actList.PushAction(new iBattleAct_Anim(m_pGroup, dst, 18 - x));	
		}
		for(sint16 x=0; x<6; ++x) {
			actList.PushAction(new iBattleAct_Anim(m_pGroup, src, 6 - x));			
		}
	} else {
		for(sint16 x=0; x<steps; ++x) {
			sint16 frame;
			if(tt == ANIM_18) {
				if(steps >= 12) {
					if(x < 6)
						frame = (18 - x); //landing
					else if(x < steps - 6) 
						frame = 12 - x%6; // flight
					else
						frame = steps - x; // takeoff						
				} else {
						frame = (x < steps / 2 ? (x + 1) : (19 - steps + x)); //quick-flight
				}	
			} else if(tt == ANIM_18_STILLSTART) {
				if(x < 6) 				
					frame = (18 - x); //landing
				else				
					frame = 12 - x%6; // flight				
			} else {
				frame = 6 - x%6;
			}
			actList.PushAction(new iBattleAct_Anim(m_pGroup, iPoint(dst.x + (sint16)(x * dx), dst.y + (sint16)(x * dy)), frame));
		}	
	}
	// for still-starters: takeoff
	if(tt == ANIM_18_STILLSTART) {
		for(sint16 x=0; x<6; ++x)
			actList.PushAction(new iBattleAct_Anim(m_pGroup, iPoint(src), 6 - x));
	}

	// Initial rotate
	if (bRotate) actList.PushAction(new iBattleAct_Rotate(m_pGroup));

	// start sound
	actList.PushAction(new iBattleAct_BeginMoving(m_pGroup));

	return true;
}

