#include "stdafx.h"
#include "RMG.h"

iRandomMapGenerator::iRandomMapParam::iRandomMapParam(): m_Size(MSIZ_SMALL), m_Difficulty(DFC_NORMAL)
{		
	iMapInfo::iPlayerInfo playerInfo(PID_RED, PTM_HUMAN_OR_COMPUTER, PT_HUMAN, CTLT_CITADEL);
	m_Players.Add(playerInfo);
	iMapInfo::iPlayerInfo playerInfoComp(PID_RED, PTM_HUMAN_OR_COMPUTER, PT_COMPUTER, CTLT_CITADEL);
	m_Players.Add(playerInfoComp);
}

void iRandomMapGenerator::Generate(const iRandomMapGenerator::iRandomMapParam &param)
{

}