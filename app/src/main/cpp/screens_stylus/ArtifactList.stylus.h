#ifndef PHEROES_GAME_ARTIFACTLIST_STYLUS_H_
#define PHEROES_GAME_ARTIFACTLIST_STYLUS_H_
#pragma once

// Abstract artifacts Drag'n'Drop item
class iArtDragDropItem : public iBaseCtrl
{
public:
	struct iDragItem : public iViewMgr::IDragGlyph {
		iDragItem(iArtDragDropItem* pSender, sint32 fromCell,const iArtCell& artCell, const iPoint& pos, const iSize& siz);
		void ComposeDragGlyph();

		iSize				m_siz;
		iPoint				m_pos;
		iArtDragDropItem*	m_pSender;
		sint32				m_fromCell;
		iArtDragDropItem*	m_pReceiver;
		sint32				m_toCell;
		iArtCell			m_artCell;
	};
	
public:
	// c-tor, d-tor
	iArtDragDropItem(iSimpleArray<iArtDragDropItem*>& competitors, const iSize& cellSiz, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid);
	virtual ~iArtDragDropItem() {}

	// Drag'n'Drop
	void BeginDrag(sint32 fromCell, const iArtCell& artCell, const iPoint& pos);
	void EndDrag();
	void Drag(const iPoint& pos);
	virtual void EnterDrop(iDragItem* pDragItem) =0;
	virtual void LeaveDrop()  =0;
	virtual bool Drop()  =0;
	virtual bool DragDrop() const  =0;
	virtual void Highlight(iDragItem *dr) =0;
	virtual void Unhighlight() =0;

	virtual iArtCell Remove(sint32 cell)  =0;
	virtual iArtCell Replace(sint32 cell, const iArtCell& newArt)  =0;

protected:
	iSimpleArray<iArtDragDropItem*>&	m_Competitors;
	iDragItem*							m_pDragItem;
	iDragItem*							m_pDropItem;
	iSize								m_cellSiz;
};


// Artifact item control for stylus interface
class iArtItemCtrl_stylus : public iArtDragDropItem, public iArtItemCtrl
{
public:
	//
	iArtItemCtrl_stylus(iSimpleArray<iArtDragDropItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 uid);
	// 
	//	
	void Highlight(iDragItem *dr);
	void EnterDrop(iDragItem* pDragItem);
	void LeaveDrop();

	bool Drop();
	bool DragDrop() const;	
	
	virtual iArtCell Remove(sint32 cell)  { return iArtItemCtrl::Remove(cell); }
	virtual iArtCell Replace(sint32 cell, const iArtCell& newArt)  { return iArtItemCtrl::Replace(cell, newArt); }


	//virtual sint32 CellByPos(const iPoint& pos) const;
	virtual void OnCompose();
	void Unhighlight();
	//virtual void Unhighlight() { iArtItemCtrl::Unhighlight(); }
	virtual void OnMouseDown(const iPoint& pos, MouseId mID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID);
	virtual void OnMouseClick(const iPoint& pos, MouseId mID);
};

class iArtBackPackCtrl_stylus : public iArtDragDropItem, public iArtBackPackCtrl
{
public:
	//
	iArtBackPackCtrl_stylus(iSimpleArray<iArtDragDropItem*>& competitors, iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, const iSize& cellSiz, uint32 cellsCount, uint32 flags, uint32 uid);

	//
	void EnterDrop(iDragItem* pDragItem);
	void LeaveDrop();
	bool Drop();
	bool DragDrop() const;
	inline void Highlight(iDragItem *dr) {};
	
	virtual iArtCell Remove(sint32 cell)  { return iArtBackPackCtrl::Remove(cell); }
	virtual iArtCell Replace(sint32 cell, const iArtCell& newArt)  { return iArtBackPackCtrl::Replace(cell, newArt); }

	virtual void Unhighlight() { iArtBackPackCtrl::Unhighlight(); }

	virtual sint32 CellByPos(const iPoint& pos) const;
	virtual void OnCompose();
	virtual void OnMouseDown(const iPoint& pos, MouseId mID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID);
	virtual void OnMouseClick(const iPoint& pos, MouseId mID);
};

#endif //PHEROES_GAME_ARTIFACTLIST_STYLUS_H_
