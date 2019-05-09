#include "stdafx.h"
#include "OverlandView.h"
#include "OverlandView.touch.h"
#include "cm_ships.h"
#include "Composer.h"


#define SHIP_FLAG_ADD   iPoint(22, 17)

const sint32 SBUFF_ALIGN = 32;
const fix32 CELL_EVT_DELAY = fix32(3.0f);

iOLComposer::iOLComposer(const iRect& rc, iView* pOwner)
: m_Rect(rc), m_pOwner(pOwner), m_lastCompAnchor(cInvalidPoint), m_Anchor(0,0), m_pAniObj(NULL), m_ptTarget(cInvalidPoint), m_bSurveyMode(false)
, m_RedCross(cInvalidPoint), m_zoom(1)
{}

iOLComposer::~iOLComposer()
{ Cleanup(); }

bool iOLComposer::Init()
{
	return true;
}

void iOLComposer::Cleanup()
{
	m_ComposeList.Cleanup();
	if (m_pAniObj) {
		delete m_pAniObj;
		m_pAniObj = NULL;
	}
}

void iOLComposer::SetAnchor(const iPoint& pos) 
{ 
	iPoint ccell = GetCenterCell(pos);
	// Valid range is (-1,-1, MapWidth, MapHeight)
	if ((ccell.x >= -1 && ccell.y >= -1 && ccell.x <= gGame.Map().GetWidth() && ccell.y <= gGame.Map().GetHeight())) {

		m_Anchor = pos;
	}
	gGame.MainView()->OnComposerAnchorChanged();
	m_pOwner->Invalidate();
}

iPoint iOLComposer::GetCenterCell(const iPoint& anchor) const
{
	iIsoMetric im(im_coef);
	return im.Screen2Map(iPoint(anchor.x + m_Rect.w/2 - MapPixW()/2-20,anchor.y+m_Rect.h/2-30));
}

iPoint iOLComposer::GetCenterViewAnchor(const iPoint& cell, const iPoint& offset)
{
	iIsoMetric im(im_coef);
	iPoint cpos = im.Map2Screen(cell);
	cpos.x += MapPixW()/2+40;
	cpos.y += 40;
	cpos.x -= m_Rect.w/2;
	cpos.y -= m_Rect.h/2;

	if(m_zoom == 2)
		cpos += iPoint(120, 80);
	return cpos + offset;
}

iPoint iOLComposer::GetCenterCell() const
{
	return GetCenterCell(m_Anchor);
}

void iOLComposer::CenterView(const iPoint& cell, const iPoint& offset)
{
	SetAnchor(GetCenterViewAnchor(cell, offset));
}


bool iOLComposer::IsOnScreen(const iPoint& pos)
{
	iRect orc;
	orc = m_Rect;
	
	iPoint anchor = m_Anchor;//-iPoint(MapPixW()/2+20+40,40);;
//	iPoint offset = iPoint(MapPixW()/2+20,40) - m_Anchor + orc.point();
	
	iIsoMetric im(im_coef);
	
	// save cell under anchor point
	iPoint fpos = im.Screen2Map(anchor-iPoint(SBUFF_ALIGN,SBUFF_ALIGN));
	//
	// Surface
	iSize dsSiz(orc.w+SBUFF_ALIGN*2, orc.h+SBUFF_ALIGN*2);
	sint32 cnt_x = dsSiz.w/cell_w+3;
	sint32 cnt_y = dsSiz.h/cell_h+5;
	
	iRect src_rect(fpos, iSize(cnt_x, cnt_y));
	
	return src_rect.PtInRect(pos);
}

// Animation CB
void iOLComposer::AniDone()
{
	iAniObj* pAniObj = RemoveAniObj();
	if (iAniObj_MovHero* pHeroAni = DynamicCast<iAniObj_MovHero*>(pAniObj)) {
		// Need to center view with new hero's position (after step)
		pHeroAni->Hero()->Step();
	}
	
	delete pAniObj;
}

bool iOLComposer::StepAni(fix32 t)
{
	bool bRedraw = false;

	// animation
	if (m_pAniObj) {
		m_pAniObj->Process(t);
		if (iAniObj_MovHero* pHeroAni = DynamicCast<iAniObj_MovHero*>(m_pAniObj)) {
			//if(!IsOnScreen(pHeroAni->Hero()->Pos()))
//#ifndef ROYAL_BOUNTY
				CenterView(pHeroAni->Hero()->Pos(), pHeroAni->PtOffset());
//#endif
		}
		bRedraw = true;
	}

	// cell events
	iEvtIt it=m_cellEvents.First();
	while(it != m_cellEvents.End()) {
		if (!bRedraw) bRedraw = true;
		(*it)->m_time -= t;
		if ( (*it)->m_time < fix32::zero){
			it = m_cellEvents.Delete(it);
			continue;
		}
		(*it)->dstate = (uint8)iCLAMP<sint32>(0,255,((*it)->m_time * fix32(255.0f) / CELL_EVT_DELAY).ceil());
		++it;
	}

	return bRedraw;
}

void iOLComposer::SetAniObj(iAniObj* pObj)
{
	//check(pObj && !m_pAniObj); 
	m_pAniObj = pObj; 
}

iAniObj* iOLComposer::RemoveAniObj()
{
	check(m_pAniObj); 
	iAniObj* pRes = m_pAniObj; 
	m_pAniObj=NULL; 
	return pRes; 
}

void iOLComposer::AddCellMsg(const iStringT& msg, const iPoint& pos)
{
	m_cellEvents.Add(new iGameEvent(CELL_EVT_DELAY, msg, pos));
}

// iPoint   m_Anchor;
// iRect    m_Rect;

always_inline uint32 CalcCellIndex(const iPoint& pos)
{
	return (pos.x + pos.y)*10 + pos.x;
}


#if 0
// Cotulla debug code for flag positions
static void LoadShipFlagPos(char* fn, iPoint *an)
{
	FILE *f;
	uint32 i;

	f = fopen(fn, "rt");
	if (!f) return;

	// 40 items - 5 frames x 8 directions
	for (uint32 k = 0; k < 8; k++)
	{           
		fscanf(f, "%d %d\n", &an[k].x, &an[k].y);
	}

	fclose(f);

}

// helper function for art work Cotulla
static void LoadShipFlagsPos()
{
	LoadShipFlagPos("ship1_flag1.txt", SHIP1_FLAG1_ANCHOR);
	LoadShipFlagPos("ship1_flag2.txt", SHIP1_FLAG2_ANCHOR);
	LoadShipFlagPos("ship2_flag1.txt", SHIP2_FLAG1_ANCHOR);
	LoadShipFlagPos("ship2_flag2.txt", SHIP2_FLAG2_ANCHOR);
}

static void SaveShipFlagPos(char* fn, iPoint *an)
{
	FILE *f;
	uint32 i;

	f = fopen(fn, "wt");
	if (!f) return;

	// 40 items - 5 frames x 8 directions
	for (uint32 k = 0; k < 8; k++)
	{
		fprintf(f, "%d %d\n", an[k].x, an[k].y);         
	}

	fclose(f);

}

static void SaveShipFlagsPos()
{
	SaveShipFlagPos("ship1_flag1.txt", SHIP1_FLAG1_ANCHOR);
	SaveShipFlagPos("ship1_flag2.txt", SHIP1_FLAG1_ANCHOR);
	SaveShipFlagPos("ship2_flag1.txt", SHIP2_FLAG1_ANCHOR);
	SaveShipFlagPos("ship2_flag2.txt", SHIP2_FLAG2_ANCHOR);
}
#endif

