#ifndef PHEROES_GAME_BATTLEPF_H_
#define PHEROES_GAME_BATTLEPF_H_

// Cell's nebs [even, odd row][right,left direction][id 0-2][x,y offset] (<-even ids(0,2,4), ->odd ids(1,3,5))
extern const sint16 HEX_NEBS[2][2][3][2];
void CalculatePath(iBattleGroup* pGroup, iPoint& dstPoint, uint8& orient);
/*
 *	Path finding
 */
class iBattlePF
{
public:
	iBattlePF(iBattleGroup* pGroup);

	void MakeDistMap(bool bFull);
	bool MakePath(const iPoint& pos, uint8 dir, bool bMove, iBattleActList& actList, ANIM_TYPE tt);
	bool MakeDirectPath(const iPoint& ipos, const iPoint& pos, uint8 dir, bool bMove, iBattleActList& actList, ANIM_TYPE tt);
	
private:
	void PushAnimation(iBattleActList& actList, iPoint cur, iPoint old, sint8 tailOffset);
	void PropagateDown(sint16 px, sint16 py, uint8 dir, uint32 step, bool bFull);
	void PropagateDown2(sint16 px, sint16 py, uint8 dir, uint32 step, bool bFull);
	void MakePath1(const iPoint& pos, uint8 dir, iBattleActList& actList, ANIM_TYPE tt);
	void MakePath2(const iPoint& pos, uint8 dir, bool bMove, iBattleActList& actList, ANIM_TYPE tt);

private:
	iBattleGroup*	m_pGroup;
};


#endif //PHEROES_GAME_BATTLEPF_H_
