#ifndef PHEROES_GAME_ARMYLIST_STYLUS_H_
#define PHEROES_GAME_ARMYLIST_STYLUS_H_
#pragma once

class iArmyListEx_stylus: public iArmyListEx
{
public:
	struct iDragItem : public iViewMgr::IDragGlyph {
		iDragItem(iArmyListEx_stylus* pSender, sint8 fromCell, bool bSplit, const iCreatGroup& cGroup, const iPoint& pos, const iSize& siz, bool bCanDismiss);
		void ComposeDragGlyph();
		
		bool			m_bCanDismiss;
		bool			m_bSplit;
		iSize			m_siz;
		iArmyListEx_stylus*	m_pSender;
		sint8			m_fromCell;
		iArmyListEx_stylus*	m_pReceiver;
		sint8			m_toCell;
		iCreatGroup		m_cGroup;
		iPoint			m_pos;
	};

public:	
	iArmyListEx_stylus(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint& pos, iCheckButton* pSplitBtn, const iSize& itemSiz, uint32 uid)
	  : iArmyListEx(pViewMgr, pCmdHandler, pos, pSplitBtn, itemSiz, uid)
	  , m_pDragItem(NULL), m_pDropItem(NULL)
	{}
	virtual void OnCompose();
	virtual void OnMouseDown(const iPoint& pos, MouseId mID);
	virtual void OnMouseUp(const iPoint& pos, MouseId mID);
	virtual void OnMouseClick(const iPoint& pos, MouseId mID);
	virtual void OnMouseTrack(const iPoint& pos, MouseId mID);

	virtual void BeginDrag(sint8 fromCell, const iCreatGroup& cGroup, const iPoint& pos);
	virtual void EndDrag();
	virtual void Drag(const iPoint& pos);
	virtual void EnterDrop(iDragItem* pDragItem) ;
	virtual void LeaveDrop();
	virtual bool Drop();
	virtual bool DragDrop() const;

private:
	iPoint			m_dragAnchor;
	iDragItem*		m_pDragItem;
	iDragItem*		m_pDropItem;
};
	
#endif //PHEROES_GAME_ARMYLIST_STYLUS_H_
