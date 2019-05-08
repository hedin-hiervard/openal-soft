#ifndef PH_COMMON_CM_EVENTS_H_
#define PH_COMMON_CM_EVENTS_H_

/*
 *	Time-events
 */

#define TEF_HUMANENABLED (1 << 0)
#define TEF_AIENABLED    (1 << 1)


struct iTimeEvent 
{
	enum EventFreq {
		EFREQ_NEVER = 0,
		EFREQ_1DAY, EFREQ_2DAYS, EFREQ_3DAYS, EFREQ_4DAYS, EFREQ_5DAYS, EFREQ_6DAYS, 
		EFREQ_1WEEK,EFREQ_2WEEKS, EFREQ_3WEEKS, EFREQ_1MONTH, 
		EFREQ_COUNT
	};

	enum WeekAffect {
		NoAffect,
		Normal,			// Any week: Normal population growth
		Plague,			// All (dwellings) population halves
		CreatPlus,		// Normal population growth, +5 of specified creature
		CreatDoubles,	// Specified creature population doubles (dwellings), generate map guards	
		WeekAffectCount
	};

	iTimeEvent(const iStringT& name, uint32 playerMask, iStringT &text, uint32 ftime, EventFreq repTime, const iMineralSet& minerals)
	: m_name(name), m_playerMask(playerMask), m_text(text), m_fTime(ftime), m_repTime(repTime), m_minerals(m_minerals), m_weekAffect(NoAffect),
	m_crType(CREAT_UNKNOWN) {
		
	}
	iTimeEvent()
	: m_playerMask(PIM_ALL), m_fTime(1), m_repTime(EFREQ_NEVER), m_weekAffect(NoAffect),
	m_crType(CREAT_UNKNOWN)  { m_minerals.Reset(); }

	bool IsConform(PLAYER_ID pid, uint32 curDay) const;

	iStringT	m_name;
	iStringT	m_text;
	uint32		m_playerMask;
	uint32		m_fTime;
	EventFreq	m_repTime;
	iMineralSet	m_minerals;
	uint32      m_flags;
	WeekAffect  m_weekAffect;
	CREATURE_TYPE m_crType;
};

// Time events manager
class iTimeEventMgr
{
public:
	// c-tor, d-tor
	iTimeEventMgr() {}
	~iTimeEventMgr() {}

	//
	inline uint32 EventsCount() const { return m_events.GetSize(); }
	inline const iTimeEvent& Event(uint32 idx) const { return m_events[idx]; }
	inline iTimeEvent& Event(uint32 idx) { return m_events[idx]; }
	inline void AddEvent(const iTimeEvent& event) { m_events.Add(event); }
	inline void DeleteEvent(uint32 idx) { m_events.RemoveAt(idx); }
	inline void DeleteAll() { m_events.RemoveAll(); }

private:
	iSimpleArray<iTimeEvent>	m_events;
};


#endif //PH_COMMON_CM_EVENTS_H_
