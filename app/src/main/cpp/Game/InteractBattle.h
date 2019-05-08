#ifndef PHEROES_GAME_INTERACTBATTLE_H_
#define PHEROES_GAME_INTERACTBATTLE_H_

class iBattleView;
class iInteractBattle : public iBattleWrapper
{
public:
	iInteractBattle();
	void OnBeginBattle();
	void OnStart();
	void OnEndBattle();
	void AddLogEvent(const iStringT& msg);
	void AddCellEvent(const iStringT& msg, const iPoint& pos);
	void OnNextGroup();
	void PlayAnim();
	inline virtual sint16 PlaySound(sint16 sid, bool loop = false) const { 
		return gSfxMgr.PlaySound(sid, loop);
	}
	inline virtual void StopSound(sint16 streamid) const {
		gApp.SndPlayer().Stop(streamid);
	}
	virtual void Block() { m_bBlocked = true; }
	virtual void Unblock() { m_bBlocked = false; }
	virtual bool IsBlocked() const { return m_bBlocked; }
private:
	iBattleView*	m_pBattleView;	
	bool			m_bBlocked;
};


#endif //PHEROES_GAME_INTERACTBATTLE_H_
