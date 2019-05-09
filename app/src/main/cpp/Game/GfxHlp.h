#ifndef PHEROES_GAME_GFXHLP_H_
#define PHEROES_GAME_GFXHLP_H_


//
const sint32 DLG_FRAME_SIZE  = 16;
const sint32 DEF_BTN_HEIGHT = 47;
//

const sint32 DLG_GLOW_FRAME_SIZE  = 32;

enum FontSize {             
	FS_PT_10,
	FS_PT_14,
	FS_PT_16,
	FS_PT_24,
	FS_PT_30,
	FS_PT_36,
	FS_PT_36b,

	FS_COUNT,
	FS_SMALL = 2, // a hack to maintain compatibility 
	FS_MEDIUM = 4,
	FS_LARGE = 5,
};  

extern iTextComposer::FontConfig dlgfc_hdr;
extern iTextComposer::FontConfig dlgfc_topic;
extern iTextComposer::FontConfig dlgfc_plain;
extern iTextComposer::FontConfig dlgfc_stopic;
extern iTextComposer::FontConfig dlgfc_splain;

extern iTextComposer::FontConfig btnfc_normal;
extern iTextComposer::FontConfig btnfc_small;
extern iTextComposer::FontConfig btnfc_pressed;
extern iTextComposer::FontConfig btnfc_disabled;

extern iTextComposer::FontConfig menufc_large;
extern iTextComposer::FontConfig menufc_medium;
extern iTextComposer::FontConfig menufc_small;
extern iTextComposer::FontConfig menufc_xsmall;

extern iTextComposer::FontConfig savefc_hdr;
extern iTextComposer::FontConfig savefc_text;
extern iTextComposer::FontConfig listfc_text;
extern iTextComposer::FontConfig creatqfc;
extern iTextComposer::FontConfig infobarfc;
extern iTextComposer::FontConfig mapeventfc;
extern iTextComposer::FontConfig rewardqfc;
extern iTextComposer::FontConfig spellbookfc;
extern iTextComposer::FontConfig cellevfc;

extern iTextComposer::FontConfig creatinfofc_normal;
extern iTextComposer::FontConfig creatinfofc_small;
extern iTextComposer::FontConfig creatinfofc_qty;


extern iTextComposer::FontConfig newmenufc_credits;
extern iTextComposer::FontConfig newmenufc_low_text;
extern iTextComposer::FontConfig newmenufc_med_text;
extern iTextComposer::FontConfig newmenufc_high_text;
extern iTextComposer::FontConfig newmenufc_big_text;
extern iTextComposer::FontConfig newmenufc_lrg_text;
extern iTextComposer::FontConfig newmenufc_grt_text;
extern iTextComposer::FontConfig minsetfc;

extern iTextComposer::FontConfig badgefc;

extern iTextComposer::FontConfig castletitlefc;
extern iTextComposer::FontConfig castletopicfc;
extern iTextComposer::FontConfig castlebldfc;
extern iTextComposer::FontConfig castlemarketfc;
extern iTextComposer::FontConfig castleheronamefc;
extern iTextComposer::FontConfig herolevelfc;

extern iTextComposer::FontConfig secskillnamefc;
extern iTextComposer::FontConfig secskilllevelfc;

extern iTextComposer::FontConfig spellscrollfc;

extern iTextComposer::FontConfig grfc_tap;
extern iTextComposer::FontConfig grfc_title;
extern iTextComposer::FontConfig grfc_text;


extern iTextComposer::FontConfig ovrlndfc_herolvl;
extern iTextComposer::FontConfig ovrlndfc_heroskills;
extern iTextComposer::FontConfig ovrlndfc_infobar;
extern iTextComposer::FontConfig ovrlndfc_infobar_small;
extern iTextComposer::FontConfig ovrlndfc_heroexp;


extern iGradient defGrad;

void InitFonts();

//
iStringT FormatShortNumber(sint32 val);
iStringT FormatNumber(sint32 val, bool bForseSign = false);

iStringT Mineral2Text(MINERAL_TYPE mt, sint32 quant, bool bBig = false);
iStringT MineralSet2Text(const iMineralSet& ms, bool bBig = false);
iStringT MineralSet2TextRelative(const iMineralSet& ms, const iMineralSet& of);
iStringT CreatQuant2Text( sint32 q, VISION_LEVEL vl);
iStringT CreatGroup2Text( CREATURE_TYPE ct, sint32 q, VISION_LEVEL vl);

void Tile3x3Sprite(iDib& dib, const iRect& rc, SpriteId sid);

void FrameRoundRect(iDib& surf, const iRect& rect, iDib::pixel clr);
void DrawRoundRect(iDib& surf, const iRect& rect, iDib::pixel fClr, iDib::pixel bClr);
void ComposeDlgBkgnd(iDib& surf, const iRect& rc, bool bTransp = true);
void ComposeDlgBkgndPanel(iDib& surf, const iRect& rect, bool bTransp = true);
void ComposeDlgBkgndGrid(iDib& surf, const iRect& rect, PLAYER_ID pid, bool bDecs, iSize sz);