void iOLComposer::Compose(iDib *targetDib, fix32 t)
{
	//gApp.Surface().Fill(0);
	//return;
	
	iTimer timer;

	iDib& outDib = targetDib != NULL ? (*targetDib):gApp.Surface(); 
	iRect orc;
	orc = m_Rect;

	iPoint anchor = m_Anchor-iPoint(MapPixW()/2+20+40,40);;
	iPoint offset = iPoint(MapPixW()/2+20,40) - m_Anchor + orc.point();

	iItemMgr& itemMgr = gGame.ItemMgr();

	iIsoMetric im(im_coef);

	// save cell under anchor point
	iPoint fpos = im.Screen2Map(anchor-iPoint(SBUFF_ALIGN,SBUFF_ALIGN));
	// offset cell
	fpos.Move(-1,-1);
	iPoint ppos = im.Map2Screen(fpos)+iPoint(SBUFF_ALIGN,SBUFF_ALIGN);
	ppos.Move(-40,0);
	iPoint offs = anchor-ppos;

	//
	// Surface
	iSize dsSiz(orc.w+SBUFF_ALIGN*2,orc.h+SBUFF_ALIGN*2 + 100);

	if (m_dibSurfLayer.GetSize() != dsSiz){
		m_lastCompAnchor = cInvalidPoint;
		m_dibSurfLayer.Resize(dsSiz);
	}

	sint32 cnt_x = dsSiz.w/cell_w + 2;
	sint32 cnt_y = dsSiz.h/cell_h + 4;

//	cnt_x;
//	cnt_y -= ;
	
	sint32 step_x = im.GetCellStepX()*2;
	sint32 step_y = im.GetCellStepY();

	const iPoint anchor_coor(fpos);
	iPoint init_map_coor(fpos);
	iPoint map_coor(fpos);


#if 1
	iDib& surfDib = outDib;
	//if (m_lastCompAnchor == cInvalidPoint || iDIF(anchor.x,m_lastCompAnchor.x) > SBUFF_ALIGN || iDIF(anchor.y,m_lastCompAnchor.y) > SBUFF_ALIGN) {
	{
		m_lastCompAnchor = anchor;
		//outDib.Fill(0);
		for (sint32 yy=0; yy<cnt_y; ++yy) {
			for (sint32 xx=0; xx<cnt_x; ++xx){
				if (gGame.Map().IsValidPos(map_coor.x,map_coor.y)){
					bool bHighlight = false;
					/*if(((iMainView_touch*)gGame.MainView())->IsHighlightedCell(iPoint(map_coor.x,map_coor.y))) { 
						bHighlight = true;
					}*/
					
//                    if (map_coor.x == 7 && map_coor.y == 6)
//                        _asm int 3
					sint32 xpos = xx*step_x;
					sint32 ypos = yy*step_y;
					if (yy%2!=0) xpos -= step_x >> 1;
					iPoint op(xpos-offs.x,ypos-offs.y);
					op += orc.point()-iPoint(SBUFF_ALIGN,SBUFF_ALIGN)-(anchor-m_lastCompAnchor);
					
					// Surf
					iGameMap::iSurfCell cell;
					uint32 cseq = CalcCellSeq(map_coor,4);
					if ( gGame.Map().GetCellSurf(map_coor, cell) ) {
						// solid cell - do nothing
						/*if(cell.lowestLayer == STYPE_WATER)						
							cseq = (cseq + (t / fix32(1.0f)).floor()) % 4;
							*/
						gGfxMgr.BlitMasked(PDGG(SURF_TILES) + cell.lowestLayer*4 + cseq, PDGG(TRANS_TILES)+14, surfDib,op);						
					} else {
						if (cell.lowestLayer == STYPE_WATER) {							
							
							uint8 cfg = 0;
							for (uint32 zz=0; zz<STYPE_COUNT; ++zz) cfg |= cell.layers[zz];
							gGfxMgr.Blit(PDGG(WS_TILES) + (cfg-1), surfDib,op);
						} else {
							gGfxMgr.BlitMasked(PDGG(SURF_TILES) + cell.lowestLayer * 4 + cseq, PDGG(TRANS_TILES)+14,surfDib,op);
						}
						for (uint32 zz=2; zz<STYPE_COUNT; ++zz) {
							if (cell.layers[zz]) {
								gGfxMgr.BlitMasked(PDGG(SURF_TILES) + zz*4 + cseq, PDGG(TRANS_TILES)+(cell.layers[zz]-1),surfDib,op);
							}
						}
										
					}
					
					// touch highlight
					if(bHighlight)
						gGfxMgr.BlitEffect(PDGG(SURF_TILES) + cell.lowestLayer*4 + cseq, surfDib,op, iGfxManager::EfxDarken);

					// Grid
#ifndef PC_VERSION
					if (gSettings.GetEntryValue(CET_OVRLANDGRID)) 
						gGfxMgr.BlitAlpha( PDGG(GRID_CELL), surfDib, iPoint(op.x,op.y), 128);
#endif
				}
				map_coor.x += 1;
				map_coor.y -= 1;
			}
			if (yy%2 != 0) {
				init_map_coor.x+=1;
				map_coor = init_map_coor;
			} else {
				init_map_coor.y+=1;
				map_coor = init_map_coor;
			}
		}
	}
#endif

	//m_dibSurfLayer.CopyToDibXY(outDib, m_lastCompAnchor - anchor);
	
	//
	// Prepare item list to draw
	//
	m_ComposeList.Cleanup();
    iRect cmpRect(orc);
#ifdef PC_VERSION
    cmpRect.h += 100;
    cmpRect.y = 0;
#else
    cmpRect.x = -300;
    cmpRect.w += 300;
    cmpRect.y = -300;
    cmpRect.h += 500;
//    cmpRect.InflateRect(50, 50, 70, 50);
#endif
	m_ComposeList.SetComposeRect(cmpRect);
 	iSimpleArray<iBaseMapObject*> objList;

#if 1
	{
		init_map_coor = iPoint(fpos.x, fpos.y);
		map_coor = init_map_coor;
		//cnt_x += dsSiz.w/40+10;
		//cnt_y += dsSiz.h/10+15;
		 
		iPoint op;
#ifdef PC_VERSION
        for (sint32 yy=0; yy<cnt_y; ++yy) {
			for (sint32 xx=0; xx<cnt_x; ++xx) {  
#else
		for (sint32 yy=-2; yy<cnt_y+10; ++yy) {
			for (sint32 xx=-5; xx<cnt_x+3; ++xx) {				
#endif
                op = im.Map2Screen(map_coor) + offset;

				if (gGame.Map().IsValidPos(map_coor.x,map_coor.y)) {
#if 1
					uint16 avatarId = gGame.Map().GetAvatar(map_coor.x, map_coor.y);
					if ( avatarId != 0xFFFF){
						// Avatars
						if ( ( avatarId & 0xFF00) == 0xFF00) {
							// path element
							iPathElT* pPe = itemMgr.m_PathElProts[avatarId & 0xFF];
							SpriteId sid = pPe->SpriteConfig(gGame.Map().PathElConfig(map_coor.x, map_coor.y));
							m_ComposeList.AddItem(sid, op, gGfxMgr.Shadow(sid), pPe->Level(), CalcCellIndex(map_coor));
						} else {
							// Basic avatar
							const iAvatar& avatar = itemMgr.m_AvatarMgr[avatarId];
					
							for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
								m_ComposeList.AddItem(avatar.GetSpriteId(sid),op+avatar.GetSpriteOffset(sid),gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(map_coor));
							}
						}
					} 
#endif
					if (iBaseMapObject* pMapObj = gGame.Map().m_CoverMap.GetAt(map_coor.x, map_coor.y)) {
						// Objects
						if (pMapObj->Pos() == map_coor) {
							if (iHero* pHero = DynamicCast<iHero*>(pMapObj)) {
								if (pHero->GetLocation()) objList.Add(pHero->GetLocation());                               
							}
							else if (iShip* pShip = DynamicCast<iShip*>(pMapObj)) {                                
							}
							objList.Add(pMapObj);
						}
					}
				} else {
					// Map border
					uint32 val = CalcCellIndex(map_coor);
					uint8 type;

					if (map_coor.x >= gGame.Map().GetWidth() || map_coor.y >= gGame.Map().GetHeight()) val += 10;

					if(map_coor.y == 0 && map_coor.x == gGame.Map().GetHeight())
						type = 0;                   
					else if(map_coor.y == gGame.Map().GetWidth() && map_coor.x == 0)
						type = 1;               
					else if(map_coor.y == gGame.Map().GetWidth() && map_coor.x < gGame.Map().GetHeight() && map_coor.x > 0)
						type = 2;
					else if(map_coor.y == gGame.Map().GetWidth() + 1 && map_coor.x <= gGame.Map().GetHeight() && map_coor.x > 0)
						type = 3;
					else if(map_coor.y == gGame.Map().GetWidth() && map_coor.x == gGame.Map().GetHeight())
						type = 4;
					else if(map_coor.y == gGame.Map().GetWidth() + 1 && map_coor.x == gGame.Map().GetHeight() + 1)
						type = 5;
					else if(map_coor.y <= gGame.Map().GetWidth() && map_coor.y > 0 && map_coor.x == gGame.Map().GetHeight() + 1)
						type = 6;
					else if(map_coor.y < gGame.Map().GetWidth() && map_coor.y > 0 && map_coor.x == gGame.Map().GetHeight())
						type = 7;
					else    
						type = 10 + iABS<sint32>((map_coor.x % 10 + map_coor.y % 10)) % 4;

					m_ComposeList.AddItem(PDGG(MAP_BORDER) + type, op, 0, SPL_OBJECT, val);  
				}

				map_coor.x += 1;
				map_coor.y -= 1;
			}
			if (yy%2 != 0) {
				init_map_coor.x+=1;
				map_coor = init_map_coor;
			} else {
				init_map_coor.y+=1;
				map_coor = init_map_coor;
			}
		}
	}
#endif
	
#if 1
	for (uint32 nn=0; nn<objList.GetSize(); ++nn) {
		iBaseMapObject* pMapObj = objList[nn];
		iMainView::HighlightType ht = iMainView::HLT_NONE;
		if(((iMainView_touch*)gGame.MainView())->ObjIsOnInfoBar(pMapObj)) {
			ht = iMainView::HLT_AVAILABLE;
		} else if(((iMainView_touch*)gGame.MainView())->IsHighlightedObject(pMapObj)) { 
			ht = gGame.MainView()->ObjHighlightType();
		}

		if (iVisCnst* pVisCnst = DynamicCast<iVisCnst*>(pMapObj)) {
			// Visitables
			const iVisCnstT* pCnstT = pVisCnst->Proto();
			for (uint32 cvr=0; cvr<pCnstT->AvatarsCount(); ++cvr){
				const iAvatar& avatar = itemMgr.m_AvatarMgr[pCnstT->AvatarEntry(cvr).aid];
				iPoint cpos(pVisCnst->Pos().x+pCnstT->AvatarEntry(cvr).ox, pVisCnst->Pos().y+pCnstT->AvatarEntry(cvr).oy);
				for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
					iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
					m_ComposeList.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)), 255, ht);
				}
			}
		} else if (iOwnCnst* pOwnCnst = DynamicCast<iOwnCnst*>(pMapObj)) {
			// Ownerables
			const iOwnCnstT* pCnstT = pOwnCnst->Proto();
			for (uint32 cvr=0; cvr<pCnstT->AvatarsCount(); ++cvr){
				const iAvatar& avatar = itemMgr.m_AvatarMgr[pCnstT->AvatarEntry(cvr).aid];
				iPoint cpos(pOwnCnst->Pos().x+pCnstT->AvatarEntry(cvr).ox, pOwnCnst->Pos().y+pCnstT->AvatarEntry(cvr).oy);
				for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
					iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
					m_ComposeList.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)), 255, ht);
				}
				if (pOwnCnst->Owner() != PID_NEUTRAL && !gGame.Map().ActPlayer()->FogMap().IsHidden(cpos)) {
					SpriteId fsid = PDGG(FLAGS) + 6 * pOwnCnst->Owner() + iABS(((t / fix32(0.1f)).ceil()) % 3);
					for (uint32 fid=0; fid<avatar.FlagsCount(); ++fid) {
						iPoint opos = im.Map2Screen(cpos) + offset + iPoint(avatar.FlagEntry(fid).ox,avatar.FlagEntry(fid).oy);
						m_ComposeList.AddItem(fsid,opos,0,SPL_OBJECT,CalcCellIndex(cpos), 255, ht);
					}
				}
			}
		} else if (iCastle* pCastle = DynamicCast<iCastle*>(pMapObj)) {
			// Castles
			const iCastleT* pCastleT = pCastle->Proto();
			for (uint32 cvr=0; cvr<pCastleT->AvatarsCount(); ++cvr){
				const iAvatar& avatar = itemMgr.m_AvatarMgr[pCastleT->AvatarEntry(cvr).aid];
				iPoint cpos(pCastle->Pos().x+pCastleT->AvatarEntry(cvr).ox, pCastle->Pos().y+pCastleT->AvatarEntry(cvr).oy);
				bool bShow = (gGame.Map().ActPlayer() && !gGame.Map().ActPlayer()->FogMap().IsHidden(cpos));
				for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
					iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
					m_ComposeList.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)), 255, ht);
				}
				if (pCastle->Owner() != PID_NEUTRAL && bShow) {
					SpriteId fsid = PDGG(FLAGS) + 6 * pCastle->Owner() + iABS((((t / fix32(0.1f)).ceil() + pCastleT->AvatarEntry(cvr).ox)) % 3);
					for (uint32 fid=0; fid<avatar.FlagsCount(); ++fid) {
						iPoint opos = im.Map2Screen(cpos) + offset + iPoint(avatar.FlagEntry(fid).ox,avatar.FlagEntry(fid).oy);
						m_ComposeList.AddItem(fsid,opos,0,SPL_OBJECT,CalcCellIndex(cpos), 255, ht);
					}
				}
			}
		} else if (iHero* pHero = DynamicCast<iHero*>(pMapObj)) {
            // Heroes
            bool bShow = (gGame.Map().ActPlayer() && !gGame.Map().ActPlayer()->FogMap().IsHidden(pHero->Pos()));
            if (bShow && !pHero->Moving())
            {
                if (!pHero->InShip())
                {
                    iPoint opos = im.Map2Screen(pHero->Pos()) + offset;
                    SpriteId fsid = PDGG(FLAGS) + 6 * pHero->Owner()->PlayerId() + iABS((t / fix32(0.1f)).ceil() % 3);
                    if (pHero->Angle()>3) fsid += 3;
                    SpriteId hsid = GetHeroSprite(pHero);
                    // Hero
					bool bCropped = (bool)gGame.Map().PosCropsSprites(pHero->Pos());
                    m_ComposeList.AddItem(hsid,opos,1,SPL_OBJECT,CalcCellIndex(pHero->Pos()), 255, ht, bCropped);
                    m_ComposeList.AddItem(fsid,opos + HERO_FLAG_ANCHOR[pHero->Angle()*9]+iPoint(15,-30),0,SPL_OBJECT,CalcCellIndex(pHero->Pos()), 255, ht);
                    // Hero fore layer (only for current hero)
                    if (pHero == gGame.Map().CurHero()) 
                    {
                        m_ComposeList.AddItem(hsid,opos,0,SPL_OBJECT,CalcCellIndex(pHero->Pos())+1024, 128, ht, bCropped);
                        m_ComposeList.AddItem(fsid,opos + HERO_FLAG_ANCHOR[pHero->Angle()*9]+iPoint(15,-30),0,SPL_OBJECT,CalcCellIndex(pHero->Pos())+1024, 128, ht);
                    }
					
                }
                else
                {
                    SpriteId fsid, ssid;
                    iPoint opos = im.Map2Screen(pHero->Pos()) + offset;
                    ssid = GetHeroInShipSprite(pHero) + 1;
                    fsid = PDGG(FLAGS_SHIP) + 2 * pHero->Owner()->PlayerId();
                    if (pHero->Angle() > 3) fsid ++;
                    // Ship
                    //SaveShipFlagsPos();
                    //LoadShipFlagsPos();                    
                    m_ComposeList.AddItem(ssid,opos,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()), 255, ht);
                    if (pHero->GetShip()->GetImgType() == 0)
                    {
                        m_ComposeList.AddItem(fsid,opos + SHIP1_FLAG1_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()), 255, ht);
                        m_ComposeList.AddItem(fsid,opos + SHIP1_FLAG2_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()), 255, ht);
                    }
                    else
                    {
                        m_ComposeList.AddItem(fsid,opos + SHIP2_FLAG1_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()), 255, ht);
                        m_ComposeList.AddItem(fsid,opos + SHIP2_FLAG2_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()), 255, ht);
                    }  
                }      
            }
		} else if (iShip* pShip = DynamicCast<iShip*>(pMapObj)) {
			// Ships
			bool bShow = (gGame.Map().ActPlayer() && 
						  (!gGame.Map().ActPlayer()->FogMap().IsHidden(pShip->Pos()) ||
						   !pShip->IsLoaded()));
			if (bShow)
			{
				SpriteId ssid;
				iPoint opos = im.Map2Screen(pShip->Pos()) + offset;
				ssid = GetShipSprite(pShip);
				m_ComposeList.AddItem(ssid,opos,0,SPL_OBJECT,CalcCellIndex(pShip->Pos()), 255, ht);                
			}        
		} else if (iMapGuard* pMapGuard = DynamicCast<iMapGuard*>(pMapObj)) {
			// map guards
			if (gGame.Map().ActPlayer() /*&& !gGame.Map().ActPlayer()->FogMap().IsHidden(pMapGuard->Pos())*/ ) {								
				iPoint opos = im.Map2Screen(pMapGuard->Pos()) + offset;
				m_ComposeList.AddItem(pMapGuard->Sprite(),opos, 1, SPL_OBJECT,CalcCellIndex(pMapGuard->Pos()), 255, ht, gGame.Map().PosCropsSprites(pMapGuard->Pos()));
			}
		} else if (DynamicCast<iMapItem_Mineral*>(pMapObj) || DynamicCast<iMapItem_ManaCrystal*>(pMapObj) || DynamicCast<iMapItem_CampFire*>(pMapObj) || DynamicCast<iMapItem_Chest*>(pMapObj) || DynamicCast<iMapItem_Artifact*>(pMapObj) || DynamicCast<iMapItem_Lamp*>(pMapObj) || DynamicCast<iMapItem_SpellScroll*>(pMapObj) || DynamicCast<iMapItem_KeyGuard*>(pMapObj)) {
			// other map items
			iMapItem* pMapItem = (iMapItem*)(pMapObj);
			if (gGame.Map().ActPlayer() /*&& !gGame.Map().ActPlayer()->FogMap().IsHidden(pMapItem->Pos())*/){
				iPoint opos = im.Map2Screen(pMapItem->Pos()) + offset;
				m_ComposeList.AddItem(pMapItem->Sprite(),opos,1,SPL_OBJECT,CalcCellIndex(pMapItem->Pos()), 255, ht);
			}
		} else {
			check(0);
		}
	}
