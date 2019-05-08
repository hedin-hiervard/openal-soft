#include "stdafx.h"
#include "Dlg_PuzzleMap.h"
#include "OverlandView.h"

iPuzzleMapDlg::iPuzzleMapDlg(iViewMgr* pViewMgr, const iPlayer& player, uint16 newOpened)
: iBaseGameDlg(pViewMgr, player.PlayerId()), m_player(player), m_newOpened(newOpened), m_aniTimer( (newOpened)?255:0 )
{
}

#define PUZZLE_SIZE_X 29
#define PUZZLE_SIZE_Y 29

iPoint anchor, offset;

void iPuzzleMapDlg::PrepareMap()
{
	iDib tmpDib(iSize(502,278), iDib::RGB);
	m_dibMap.Init(iSize(251,139), iDib::RGB);
	iIsoMetric im(5);
	iRandomizer r;
	r.SetNewSeed(gGame.Map().GetHeight() * 1000 * gGame.Map().UltimatePos().x);

	anchor = im.Map2Screen(gGame.Map().UltimatePos());
	anchor.x += gGame.Map().GetWidth() * 20 + 40;
	anchor.y += 50;
	offset = iPoint(r.Rand(tmpDib.GetWidth()), r.Rand(tmpDib.GetHeight()));
	anchor.x += -(sint32)tmpDib.GetWidth()  + offset.x;
	anchor.y += -(sint32)tmpDib.GetHeight() + offset.y;

	iOLComposer::Compose(tmpDib, anchor, true, gGame.Map().UltimatePos());
    
    tmpDib.DarkenBWRect(tmpDib.GetSize());

    // make point with color
    // copied from iOLComposer::Compose()    
    iPoint offset = iPoint((gGame.Map().GetWidth() * 40)/2+20,40) - anchor;
    uint32 psid = PDGG_TRACK;
    iPoint opos = im.Map2Screen(gGame.Map().UltimatePos()) + offset;
    gGfxMgr.BlitEffect( psid, tmpDib, opos, iGfxManager::EfxShadow2D);
    gGfxMgr.Blit( psid, tmpDib, opos);       

	iDibTransform::PyramidalStretch(tmpDib, m_dibMap, 1);

	// Prepare puzzled cell dib
	m_puzzledCell.Init(iSize(PUZZLE_SIZE_X,PUZZLE_SIZE_Y), iDib::RGB);
	m_puzzledCell.FrameRect(m_puzzledCell.GetSize(), cColor_Black);
	gGfxMgr.BlitTile(PDGG_BKTILE,m_puzzledCell,iRect(1,1,PUZZLE_SIZE_X-2,PUZZLE_SIZE_Y-2));
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
	//AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
	PrepareMap();
	if (m_newOpened) m_pMgr->SetTimer(500, 1, this);	
	Invalidate();
}

void iPuzzleMapDlg::OnTimer(uint32 tid)
{
	if (m_aniTimer > 32) {
		m_aniTimer -= 32;
		m_pMgr->SetTimer(100, 1, this);
	} else m_aniTimer = 0;
	Invalidate();
}

void iPuzzleMapDlg::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
		
	iRect orc(rc.x + (rc.w/2-m_dibMap.GetWidth()/2)-2, rc.y+20, m_dibMap.GetWidth()+4, m_dibMap.GetHeight()+4);
	gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(), rc, gTextMgr[TRID_PUZZLE_MAP], rc, AlignTop);
		
	ButtonFrame(gApp.Surface(), orc, iButton::Pressed);
	orc.DeflateRect(1);
	gApp.Surface().FrameRect(orc, cColor_Black);
		
	// Compose map
	m_dibMap.CopyToDibXY(gApp.Surface(), iPoint(orc.x+1, orc.y+1));


	// if not all obelisks found, mask the cross
	if(m_player.OpenedPuzzles() != m_player.TotalPuzzles()) {
		iIsoMetric im(5);
		sint32 xp = 8 - (offset.x / 2) / (PUZZLE_SIZE_X - 1);
		sint32 yp = 4 - (offset.y / 2) / (PUZZLE_SIZE_Y - 1);
		m_puzzledCell.CopyToDibXY(gApp.Surface(), iPoint(orc.x+xp*(PUZZLE_SIZE_X-1),orc.y+yp*(PUZZLE_SIZE_Y-1)));
	}

	// Compose puzzles
	for (uint32 xx=m_player.OpenedPuzzles(); xx<45; ++xx ) {
		sint32 xp = m_player.PuzzledCard(xx) % 9;
		sint32 yp = m_player.PuzzledCard(xx) / 9;
		m_puzzledCell.CopyToDibXY(gApp.Surface(), iPoint(orc.x+xp*(PUZZLE_SIZE_X-1),orc.y+yp*(PUZZLE_SIZE_Y-1)));
	}

	if (m_aniTimer > 0) {
		for (uint32 xx=m_player.OpenedPuzzles()-m_newOpened; xx<m_player.OpenedPuzzles(); ++xx ) {
			sint32 xp = m_player.PuzzledCard(xx) % 9;
			sint32 yp = m_player.PuzzledCard(xx) / 9;
			m_puzzledCell.CopyRectToDibXY(gApp.Surface(), iRect(1,1,(PUZZLE_SIZE_X-2),(PUZZLE_SIZE_Y-2)), iPoint(orc.x+xp*(PUZZLE_SIZE_X-1)+1,orc.y+yp*(PUZZLE_SIZE_Y-1)+1), m_aniTimer);
		}
		
		// reveal the last part of the puzzle
		if(m_player.OpenedPuzzles() == m_player.TotalPuzzles()) {
			iIsoMetric im(5);
			sint32 xp = 8 - (offset.x / 2) / (PUZZLE_SIZE_X - 1);
			sint32 yp = 4 - (offset.y / 2) / (PUZZLE_SIZE_Y - 1);
			m_puzzledCell.CopyRectToDibXY(gApp.Surface(), iRect(1,1,(PUZZLE_SIZE_X-2),(PUZZLE_SIZE_Y-2)), iPoint(orc.x+xp*(PUZZLE_SIZE_X-1)+1,orc.y+yp*(PUZZLE_SIZE_Y-1)+1), m_aniTimer);
		}
	}
}

iSize iPuzzleMapDlg::ClientSize() const
{
	return iSize(280, 170 + DEF_BTN_HEIGHT);
}

void iPuzzleMapDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	{
		m_pMgr->CleanupTimers(this);
		EndDialog(uid);
	}
}

void iPuzzleMapDlg::OnMouseClick(const iPoint &pos, MouseId mID)
{
	m_pMgr->CleanupTimers(this);
	EndDialog(0);
	gGame.ShowView(iMainView::CV_OVERLAND);
}

