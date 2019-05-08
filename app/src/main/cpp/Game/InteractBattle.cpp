#include "stdafx.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif 


#include "InteractBattle.h"
#include "BattleView.h"


void Debug(iStringA str);

iInteractBattle::iInteractBattle()
: iBattleWrapper(false), m_pBattleView(NULL), m_bBlocked(false)
{
}

void iInteractBattle::OnNextGroup() {
	//if(gGame.View(iChildGameView::CV_BATTLE))
		((iBattleView*)gGame.View(iChildGameView::CV_BATTLE))->OnGroupChanged();
	PlayAnim();
}

void iInteractBattle::PlayAnim() {
	if(m_engine.ActionCount()) {
		((iBattleView*)gGame.ActView())->BeginAni();
#ifdef MULTIPLAYER
		Debug("entering interactbattle:: playanim");
#endif
		while(gGame.View(iChildGameView::CV_BATTLE) && ((iBattleView*)gGame.View(iChildGameView::CV_BATTLE))->IsAni()) gApp.Cycle();			
#ifdef MULTIPLAYER
		Debug("exiting interactbattle:: playanim");
#endif
	}
}

enum BATTLE_SKY_CHOICE {
	BSC_FIRST = 1,
	BSC_SEA,	
	BSC_SEA_RAND
};

struct bbdesc {
	BATTLE_BACK_TYPE back;
	BATTLE_SKY_TYPE skies[3];
	BATTLE_SKY_CHOICE choice;
};


bbdesc battle_backgrounds[STYPE_COUNT] = {
	{
		BBTYPE_SHIP,
		{BSTYPE_SHIP, BSTYPE_INVALID, BSTYPE_INVALID}, BSC_FIRST},
	{
		BBTYPE_BEACH,
		{BSTYPE_BEACH_SEA, BSTYPE_INVALID, BSTYPE_INVALID}, BSC_FIRST},
	{
		BBTYPE_DIRT,
		{BSTYPE_DIRT_SEA, BSTYPE_DIRT_MOUNT, BSTYPE_DIRT_FOREST}, BSC_SEA_RAND},
	{
		BBTYPE_GRASS,
		{BSTYPE_GRASS_SEA, BSTYPE_GRASS_FOREST, BSTYPE_INVALID}, BSC_SEA},
	{
		BBTYPE_SWAMP,
		{BSTYPE_SWAMP_FOREST, BSTYPE_INVALID, BSTYPE_INVALID}, BSC_FIRST},
	{
		BBTYPE_LAVA,
		{BSTYPE_LAVA_MOUNT, BSTYPE_INVALID, BSTYPE_INVALID}, BSC_FIRST},
	{
		BBTYPE_WASTELAND,
		{BSTYPE_WASTELAND_MOUNT, BSTYPE_INVALID, BSTYPE_INVALID}, BSC_FIRST},
	{
		BBTYPE_GOLDEN_SAND,
		{BSTYPE_GOLDEN_SAND_SEA, BSTYPE_GOLDEN_SAND_MOUNT, BSTYPE_INVALID}, BSC_SEA},
	{
		BBTYPE_SNOW,
		{BSTYPE_SNOW_SEA, BSTYPE_SNOW_MOUNT, BSTYPE_INVALID}, BSC_SEA},
	{
		BBTYPE_GOLDEN_SAND,
		{BSTYPE_GOLDEN_SAND_SEA, BSTYPE_GOLDEN_SAND_MOUNT, BSTYPE_INVALID}, BSC_SEA},
	{
		BBTYPE_BRICK,
		{BSTYPE_BRICK_CITY, BSTYPE_INVALID, BSTYPE_INVALID}, BSC_FIRST},
	{
		BBTYPE_BARREN,
		{BSTYPE_BARREN_MOUNT, BSTYPE_INVALID, BSTYPE_INVALID}, BSC_FIRST},
};

void iInteractBattle::OnBeginBattle()
{
#ifdef MULTIPLAYER	
	gMPMgr.SetBattleWrapper(this);
	if(gMPMgr.IsMultiplayer() && !gMPMgr.IsReplaying())
		gSfxMgr.PlaySound(CSND_PICKUP01);
#endif


	gTutorial.Trigger(TUT_BATTLE);
    gGame.ShowView(iChildGameView::CV_BATTLE);

    
	m_pBattleView = (iBattleView*)gGame.View(iChildGameView::CV_BATTLE);
	check(m_pBattleView);
	iPoint dpos = m_engine.GetBattleInfo().Defender()->Pos();
	iCell c = gGame.Map().GetAt(dpos.x,dpos.y);
		
	SURF_TYPE upSurf = (SURF_TYPE)c.SurfNode(0);
	BATTLE_BACK_TYPE back = BBTYPE_GRASS;
	BATTLE_SKY_TYPE sky = BSTYPE_GRASS_FOREST;
	bool bIsSea = false;
	for(sint32 dx = -3; dx <= 3; dx++) {
		for(sint32 dy = -3; dy <= 3; dy++)
			if(gGame.Map().IsValidPos(dpos.x + dx, dpos.y + dy)) {
				iCell c = gGame.Map().GetAt(dpos.x + dx,dpos.y + dy);	
				SURF_TYPE surf = (SURF_TYPE)c.SurfNode(0);
				if(surf == STYPE_WATER) {
						bIsSea = true;
						break;
				} else if(upSurf == STYPE_WATER)
					upSurf = surf;
			}
		if(bIsSea)
			break;
	}	
	if( DynamicCast<iBattleMember_Castle*>(m_engine.GetBattleInfo().m_pDefender)) {
		back = BBTYPE_BRICK;
		sky = BSTYPE_BRICK_CITY;
	} else if(m_engine.OnShip()) {
		back = BBTYPE_SHIP;
		sky = BSTYPE_SHIP;
	} else {
		back = battle_backgrounds[upSurf].back;
		switch(battle_backgrounds[upSurf].choice) {
			case BSC_FIRST: sky = battle_backgrounds[upSurf].skies[0]; break;
			case BSC_SEA: sky = bIsSea ? battle_backgrounds[upSurf].skies[0] : battle_backgrounds[upSurf].skies[1]; break;
			case BSC_SEA_RAND: 
				if(bIsSea)
					sky = battle_backgrounds[upSurf].skies[0];
				else
					sky = (gGame.Map().Rand(1) == 0 ? battle_backgrounds[upSurf].skies[1] : battle_backgrounds[upSurf].skies[2]); 
				break;
		}
	}

	m_pBattleView->BeginBattle(this, upSurf, back, sky);
}

void iInteractBattle::OnStart()
{
	m_pBattleView->Start();
}

void iInteractBattle::OnEndBattle()
{
	// Hide BattleView
	gGame.HideView(iChildGameView::CV_BATTLE);
	m_pBattleView = NULL;
}

void iInteractBattle::AddLogEvent(const iStringT& msg)
{
	if (m_pBattleView) m_pBattleView->AddLogEvent(msg);
}

void iInteractBattle::AddCellEvent(const iStringT& msg, const iPoint& pos)
{
	if (m_pBattleView) m_pBattleView->AddCellEvent(msg, pos);
}