#endif
	
	// Cell animation
	if (m_pAniObj && m_pAniObj->Type() == iAniObj::CellAni){
		iPoint opos = im.Map2Screen(m_pAniObj->Pos()) + offset;
		if ( iAniObj_MovHero* pHeroAni = DynamicCast<iAniObj_MovHero*>(m_pAniObj) )
		{
			iHero *pHero = pHeroAni->Hero();
			iMainView::HighlightType ht = iMainView::HLT_NONE;
			if (!pHero->InShip())
			{
				SpriteId spOffset=0;
				iPoint  ptOffset;
				pHeroAni->GetAniInfo(spOffset, ptOffset);
				opos += ptOffset;
				SpriteId fsid = PDGG(FLAGS) + 6 * pHero->Owner()->PlayerId();
				if (pHero->Angle()>3) fsid += 3;
				uint8 crop = gGame.Map().PosCropsSprites(pHero->Pos());
				SpriteId hsid = GetHeroSprite(pHero) + spOffset;
				m_ComposeList.AddItem(hsid,opos,1,SPL_OBJECT,CalcCellIndex(pHeroAni->TargetPos()), 255, ht, crop);
				m_ComposeList.AddItem(fsid,opos + HERO_FLAG_ANCHOR[pHero->Angle()*9+spOffset]+iPoint(15,-30),0,SPL_OBJECT,CalcCellIndex(pHeroAni->TargetPos()), 255, ht);
			}
			else
			{
				SpriteId fsid, ssid;
				SpriteId spOffset = 0;
				iPoint  ptOffset;                
				pHeroAni->GetAniInfo(spOffset, ptOffset);
				opos += ptOffset;
				fsid = PDGG(FLAGS_SHIP) + 2 * pHero->Owner()->PlayerId();
				if (pHero->Angle() > 3) fsid++;
				ssid = GetHeroInShipSprite(pHero) + 1 + spOffset;                
				m_ComposeList.AddItem(ssid,opos,0,SPL_OBJECT,CalcCellIndex(pHeroAni->TargetPos()));
				if (pHero->GetShip()->GetImgType() == 0)
				{
					m_ComposeList.AddItem(fsid,opos + SHIP1_FLAG1_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHeroAni->TargetPos()));
					m_ComposeList.AddItem(fsid,opos + SHIP1_FLAG2_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHeroAni->TargetPos()));
				}
				else
				{
					m_ComposeList.AddItem(fsid,opos + SHIP2_FLAG1_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHeroAni->TargetPos()));
					m_ComposeList.AddItem(fsid,opos + SHIP2_FLAG2_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHeroAni->TargetPos()));
				}                                                                            
			}
		} else if ( iAniObj_FadeOut* pFadeAni = DynamicCast<iAniObj_FadeOut*>(m_pAniObj) ) {
			SpriteId sid;
			uint8 a;
			pFadeAni->GetAniInfo(sid, a);
			m_ComposeList.AddItem(sid,opos,0,SPL_OBJECT,CalcCellIndex(pFadeAni->Pos()),a);
		} else {
			check(0 == "Unknown cell animation type!");
		}
	}

	// Path
	sint32 movAcc=0;
	if (gGame.Map().CurHero() && gGame.Map().IsCurPlayerActive()){
		iHero* pCurHero = gGame.Map().CurHero();
		movAcc=pCurHero->Moves();
		if (pCurHero->Path().m_Path.GetSize()>1){
			uint32 fidx = 0;
			if (pCurHero->Moving()) {
				movAcc -= pCurHero->Path().GetStepCost();
				fidx ++;
			}
			SpriteId psid;
			for (uint32 xx=fidx; xx<pCurHero->Path().m_Path.GetSize()-1; ++xx) 
			{
				movAcc -= pCurHero->Path().m_Path[xx].m_Cost;                
				iPoint opos = im.Map2Screen(pCurHero->Path().m_Path[xx].m_Pos) + offset;
				psid = PDGG(PATH);
#ifndef ROYAL_BOUNTY
				if (movAcc < 0) psid += 9; // if no moves available - draw red sprites
#endif
				// if action of this step is "leave", draw it instead of usual "move"
				if (pCurHero->Path().m_Path[xx].m_action == iPath::Leave)
					psid += 6;
				
				m_ComposeList.AddItem(psid, opos, 0, SPL_OBJECT,CalcCellIndex(pCurHero->Path().m_Path[xx].m_Pos));
			}
		}
	}
	// draw "unreachable red cross"
	/*if(m_RedCross != cInvalidPoint) {
		iPoint opos = im.Map2Screen(m_RedCross) + offset;
		m_ComposeList.AddItem(PDGG(PATH) + 9,opos, 0, SPL_OBJECT, 10000);      
	}*/


	
	// 
	// Draw ground-level sprites
	for (uint32 cId=0; cId<m_ComposeList.Size(); ++cId){
		if (m_ComposeList[cId].value.m_level == SPL_GROUND) 
        {
			if (m_ComposeList[cId].value.Solid()) gGfxMgr.Blit( m_ComposeList[cId].value.m_sid, outDib, m_ComposeList[cId].value.m_pos);
			else gGfxMgr.BlitAlpha( m_ComposeList[cId].value.m_sid, outDib, m_ComposeList[cId].value.m_pos, m_ComposeList[cId].value.m_alpha);
			if(m_ComposeList[cId].value.m_ht != iMainView::HLT_NONE) {
				iGfxManager::Effects efx;
				switch(m_ComposeList[cId].value.m_ht)
				{
					case iMainView::HLT_AVAILABLE:
						efx = iGfxManager::EfxGlow;
						break;
					case iMainView::HLT_NOT_AVAILABLE:
						efx = iGfxManager::EfxDarken;
						break;
					default:			
						check(0);
				}
				gGfxMgr.BlitEffect( m_ComposeList[cId].value.m_sid, outDib, m_ComposeList[cId].value.m_pos, efx);	
			}
		}
	}
	//
