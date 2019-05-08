#ifndef PHEROES_GAME_ARTIFACTLIST_TOUCH_H_
#define PHEROES_GAME_ARTIFACTLIST_TOUCH_H_
#pragma once


// Abstract artifacts clickable item

class iArtClickItem : public iBaseCtrl
	{
	public:
		// c-tor, d-tor
		iArtClickItem(iSimpleArray<iArtClickItem*>& competitors, const iSize& cellSiz, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
		virtual ~iArtClickItem() {}
				
		
		virtual iArtCell Remove(sint32 cell)  =0;
		virtual iArtCell Replace(sint32 cell, const iArtCell& newArt)  =0;				

		sint32								m_selectedItem;		
		virtual uint16 ArtId() const =0;
		
		virtual void Highlight(iArtClickItem* pSource, uint16 artId) =0;
		virtual void Unhighlight() =0;
	protected:
		iSimpleArray<iArtClickItem*>&	m_Competitors;
		iSize								m_cellSiz;
	};



// Artifact item control for touch interface (for Hero Doll)
class iArtItemCtrl_touch : public iArtClickItem, public iArtItemCtrl	
{
public:
	//
	iArtItemCtrl_touch(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 uid);	
	void Highlight(iArtClickItem* pSource, uint16 artId);	
	virtual void Unhighlight() ;
	virtual void OnCompose();		

	virtual iArtCell Remove(sint32 cell) { return iArtItemCtrl::Remove(cell); }
	virtual iArtCell Replace(sint32 cell, const iArtCell& newArt) { return iArtItemCtrl::Replace(cell, newArt); }

	virtual uint16 ArtId() const { return m_pArtCell->artId; }
	virtual void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);	
	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
};


class iArtBackPackCtrl_touch : public iArtClickItem, public iArtBackPackCtrl
{
public:
	//
	iArtBackPackCtrl_touch(iSimpleArray<iArtClickItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags, uint32 uid);

	virtual uint16 ArtId() const { 	
		if(m_selectedItem == -1)
			return INVART;
		sint32 bpIdx = BackPackIdxByCell(m_selectedItem);
		if (bpIdx != -1) {
			return m_pBackPack->At(bpIdx);
		} else
			return INVART;
	}
	sint32 CellByPos(const iPoint& pos) const;
	virtual void OnCompose();
	virtual void ListPageUp();
	virtual void ListPageDown();
	
	virtual void Highlight(iArtClickItem* pSource, uint16 artId){};
	// SHL: wrong Highlight? See ancestor.
	inline void Highlight(uint16 artId) {};
	virtual void Unhighlight() {}

	virtual iArtCell Remove(sint32 cell) { return iArtBackPackCtrl::Remove(cell); }
	virtual iArtCell Replace(sint32 cell, const iArtCell& newArt) { return iArtBackPackCtrl::Replace(cell, newArt); }

	virtual void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) {}
	virtual void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	virtual void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
private:
	void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	
private:
	sint8		m_lastClickedItem;
};

#endif //PHEROES_GAME_ARTIFACTLIST_TOUCH_H_
