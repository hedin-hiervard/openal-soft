#ifndef PHEROES_GAME_HIDDENBATTLE_H_
#define PHEROES_GAME_HIDDENBATTLE_H_

class iHiddenBattle : public iBattleWrapper
{
public:
	iHiddenBattle(bool bShowResult = false);
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
	bool Step(iBattleResult& br);
	bool	m_bShowResult;
};


#endif //PHEROES_GAME_HIDDENBATTLE_H_