#if 1
	// Draw shadow
	for (uint32 cId=0; cId<m_ComposeList.Size(); ++cId){
		if (uint8 sh = m_ComposeList[cId].value.m_shadow ) {
			iRect src_rect(iPoint(), gGfxMgr.Dimension(m_ComposeList[cId].value.m_sid));
			iPoint pt = m_ComposeList[cId].value.m_pos;
			/*if(uint8 crop = m_ComposeList[cId].value.m_crop) {
				src_rect.h = gGfxMgr.ObjRect(m_ComposeList[cId].value.m_sid).y2();
				src_rect.h -= crop;
				pt.x += crop;
				pt.y -= crop * 2;
			}*/
			gGfxMgr.BlitUniversal( m_ComposeList[cId].value.m_sid, outDib, pt, src_rect, iGfxManager::EfxShadowIso);
		}
	}
#endif
	// Destination Point overlay (circle)
	if (gGame.Map().CurHero() && gGame.Map().IsCurPlayerActive()){
		iHero* pCurHero = gGame.Map().CurHero();
		if (pCurHero->Path().m_Path.GetSize() > 0){
			iPath::iStep pstep = pCurHero->Path().m_Path.GetLast();
			movAcc -= pstep.m_Cost;
			iPoint pos = pstep.m_Pos;
			uint32 psid = PDGG(PATH);
#ifndef ROYAL_BOUNTY
			if (movAcc < 0) psid += 9;
#endif
			movAcc += pstep.m_Cost;

			psid += 8;
			iPoint opos = im.Map2Screen(pos) + offset + iPoint(3, 2);

			//gGfxMgr.BlitEffect( psid, outDib, opos, iGfxManager::EfxShadow2D);
			gGfxMgr.Blit( psid, outDib, opos);			
		}
	}

