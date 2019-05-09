#include "stdafx.h"
#include "AutoBattle.h"
#include "BattleAI.h"
#include "Dlg_BattleResult.h"

iAutoBattle::iAutoBattle(bool bShowResult)
: iBattleWrapper(true), m_bShowResult(bShowResult)
{
}

void iAutoBattle::OnBeginBattle()
{
    
}

void iAutoBattle::OnNextGroup()
{	
}

void iAutoBattle::PlayAnim()
{

}

void iAutoBattle::OnStart()
{
#ifdef MULTIPLAYER 
	if(gMPMgr.IsMultiplayer())
		gMPMgr.SetPlayingAutobattle(true);
#endif
	iBattleResult br;
	while (1) {
		bool bCasted = ProcessSpellCastAI(m_engine);

		bool bTried;
		if (!CheckBattleStateAI(m_engine, false, bTried, br)) {
			break;
		}
			
		// this was changed, because the commented line could cause infinite loop if forever defending/waiting with all units for one player, Hedin 22.04.11
		/*if (!bCasted) {
			bool bRes = ProcessBattleStepAI(m_engine);
			bRes = true;
		}*/		
		// THE ABOVE LOOPS AS WELL, RETURN TO ORIGINAL CODE, Hedin, 17.06.11, the test case is in Test_maps/wait-defend-loop.phs
		
		if (!bCasted && !ProcessBattleStepAI(m_engine)) continue;
		
		while (m_engine.ActionCount()) {
			iBattleAct* pAct = m_engine.StepAction();
			check(pAct);
			delete pAct;
		}
		if (m_engine.CheckBattleState() != BR_NA) {
			br.m_result = m_engine.CheckBattleState();
			br.m_defCause = DC_DEFEAT;
			break;
		}
		m_engine.NextGroup();
	}
	
	m_engine.PrepareBattleResult(br);
	if (m_bShowResult) {
		iDlg_BattleResult brd(&gApp.ViewMgr(), m_engine, br);
		brd.DoModal();
	}

	EndBattle(br);
#ifdef MULTIPLAYER 
	if(gMPMgr.IsMultiplayer())
		gMPMgr.SetPlayingAutobattle(false);
#endif
}

void iAutoBattle::OnEndBattle()
{
	/*
	int a = 0;
	const iArmy& aArmy = m_engine.GetBattleInfo().m_pAssaulter->Army();
	for (uint32 aidx=0; aidx<m_engine.AArmy().Count(); ++aidx) {
	}

	const iArmy& dArmy = m_engine.GetBattleInfo().m_pDefender->Army();
	for (uint32 didx=0; didx<m_engine.DArmy().Count(); ++didx) {
	}
	*/
}

void iAutoBattle::AddLogEvent(const iStringT& msg) 
{
}

void iAutoBattle::AddCellEvent(const iStringT& msg, const iPoint& pos)
{
}

