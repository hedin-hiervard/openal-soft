#ifndef PHEROES_GAME_REMOTE_PLAYER_H_
#define PHEROES_GAME_REMOTE_PLAYER_H_
/*
 *	Remote Player
 */
class iRemote_Player : public iPlayer
{
private:
	bool StepHero();	
public:
	IMPL_TYPEAWARE( iRemote_Player );
	iRemote_Player(PLAYER_TYPE_MASK playerTypeMask, PLAYER_ID playerId, NATION_TYPE nation, const iSize& map_siz);
	iRemote_Player::iRemote_Player(iPlayer& other);
	~iRemote_Player();
	inline PLAYER_TYPE PlayerType() const { return PT_REMOTE; }
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
#endif //PHEROES_GAME_REMOTE_PLAYER_H_