#if 1
	//
	// Draw other sprites
	for (uint32 cId=0; cId<m_ComposeList.Size(); ++cId){
		if (m_ComposeList[cId].value.m_level != SPL_GROUND) {

			//    if (m_ComposeList[cId].value.m_sid == 4359 || (m_ComposeList[cId].value.m_sid >= 901 && m_ComposeList[cId].value.m_sid < 901 + 96))
			//        _asm int 3

			iRect src_rect(iPoint(), gGfxMgr.Dimension(m_ComposeList[cId].value.m_sid));
            
			/*if(m_ComposeList[cId].value.m_crop) {
				src_rect.h = gGfxMgr.ObjRect(m_ComposeList[cId].value.m_sid).y2();
				src_rect.h -= m_ComposeList[cId].value.m_crop;
			}*/
			if (m_ComposeList[cId].value.Solid()) 
				gGfxMgr.Blit( m_ComposeList[cId].value.m_sid, outDib, src_rect, m_ComposeList[cId].value.m_pos);
			else 
				gGfxMgr.BlitAlpha( m_ComposeList[cId].value.m_sid, outDib, src_rect, m_ComposeList[cId].value.m_pos, m_ComposeList[cId].value.m_alpha);
			if(m_ComposeList[cId].value.m_ht != -1) {
				iGfxManager::Effects efx;
				switch(m_ComposeList[cId].value.m_ht)
				{
					case iMainView::HLT_AVAILABLE:
						efx = iGfxManager::EfxGlow;
						break;
					case iMainView::HLT_NOT_AVAILABLE:
						efx = iGfxManager::EfxDarken;
						break; 
					default:			
						check(0);
				}
				gGfxMgr.BlitEffect(m_ComposeList[cId].value.m_sid, outDib, m_ComposeList[cId].value.m_pos, efx);	
			}

		}
	}
