#ifndef PHEROES_GAME_MAPENUM_H_
#define PHEROES_GAME_MAPENUM_H_
#pragma  once


typedef iSimpleArray<iMapInfo*> iScenList;

#define STATS_VERSION_SELECTOR 0xFFFFFFFF
#define STATS_VERSION 1

class iMapEnum
{
public:
	struct iMapStatInfo {
		iStringT id;
		uint32 played;
        uint32 finished;
	};
	
public:

    iMapEnum();
    ~iMapEnum();
    iScenList Filter(uint32 mt);
    void Filter(iScenList &sl, uint32 mt, bool hotseat);
    void EnumScenarios();    
    bool Init();
	void MarkMapStarted(const iStringT& id);
	void MarkMapFinished(const iStringT& id);
	void ResetMapStats(const iStringT& id);

	
    void Update();
    uint32 GetCount(uint32 mask, bool hotseat);
    uint32 GetTutorialsNum();
	uint32 GetUnplayedNum();
	void SaveStat();
	uint32 GetPlayed(const iStringT& id);
	uint32 GetFinished(const iStringT& id);
    
    iMapInfo* GetScenListItem( uint32 nIndex );
    uint32 GetScenListSize() { return m_scList.GetSize(); }
    
protected:
    void Serialize(iFileI* to);
    void Unserialize(iDynamicBuffer& dbuff);
    void Cleanup();

    iScenList m_scList;
	iSimpleArray<iMapStatInfo> m_stats;
};



// END OF FILE
#endif //PHEROES_GAME_MAPENUM_H_
