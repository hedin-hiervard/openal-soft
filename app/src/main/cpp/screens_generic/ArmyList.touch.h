#pragma once

class iArmyListEx_touch: public iArmyListEx
{
public:
	enum GarnisonType {
		GT_TOP = 0,
		GT_BOTTOM,
		GT_MIDDLE,
	};
public:	
	iArmyListEx_touch(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, iCheckButton* pSplitBtn, const iSize& itemSiz, uint32 uid, GarnisonType gt, bool bSwapProhibited = false, bool bGUI = true)
	  : iArmyListEx(pViewMgr, pCmdHandler, pos, pSplitBtn, itemSiz, uid), m_bSwapProhibited(bSwapProhibited), m_gt(gt)
	, m_selectedItem(-1)
	, m_lastClickedItem(-1)
	, m_bGUI(bGUI)
	, m_highlighedCell(-1)
	, m_bInfoMode(false)
	{}		
	virtual void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);	
	virtual void OnMouseEntered(const iPoint& pos, MouseId mID);
	virtual void OnMouseLeave(const iPoint& pos, MouseId mID);
	virtual void OnCompose();
	inline sint32 SelectedItem() { return m_selectedItem; }
	inline void SetSelectedItem(sint32 item) { m_selectedItem = item; }
	uint32 TotalCreatures() const;
	inline void SetGT(GarnisonType gt) { m_gt = gt; Invalidate(); }
	inline void SetInfoMode(bool bInfoMode) { m_bInfoMode = bInfoMode; }
	iRect GetVisualRect();
	void BlinkCell(sint32 idx);
public:
	static iSize sm_defCellSize;
private:
	iRect GetCellRect(uint32 idx) const;
	iRect GetCellVisualRect(uint32 idx) const;
	inline sint8 Pos2Cell(const iPoint& pos) const;	
	sint32 GetVisualOffset() const;
	void ShowInfo(sint32 idx);
private:
	typedef iSimpleArray<iArmyListEx*> iCompList;
	sint32			m_selectedItem;
	sint32			m_lastClickedItem;
	bool			m_bSwapProhibited;
	GarnisonType	m_gt;
	bool			m_bGUI;
	sint32			m_highlighedCell;
	bool			m_bInfoMode;
};
