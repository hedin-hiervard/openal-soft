#ifndef PHEROES_GAME_DLG_HALLOFFAME_H_
#define PHEROES_GAME_DLG_HALLOFFAME_H_
//////////////////////////////////////////////////////////////////////////
class iHighScore
{
public:
	struct iEntry {
		iEntry() {}
		iEntry(const iStringT& land_, uint32 date_, uint32 days_, uint32 score_) : land(land_), date(date_), days(days_), score(score_){}
		iStringT	land;
		uint32		date;
		uint32		days;
		uint32		score;
	};
	
public:
	void Reset();
	void Load(const iStringT& fname);
	void Save(const iStringT& fname) const;
	sint32 AddEntry(const iHighScore::iEntry& entry);

	inline uint32 Count() const { return m_entries.GetSize(); }
	inline const iEntry& Entry(uint32 idx) const { return m_entries[idx]; }

private:
	iSimpleArray<iEntry>	m_entries;
};
//////////////////////////////////////////////////////////////////////////
class iDlg_HallOfFame : public iBaseGameDlg
{
public:
	iDlg_HallOfFame(iViewMgr* pViewMgr, const iStringT& fname);
	iDlg_HallOfFame(iViewMgr* pViewMgr, const iStringT& fname, const iHighScore::iEntry& entry);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	sint32		m_curScore;
	iStringT	m_fname;
	iHighScore	m_hScore;
};


#endif //PHEROES_GAME_DLG_HALLOFFAME_H_
