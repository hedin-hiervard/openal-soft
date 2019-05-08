#ifndef PHEROES_GAME_HERO_SKILLSARMY_STYLUS_H_
#define PHEROES_GAME_HERO_SKILLSARMY_STYLUS_H_
#pragma once

class iSkillsArmyTab_stylus: public iSkillsTab
{
public:
	iSkillsArmyTab_stylus(iViewMgr* pViewMgr);
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void OnHeroChanged();	
	void OnCompose() {}
};
#endif //PHEROES_GAME_HERO_SKILLSARMY_STYLUS_H_
