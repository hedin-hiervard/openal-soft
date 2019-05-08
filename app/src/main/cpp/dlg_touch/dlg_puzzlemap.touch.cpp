#include "stdafx.h"
#include "Dlg_PuzzleMap.touch.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"

iPuzzleMapDlg::iPuzzleMapDlg(iViewMgr* pViewMgr, const iPlayer& player, uint16 newOpened)
: iDialog(pViewMgr), m_player(player), m_newOpened(newOpened), m_aniTimer( (newOpened)?255:0 )
{
}

uint32 PUZZLE_SIZE_X;
uint32 PUZZLE_SIZE_Y;

iPoint anchor, offset;

void iPuzzleMapDlg::PrepareMap()
{
    iSize sz = gGfxMgr.Dimension(PDGG(PUZZLEMAP_BACK));
    iRect clRect = AlignRect(sz, GetScrRect(), AlignCenter);
    
	
	/* init the source dib (tmpdib, 2x of the required size) and m_dibMap of the required size */
	iDib tmpDib(clRect.size() * 2, iDib::RGB);
	m_dibMap.Init(clRect.size(), iDib::RGB);	

	iIsoMetric im(im_coef);
	iRandomizer r;

	/* the random seed must be the same for each Ultimate pos */
	r.SetNewSeed(gGame.Map().GetHeight() * 1000 * gGame.Map().UltimatePos().x);

	/* anchor is the offset of the top left corner for the map composer */

	/* set the top left corner the Ultimate pos */
	anchor = im.Map2Screen(gGame.Map().UltimatePos());

	/* move halfway the X coord right */
	anchor.x += gGame.MainView()->Composer().MapPixW() / 2;
	//anchor.y += gGame.MainView()->Composer().MapPixH() / 2;
	
	/* offset is the random offset of the X from the center within the DIB borders, depends only on the Ult pos */
	offset = iPoint(r.Rand(tmpDib.GetWidth()), r.Rand(tmpDib.GetHeight()));
	anchor.x += -(sint32)tmpDib.GetWidth()  + offset.x;
	anchor.y += -(sint32)tmpDib.GetHeight() + offset.y;

	/* compose the map */
	iOLComposer::Compose(tmpDib, anchor, true, gGame.Map().UltimatePos());
    
	/* make the map BW */
    tmpDib.DarkenBWRect(tmpDib.GetSize());

    /* overdraw the X with green color */
	iPoint offset = iPoint(gGame.MainView()->Composer().MapPixW() / 2 + 20 + 40, 40) - anchor;
    uint32 psid = PDGG(TRACK);
    iPoint opos = im.Map2Screen(gGame.Map().UltimatePos()) + offset + iPoint(20, 10);
    gGfxMgr.BlitEffect( psid, tmpDib, opos, iGfxManager::EfxShadow2D);
    gGfxMgr.Blit( psid, tmpDib, opos);       

	/* resize the dib to the working size */
	iDibTransform::PyramidalStretch(tmpDib, m_dibMap, 1);

	// Prepare puzzled cell dib
	m_puzzledCell.Init(iSize(PUZZLE_SIZE_X,PUZZLE_SIZE_Y), iDib::RGB);
	m_puzzledCell.FrameRect(m_puzzledCell.GetSize(), cColor_Black);
	gGfxMgr.BlitTile(PDGG(BKTILE),m_puzzledCell,iRect(1,1,PUZZLE_SIZE_X-2,PUZZLE_SIZE_Y-2));
	m_puzzledCell.Darken25Rect(iRect(1,1,PUZZLE_SIZE_X-2,PUZZLE_SIZE_Y-2));
	ButtonFrame(m_puzzledCell, iRect(1,1,PUZZLE_SIZE_X-2,PUZZLE_SIZE_Y-2), 0);

}

void iPuzzleMapDlg::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	iRect clRect = GetScrRect();
	PUZZLE_SIZE_X = clRect.w / 9;
	PUZZLE_SIZE_Y = clRect.h / 5;
	//AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
	PrepareMap();
#if defined( OS_MACOS )
	m_mask.Init(iSize( GetScreenWidth(), GetScreenHeight() ), iDib::RGB);
#elif defined( OS_WIN32 )
	m_mask.Init(iSize( GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) ), iDib::RGB);
#else
	m_mask.Init( clRect.size() , iDib::RGB);
#endif
	gGfxMgr.Blit(PDGG(PUZZLEMAP_BACK), m_mask, clRect.point());
	if (m_newOpened) m_pMgr->SetTimer(500, 1, this);	
	Invalidate();
}

#if defined( PC_VERSION )
void iPuzzleMapDlg::UpdateSize(){

	iRect clRect = GetScrRect();


}

iRect iPuzzleMapDlg::GetScrRect(){

#if defined( OS_WIN32 )
	HWND hWnd = gApp.Window().GetHWND();
	RECT rect;

	GetWindowRect( hWnd, &rect );

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if( gApp.IsFullScreen() )
		return iRect( (GetSystemMetrics(SM_CXSCREEN)-width)/2+(width-1024)/2, (GetSystemMetrics(SM_CYSCREEN)-height)/2+(height-768)/2, 1024,768);
	else
		return iRect( (width-1040)/2, (height-806)/2, 1024,768);
#else
	return iRect( (GetWindowWidth()-1024)/2, (GetWindowHeight()-768)/2, 1024, 768 );
#endif
}
#endif

