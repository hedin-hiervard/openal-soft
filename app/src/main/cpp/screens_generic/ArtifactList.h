#ifndef PHEROES_GAME_ARTIFACTLIST_H_
#define PHEROES_GAME_ARTIFACTLIST_H_

const uint16 INVART = 0xFFFF;

void ComposeArtifactCell(const iRect& rect, uint16 artId, uint32 flags, bool touch, bool bFrameRect = false);

enum ART_BUTTON_FLAGS {
	ABF_PRESSED = 0x1
};

class iArtItemCtrl
{
public:	
	iArtItemCtrl();
	inline void SetArtCell(iArtCell* pArtCell) { m_pArtCell = pArtCell; }
	inline const iArtCell* GetArtCell() const { return m_pArtCell; }
	inline void SetOwner(iHero* pOwner) { m_pOwner = pOwner; }
	inline const iHero* GetOwner() const { return m_pOwner; }
	virtual void Unhighlight() =0;	
	iArtCell Remove(sint32 cell);
	iArtCell Replace(sint32 cell, const iArtCell& newArt);
protected:
	void OnCompose();
	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID) =0;
protected:
	bool		m_bClickMode;
	iHero*		m_pOwner;
	iArtCell*	m_pArtCell;
	iPoint		m_dragAnchor;
	bool		m_bPressed;
	bool        m_bHighlighted;
};


// Artifacts backpack control
class iArtBackPackCtrl : public IViewCmdHandler
{
public:
	enum Flags {
		Vertical	= 0x0,
		Horizontal	= 0x1
	};
public:
	iArtBackPackCtrl(const iSize& cellSiz, uint32 cellsCount, uint32 flags);
	//
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

	//
	inline void SetBackPack(iArtList* pBackPack, iHero* pOwner) { m_pBackPack = pBackPack; m_pOwner = pOwner; }
	inline const iArtList* GetBackPack() const { return m_pBackPack; }

	inline void Unhighlight() {};
	//
	iArtCell Remove(sint32 cell);
	iArtCell Replace(sint32 cell, const iArtCell& newArt);

protected:
	void SetListOffset(sint32 offs);
	virtual void ListPageUp();
	virtual void ListPageDown();
	sint32 CellByPos(const iPoint& pos) const;
	virtual	sint32 BackPackIdxByCell(sint32 cellIdx) const;
	void OnCompose();
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);	
protected:
	bool		m_bClickMode;
	sint32		m_dCell;
	iPoint		m_dragAnchor;
	sint32		m_listOffset;
	uint32		m_flags;
	uint32		m_cellsCount;
	iSize		m_cellSize;
	iArtList*	m_pBackPack;
	iHero*		m_pOwner;
};


// simple Artifact list control (w/o drag'n' drop)
class iArtListCtrl : public IViewCmdHandler, public iBaseCtrl
{
public:
	enum Flags {
		Vertical	= 0x0,
		Horizontal	= 0x1,
	};
	
public:	
	iArtListCtrl(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iArtList& artList, const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags, uint32 uid, bool fullSize, iSize maxSize);
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	void SetListOffset(sint32 offs);
	void ListPageUp();
	void ListPageDown();
	sint32 CellByPos(const iPoint& pos) const;
	sint32 BackPackIdxByCell(sint32 cellIdx) const;
	void OnCompose();
	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);

private:
	sint32			m_clickItem;
	bool			m_bClickMode;
	sint32			m_dCell;
	iPoint			m_dragAnchor;
	sint32			m_listOffset;
	uint32			m_flags;
	uint32			m_cellsCount;
	iSize			m_cellSize;
	const iArtList&	m_artList;
	bool			m_bFullSize;
};

#endif //PHEROES_GAME_ARTIFACTLIST_H_
