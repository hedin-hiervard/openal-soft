#ifndef SPELLCAST_ANIMATION_H
#define SPELLCAST_ANIMATION_H


typedef void(*AniCBFunc)();

class iAnimationHost;

class iGAniObj {
public:		
	bool Process(fix32 t);
	iGAniObj(fix32 length);
	virtual bool OnStart() { return false; }
	virtual bool OnProcess() { return false; };
	virtual bool OnEnd() { return false; }
	inline void ResetTimer() { m_Timer = fix32::zero; }
	~iGAniObj() {}
	void SetHost(iAnimationHost* host) { m_host = host; }
	inline iView* GetHost()  { return (iView*)m_host; }
	inline fix32 GetDuration() { return m_Duration; }
protected:
	fix32	m_Timer;
	fix32	m_Duration;
	iAnimationHost* m_host;
};

class iGAniObj_ChangeSprite: public iGAniObj {
public:
	iGAniObj_ChangeSprite(fix32 length, sint16& target, sint16 to): iGAniObj(length), m_target(target), m_to(to) {}
	virtual bool OnEnd() { 
		m_target = m_to;
		return true;
	}
private:
	sint16& m_target;
	sint16 m_to;	
};

template <typename T>
class iGAniObj_ChangeValDisc: public iGAniObj {
public:
	iGAniObj_ChangeValDisc(fix32 length, T& target, T to): iGAniObj(length), m_target(target), m_to(to) {}
	virtual bool OnEnd() { 
		m_target = m_to;
		return true;
	}
private:
	T& m_target;
	T m_to;	
};


class iGAniObj_CustomCallback: public iGAniObj {	
public:
	iGAniObj_CustomCallback(fix32 length, AniCBFunc cb): iGAniObj(length), m_cb(cb) {}
	virtual bool OnEnd() { 
		(*m_cb)();
		return false;
	}
	
private:
	 AniCBFunc m_cb;
};

typedef struct {
	iSimpleArray<iGAniObj*> queue;
	bool loop;
	uint32 pos;
	bool enabled;
	bool fixId; // if set to true, AnimationHost will allow reuse of this queue only by direct request with certain QID 
} iAniQ;
typedef sint32 AniQID;
#define INVALID_QUEUE (-1)


class iGAniObj_EnableQueue: public iGAniObj {	
public:
	iGAniObj_EnableQueue(AniQID qid): iGAniObj(fix32::zero), m_qid(qid) {}
	virtual bool OnEnd();
	
private:
	 AniQID m_qid;
};

class iGAniObj_LoopMarker: public iGAniObj {	
public:
	iGAniObj_LoopMarker(AniQID qid): iGAniObj(fix32::zero), m_qid(qid) {}
	virtual bool OnEnd();
	
private:
	 AniQID m_qid;
};

class iGAniObj_Delay: public iGAniObj {	
public:
	iGAniObj_Delay(fix32 t): iGAniObj(t){}		
};


template <typename T> 
class iGAniObj_ChangeVal: public iGAniObj {	
public:
	iGAniObj_ChangeVal(fix32 length, T& target, T to): iGAniObj(length), m_target(target), m_to(to), m_initial(target) {}
	virtual bool OnProcess() {
		if(m_Duration == fix32::zero)
			m_target = m_to;
		else if(m_to > m_initial)
			m_target = m_initial + (((m_to - m_initial) * m_Timer) / m_Duration).floor();
		else
			m_target = m_initial - (((m_initial - m_to) * m_Timer) / m_Duration).floor();
		return true;
	}
	virtual bool OnStart() {
		m_initial = m_target;
		return false;
	}

	virtual bool OnEnd() { 
		m_target = m_to;
		return true;
	}
private:
	T& m_target;
	T m_to;	
	T m_initial;
};


class iGAniObj_Move: public iGAniObj {	
public:
	iGAniObj_Move(fix32 length, iRect& target, iRect to): iGAniObj(length), m_target(target), m_to(to), m_initial(target) {}
	virtual bool OnProcess() {
		if(m_Duration == fix32::zero)
			m_target = m_to;
		else  {
			m_target.x = m_initial.x + (((m_to.x - m_initial.x) * m_Timer) / m_Duration).floor();
			m_target.y = m_initial.y + (((m_to.y - m_initial.y) * m_Timer) / m_Duration).floor();
			m_target.w = m_initial.w + (((m_to.w - m_initial.w) * m_Timer) / m_Duration).floor();
			m_target.h = m_initial.h + (((m_to.h - m_initial.h) * m_Timer) / m_Duration).floor();
		}
		return true;
	}
	virtual bool OnStart() {
		m_initial = m_target;
		return false;
	}

	virtual bool OnEnd() { 
		m_target = m_to;
		return true;
	}
private:
	iRect& m_target;
	iRect m_to;	
	iRect m_initial;
};


/* Global object, holding all animation queues */ 
class iAnimationHost {
public:
	iAnimationHost(); 
	~iAnimationHost();
	void SetLoop(bool loop, AniQID qid) { m_qList[qid].loop = loop; m_qList[qid].pos = 0; }
	AniQID AddAniObj(iGAniObj* obj, bool newq = false, AniQID waitForQueue = INVALID_QUEUE);
	AniQID AddAniObjToQ(iGAniObj* obj, AniQID &qid, AniQID waitForQueue = INVALID_QUEUE);		
	void Process(fix32 t);
	bool IsPlaying(AniQID qid = INVALID_QUEUE);
	inline void SetQFixed(AniQID qid, bool fix = true) { m_qList[qid].fixId = fix; } // call this right after creation of new queue
	bool IsQFixed(AniQID qid) { return m_qList[qid].fixId; }
	void StopAllAnimation(bool endAniObjects = false); 
    void StopAnimation(AniQID qid, bool endAniObjects = false);
	inline void SetQEnabled(AniQID qid, bool enable) { m_qList[qid].enabled = enable; }
	void Finish(AniQID qid);

private:
	fix32 TimeLeft(AniQID qid);
	iSimpleArray<iAniQ >    m_qList;	
};

#endif