#endif
	
	//FillStaredRect(outDib, outDib.GetSize(), anchor);
	//
	// Fog
	init_map_coor = anchor_coor;
	map_coor = anchor_coor;
	for (sint32 yy=0; yy<cnt_y; ++yy) {
		for (sint32 xx=0; xx<cnt_x; ++xx){
			if (gGame.Map().ActPlayer() && gGame.Map().ActPlayer()->FogMap().IsValidCell(map_coor)){
				sint32 xpos = xx*step_x;
				sint32 ypos = yy*step_y;
				if (yy%2!=0) xpos -= step_x >> 1;
				iPoint op(xpos-offs.x+orc.x-SBUFF_ALIGN ,ypos-offs.y+orc.y-SBUFF_ALIGN);
				// Fog
				uint8 dcfg, fcfg;
				if (!gGame.Map().ActPlayer()->FogMap().GetFogCfg(map_coor,dcfg,fcfg)){
					if (fcfg && dcfg != fcfg) gGfxMgr.BlitEffect(PDGG(FOG_TILES)+(fcfg-1),outDib,op,iGfxManager::EfxTransparent);
					if (dcfg) gGfxMgr.Blit(PDGG(FOG_TILES)+(dcfg-1),outDib,op);
				}
			}
			map_coor.x += 1;
			map_coor.y -= 1;
		}
		if (yy%2 != 0) {
			init_map_coor.x+=1;
			map_coor = init_map_coor;
		} else {
			init_map_coor.y+=1;
			map_coor = init_map_coor;
		}
	}

	// Destination Point
	if (gGame.Map().CurHero() && gGame.Map().IsCurPlayerActive()){
		iHero* pCurHero = gGame.Map().CurHero();
		if (pCurHero->Path().m_Path.GetSize() > 0){
			iPath::iStep pstep = pCurHero->Path().m_Path.GetLast();
			movAcc -= pstep.m_Cost;
			iPoint pos = pstep.m_Pos;
			uint32 psid = PDGG(PATH);
#ifndef ROYAL_BOUNTY
			if (movAcc < 0) psid += 9;
#endif

			psid += pCurHero->Path().TargetAction();
			iPoint opos = im.Map2Screen(pos)+offset;

			//gGfxMgr.BlitEffect( psid, outDib, opos, iGfxManager::EfxShadow2D);

			iPoint offset(0, -30 + 10 * sin((t * fix32(2 * PI) / fix32(2.0f)).to_float()));			
			gGfxMgr.Blit( psid, outDib, opos + offset);

#ifndef ROYAL_BOUNTY
			if (gSettings.GetEntryValue(CET_TRVPTSMODE)) {
				opos.Move(28,14);
				gTextComposer.TextOut(creatqfc,outDib,opos,iFormat(_T("%d (%d)"),pCurHero->Path().PathCost(),pCurHero->Moves()));
			} else {
				if (pCurHero->Path().PathCost() > pCurHero->Moves()) {
					iTextComposer::FontConfig fc(FS_MEDIUM, iDibFont::ComposeProps(RGB16(255,192,0), cColor_Black, iDibFont::DecBorder));
					opos.Move(25,11);
					sint32 steps = (pCurHero->Path().PathCost() - pCurHero->Moves()) / pCurHero->TotalMoves() + 2;
					gTextComposer.TextOut(fc,outDib,opos,iFormat(_T("%d"),steps));
				}
			}
#endif
		}
	}

	// Target point. This is probably not needed any more. Uncomment if something missing (Hedin)
#if 0 
	if (m_ptTarget != cInvalidPoint) {
		iPoint opos = im.Map2Screen(m_ptTarget)+offset;
		gGfxMgr.BlitEffect( m_crTarget, outDib, opos, iGfxManager::EfxShadow2D);
		gGfxMgr.Blit( m_crTarget, outDib, opos);
	}
#endif

	// cell events
	for (iEvtCIt it=m_cellEvents.First(); it != m_cellEvents.End(); ++it) {
		iPoint pos = im.Map2Screen((*it)->pos) + offset;;
		pos.x += im.GetCellWidth()/2;
		pos.y -= 10 + (255 - (*it)->dstate) / 7;
		uint8 a = (*it)->dstate;
		//if (a < 128) a *= 2; else a = 255;		
		gTextComposer.TextOut(cellevfc, outDib, pos, iFormat(_T("#A%02X"),a)+(*it)->text, iRect(pos,iSize()), AlignTop);
	}

	// Full screen animation
	if (m_pAniObj && m_pAniObj->Type() == iAniObj::FullScreenAni) {
		if ( iAniObj_Teleport* pTeleportAni = DynamicCast<iAniObj_Teleport*>(m_pAniObj) ) {
			pTeleportAni->Compose(outDib);
		} else {
			check(0 == "Unknown cell animation type!");
		}
	}
}

