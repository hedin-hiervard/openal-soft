#ifndef PHEROES_GAME_DLG_CREATINFO_H_
#define PHEROES_GAME_DLG_CREATINFO_H_

class iPerksBtn;

const uint32 CDV_COST = (1 << 0);
const uint32 CDV_BKND = (1 << 1);
const uint32 CDV_GLOW = (1 << 2);
const uint32 CDV_TITLE = (1 << 3);
const uint32 CDV_STATS = (1 << 5);
const uint32 CDV_LINEAR_PERKS = (1 << 6);
const uint32 CDV_NOTITLE_PERKS = (1 << 7);
const uint32 CDV_LINEAR_BATTLE_PERKS = (1 << 8);

class iCreatDescView : public iView, public IViewCmdHandler
{
public:
	iCreatDescView(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, iRect rc, uint32 uid, uint32 flags, iBattleGroup* pGroup);	
	void SetType(CREATURE_TYPE tp);
	inline void SetAmount(sint32 amount) { m_amount = amount; }
	void SetGroup(iBattleGroup* pGroup);
	inline void SetMinerals(const iMineralSet* ms) { m_ms = ms; }
	inline void SetCost(const iMineralSet cost) { m_cost = cost; m_bCost = true; }
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    inline void SetFlags(uint32 flags) { m_flags = flags; }
#endif
    void UpdateButtons();

    void OnMouseEntered(const iPoint& pos, MouseId mID);
	void OnMouseLeave(const iPoint& pos, MouseId mID);
public:
	static iSize GetSize(sint32 w, CREATURE_TYPE type, sint32 amount, uint32 flags, iBattleGroup* pGroup);
protected:
	virtual const iMineralSet& GetCost() { return m_bCost ? m_cost : CREAT_DESC[m_type].cost; }	
private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void OnCompose();	
protected:		
	IViewCmdHandler* m_pCmdHandler;	
	CREATURE_TYPE m_type;
	sint32 m_amount;
	iSimpleArray<iPerksBtn*> m_pPerks;	
	iBattleGroup* m_pGroup;	
	uint32 m_flags;
	const iMineralSet* m_ms;
	iMineralSet m_cost;
	bool m_bCost;
    bool m_bEntered;
};
	
class iDlg_CreatInfo : public iGlowGameDlg
{
public:
	iDlg_CreatInfo(iViewMgr* pViewMgr, PLAYER_ID pid, iArmyOwner* owner, sint32 idx, const iFurtSkills& furtSkills, bool bCanDismiss, sint32 moraleMod, iCreatBonusList cbl);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
    void OnCompose();
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
#ifdef PC_VERSION
    void UpdateSize();
#else
    void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual inline bool AlwaysReceiveMouse() { return true; }
#endif
private:
	sint32			m_moraleMod;
	const iCreatGroup&	m_cGroup;
	sint32			m_idx;
	iFurtSkills		m_furtSkills;
	bool			m_bCanDismiss;
	iCreatBonusList m_cbl;
	iArmyOwner*		m_pOwner;
    iCreatDescView* desc;
    iButton*    m_pBtnDismiss;
    iButton*    m_pBtnOk;
};


#endif //PHEROES_GAME_DLG_CREATINFO_H_
