#include "stdafx.h"
#include "GameViewInterface.h"
#include "TransitionView.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"
/*
 *  Generic GameView
 */
iChildGameView::iChildGameView(bool bEternal, CHILD_VIEW parentView) : iTopmostView(&gApp.ViewMgr()), m_bEternal(bEternal), m_parentView(parentView) {}

void iChildGameView::OnCompose(){
    
    //	if( bFirstDarken ){
    
    //		bFirstDarken = false;
    //	}
    
    uint32 toffs = 20;
    iRect rect(m_Rect);
    
    if((rect.w > toffs * 2 + 3) && (rect.h > toffs * 2 + 3) && rect.y > 3 && rect.x > 3) {
        gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        
        // top/bottom tiles
        gGfxMgr.BlitTile(PDGG(DLG_HTILES), gApp.Surface(), iRect(rect.x+toffs,rect.y-3,rect.w-toffs*2,3));
        gGfxMgr.BlitTile(PDGG(DLG_HTILES), gApp.Surface(), iRect(rect.x+toffs,rect.y2()+1,rect.w-toffs*2, 3));
        
        // left/right tiles
        gGfxMgr.BlitTile(PDGG(DLG_VTILES), gApp.Surface(), iRect(rect.x-3,rect.y+toffs,3, rect.h - toffs*2));
        gGfxMgr.BlitTile(PDGG(DLG_VTILES), gApp.Surface(), iRect(rect.x2()+1,rect.y+toffs,3, rect.h - toffs*2));
        
        // corners
        //if (bDecs){
        //        gGfxMgr.Blit(PDGG(DLG_CORNERS)+0, gApp.Surface(), iPoint(rect.x-8,rect.y-8));
        //        gGfxMgr.Blit(PDGG(DLG_CORNERS)+1, gApp.Surface(), iPoint(rect.x2()-20,rect.y-8));
        //        gGfxMgr.Blit(PDGG(DLG_CORNERS)+2, gApp.Surface(), iPoint(rect.x-8,rect.y2()-20));
        //        gGfxMgr.Blit(PDGG(DLG_CORNERS)+3, gApp.Surface(), iPoint(rect.x2()-20,rect.y2()-20));
    }
    
    DoCompose();
}

void iChildGameView::AfterCompose()
{
    if( gGame.ActViewType() == iChildGameView::CV_CASTLE )
        gGame.MainView()->ResourceBar()->OnCompose();
}

void iChildGameView::FallDown()
{
#if defined(OS_IPHONE) || defined(OS_ANDROID)
    iDib dib(gApp.Surface().GetSize(), iDib::RGB);
    iRect rc = GetScrRect();
    SetRect(rc);
    UpdateSize();
    Compose(rc);
    
    gApp.Surface().CopyToDibXY(dib, iPoint());
    
    iTransitionView *trView = new iTransitionView(&gApp.ViewMgr(), iTransitionView::TVM_VRT_OVERLAY,
                                                  iTransitionView::MM_AUTO, dib, gGame.PeekScreen());
    sint32 res = trView->DoModal();
    delete trView;
    if(res == iTransitionView::TVR_UP)
        gGame.HideViewDelayed(Type());
    Invalidate();	
#endif
}