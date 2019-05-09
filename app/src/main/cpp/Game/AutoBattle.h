#ifndef PHEROES_GAME_AUTOBATTLE_H_
#define PHEROES_GAME_AUTOBATTLE_H_

class iAutoBattle : public iBattleWrapper
{
public:
	iAutoBattle(bool bShowResult = false);
	void OnBeginBattle();
	void OnStart();
	void OnEndBattle();
	void AddLogEvent(const iStringT& msg);
	void AddCellEvent(const iStringT& msg, const iPoint& pos);
	void OnNextGroup();
	void PlayAnim();
	inline virtual sint16 PlaySound(sint16 sid, bool loop = false) const { return -1; }
	inline virtual void StopSound(sint16 streamid) const {}
	virtual void Block() {};
	virtual void Unblock() {};
	virtual bool IsBlocked() const { return false; }
private:
	bool	m_bShowResult;
};


#endif //PHEROES_GAME_AUTOBATTLE_H_
