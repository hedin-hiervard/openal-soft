#include "stdafx.h"
#include "MinimapView.h"

iMinimapRawView::iMinimapRawView(iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid, bool bSmall)
: iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled), m_pHandler(pHandler), m_CenterCell(0, 0), m_bSmall( bSmall )
#if defined( OS_MACOS )
, m_bEntered( false )
#endif
{
	SetZoom(3);
	PrepareSurface();	
}

void iMinimapRawView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	m_TrackCell = m_CenterCell;
	m_TrackPos = pos;
	m_tracked = false;
//	SetCenterCell(Screen2Map(pos - GetOffset()));	
}

void iMinimapRawView::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
#if defined( OS_MACOS )
	m_bEntered = false;
#endif	
	if(!m_tracked) {
#if defined( PC_VERSION )
		iPoint pos_center = iPoint();

#if defined( OS_MACOS )	
		int width = GetWindowWidth();
		int height = GetWindowHeight();
#else
		HWND hWnd = gApp.Window().GetHWND();
		RECT rect;

		GetWindowRect( hWnd, &rect );

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
#endif

		if( !m_bSmall ){

			pos_center = iPoint( width/2, height/2 );
		}
		else{
			pos_center = iPoint( width - m_Rect.size().w/2, height - m_Rect.size().h/2 );
		}
		
		iPoint tmp = pos_center - pos + iPoint(m_Rect.w / 2, 0);
		SetCenterCell(m_CenterCell - Screen2Map(tmp));
#endif
		//gGame.HideViewDelayed(CV_MINIMAP);
		//Invalidate();
		return;
	}
	Invalidate();
}

void iMinimapRawView::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	iPoint tmp = m_TrackPos - pos + iPoint(m_Rect.w / 2, 0);
#ifndef PC_VERSION	
	SetCenterCell(m_TrackCell + Screen2Map(tmp));
#else
	SetCenterCell(m_TrackCell - Screen2Map(tmp));
#endif	
	m_tracked = true;
	Invalidate();
}

void iMinimapRawView::SetCenterCell(const iPoint& pos, bool bInformHandler)
{ 
	m_CenterCell.x = iCLAMP<sint32>(0, gGame.Map().GetWidth()-1, pos.x);
	m_CenterCell.y = iCLAMP<sint32>(0, gGame.Map().GetHeight()-1, pos.y);
	m_CenterPos = pos;
	if(bInformHandler)
		m_pHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, 0);
}

void iMinimapRawView::OnZoomChanged()
{
	iView* pTB = GetChildById(VPUID_MMAPTOOLBARVIEW);
	check(pTB);
	pTB->GetChildById(VPUID_BTN_ZOOMIN)->SetEnabled(m_Scale < ZOOM_MAX);
	pTB->GetChildById(VPUID_BTN_ZOOMOUT)->SetEnabled(m_Scale > ZOOM_MIN);
	Invalidate();
}

void iMinimapRawView::PrepareSurface()
{
	m_Surf.Allocate(gGame.Map().GetWidth()*gGame.Map().GetHeight());
	uint16 *surfPtr = m_Surf.GetPtr();

	for (sint32 yy=0; yy<gGame.Map().GetHeight(); ++yy){
		for (sint32 xx=0; xx<gGame.Map().GetWidth(); ++xx){
			iCell cell = gGame.Map().GetAt(xx,yy);
			*surfPtr = (uint16)((cell.avatar)?Darken25(SURF_COLOR[cell.SurfNode(1)]):SURF_COLOR[cell.SurfNode(1)]);
			surfPtr++;
		}
	}
}

