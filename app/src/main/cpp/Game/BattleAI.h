#ifndef PHEROES_GAME_BATTLEAI_H_
#define PHEROES_GAME_BATTLEAI_H_

class iBattleAI
{
public:
	iBattleAI(iBattleEngine& engine);

private:
	iBattleEngine&	m_engine;
};

iBattleGroup* SelectTurretTarget(iBattleEngine& engine);
bool ProcessBattleStepAI(iBattleEngine& engine);
bool CheckBattleStateAI(iBattleEngine& engine, bool bUserCanCancel, bool& bTriedToEscape, iBattleResult& br);
bool ProcessSpellCastAI(iBattleEngine& engine);


#endif //PHEROES_GAME_BATTLEAI_H_