void BlitIcon(iDib& dib, SpriteId sid, const iRect& orc, uint8 a=255, const iPoint& offset = iPoint(), bool bHighlight = false); // alpha - 0-255
void ButtonFrame(iDib& dib, const iRect& rc, uint32 state);
void ComposeIconButton(iDib& dib, const iRect& rc, uint32 state, SpriteId sid);
void ComposeProgressBar(bool bVertical, iDib& dib, const iRect& rc, iDib::pixel clr, sint32 cval, sint32 mval);

const iTextComposer::FontConfig& GetButtonFont(uint32 state);
void ComposeDlgButton(iDib& dib, const iRect& rc, uint32 state);
void ComposeDlgTextButton(iDib& dib, const iRect& rc, uint32 state, TextResId textKey);
void ComposeDlgIconButton(iDib& dib, const iRect& rc, uint32 state, SpriteId sid);
void ComposeDlgTab(iDib& dib, const iRect& rc, uint32 state, SpriteId sid);

void ComposeCreatureIcon(iDib& surf, const iRect& rc, CREATURE_TYPE ct, bool bHighlighted, bool bFlipped);
void ComposeCreatureButton(iDib& surf, const iRect& rc, CREATURE_TYPE ct, sint32 cnt, VISION_LEVEL vl, bool bPressed, bool bDrag, bool bDrop);
void ComposeCreatureButtonWithText(iDib& surf, const iRect& rc, CREATURE_TYPE ct, iStringT text, VISION_LEVEL vl, bool bPressed, bool bDrag, bool bDrop);
void ComposeCreatureCell(iDib& surf, const iRect& rc, CREATURE_TYPE ct, sint32 cnt, VISION_LEVEL vl, bool bBack = true);
iSize CreatureCellSize();
iSize CalcArmyCellsSize(const iSize& cellSize);
void ComposeArmyCells(iDib& surf, const iPoint& pos, const iSize& cellSize, const iArmy& army, VISION_LEVEL vl);
void ComposeArmyCellsRow(iDib& surf, const iPoint& pos, const iSize& cellSize, const iArmy& army, VISION_LEVEL vl, bool bBack = true);
void ComposeGuard(iDib& surf, const iPoint& pos, const iSize& cellSize, const iArmy& army, VISION_LEVEL vl, bool bBack = true);
void ComposeSpellLevel(iDib& surf, const iRect& rect, MAGIC_SPELL spell, const iHero* pCaster);

void ComposeMoraleGlyph(iDib& surf, const iRect& orc, sint8 val, bool bHighlight, bool bBig = true);
void ComposeLuckGlyph(iDib& surf, const iRect& orc, sint8 val, bool bHighlight, bool bBig = true);

void FillStaredRect(iDib& surf, const iRect& rect, const iPoint& anchor);

iPoint GetRealSpritePos( SpriteId sid, const iRect& rc, bool bFlipped, Alignment al );

void ComposeGlowBkgnd(iDib& surf, const iRect& rect, bool bTransp = true, bool bTopCorners = true);
void ComposeGlowBkgndGrid(iDib& surf, const iRect& rect, iSize sz);
void ComposeGlowTextButton(iDib& dib, const iRect& rc, uint32 state, TextResId textKey);
sint32 ComposeHeroAvatar(iDib& surf, const iRect& rc, const iHero* pHero, bool bPressed = false, bool bMoveBar = false, bool bManaBar = false);
sint32 ComposeHeroLevel(iDib& surf, const iRect& rc, const iHero *pHero, bool bHighlight = false);
sint32 ComposeHeroTitleName(iDib& surf, const iRect& rc, const iHero* pHero);
sint32 ComposeHeroAvatarTitleName(iDib& surf, const iRect& rc, const iHero* pHero, bool bPressed, bool bMoveBar, bool bManaBar);
sint32 ComposeHeroAvatarName(iDib& surf, const iRect& rc, const iHero* pHero, bool bPressed, bool bMoveBar, bool bManaBar);
sint32 ComposeHeroAvatarLevel(iDib& surf, const iRect& rc, const iHero* pHero);
void ComposeResourceIcon(MINERAL_TYPE mineral, const iRect &rc, bool bPressed, bool bEnabled, const iStringT& text, bool bGlow, bool bDeluxe = false, bool bMini = false);
iSize GetMineralSetComposeSize( const iMineralSet& cost, bool bGlow, bool bMini = false, bool bZeroAlso = false);
void ComposeMineralSet( const iMineralSet& cost, const iMineralSet& resources, const iRect& rc, bool bGlow, bool bShorten = false, bool bDeluxe = false, bool bMini = false, bool bZeroAlso = false);
sint32 ComposeChildScreenTitle(iDib& surf, const iRect& rc, const iStringT& text);
sint32 ComposeHeroTitleName(iDib& surf, const iRect& rc, const iHero* pHero);
void ComposeResourcesBarGlow(const iRect& rc, const iMineralSet& ms);

#endif //PHEROES_GAME_GFXHLP_H_