void iMinimapRawView::OnCompose()
{
	iRect rc = GetScrRect();
	gApp.Surface().FillRect(rc,cColor_Black);

	iPoint anchor = GetOffset();
	const sint32 const_x = rc.x+((sint32)(rc.w/2)) + anchor.x;
	const sint32 const_y = rc.y + anchor.y;
	sint32 base_x = const_x;
	sint32 base_y = const_y;
	sint32 pos_x=base_x;
	sint32 pos_y=base_y;

	uint16 *surfPtr = m_Surf.GetPtr();
	for (sint32 yy=0; yy<gGame.Map().GetHeight(); ++yy) {
		for (sint32 xx=0; xx<gGame.Map().GetWidth(); ++xx) {
			iRect orc(pos_x,pos_y,m_Scale*2,m_Scale);
			iRect tstrc(rc);
			tstrc.DeflateRect(5);
			if (iClipper::IsIntersectRect(tstrc,orc)){
				uint8 dcfg,fcfg;
				gGame.Map().ActPlayer()->FogMap().GetFogCfg(iPoint(xx,yy),dcfg,fcfg);
				if (dcfg) {
					gApp.Surface().FillRect(orc,cColor_Black);
				} else {
					gApp.Surface().FillRect(orc,*surfPtr);
					if (gGame.Map().GetAt(xx,yy).avatar != 0xFFFF) {
						if (gGame.Map().m_PassMap.GetAt(xx,yy) == -1) gApp.Surface().FillRect(orc,cColor_Black,128);
						else gApp.Surface().FillRect(orc,cColor_Black,32);
					}
					if (fcfg) {
						gApp.Surface().FillRect(orc,cColor_Black,64);
					} else if (iBaseMapObject* pObj = gGame.Map().m_CoverMap.GetAt(xx,yy)){
						if (iOwnCnst* pOwnCnst = DynamicCast<iOwnCnst*>(pObj)) {
							iDib::pixel clr = (pOwnCnst->Owner() == PID_NEUTRAL)?cColor_Gray192:PLAYER_COLORS[pOwnCnst->Owner()];
							gApp.Surface().FillRect(orc,clr);
						} else if (iCastle* pCastle = DynamicCast<iCastle*>(pObj)) {
							iDib::pixel clr = (pCastle->Owner() == PID_NEUTRAL)?cColor_Gray192:PLAYER_COLORS[pCastle->Owner()];
							gApp.Surface().FillRect(orc,clr);
						}
					}
				}
			}
			pos_x+=m_Scale;
			pos_y+=m_Scale;
			surfPtr++;

		}
		
		base_x-=m_Scale;
		base_y+=m_Scale;
		pos_x = base_x;
		pos_y = base_y;
	}

	
	/// Heroes
	for (iGameWorld::iPLIt pit = gGame.Map().m_Players.First(); pit != gGame.Map().m_Players.End(); ++pit){
		for (iPlayer::iHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit){
			if (!gGame.Map().ActPlayer()->FogMap().IsHidden((*hit)->Pos())) {
				sint32 px = const_x + ((*hit)->Pos().x - (*hit)->Pos().y)*m_Scale;
				sint32 py = const_y + ((*hit)->Pos().x + (*hit)->Pos().y)*m_Scale;
				if (rc.PtInRect(iPoint(px,py))){
					gApp.Surface().FillRect(iRect(px,py,m_Scale,m_Scale),PLAYER_COLORS[(*hit)->Owner()->PlayerId()]);
				}
			}
		}
	}
	// current area rect
	iPoint tpoint = Map2Screen(m_CenterCell);
	iRect orc(tpoint.x+anchor.x+rc.x-8*m_Scale,tpoint.y+anchor.y+rc.y-9*m_Scale,16*m_Scale,18*m_Scale);
	gApp.Surface().FrameRect(orc, cColor_Yello);
#if defined( OS_MACOS )
	if( m_bEntered ){
/*	
		if( text.Length() ){
		
			iSize sz = gTextComposer.GetTextSize(text, btnfc_normal);
			iSize szcntx_l = gGfxMgr.Dimension(PDGG(CONTEXT_L));

			iRect pos = iRect(iPoint(1600 - 214, 900 - 137), iSize(214, 137));
						
			iRect cntx = iRect( pos.x - 2*szcntx_l.w - sz.w, pos.y - szcntx_l.h, szcntx_l.w, szcntx_l.h );
			gGfxMgr.Blit(PDGG(CONTEXT_L), gApp.Surface(), cntx);
			
			cntx = iRect( pos.x - szcntx_l.w - sz.w, pos.y - szcntx_l.h, sz.w, szcntx_l.h );		
			gGfxMgr.BlitTile(PDGG(CONTEXT_M), gApp.Surface(), cntx);
			
			cntx = iRect( pos.x - szcntx_l.w, pos.y - szcntx_l.h, szcntx_l.w, szcntx_l.h );		
			gGfxMgr.Blit(PDGG(CONTEXT_R), gApp.Surface(), cntx);
			
			iPoint txtpnt = iPoint(pos.x - szcntx_l.w - sz.w, pos.y - szcntx_l.h+(szcntx_l.h - sz.h)/2);
			
			gTextComposer.TextOut(btnfc_normal, gApp.Display().GetSurface(), txtpnt, text, cInvalidRect, AlignLeft);
		}
*/
	}
#endif
} 