void iPuzzleMapDlg::OnTimer(uint32 tid)
{
	if (m_aniTimer > 32) {
		m_aniTimer -= 32;
		m_pMgr->SetTimer(100, 1, this);
	} else m_aniTimer = 0;
	Invalidate();
}

iPoint iPuzzleMapDlg::GetPuzzleCardCoord(uint32 idx)
{
	return iPoint(m_player.PuzzledCard(idx) % 9, m_player.PuzzledCard(idx) / 9);
}

iRect iPuzzleMapDlg::GetPuzzleCardRect(iPoint pt)
{
    iRect rc = GetScrRect();
	return iRect(rc.x + pt.x * rc.w / 9, rc.y + pt.y * rc.h / 5, PUZZLE_SIZE_X + 1, PUZZLE_SIZE_Y + 1);		
}

iRect iPuzzleMapDlg::GetPuzzleCardRectWithPoint(const iPoint& pt)
{
    iRect rc = GetScrRect();
    iPoint cd((pt.x - rc.x) / (rc.w / 9), (pt.y - rc.y) / (rc.h / 5));
	return GetPuzzleCardRect(cd);
}

void iPuzzleMapDlg::OnCompose()
{
    gApp.Surface().Fill(cColor_Black);
    
#ifdef PC_VERSION
	uint32 toffs = 20;
	iRect rect = GetScrRect();
	
   	gGfxMgr.Blit(PDGG(PUZZLEMAP_BACK), m_mask, rect.point());
    
    if((rect.w > toffs * 2 + 3) && (rect.h > toffs * 2 + 3) && rect.y > 3 && rect.x > 3) {
#if defined( OS_MACOS )
		gApp.Surface().Darken50Rect(iRect(0, 0, GetScreenWidth(), GetScreenHeight()));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetScreenWidth(), GetScreenHeight()));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetScreenWidth(), GetScreenHeight()));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetScreenWidth(), GetScreenHeight()));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetScreenWidth(), GetScreenHeight()));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetScreenWidth(), GetScreenHeight()));
#elif defined( OS_WIN32 )
		gApp.Surface().Darken50Rect(iRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
		gApp.Surface().Darken50Rect(iRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
#endif
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
#endif
    iSize sz = gGfxMgr.Dimension(PDGG(PUZZLEMAP_BACK));
	iRect rc = AlignRect(sz, GetScrRect(), AlignCenter);
    
	// Compose map
	m_dibMap.CopyToDibXY(gApp.Surface(), rc.point());

	// if not all obelisks found, mask the cross
	if(m_player.OpenedPuzzles() != m_player.TotalPuzzles()) {
		iIsoMetric im(im_coef);
		iPoint offset = iPoint(gGame.MainView()->Composer().MapPixW() / 2 + 20 + 40, 40) - anchor;
		iPoint opos = im.Map2Screen(gGame.Map().UltimatePos()) + offset;
		
		opos.x /= 2; opos.y /= 2;
		iRect mrc = GetPuzzleCardRectWithPoint(opos);
		m_mask.CopyRectToDibXY(gApp.Surface(), mrc, mrc.point() + rc.point(), 255);
	}

	// Compose puzzles
	for (uint32 xx=m_player.OpenedPuzzles(); xx<45; ++xx ) {
		iRect mrc = GetPuzzleCardRect(GetPuzzleCardCoord(xx));
		m_mask.CopyRectToDibXY(gApp.Surface(), mrc, mrc.point() + rc.point(), 255);
	}

	if (m_aniTimer > 0) {
		for (uint32 xx=m_player.OpenedPuzzles()-m_newOpened; xx<m_player.OpenedPuzzles(); ++xx ) {
			iRect mrc = GetPuzzleCardRect(GetPuzzleCardCoord(xx));
			m_mask.CopyRectToDibXY(gApp.Surface(), mrc, mrc.point() + rc.point(), m_aniTimer);
		}
		
		// reveal the last part of the puzzle
		if(m_player.OpenedPuzzles() == m_player.TotalPuzzles()) {
			//iIsoMetric im(im_coef);
			sint32 xp = 8 - (offset.x / 2) / (PUZZLE_SIZE_X - 1);
			sint32 yp = 4 - (offset.y / 2) / (PUZZLE_SIZE_Y - 1);
			iRect mrc = GetPuzzleCardRect(iPoint(xp, yp));
			m_mask.CopyRectToDibXY(gApp.Surface(), mrc, mrc.point() + rc.point(), m_aniTimer);
		}
	}
}

void iPuzzleMapDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	{
		m_pMgr->CleanupTimers(this);
		EndDialog(uid);
	}
}

void iPuzzleMapDlg::OnMouseClick(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	m_pMgr->CleanupTimers(this);
	EndDialog(0);	
}

iSize iPuzzleMapDlg::GetDlgMetrics() const
{
	return gApp.Surface().GetSize();	
}


