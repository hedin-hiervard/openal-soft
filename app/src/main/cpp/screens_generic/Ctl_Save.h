#ifndef PHEROES_GAME_CTL_SAVE_H_
#define PHEROES_GAME_CTL_SAVE_H_

const uint32 SAVE_GAME_SLOTS = 100;

struct iSaveSlot {
	iMapInfo *mapInfo;
	uint32 idx;
};

typedef iSimpleArray<iSaveSlot> iSaveSlots;

class iSaveListBox;
//////////////////////////////////////////////////////////////////////////
class iSaveGameView : public iView, public IViewCmdHandler
{
public:
    iSaveGameView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool bSave) ;
    ~iSaveGameView();
    static void GetSaveFileName(iStringT& fname, uint32 slot);
    bool SelFile(iStringT& fname) const;
    const iMapInfo& SelScenario() const;
	const iMapInfo* GetAutoSave() const;
    inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}
    bool IsGoodSelection() const;
    void LoadData();
	bool Process(fix32 t);
private:
    enum SortBy 
    {
        Name = 0,
        RevName,
        Date,
        RevDate,
        Progress,
        RevProgress,
        Size,
        RevSize
    };
    void SortSaves(SortBy sort_by);

    void OnCompose() ;
    void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
    inline bool CanSelect() const { return m_selSlot != -1 && (m_bSave || m_saveSlots[m_selSlot].mapInfo != NULL); }
    bool Confirmed() const;
    void SendResult(uint32 res);
    void CheckHeaderButton(uint32 id);   
	


private:
    bool		m_bSave;
    sint32		m_selSlot;
    iSaveSlots	m_saveSlots;
    IViewCmdHandler*  m_pCmdHandler; 
    iSaveListBox *m_pLB;
    SortBy      m_sort;
};



#endif //PHEROES_GAME_CTL_SAVE_H_