void iMinimapRawView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	/*if (uid == VPUID_BTN_ZOOMIN) ZoomIn();
	else if (uid == VPUID_BTN_ZOOMOUT) ZoomOut();
	else if (uid == VPUID_BTN_CLOSEMMAP) gGame.HideView(CV_MINIMAP);
	*/
	
}

void iMinimapRawView::OnPinchOut(sint32 length)
{
//	if(length > 80)
//		gGame.HideView(CV_MINIMAP);
}

void iMinimapRawView::UpdateSize()
{       
    iView *p;  
  /*  iRect Rect = m_pMgr->Metrics();  
    SetRect(Rect);
   */
    if (p = GetChildByIndex(0)) // toolbar
    {
        p->SetRect(iRect(m_Rect.x,m_Rect.y2()-20,m_Rect.w,21));
        p->UpdateSize();
        p->Invalidate();
    }    
}

iMinimapView::iMinimapView() : iChildGameView(false, CV_OVERLAND)
{
	AddChild(m_pMRView = new iMinimapRawView(&gApp.ViewMgr(), this, m_Rect, 100 ) );//gTextMgr[TRID_HOVER_TOGGLE_MINIMAP]));
}


void iMinimapView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{

}

void iMinimapView::SetCenterCell( const iPoint& cell )
{
	m_pMRView->SetCenterCell(cell);
}


/*
 *	Minimap Toolbar 
 */
iMinimapToolBar::iMinimapToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect)
: iView(pViewMgr, rect, GENERIC_VIEWPORT,VPUID_MMAPTOOLBARVIEW, Enabled|Visible)
{
	AddChild(new iIconButton(pViewMgr, pCmdHandler, iRect(0,1,20,20),PDGG(BTN_PLUS),VPUID_BTN_ZOOMIN));
	AddChild(new iIconButton(pViewMgr, pCmdHandler, iRect(20,1,20,20),PDGG(BTN_MINUS),VPUID_BTN_ZOOMOUT));
	AddChild(new iIconButton(pViewMgr,pCmdHandler, iRect(rect.w-30,1,30,20), PDGG(BTN_CLOSE), VPUID_BTN_CLOSEMMAP));
}

void iMinimapToolBar::OnCompose()
{
	iRect rect = GetScrRect();
	gApp.Surface().FillRect(rect,cColor_Black);

	iRect rc(rect.x+41,rect.y+1,rect.w-31-41,20);
	gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rc);
	ButtonFrame(gApp.Surface(), rc, 0);

	//ComposeTextButton(gApp.Surface(), rc, 0, TRID_VOID);
}

void iMinimapToolBar::UpdateSize()
{
    iView *p;  

    if (p = GetChildByIndex(2)) // button2, other buttons are static
    {
        p->SetRect(iRect(GetSize().w - 30, 1, 30, 20));
        p->Invalidate();
    }     
}
