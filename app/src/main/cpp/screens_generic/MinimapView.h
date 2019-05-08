#ifndef PHEROES_GAME_MINIMAPVIEW_H_
#define PHEROES_GAME_MINIMAPVIEW_H_

const sint32 ZOOM_MIN = 1;
const sint32 ZOOM_MAX = 4;

class iMinimapRawView : public iView
{
public:
	iMinimapRawView(iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, bool bSmall = false);

	void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
	
	void OnMouseEntered(const iPoint& pos, MouseId mID) { 
#if defined( OS_MACOS )		
		m_bEntered = true; 
#endif	
	}
	void OnMouseLeave(const iPoint& pos, MouseId mID) { 
#if defined( OS_MACOS )		
		m_bEntered = false; 
#endif	
	}
	
	void OnPinchOut(sint32 length);
	
	void PrepareSurface();
    void UpdateSize();

	void SetCenterCell(const iPoint& pos, bool bInformHandler = true);
	iPoint GetCenterCell() const { return m_CenterCell; }
	
	void OnZoomChanged();
	sint32 GetScale() const	{ return m_Scale; }
	inline void SetZoom(sint32 nScale) { m_Scale = iCLAMP<sint32>(ZOOM_MIN, ZOOM_MAX, nScale); }
	inline void ZoomIn() { m_Scale = iMIN<sint32>(m_Scale+1,ZOOM_MAX); OnZoomChanged(); }
	inline void ZoomOut() { m_Scale = iMAX<sint32>(m_Scale-1,ZOOM_MIN); OnZoomChanged(); }

	inline iPoint Screen2Map(const iPoint& pos) const
	{
		sint32 mx = ((pos.x-(sint32)(m_Rect.w/2))/m_Scale+pos.y/m_Scale)/2;
		sint32 my = pos.y/m_Scale-mx;
		return iPoint(mx,my);
	}

	inline iPoint Map2Screen(const iPoint& pos) const
	{ return iPoint((-pos.y+pos.x)*m_Scale+m_Rect.w/2,(pos.x+pos.y)*m_Scale); }

	inline iPoint GetOffset() const
	{ return iPoint(m_Rect.w/2,m_Rect.h/2) - Map2Screen(m_CenterCell); }

	void OnCompose();

private:
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);

private:
	sint32			m_Scale;
	iBuff<uint16>	m_Surf;
	iPoint			m_CenterCell;
	iPoint			m_CenterPos;
	iPoint			m_TrackCell;
	iPoint			m_TrackPos;
	bool			m_tracked;
	IViewCmdHandler *m_pHandler;
	bool			m_bSmall;
#if defined( OS_MACOS )
	bool			m_bEntered;
#endif	
};

class iMinimapView: public iChildGameView
{
public:
	iMinimapView();
	virtual CHILD_VIEW Type() const { return CV_MINIMAP; }
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void SetCenterCell( const iPoint& cell );
	
	virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
	void SetZoom(sint32 nScale) { m_pMRView->SetZoom(nScale); }
private:
	iMinimapRawView *m_pMRView;
};

/*
 *	Minimap Toolbar 
 */
class iMinimapToolBar : public iView
{
public:
	iMinimapToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect);
	void OnCompose();
    void UpdateSize();
};


#endif //PHEROES_GAME_MINIMAPVIEW_H_