void iOLComposer::Compose(iDib& outDib, const iPoint& out_anchor, bool bPuzzleMap, const iPoint& tgtCell)
{
	//
	iPoint anchor = out_anchor-iPoint((gGame.Map().GetWidth() * 40)/2+20+40,40);
	iPoint offset = iPoint((gGame.Map().GetWidth() * 40)/2+20+40,40) - out_anchor;

	iItemMgr& itemMgr = gGame.ItemMgr();
	iIsoMetric im(im_coef);
	// save cell under anchor point
	iPoint fpos = im.Screen2Map(anchor);
	// offset cell
	fpos.Move(-1,-1);
	iPoint ppos = im.Map2Screen(fpos);
	ppos.Move(-40,0);
	iPoint offs = anchor-ppos;

	//
	// Surface
	sint32 cnt_x = outDib.GetWidth()/40+3;
	sint32 cnt_y = outDib.GetHeight()/10+5;

	sint32 step_x = im.GetCellStepX()*2;
	sint32 step_y = im.GetCellStepY();

	const iPoint anchor_coor(fpos);
	iPoint init_map_coor(fpos);
	iPoint map_coor(fpos);

	sint32 xx,yy;

	for (yy=0; yy<cnt_y; ++yy) {
		for (xx=0; xx<cnt_x; ++xx){
			if (gGame.Map().IsValidPos(map_coor.x,map_coor.y)){
				sint32 xpos = xx*step_x;
				sint32 ypos = yy*step_y;
				if (yy%2!=0) xpos -= step_x >> 1;
				iPoint op(xpos-offs.x,ypos-offs.y);

				// Surf
				iGameMap::iSurfCell cell;
				uint32 cseq = CalcCellSeq(map_coor,4);
				if ( gGame.Map().GetCellSurf(map_coor, cell) ) {
					// solid cell - do nothing
					gGfxMgr.BlitMasked(PDGG(SURF_TILES) + cell.lowestLayer*4 + cseq, PDGG(TRANS_TILES)+14,outDib,op);
				} else {
					if (cell.lowestLayer == STYPE_WATER) {
						uint8 cfg = 0;
						for (uint32 zz=0; zz<STYPE_COUNT; ++zz) cfg |= cell.layers[zz];
						gGfxMgr.Blit(PDGG(WS_TILES) + (cfg-1), outDib,op);
					} else {
						gGfxMgr.BlitMasked(PDGG(SURF_TILES) + cell.lowestLayer * 4 + cseq, PDGG(TRANS_TILES)+14,outDib,op);
					}
					for (uint32 zz=2; zz<STYPE_COUNT; ++zz) {
						if (cell.layers[zz]) {
							gGfxMgr.BlitMasked(PDGG(SURF_TILES) + zz*4 + cseq, PDGG(TRANS_TILES)+(cell.layers[zz]-1),outDib,op);
						}
					}
				}
			}
			map_coor.x += 1;
			map_coor.y -= 1;
		}
		if (yy%2 != 0) {
			init_map_coor.x+=1;
			map_coor = init_map_coor;
		} else {
			init_map_coor.y+=1;
			map_coor = init_map_coor;
		}
	}

	//
	// Prepare item list to draw
	iComposeList clist;
	clist.SetComposeRect(outDib.GetSize());

	// Avatars
	for (yy=0; yy<gGame.Map().GetHeight(); ++yy) {
		for (xx=0; xx<gGame.Map().GetWidth(); ++xx){
			uint16 avatarId = gGame.Map().GetAvatar(xx,yy);
			if ( avatarId != 0xFFFF){
				iPoint opos = im.Map2Screen(iPoint(xx,yy)) + offset;
				if ( ( avatarId & 0xFF00) == 0xFF00) {
					// path element
					if (!bPuzzleMap) {
						iPathElT* pPe = itemMgr.m_PathElProts[avatarId & 0xFF];
						SpriteId sid = pPe->SpriteConfig(gGame.Map().PathElConfig(xx, yy));
						clist.AddItem(sid, opos, gGfxMgr.Shadow(sid), pPe->Level(), CalcCellIndex(iPoint(xx,yy)));
					}
				} else {
					// avatar
					const iAvatar& avatar = itemMgr.m_AvatarMgr[avatarId];
					for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
						clist.AddItem(avatar.GetSpriteId(sid),opos+avatar.GetSpriteOffset(sid),gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(xx,yy)));
					}
				}
			}
		}
	}

	if (!bPuzzleMap) {

		// Visitables
		for (iGameWorld::iVCIt vcit = gGame.Map().m_VisCnsts.First(); vcit != gGame.Map().m_VisCnsts.End(); ++vcit) {
			iVisCnst* pCnst = *vcit;
			const iVisCnstT* pCnstT = pCnst->Proto();
			for (uint32 cvr=0; cvr<pCnstT->AvatarsCount(); ++cvr){
				const iAvatar& avatar = itemMgr.m_AvatarMgr[pCnstT->AvatarEntry(cvr).aid];
				iPoint cpos(pCnst->Pos().x+pCnstT->AvatarEntry(cvr).ox, pCnst->Pos().y+pCnstT->AvatarEntry(cvr).oy);
				for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
					iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
					clist.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)));
				}
			}
			// Guards
			if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pCnst->Pos()) && !pCnst->Guard().Empty()) {
				iPoint opos = im.Map2Screen(pCnst->Pos()) + offset;
				clist.AddItem(PDGG(MINIMON)+pCnst->Guard().StrongestCreature(),opos,0,SPL_OBJECT,CalcCellIndex(pCnst->Pos()));
			}
		}

		// Neutral ownerables
		for (iGameWorld::iOCIt ocit = gGame.Map().m_OwnCnsts.First(); ocit != gGame.Map().m_OwnCnsts.End(); ++ocit) {
			iOwnCnst* pCnst = *ocit;
			const iOwnCnstT* pCnstT = pCnst->Proto();
			for (uint32 cvr=0; cvr<pCnstT->AvatarsCount(); ++cvr){
				const iAvatar& avatar = itemMgr.m_AvatarMgr[pCnstT->AvatarEntry(cvr).aid];
				iPoint cpos(pCnst->Pos().x+pCnstT->AvatarEntry(cvr).ox, pCnst->Pos().y+pCnstT->AvatarEntry(cvr).oy);
				for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
					iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
					clist.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)));
				}
			}
			// Guards
			if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pCnst->Pos()) && !pCnst->Guard().Empty()) {
				iPoint opos = im.Map2Screen(pCnst->Pos()) + offset;
				clist.AddItem(PDGG(MINIMON)+pCnst->Guard().StrongestCreature(),opos,0,SPL_OBJECT,CalcCellIndex(pCnst->Pos()));
			}
		}

		// Neutral castles
		for (iGameWorld::iCtIt ctit = gGame.Map().m_Castles.First(); ctit != gGame.Map().m_Castles.End(); ++ctit) {
			iCastle* pCastle = *ctit;
			const iCastleT* pCastleT = pCastle->Proto();
			for (uint32 cvr=0; cvr<pCastleT->AvatarsCount(); ++cvr){
				const iAvatar& avatar = itemMgr.m_AvatarMgr[pCastleT->AvatarEntry(cvr).aid];
				iPoint cpos(pCastle->Pos().x+pCastleT->AvatarEntry(cvr).ox, pCastle->Pos().y+pCastleT->AvatarEntry(cvr).oy);
				for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
					iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
					clist.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)));
				}
			}
		}

		// Ownerables and castles flags
		iGameWorld::iPLIt pit;
		for (pit = gGame.Map().m_Players.First(); pit != gGame.Map().m_Players.End(); ++pit){
			// Ownerables, its flags, and guards
			for (iPlayer::iCLIt cit = (*pit)->m_Cnsts.First(); cit != (*pit)->m_Cnsts.End(); ++cit){
				iOwnCnst* pCnst = (*cit);
				const iOwnCnstT* pCnstT = pCnst->Proto();
				for (uint32 cvr=0; cvr<pCnstT->AvatarsCount(); ++cvr){
					const iAvatar& avatar = itemMgr.m_AvatarMgr[pCnstT->AvatarEntry(cvr).aid];
					iPoint cpos(pCnst->Pos().x+pCnstT->AvatarEntry(cvr).ox, pCnst->Pos().y+pCnstT->AvatarEntry(cvr).oy);
					for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
						iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
						clist.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)));
					}
					if (!gGame.Map().ActPlayer()->FogMap().IsHidden(cpos)) {
						SpriteId fsid = PDGG(FLAGS) + 6 * pCnst->Owner();
						for (uint32 fid=0; fid<avatar.FlagsCount(); ++fid) {
							iPoint opos = im.Map2Screen(cpos) + offset + iPoint(avatar.FlagEntry(fid).ox,avatar.FlagEntry(fid).oy);
							clist.AddItem(fsid,opos,0,SPL_OBJECT,CalcCellIndex(pCnst->Pos()+1));
						}
					}
				}

				// Guards
				if (!gGame.Map().ActPlayer()->FogMap().IsHidden(pCnst->Pos()) && !pCnst->Guard().Empty()) {
					iPoint opos = im.Map2Screen(pCnst->Pos()) + offset;
					clist.AddItem(PDGG(MINIMON)+pCnst->Guard().StrongestCreature(),opos,0,SPL_OBJECT,pCnst->Pos().x+pCnst->Pos().y);
				}
			}

			// Castles and its flags
			for (iPlayer::iCtLIt ctit = (*pit)->CastleFirst(); ctit != (*pit)->CastleEnd(); ++ctit){
				iCastle* pCastle = *ctit;
				const iCastleT* pCastleT = pCastle->Proto();
				for (uint32 cvr=0; cvr<pCastleT->AvatarsCount(); ++cvr){
					const iAvatar& avatar = itemMgr.m_AvatarMgr[pCastleT->AvatarEntry(cvr).aid];
					iPoint cpos(pCastle->Pos().x+pCastleT->AvatarEntry(cvr).ox, pCastle->Pos().y+pCastleT->AvatarEntry(cvr).oy);
					for (uint32 sid=0; sid<avatar.SpritesCount(); ++sid){
						iPoint opos = im.Map2Screen(cpos) + offset + avatar.GetSpriteOffset(sid);
						clist.AddItem(avatar.GetSpriteId(sid),opos,gGfxMgr.Shadow(avatar.GetSpriteId(sid)),avatar.GetSpriteLevel(sid), CalcCellIndex(iPoint(cpos.x,cpos.y)));
					}
					if (!gGame.Map().ActPlayer()->FogMap().IsHidden(cpos)) {
						SpriteId fsid = PDGG(FLAGS) + 6 * pCastle->Owner();
						for (uint32 fid=0; fid<avatar.FlagsCount(); ++fid) {
							iPoint opos = im.Map2Screen(cpos) + offset + iPoint(avatar.FlagEntry(fid).ox,avatar.FlagEntry(fid).oy);
							clist.AddItem(fsid,opos,0,SPL_OBJECT,CalcCellIndex(pCastle->Pos()+1));
						}
					}
				}
			}
		}


		// Ships
		// this code draw only ships without hero
		for (iGameWorld::iSLIt ctit = gGame.Map().m_Ships.First(); ctit != gGame.Map().m_Ships.End(); ++ctit) 
		{
			iShip* pShip = *ctit;
			bool bShow = (gGame.Map().ActPlayer() && !gGame.Map().ActPlayer()->FogMap().IsHidden(pShip->Pos()) && !pShip->IsLoaded());
			if (bShow)
			{
				SpriteId ssid;
				iPoint opos = im.Map2Screen(pShip->Pos()) + offset;
				ssid = GetShipSprite(pShip);
				clist.AddItem(ssid, opos, 0, SPL_OBJECT, CalcCellIndex(pShip->Pos()));                
			}
		}

		// Heroes
		for (pit = gGame.Map().m_Players.First(); pit != gGame.Map().m_Players.End(); ++pit){
			// Heroes sprites and flags
			for (iPlayer::iHLIt hit = (*pit)->HeroFirst(); hit != (*pit)->HeroEnd(); ++hit)
			{
				iHero *pHero = (*hit);
				bool bShow = (gGame.Map().ActPlayer() && !gGame.Map().ActPlayer()->FogMap().IsHidden(pHero->Pos()));
				if (bShow && !pHero->Moving())
				{
					iPoint opos = im.Map2Screen(pHero->Pos()) + offset;
					if (!pHero->InShip())
					{       
						// Hero draw
						SpriteId fsid = PDGG(FLAGS) + 6 * pHero->Owner()->PlayerId();
						if (pHero->Angle() > 3) fsid += 3;
						SpriteId hsid = GetHeroSprite(pHero);
						// Hero
						clist.AddItem(hsid,opos,1,SPL_OBJECT,CalcCellIndex(pHero->Pos()));
						clist.AddItem(fsid,opos + HERO_FLAG_ANCHOR[pHero->Angle() * 9] + iPoint(15, -29),0,SPL_OBJECT,CalcCellIndex(pHero->Pos()));
					}
					else
					{
						// Ship draw
						SpriteId fsid, ssid;
						ssid = GetHeroInShipSprite(pHero) + 1;
						fsid = PDGG(FLAGS_SHIP) + 2 * pHero->Owner()->PlayerId();
						if (pHero->Angle() > 3) fsid ++;
						clist.AddItem(ssid, opos, 0, SPL_OBJECT, CalcCellIndex(pHero->Pos()));
						if (pHero->GetShip()->GetImgType() == 0)
						{
							clist.AddItem(fsid,opos + SHIP1_FLAG1_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()));
							clist.AddItem(fsid,opos + SHIP1_FLAG2_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()));
						}
						else
						{
							clist.AddItem(fsid,opos + SHIP2_FLAG1_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()));
							clist.AddItem(fsid,opos + SHIP2_FLAG2_ANCHOR[pHero->Angle()]+SHIP_FLAG_ADD,0,SPL_OBJECT,CalcCellIndex(pHero->Pos()));
						}
					}
				}
			}
		}

		// Map Items
		for (iGameWorld::iMIt mit = gGame.Map().m_MapItems.First(); mit != gGame.Map().m_MapItems.End(); ++mit){
			if (!gGame.Map().ActPlayer()->FogMap().IsHidden((*mit)->Pos())){
				iPoint opos = im.Map2Screen(iPoint((*mit)->Pos().x,(*mit)->Pos().y)) + offset;
				clist.AddItem((*mit)->Sprite(),opos,1,SPL_OBJECT,CalcCellIndex((*mit)->Pos()));
			}
		}

		// Map Guards
		for (iGameWorld::iGIt git = gGame.Map().m_MapGuards.First(); git != gGame.Map().m_MapGuards.End(); ++git) {
			//iMapGuard* pGuard = *git;
			if (gGame.Map().ActPlayer() && !gGame.Map().ActPlayer()->FogMap().IsHidden((*git)->Pos())){
				iPoint opos = im.Map2Screen(iPoint((*git)->Pos().x,(*git)->Pos().y)) + offset;
				clist.AddItem((*git)->Sprite(),opos,1,SPL_OBJECT,CalcCellIndex((*git)->Pos()));
			}
		}
	}



	// 
	// Draw ground-level sprites
	for (uint32 cId=0; cId<clist.Size(); ++cId){
		if (clist[cId].value.m_level == SPL_GROUND) {
			if (clist[cId].value.Solid()) gGfxMgr.Blit( clist[cId].value.m_sid, outDib, clist[cId].value.m_pos);
			else gGfxMgr.BlitAlpha( clist[cId].value.m_sid, outDib, clist[cId].value.m_pos, clist[cId].value.m_alpha);

		}
	}
	//
	// Draw shadow
	for (uint32 cId=0; cId<clist.Size(); ++cId){
		if (uint8 sh = clist[cId].value.m_shadow ) {
			gGfxMgr.BlitEffect( clist[cId].value.m_sid, outDib, clist[cId].value.m_pos, (sh==1)?iGfxManager::EfxShadowIso:iGfxManager::EfxShadow2D);
		}
	}

	//
	// Draw other sprites
	for (uint32 cId=0; cId<clist.Size(); ++cId){
		if (clist[cId].value.m_level != SPL_GROUND) {
			if (clist[cId].value.Solid()) gGfxMgr.Blit( clist[cId].value.m_sid, outDib, clist[cId].value.m_pos);
			else gGfxMgr.BlitAlpha( clist[cId].value.m_sid, outDib, clist[cId].value.m_pos, clist[cId].value.m_alpha);

		}
	}


	// Destination Point
	if (bPuzzleMap){
		uint32 psid = PDGG(TRACK);
		iPoint opos = im.Map2Screen(tgtCell)+offset + iPoint(20, 10);
		
		gGfxMgr.BlitEffect( psid, outDib, opos, iGfxManager::EfxShadow2D);
		gGfxMgr.Blit( psid, outDib, opos);
	}

	//
	// Fog
	if (!bPuzzleMap)  {
		init_map_coor = anchor_coor;
		map_coor = anchor_coor;
		for (yy=0; yy<cnt_y; ++yy) {
			for (sint32 xx=0; xx<cnt_x; ++xx){
				if (gGame.Map().ActPlayer() && gGame.Map().ActPlayer()->FogMap().IsValidCell(map_coor)){
					sint32 xpos = xx*step_x;
					sint32 ypos = yy*step_y;
					if (yy%2!=0) xpos -= step_x >> 1;
					iPoint op(xpos-offs.x ,ypos-offs.y);
					// Fog
					uint8 dcfg, fcfg;
					if (!gGame.Map().ActPlayer()->FogMap().GetFogCfg(map_coor,dcfg,fcfg)){
						if (fcfg && dcfg != fcfg) gGfxMgr.BlitEffect(PDGG(FOG_TILES)+(fcfg-1),outDib,op,iGfxManager::EfxTransparent);
						if (dcfg) gGfxMgr.Blit(PDGG(FOG_TILES)+(dcfg-1),outDib,op);
					}
				}
				map_coor.x += 1;
				map_coor.y -= 1;
			}
			if (yy%2 != 0) {
				init_map_coor.x+=1;
				map_coor = init_map_coor;
			} else {
				init_map_coor.y+=1;
				map_coor = init_map_coor;
			}
		}   
	}

}


iPoint iOLComposer::ActAnchor(const iPoint& pos) const 
{ 
	return iPoint(m_Anchor.x-MapPixW()/2-65+pos.x / m_zoom,m_Anchor.y-40+pos.y/m_zoom-15); 
}

void iComposeList::AddItem(SpriteId sid, const iPoint& pos, uint8 shadow, SPRITE_LEVEL level, uint32 snum, uint8 a, sint8 ht, uint8 cropped)
{
	check(m_ComposeRect != cInvalidRect);
    iPoint pnt = gGfxMgr.Anchor(sid);
    iSize sz = gGfxMgr.Dimension(sid);
    
	iRect srect(pos+pnt,sz);
	if (iClipper::IsIntersectRect(srect,m_ComposeRect)) {
		Insert(ComposeItem(sid,pos,shadow,level, a, ht, cropped),snum);
	}
}