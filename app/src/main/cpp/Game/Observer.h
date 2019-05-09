#ifndef PHEROES_GAME_OBSERVER_H_
#define PHEROES_GAME_OBSERVER_H_
/*
 *	Observer Player
 */
class iObserver_Player : public iPlayer
{
private:
	bool StepHero();	
public:
	IMPL_TYPEAWARE( iObserver_Player );
	iObserver_Player();
	~iObserver_Player();
	inline PLAYER_TYPE PlayerType() const { return PT_OBSERVER; }
	void Process(fix32 t);
	void OnSelect(bool bNewWeek, bool bAfterLoad);	

	void ProcessChest(iHero* pHero, iMapItem_Chest* pChest);

	bool ProcessRecruitCreatures(iArmy& army, iCreatGroup& cgroup);
	bool ProcessJoinCreatures(iArmy& army, const iCreatGroup& cgroup);

	bool NeedMeeting(const iHero* pHero1, const iHero* pHero2) const;
	void MeetHeroes(iHero* pHero1, iHero* pHero2);
	void UpdateUltSeekPoint();

	bool NeedStopHero() const;
	void ProcessItemChanges();
	void OnHeroDead(iHero* pHero);
	sint32 MineralValue(MINERAL_TYPE mt) const;	
};
#endif //PHEROES_GAME_OBSERVER_H_
