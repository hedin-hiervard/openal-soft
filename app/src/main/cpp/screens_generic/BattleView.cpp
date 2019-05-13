#include "stdafx.h"

#ifdef _DEEP_DEBUG_
#	define new DEBUG_NEW
#endif


#include "BattleView.h"
#include "BattleAI.h"
#include "Dlg_BattleResult.h"
#include "Dlg_BattleHero.h"
#include "Dlg_BattleCastle.h"
#include "Dlg_SurrenderCond.h"
#include "Dlg_TownList.h"
#include "Dlg_BattleLog.h"
#include "Dlg_Spellbook.h"
#include "Dlg_Settings.h"
#include "Dlg_ChooseBattleGroup.h"
#include "GameMenuDlg.h"
#include "Dlg_Save.h"
#include "Dlg_ScenProps.h"
//#include "xxc/xxc.security.h"
#include "OverlandView.h"
#include "FileAccessor/FileAccessor.h"

using namespace fileaccessor;

const fix32 CELL_EVT_DELAY = fix32(2.0f);
const fix32 HIGHLIGHT_BLINK_DELAY = fix32(0.5f);

uint16 iBattleView::sm_hexWidth;
uint16 iBattleView::sm_hexHalfWidth;
uint16 iBattleView::sm_hexHeight;
uint8 iBattleView::sm_gridZoom;
uint16 iBattleView::sm_backOffset;

const iDib::pixel SKY_GRAD_COLORS[STYPE_COUNT][2] = {
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Water
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Sand
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Dirt
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Grass
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Swamp
	{RGB16(16, 0, 0), RGB16(255, 0, 0) },			// Lava
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Wasteland
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Desert
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Snow
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// New Desert
	{RGB16(255, 255, 255), RGB16(64, 192, 255) },	// Pavement
	{RGB16(255, 255, 255), RGB16(64, 192, 255) }	// New Wasteland
};

const iDib::pixel TOOLTIP_COLORS[STYPE_COUNT][2] = {
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Water
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Sand
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Dirt
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Grass
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Swamp
	{RGB16(16, 0, 0), RGB16(255, 128, 128) },		// Lava
	{RGB16(0, 64, 128), RGB16(255, 255, 255)},		// Wasteland
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Desert
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Snow
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// New Desert
	{RGB16(32, 32, 96), RGB16(220, 220, 255) },		// Pavement
	{RGB16(0, 64, 128), RGB16(255, 255, 255)}		// New Wasteland
};

struct iCtlDecDesc {
	SpriteId	sid;
	iPoint		offset;
	bool		bGround;
};

// Fort decorations [iCastleFort::ElementType][iCastleFort::ElementState]
#ifdef PC_VERSION
const iCtlDecDesc FORT_DECS[iCastleFort::TypeCount][iCastleFort::StateCount] = {
	{
		{PDGG(COMBAT_UTOWER), iPoint(-30, 8), false}, {PDGG(COMBAT_UTURRET), iPoint(-30, 8), false}, {PDGG(COMBAT_UTOWER) + 1, iPoint(-30, 8), false}, {PDGG(COMBAT_UTOWER) + 2, iPoint(-30, 8),false}
	},{
		{PDGG(COMBAT_UWALL), iPoint(-34, 15), false}, {PDGG(COMBAT_UWALL),  iPoint(-34, 15), false}, {PDGG(COMBAT_UWALL) + 1,  iPoint(-34, 15), false}, {PDGG(COMBAT_UWALL) + 2,  iPoint(-34, 15), false}
	},{
		{PDGG(COMBAT_UTOWER), iPoint(-11, 8), false}, {PDGG(COMBAT_UTOWER) , iPoint(-11, 8), false}, {PDGG(COMBAT_UTOWER) + 1, iPoint(-11, 8),  false}, {PDGG(COMBAT_UTOWER) + 2, iPoint(-11, 8), false}
	},{
		{PDGG(COMBAT_UWALL), iPoint(-25, 16), false}, {PDGG(COMBAT_UWALL), iPoint(-25, 16), false}, {PDGG(COMBAT_UWALL) + 1, iPoint(-25, 16), false}, {PDGG(COMBAT_UWALL) + 2, iPoint(-25, 16), false}
	},{
		{PDGG(COMBAT_GUTOWER), iPoint(12, 0), false}, {PDGG(COMBAT_GUTOWER), iPoint(12, 0), false}, {PDGG(COMBAT_GUTOWER), iPoint(12, 0), false}, {PDGG(COMBAT_GUTOWER), iPoint(12, 0), false}
	},{
		{PDGG(COMBAT_BRIDGE), iPoint(-20, -10), false}, {PDGG(COMBAT_BRIDGE) + 1,  iPoint(-76, 11), true}, {PDGG(COMBAT_BRIDGE),  iPoint(-20, -10),true}, {PDGG(COMBAT_BRIDGE) + 2,  iPoint(-76, 11),true}
	},{
		{PDGG(COMBAT_UTOWER),  iPoint(-5, 0), false}, {PDGG(COMBAT_UTURRET),  iPoint(-5, 0), false}, {PDGG(COMBAT_UTOWER) + 1,  iPoint(-5, 0), false}, {PDGG(COMBAT_UTOWER) + 2,  iPoint(-5, 0), false}
	},{
		{PDGG(COMBAT_GBTOWER),  iPoint(14, -25), false}, {PDGG(COMBAT_GBTOWER),  iPoint(14, -25), false}, {PDGG(COMBAT_GBTOWER),  iPoint(14, -25), false}, {PDGG(COMBAT_GBTOWER),  iPoint(14, -25), false}
	},{
		{PDGG(COMBAT_BWALL), iPoint(-5, 2), false}, {PDGG(COMBAT_BWALL), iPoint(-5, 2), false}, {PDGG(COMBAT_BWALL) + 1, iPoint(-5, 2), false}, {PDGG(COMBAT_BWALL) + 2, iPoint(-5, 2), false}
	},{
		{PDGG(COMBAT_BTOWER), iPoint(-25, -9), false}, {PDGG(COMBAT_BTOWER),  iPoint(-25, -9), false}, {PDGG(COMBAT_BTOWER) + 1,  iPoint(-25, -9), false}, {PDGG(COMBAT_BTOWER) + 2,  iPoint(-25, -9), false}
	},{
		{PDGG(COMBAT_BWALL), iPoint(-25, -3), false}, {PDGG(COMBAT_BWALL),  iPoint(-25, -3), false}, {PDGG(COMBAT_BWALL) + 1,  iPoint(-25, -3), false}, {PDGG(COMBAT_BWALL) + 2,  iPoint(-25, -3), false}
	},{
		{PDGG(COMBAT_BTOWER), iPoint(-33, -2), false}, {PDGG(COMBAT_BTURRET), iPoint(-33, -2), false}, {PDGG(COMBAT_BTOWER) + 1, iPoint(-33, -2), false}, {PDGG(COMBAT_BTOWER) + 2, iPoint(-33, -2), false}
	}
};
#else

const iCtlDecDesc FORT_DECS_IPHONE[iCastleFort::TypeCount][iCastleFort::StateCount] = {
    {
        {PDGG_IPHONE(COMBAT_UTOWER), iPoint(-3, 43), false}, {PDGG_IPHONE(COMBAT_UTURRET), iPoint(-3, 43), false}, {PDGG_IPHONE(COMBAT_UTOWER) + 1, iPoint(-3, 43), false}, {PDGG_IPHONE(COMBAT_UTOWER) + 2, iPoint(-3, 43),false}
    },{
        {PDGG_IPHONE(COMBAT_UWALL), iPoint(-23, 39), false}, {PDGG_IPHONE(COMBAT_UWALL),  iPoint(-23, 39), false}, {PDGG_IPHONE(COMBAT_UWALL) + 1,  iPoint(-23, 39), false}, {PDGG_IPHONE(COMBAT_UWALL) + 2,  iPoint(-23, 39), false}
    },{
        {PDGG_IPHONE(COMBAT_UTOWER), iPoint(1, 52), false}, {PDGG_IPHONE(COMBAT_UTOWER) , iPoint(1, 52), false}, {PDGG_IPHONE(COMBAT_UTOWER) + 1, iPoint(1, 52),  false}, {PDGG_IPHONE(COMBAT_UTOWER) + 2, iPoint(1, 52), false}
    },{
        {PDGG_IPHONE(COMBAT_UWALL), iPoint(-18, 48), false}, {PDGG_IPHONE(COMBAT_UWALL), iPoint(-18, 48), false}, {PDGG_IPHONE(COMBAT_UWALL) + 1, iPoint(-18, 48), false}, {PDGG_IPHONE(COMBAT_UWALL) + 2, iPoint(-18, 48), false}
    },{
        {PDGG_IPHONE(COMBAT_GUTOWER), iPoint(10, 10), false}, {PDGG_IPHONE(COMBAT_GUTOWER), iPoint(10, 10), false}, {PDGG_IPHONE(COMBAT_GUTOWER), iPoint(10, 10), false}, {PDGG_IPHONE(COMBAT_GUTOWER), iPoint(10, 10), false}
    },{
        {PDGG_IPHONE(COMBAT_BRIDGE), iPoint(-30, -8), false}, {PDGG_IPHONE(COMBAT_BRIDGE) + 1,  iPoint(-50, 11), true}, {PDGG_IPHONE(COMBAT_BRIDGE),  iPoint(-30, -8),true}, {PDGG_IPHONE(COMBAT_BRIDGE) + 2,  iPoint(-50, 11),true}
    },{
        {PDGG_IPHONE(COMBAT_UTOWER),  iPoint(-2, 40), false}, {PDGG_IPHONE(COMBAT_UTURRET),  iPoint(-2, 40), false}, {PDGG_IPHONE(COMBAT_UTOWER) + 1,  iPoint(-2, 40), false}, {PDGG_IPHONE(COMBAT_UTOWER) + 2,  iPoint(-2, 40), false}
    },{
        {PDGG_IPHONE(COMBAT_GBTOWER),  iPoint(14, 0), false}, {PDGG_IPHONE(COMBAT_GBTOWER),  iPoint(14, 0), false}, {PDGG_IPHONE(COMBAT_GBTOWER),  iPoint(14, 0), false}, {PDGG_IPHONE(COMBAT_GBTOWER),  iPoint(14, 0), false}
    },{
        {PDGG_IPHONE(COMBAT_BWALL), iPoint(3, 33), false}, {PDGG_IPHONE(COMBAT_BWALL), iPoint(3, 33), false}, {PDGG_IPHONE(COMBAT_BWALL) + 1, iPoint(3, 33), false}, {PDGG_IPHONE(COMBAT_BWALL) + 2, iPoint(3, 33), false}
    },{
        {PDGG_IPHONE(COMBAT_BTOWER), iPoint(5, 33), false}, {PDGG_IPHONE(COMBAT_BTOWER),  iPoint(5, 33), false}, {PDGG_IPHONE(COMBAT_BTOWER) + 1,  iPoint(5, 33), false}, {PDGG_IPHONE(COMBAT_BTOWER) + 2,  iPoint(5, 33), false}
    },{
        {PDGG_IPHONE(COMBAT_BWALL), iPoint(-6, 28), false}, {PDGG_IPHONE(COMBAT_BWALL),  iPoint(-6, 28), false}, {PDGG_IPHONE(COMBAT_BWALL) + 1,  iPoint(-6, 28), false}, {PDGG_IPHONE(COMBAT_BWALL) + 2,  iPoint(-6, 28), false}
    },{
        {PDGG_IPHONE(COMBAT_BTOWER), iPoint(0, 40), false}, {PDGG_IPHONE(COMBAT_BTURRET), iPoint(0, 40), false}, {PDGG_IPHONE(COMBAT_BTOWER) + 1, iPoint(0, 40), false}, {PDGG_IPHONE(COMBAT_BTOWER) + 2, iPoint(0, 40), false}
    }
};
const iCtlDecDesc FORT_DECS_IPAD[iCastleFort::TypeCount][iCastleFort::StateCount] = {
    {
        {PDGG_IPAD(COMBAT_UTOWER), iPoint(-30, 8), false}, {PDGG_IPAD(COMBAT_UTURRET), iPoint(-30, 8), false}, {PDGG_IPAD(COMBAT_UTOWER) + 1, iPoint(-30, 8), false}, {PDGG_IPAD(COMBAT_UTOWER) + 2, iPoint(-30, 8),false}
    },{
        {PDGG_IPAD(COMBAT_UWALL), iPoint(-34, 15), false}, {PDGG_IPAD(COMBAT_UWALL),  iPoint(-34, 15), false}, {PDGG_IPAD(COMBAT_UWALL) + 1,  iPoint(-34, 15), false}, {PDGG_IPAD(COMBAT_UWALL) + 2,  iPoint(-34, 15), false}
    },{
        {PDGG_IPAD(COMBAT_UTOWER), iPoint(-11, 8), false}, {PDGG_IPAD(COMBAT_UTOWER) , iPoint(-11, 8), false}, {PDGG_IPAD(COMBAT_UTOWER) + 1, iPoint(-11, 8),  false}, {PDGG_IPAD(COMBAT_UTOWER) + 2, iPoint(-11, 8), false}
    },{
        {PDGG_IPAD(COMBAT_UWALL), iPoint(-25, 16), false}, {PDGG_IPAD(COMBAT_UWALL), iPoint(-25, 16), false}, {PDGG_IPAD(COMBAT_UWALL) + 1, iPoint(-25, 16), false}, {PDGG_IPAD(COMBAT_UWALL) + 2, iPoint(-25, 16), false}
    },{
        {PDGG_IPAD(COMBAT_GUTOWER), iPoint(12, 0), false}, {PDGG_IPAD(COMBAT_GUTOWER), iPoint(12, 0), false}, {PDGG_IPAD(COMBAT_GUTOWER), iPoint(12, 0), false}, {PDGG_IPAD(COMBAT_GUTOWER), iPoint(12, 0), false}
    },{
        {PDGG_IPAD(COMBAT_BRIDGE), iPoint(-20, -10), false}, {PDGG_IPAD(COMBAT_BRIDGE) + 1,  iPoint(-76, 11), true}, {PDGG_IPAD(COMBAT_BRIDGE),  iPoint(-20, -10),true}, {PDGG_IPAD(COMBAT_BRIDGE) + 2,  iPoint(-76, 11),true}
    },{
        {PDGG_IPAD(COMBAT_UTOWER),  iPoint(-5, 0), false}, {PDGG_IPAD(COMBAT_UTURRET),  iPoint(-5, 0), false}, {PDGG_IPAD(COMBAT_UTOWER) + 1,  iPoint(-5, 0), false}, {PDGG_IPAD(COMBAT_UTOWER) + 2,  iPoint(-5, 0), false}
    },{
        {PDGG_IPAD(COMBAT_GBTOWER),  iPoint(14, -25), false}, {PDGG_IPAD(COMBAT_GBTOWER),  iPoint(14, -25), false}, {PDGG_IPAD(COMBAT_GBTOWER),  iPoint(14, -25), false}, {PDGG_IPAD(COMBAT_GBTOWER),  iPoint(14, -25), false}
    },{
        {PDGG_IPAD(COMBAT_BWALL), iPoint(-5, 2), false}, {PDGG_IPAD(COMBAT_BWALL), iPoint(-5, 2), false}, {PDGG_IPAD(COMBAT_BWALL) + 1, iPoint(-5, 2), false}, {PDGG_IPAD(COMBAT_BWALL) + 2, iPoint(-5, 2), false}
    },{
        {PDGG_IPAD(COMBAT_BTOWER), iPoint(-25, -9), false}, {PDGG_IPAD(COMBAT_BTOWER),  iPoint(-25, -9), false}, {PDGG_IPAD(COMBAT_BTOWER) + 1,  iPoint(-25, -9), false}, {PDGG_IPAD(COMBAT_BTOWER) + 2,  iPoint(-25, -9), false}
    },{
        {PDGG_IPAD(COMBAT_BWALL), iPoint(-25, -3), false}, {PDGG_IPAD(COMBAT_BWALL),  iPoint(-25, -3), false}, {PDGG_IPAD(COMBAT_BWALL) + 1,  iPoint(-25, -3), false}, {PDGG_IPAD(COMBAT_BWALL) + 2,  iPoint(-25, -3), false}
    },{
        {PDGG_IPAD(COMBAT_BTOWER), iPoint(-33, -2), false}, {PDGG_IPAD(COMBAT_BTURRET), iPoint(-33, -2), false}, {PDGG_IPAD(COMBAT_BTOWER) + 1, iPoint(-33, -2), false}, {PDGG_IPAD(COMBAT_BTOWER) + 2, iPoint(-33, -2), false}
    }
};

#endif

/*
*	Helpers and constants
*  TODO:: PRE: Hides declaration - need to rename const
*/

iPoint iBattleView::anchor;

const fix32 ACT_STEP = fix32(0.4f);
/*
*	Creature information popup view
*/
iCreatInfoPopup::iCreatInfoPopup(iViewMgr* pViewMgr, PLAYER_ID pid, const iBattleGroup* pGroup)
: iFramePopupView(pViewMgr, pid), m_pGroup(pGroup)
{
}

void iCreatInfoPopup::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	CREATURE_TYPE ct = m_pGroup->Type();
	sint32 cnt = m_pGroup->Count();


	// title
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(),gTextMgr[ct*3+TRID_CREATURE_PEASANT_F2], iRect(rc.x,rc.y,rc.w,15));

	// icon
	rc.y+=45;

	// Perks
	iSimpleArray<uint16>	perks;
	for (uint32 nn=0; nn<CPERKS_COUNT; ++nn) {
		if (CREAT_DESC[ct].perks & (1<<nn)) perks.Add(nn);
	}

	sint32 tw = perks.GetSize() * 15 - 1;
	sint32 ox = rc.x + ((sint32)(rc.w/2) - tw/2);
	for (uint32 nn=0; nn<perks.GetSize(); ++nn) {
		gGfxMgr.Blit(PDGG(CREAT_PERKS) + perks[nn], gApp.Surface(), iPoint(ox, rc.y));
		ox += 15;
	}
	rc.y += 15;
	// specs
	iTextComposer::FontConfig fc(dlgfc_splain.fontSize, RGB16(192,192,220));
	const sint32 lh = gTextComposer.GetFontHeight(fc.fontSize) * 3 / 2;
	iRect trc(rc.x,rc.y,68,lh);

	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_QUANT_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_ATTACK_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_DEFENCE_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;
	if (m_pGroup->CanShoot()) { gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_SHOTS_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh; }
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_DAMAGE_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_HEALTH_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_SPEED_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_MORALE_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iStringT(gTextMgr[TRID_SKILL_LUCK_S])+_T(" :"),trc,AlignTopRight); trc.y+=lh;

	fc.cmpProps.faceColor = RGB16(220,220,160);
	trc = iRect(rc.x+70,rc.y,rc.w-40,lh);
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),FormatNumber(cnt),trc,AlignTopLeft); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iFormat(_T("%d (%d)"),CREAT_DESC[ct].attack,m_pGroup->AttackSkill()),trc,AlignTopLeft); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iFormat(_T("%d (%d)"),CREAT_DESC[ct].defence,m_pGroup->DefenceSkill()),trc,AlignTopLeft); trc.y+=lh;
	if (m_pGroup->CanShoot()) { gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iFormat(_T("%d/%d"),m_pGroup->Shots(),CREAT_DESC[ct].shots),trc,AlignTopLeft); trc.y+=lh; }
	if (m_pGroup->MinDamage() == m_pGroup->MaxDamage()) gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),FormatNumber(m_pGroup->MinDamage()),trc,AlignTopLeft);
	else gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iFormat(_T("%d-%d"),m_pGroup->MinDamage(),m_pGroup->MaxDamage()),trc,AlignTopLeft);
	trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iFormat(_T("%d/%d"),m_pGroup->HitPointsLeft(),m_pGroup->HitPoints()),trc,AlignTopLeft); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),iFormat(_T("%d (%d)"),CREAT_DESC[ct].speed,m_pGroup->Speed()),trc,AlignTopLeft); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),FormatNumber(m_pGroup->Morale()),trc,AlignTopLeft); trc.y+=lh;
	gTextComposer.TextOut(fc, gApp.Surface(),trc.point(),FormatNumber(m_pGroup->Luck()),trc,AlignTopLeft); trc.y+=lh;

	// spells
	if (m_pGroup->SpellList().GetSize()) {
		fc.cmpProps.faceColor = RGB16(192,192,192);
		rc.y = trc.y + lh;
		uint32 cnt = iMIN<uint32>(5,m_pGroup->SpellList().GetSize());
		for (uint32 xx=0; xx<cnt; ++xx) {
			if (m_pGroup->SpellList()[xx].m_duration == iBattleGroup::iSpellEntry::DurInfinit) gTextComposer.TextOut(fc, gApp.Surface(),rc.point(),iFormat(_T("%s (-)"), gTextMgr[m_pGroup->SpellList()[xx].m_pSpell->SpellLabel()]),rc,AlignTop);
			else gTextComposer.TextOut(fc, gApp.Surface(),rc.point(),iFormat(_T("%s (%d)"), gTextMgr[m_pGroup->SpellList()[xx].m_pSpell->SpellLabel()], m_pGroup->SpellList()[xx].m_duration),rc,AlignTop);
			rc.y+=lh;
		}
	}

}

iSize iCreatInfoPopup::ClientSize()
{
	sint32 w = 120;
	sint32 h = 0;

	// title
	h += gTextComposer.GetTextBoxSize(gTextMgr[m_pGroup->Type()*3+TRID_CREATURE_PEASANT_F2], 80, dlgfc_topic).h;

	// icon
	h += 45;

	// perks
	h += 15;

	// specs
	const uint32 mult = gTextComposer.GetFontHeight(dlgfc_topic.fontSize) * 3 / 2;
	h += 8 * mult;
	if (m_pGroup->CanShoot()) h += mult;

	// spells
	if (m_pGroup->SpellList().GetSize()) {
		h += mult + iMIN<sint32>(5, m_pGroup->SpellList().GetSize()) * mult;
	}

	//
	h += 5;
	return iSize(w,h);
}

iPoint iBattleView::CreatPos(iPoint anchor, iBattleGroup::ORIENT orient, uint8 size)
{
	iPoint cop = anchor;

	cop.y -= gGfxMgr.Dimension(PDGG(CREAT_PEASANT)).h / 2;
	if(orient == iBattleGroup::Left)
		cop.x -= gGfxMgr.Dimension(PDGG(CREAT_PEASANT)).w;

	cop += iPoint(iBattleView::sm_hexHalfWidth, iBattleView::sm_hexHeight / 2 - 20	);
	return cop;
}


iPoint iBattleView::CreatCorpsePos(iPoint anchor, iBattleGroup::ORIENT orient, uint8 size)
{
	iPoint cop = anchor;

	cop.y -= 50;

	if(size == 2) {
		if (orient == iBattleGroup::Right)
			cop.x -= 0;//11;
		else
			cop.x += 25;
	}
	return cop;
}


/*
*	Battle composer
*/
struct iBatCmpItem {
	iBatCmpItem(const iPoint& pos, bool bCurrent) : m_pos(pos), m_bCurrent(bCurrent) {}

	virtual void ComposeShadow(iDib& surf, const iPoint& anchor) const  =0;
	virtual void Compose(iDib& surf, const iPoint& anchor, bool bInt) const  =0;

	iPoint	m_pos;
	bool	m_bCurrent;
};
//

struct iBatCmpItem_Creature : public iBatCmpItem {
	iBatCmpItem_Creature(const iBattleGroup* pGroup, bool bCurrent, bool bHighlighted, bool bCropped) :
iBatCmpItem(pGroup->ScreenPos() != cInvalidPoint ? pGroup->ScreenPos() :
			iBattleView::Map2Screen(pGroup->Pos()), bCurrent), m_pGroup(pGroup), m_bHighlighted(bHighlighted), m_bCropped(bCropped) {}

void ComposeShadow(iDib& surf, const iPoint& anchor) const
{
	sint16 ct = m_pGroup->Type();
	iPoint cop;
	if (m_pGroup->IsAlive()) {

		uint16 sid = CREAT_DESC[ct].sprite;
		cop = GetRealSpritePos(sid, iRect(anchor, iSize(iBattleView::sm_hexWidth, iBattleView::sm_hexHeight / 2)), m_pGroup->Orient() == (iBattleGroup::Left), AlignBottom);

		// Draw group
		sint32 frames = CREAT_DESC[ct].movAnim == ANIM_6 ? 7 : 19;
		if (m_pGroup->State() == iBattleGroup::Melee || m_pGroup->State()==iBattleGroup::Shooting) sid += frames;
		else if (m_pGroup->State() == iBattleGroup::RecDamage) sid += frames * 2;
		else sid += m_pGroup->Frame();

		iRect objrc = gGfxMgr.ObjRect(sid);
		iRect srcrect = iRect(0, 0, gGfxMgr.Dimension(sid).w, objrc.y2());

		if(m_bCropped) {
			srcrect.h -= 12;
			cop += iPoint(6, -6);
		}

		gGfxMgr.BlitUniversal(sid, surf, cop , srcrect, iGfxManager::EfxShadowBattle, false, -1, 255, (m_pGroup->Orient() == (iBattleGroup::Right)) ? iGfxManager::FlipNone : iGfxManager::FlipVert );
	} else {
		uint16 sid = CREAT_DESC[ct].sprite;

		cop = GetRealSpritePos(sid, iRect(anchor, iSize(iBattleView::sm_hexWidth, iBattleView::sm_hexHeight / 2)), false, AlignBottom);
		sint32 frames = CREAT_DESC[ct].movAnim == ANIM_6 ? 7 : 19;
		sid += frames * 3;

		iRect objrc = gGfxMgr.ObjRect(sid);
		iRect srcrect = iRect(0, 0, gGfxMgr.Dimension(sid).w, objrc.y2());

		if(m_bCropped)
			srcrect.h -= 6;

		// Draw group
		gGfxMgr.Blit(sid, surf, srcrect, cop);
	}
}

void Compose(iDib& surf, const iPoint& anchor, bool bInt) const
{
	sint16 ct = m_pGroup->Type();
	if (!m_pGroup->IsAlive()) return;

	uint16 sid = CREAT_DESC[ct].sprite;
	iPoint cop = GetRealSpritePos(sid, iRect(anchor, iSize(iBattleView::sm_hexWidth, iBattleView::sm_hexHeight / 2)), m_pGroup->Orient() == (iBattleGroup::Left), AlignBottom);
		//iBattleView::CreatPos(anchor, m_pGroup->Orient(), m_pGroup->Size());

	// Draw group
	sint32 frames = CREAT_DESC[ct].movAnim == ANIM_6 ? 7 : 19;
	if (m_pGroup->State() == iBattleGroup::Melee || m_pGroup->State()==iBattleGroup::Shooting) sid += frames;
	else if (m_pGroup->State() == iBattleGroup::RecDamage) sid += frames * 2;
	else sid += m_pGroup->Frame();

	iRect objrc = gGfxMgr.ObjRect(sid);
	iRect srcrect = iRect(0, 0, gGfxMgr.Dimension(sid).w, objrc.y2());

	if(m_bCropped)
		srcrect.h -= 6;

	gGfxMgr.BlitUniversal(sid, surf, cop, srcrect, m_bHighlighted ? iGfxManager::EfxGlow : iGfxManager::EfxNone, false, -1, 255, m_pGroup->Orient() == (iBattleGroup::Right) ? iGfxManager::FlipNone : iGfxManager::FlipVert);

	// Draw current group edging
	if (bInt && m_bCurrent && m_pGroup->State() == iBattleGroup::Idle) {
		gGfxMgr.BlitUniversal(sid + frames * 4, surf, cop, cInvalidRect, iGfxManager::EfxNone, false, -1, 255, m_pGroup->Orient() == (iBattleGroup::Right) ? iGfxManager::FlipNone : iGfxManager::FlipVert);
	}

	iPoint p = anchor + iPoint(iBattleView::sm_hexHalfWidth - 15, iBattleView::sm_hexHeight / 2 - 8);
	// Draw group's quantity
	if(m_pGroup->State() == iBattleGroup::Idle) {
		iRect rc(p.x+1,p.y+9,30,16);
		SPELL_DISPOS sd = m_pGroup->SpellsDispos();
		if(m_pGroup->Orient() == (iBattleGroup::Left))
			rc.x--;
		if (sd == SPD_NEUTRAL) surf.VGradientRect(rc,RGB16(140,140,46),RGB16(58,58,0));
		else if (sd == SPD_POSITIVE) surf.VGradientRect(rc,RGB16(56,160,22),RGB16(32,64,16));
		else if (sd == SPD_NEGATIVE) surf.VGradientRect(rc,RGB16(192,76,32),RGB16(80,32,16));
		else surf.VGradientRect(rc,RGB16(120,76,160),RGB16(44,8,80));

		//if (composeList[xx].value->PassMap().GetAt(composeList[xx].value->Pos().x+TAIL_OFFSET[!composeList[xx].value->Orient()], composeList[xx].value->Pos().y) == CT_PASSABLE) rc.x += TAIL_OFFSET[!composeList[xx].value->Orient()] * 20;
		/*if (m_pGroup->InMoat()) surf.VGradientRect(rc,RGB16(192,76,32),RGB16(80,32,16));
		else */

		surf.FrameRect(rc,RGB16(208,176,28));
		gTextComposer.TextOut(creatqfc, surf, rc, FormatShortNumber(m_pGroup->Count()), rc, AlignCenter);
	}

	// receive damage glyph
	if (m_pGroup->State() == iBattleGroup::RecDamage) gGfxMgr.Blit(PDGG(BTL_RECDMG), surf, p);
	else if (m_pGroup->State() == iBattleGroup::ResistSpell) gGfxMgr.Blit(PDGG(BTL_RESIST_SPELL), surf, p);
}

const iBattleGroup*	m_pGroup;
bool	m_bHighlighted;
bool	m_bCropped;
};
//
struct iBatCmpItem_Obstacle : public iBatCmpItem {
	iBatCmpItem_Obstacle(const iBatObstEntry* pObstacle) : iBatCmpItem(iBattleView::Map2Screen(pObstacle->cell), false), m_pObstacle(pObstacle) {}

	void ComposeShadow(iDib& surf, const iPoint& anchor) const
	{
		iPoint cop = anchor + gGame.ItemMgr().BatObstMgr().Item(m_pObstacle->obstId).offset;
		cop.x -= 15;
		SpriteId sid = gGame.ItemMgr().BatObstMgr().Item(m_pObstacle->obstId).sprite;
		gGfxMgr.BlitEffect(sid, surf, cop,iGfxManager::EfxShadowBattle);
	}

	void Compose(iDib& surf, const iPoint& anchor, bool bInt) const
	{
		iPoint cop = anchor + gGame.ItemMgr().BatObstMgr().Item(m_pObstacle->obstId).offset;
		SpriteId sid = gGame.ItemMgr().BatObstMgr().Item(m_pObstacle->obstId).sprite;
		gGfxMgr.Blit(sid, surf, cop);
	}

	const iBatObstEntry*	m_pObstacle;
};
//

struct iBatCmpItem_Catapult : public iBatCmpItem {
	iBatCmpItem_Catapult(iBattleUnit_Catapult* pCatapult) : iBatCmpItem(iBattleView::Map2Screen(iPoint(1, 9)), false), m_pCatapult(pCatapult) {}

	void ComposeShadow(iDib& surf, const iPoint& anchor) const
	{
	}

	void Compose(iDib& surf, const iPoint& anchor, bool bInt) const
	{
		SpriteId sid = PDGG(SHOOT_AMMO);
		if(m_pCatapult->State() == iBattleUnit_Catapult::Shooting)
			sid++;
		gGfxMgr.Blit(sid, surf, GetRealSpritePos(sid, iRect(anchor, iSize(iBattleView::sm_hexWidth, iBattleView::sm_hexHeight / 2)), false, AlignBottom));
	}

	iBattleUnit_Catapult* m_pCatapult;
};

//
struct iBatCmpItem_Ammo : public iBatCmpItem {
	iBatCmpItem_Ammo(iAmmo& ammo) : iBatCmpItem(ammo.m_pos, false), m_ammo(ammo) {}

	void ComposeShadow(iDib& surf, const iPoint& anchor) const
	{
	}

	void Compose(iDib& surf, const iPoint& anchor, bool bInt) const
	{
		SpriteId sid = m_ammo.sid;
		if(m_ammo.reversed)
			gGfxMgr.BlitFlipped(sid, surf, anchor + iPoint(-100, 0));
		else
			gGfxMgr.Blit(sid, surf, anchor + iPoint(0, 0));
	}

	iAmmo &m_ammo;
};

//

struct iBatCmpItem_FortElement : public iBatCmpItem {
	iBatCmpItem_FortElement(const iCastleFort::iElement* pElement) : iBatCmpItem(iBattleView::Map2Screen(iPoint(iCastleFort::ElCells[pElement->type][0], iCastleFort::ElCells[pElement->type][1])), false), m_pElement(pElement) {}

	void ComposeShadow(iDib& surf, const iPoint& anchor) const
	{
        bool bGround;
        if(gUseIpadUI)
            bGround = FORT_DECS_IPAD[m_pElement->type][m_pElement->state].bGround;
        else
            bGround = FORT_DECS_IPHONE[m_pElement->type][m_pElement->state].bGround;

		if (bGround) {
            SpriteId sid;
            if(gUseIpadUI)
                sid = FORT_DECS_IPAD[m_pElement->type][m_pElement->state].sid;
            else
                sid = FORT_DECS_IPHONE[m_pElement->type][m_pElement->state].sid;

			gGfxMgr.Blit(sid, surf, iBattleView::FortElemPos(anchor, m_pElement->type, m_pElement->state));
		}
	}

	void Compose(iDib& surf, const iPoint& anchor, bool bInt) const
	{
		iPoint anch = anchor;

        SpriteId sid;
        bool bGround;

        if(gUseIpadUI)
            sid = FORT_DECS_IPAD[m_pElement->type][m_pElement->state].sid;
        else
            sid = FORT_DECS_IPHONE[m_pElement->type][m_pElement->state].sid;

        if(gUseIpadUI)
            bGround = FORT_DECS_IPAD[m_pElement->type][m_pElement->state].bGround;
        else
            bGround = FORT_DECS_IPHONE[m_pElement->type][m_pElement->state].bGround;


		if (!bGround) {
			gGfxMgr.Blit(sid, surf, iBattleView::FortElemPos(anch, m_pElement->type, m_pElement->state));
		}
		if (m_pElement->bRecDmg)
			gGfxMgr.Blit(PDGG(BTL_RECDMG), surf, iBattleView::FortElemHitPos(anch, m_pElement->type, m_pElement->state));
	}

	const iCastleFort::iElement*	m_pElement;
};

//
struct iBatCmpItem_Sprite : public iBatCmpItem {
	iBatCmpItem_Sprite(const iPoint& pos, SpriteId sid, const iPoint& offset) : iBatCmpItem(iBattleView::Map2Screen(pos), false), m_sprite(sid), m_offset(offset) {}

	void ComposeShadow(iDib& surf, const iPoint& anchor) const
	{
	}

	void Compose(iDib& surf, const iPoint& anchor, bool bInt) const
	{
		iPoint cop = anchor + m_offset;
		gGfxMgr.Blit(m_sprite, surf, cop);
	}

	SpriteId	m_sprite;
	iPoint		m_offset;
};


/*
*	Battle View
*/
iBattleView::iBattleView()
: iChildGameView(false, CV_OVERLAND)
, m_trackCell(cInvalidPoint), m_trackPos(cInvalidPoint)
, m_pBattle(NULL), m_bAni(false)
, m_pMeleeTrack(NULL), m_pShootTrack(NULL), m_pAutoBattleToolBar(NULL)
, m_pCreatInfoPopup(NULL), m_pCastSpellToolBar(NULL)
, m_bForceInfo(false), m_bHumanTurn(false)
, m_blinkTimer(fix32::zero), m_bHighlight(false)
, m_highlightedTarget(cInvalidPoint)
, m_bDontMove(true)
, m_MoveEntry(cInvalidPoint)
, m_highlightedGroup(cInvalidPoint)
#ifdef PC_VERSION
, m_bDefender(false)
, m_bBeginAutocombat(false)
, m_bEndAutocombat(false)
, m_Entered( cInvalidPoint )
, m_bWait(false)
, m_bSpell(false)
#endif
{
	Recalc(); // recalc anchors and positions
	AddChild(m_pAutoBattleToolBar = new iAutoBattleToolBar(m_pMgr, this, iRect(0, m_Rect.y2()-DEF_BTN_HEIGHT - 5,m_Rect.w, DEF_BTN_HEIGHT + 5)));
	m_pAutoBattleToolBar->SetVisible(false);
}


iBattleView::~iBattleView()
{ }


void iBattleView::Start()
{
	//	gSfxMgr.PlaySound(CSND_PREBATTLE);
	OnGroupChanged();
}

void iBattleView::EndBattle(iBattleResult br)
{
	// Prepare battle result
	m_pBattle->Engine().PrepareBattleResult(br);

#ifdef MULTIPLAYER
	gMPMgr.OnCombatEnd(br);
	gMPMgr.SetBattleWrapper(NULL);
#endif

	// Show result
	iDlg_BattleResult brd(m_pMgr, m_pBattle->Engine(), br);
	brd.DoModal();

	// End battle
	m_pBattle->EndBattle(br);

	// remove the cell events left
	m_cellEvents.DeleteAll();
}



bool iBattleView::BeginSpellTrack(iCombatSpell* pSpell)
{
	bool res = true;

    m_bTracking[0] = false;
	check(!m_spellTargets.GetSize());
	iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
	check(pCaster);
	MAGIC_SCHOOL_LEVEL msl = pSpell->GetSchoolLevel(pCaster);
	if (pSpell->TargetMode(msl) == STM_CREAT_GROUP) {
		m_spellTargets.RemoveAll();
		m_pBattle->Engine().GetValidSpellTargets(pSpell, msl, m_spellTargets);
		if (!m_spellTargets.GetSize()) res = false;
	} else {
		iSimpleArray<iBattleGroup*> targets;
		m_pBattle->Engine().SelectSpellTargets(pSpell, targets);
		if (!targets.GetSize()) res = false;
	}

	if(res)
		AddChild(m_pCastSpellToolBar = new iCastSpellToolBar(m_pMgr, this,
#ifdef PC_VERSION
                                                             iRect(0, m_Rect.y2()-DEF_BTN_HEIGHT - 5, m_Rect.w, DEF_BTN_HEIGHT + 5),
#else
                                                             iRect(),
#endif
                                                             pCaster,  pSpell));

    return res;
}

iBattleGroup* iBattleView::GetSpellTarget(const iPoint& cell) {
	iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
	check(pCaster);
	MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);

	iBattleMember *targetOwner  = NULL;
	uint16 tt = m_pCastSpellToolBar->Spell()->TargetType(msl);

	iBattleMember::Side ourSide = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->GetSide();
	iBattleMember::Side enemySide;
	if(ourSide == iBattleMember::Assaulter)
		enemySide = iBattleMember::Defender;
	else
		enemySide = iBattleMember::Assaulter;

	if(((tt & STT_FRIENDLY) == 0) && ((tt & STT_ENEMY) != 0))
		targetOwner =  m_pBattle->Engine().GetBattleInfo().Member(enemySide);
	else if (((tt & STT_FRIENDLY) != 0) && ((tt & STT_ENEMY) == 0))
		targetOwner =  m_pBattle->Engine().GetBattleInfo().Member(ourSide);
	//else leave NULL to select all

	iSimpleArray<iBattleGroup*> targets = m_pBattle->Engine().FindAllGroups(cell,
		targetOwner, m_pCastSpellToolBar->Spell()->AffectsToDead(), m_pCastSpellToolBar->Spell()->Id() == MSP_ANIMATEDEAD);

	//uncomment to test:
	//targets.Add(m_pBattle->Engine().AArmy().Groups().At(0));
	//targets.Add(m_pBattle->Engine().DArmy().Groups().At(0));

	iBattleGroup *pTarget = NULL;
	if(targets.GetSize() > 1) {
		uint32 idx;
		iSimpleArray<iStringT> texts;
		for(uint32 xx=0; xx<targets.GetSize(); xx++)
		{
			if(targets[xx]->IsAlive())
				texts.Add(iFormat<iCharT>(_T("%d"), targets[xx]->Count()));
			else if(m_pCastSpellToolBar->Spell()->SpellClass() == SPC_RESURRECT) {
				sint32 hp =  ((iSpell_Resurrect*)(m_pCastSpellToolBar->Spell()))->EstimateHitPoints(pCaster);
				texts.Add(iFormat<iCharT>(_T("%d/%d"),
					targets[xx]->CountResurrected(hp),
					targets[xx]->InitialCount()));
			}
			else
				texts.Add(iStringT(_T("N/A")));

		}
		iDlg_ChooseBattleGroup dlg(m_pMgr, targets, texts, pCaster->Owner()->PlayerId(), &idx);
		if(dlg.DoModal() == DRC_OK)
			pTarget = targets.At(idx);
		else
			pTarget = NULL;
	} else if(targets.GetSize() == 0)
		pTarget = NULL;
	else
		pTarget = targets.At(0);
	return pTarget;
}

void iBattleView::EndSpellTrack(const iPoint& cell)
{
	//check(m_pCastSpellToolBar);
	if (cell != cInvalidPoint) {
		iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
		check(pCaster);
		MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);

		if (m_pCastSpellToolBar->Spell()->TargetMode(msl) != STM_CREAT_GROUP || m_spellTargets.Find(cell) != -1) {
			iBattleGroup* pTarget = GetSpellTarget(cell);
			if (pTarget || m_pCastSpellToolBar->Spell()->TargetMode(msl) != STM_CREAT_GROUP)
				if(!m_pBattle->Engine().CastSpell(m_pCastSpellToolBar->Spell(), cell, pTarget)) {
					iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_SPELL_AFFECT_NO_ONE], pCaster->Owner()->PlayerId());
					tdlg.DoModal();
				} else BeginAni();
		}
	}
	if(m_pCastSpellToolBar) {
		RemoveChild(m_pCastSpellToolBar);
		delete m_pCastSpellToolBar;
		m_pCastSpellToolBar = NULL;
	}

	m_toolTip = _T("");
	m_spellTargets.RemoveAll();
	EnableControls(Nothing);
}

bool iBattleView::OnGroupChanged()
{
	iBattleUnit* pCurUnit = m_pBattle->Engine().TurnSeq().CurUnit();

	if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit)) {
		iBattleGroup* pGroup  = pCurCreatGroup->GetCreatGroup();
		// First check morale
		if (pGroup->RoundMorale() == iBattleGroup::MLM_NEGATIVE) {
			m_pBattle->Engine().BadMorale();
			return true;
		}

		PLAYER_ID pid = pGroup->Owner()->Owner();
		bool bAiPlayer = (pid == PID_NEUTRAL || gGame.Map().FindPlayer(pid)->PlayerType() == PT_COMPUTER);
#ifdef MULTIPLAYER
		bool bRemPlayer = (pid != PID_NEUTRAL && gGame.Map().FindPlayer(pid)->PlayerType() == PT_REMOTE) ||
			(pid != PID_NEUTRAL && gMPMgr.IsPRA() && gGame.Map().FindPlayer(pid)->PlayerType() == PT_COMPUTER);
		if(gMPMgr.IsReplaying()) {
			m_pToolBar->EnableControls(RemoteTurn);

		} else if(bRemPlayer) {
			m_bHumanTurn = false;
			EnableControls(RemoteTurn);
		} else
#endif

			// Cotulla: autobattle add
			if (m_pAutoBattleToolBar)
			{
				if (IsAnyAutobattle() && (IsAutobattle() || bAiPlayer))
					m_pAutoBattleToolBar->SetVisible(true);
				else
					m_pAutoBattleToolBar->SetVisible(false);
			}

			if (bAiPlayer || (IsAutobattle())) {
#ifdef MULTIPLAYER
				gMPMgr.DisablePRA();
#endif
				EnableControls(EnemyTurn);
				m_bHumanTurn = false;

				// Try to cast the spell
				bool bCasted = ProcessSpellCastAI(m_pBattle->Engine());

				// Want retreat ?
				bool bTriedToEscape;
				iBattleResult bres;
				if (!CheckBattleStateAI(m_pBattle->Engine(), true, bTriedToEscape, bres)) {
					EndBattle(bres);
					return false;
				}
				// anyway if AI failed to escape, autobattle should be stopped now
				if(IsAutobattle() && bTriedToEscape)
					EndAutobattle();

				// Process battle step
				if (!bCasted) {
					if (ProcessBattleStepAI(m_pBattle->Engine()))
						BeginAni();
					else if (!OnGroupChanged())
						return false;
				}
			} else {
				m_bHumanTurn = true;

				uint32 flags = CanInfo | CanMelee;
				if (pGroup->Shots() && pGroup->ShootListCount()) flags |= CanShoot;
				if (pGroup->Owner()->CanCastSpell()) flags |= CanCast;
				if (m_pBattle->Engine().CanWait()) flags |= CanWait;

				EnableControls(flags);
				UpdateToolbarMode();
			}

	} else if (DynamicCast<iBattleUnit_Catapult*>(pCurUnit)) {
		iCastleFort::ElementType target = m_pBattle->Engine().CastleFort()->GetTarget();
		if (target != iCastleFort::InvalidElement) {
			m_pBattle->Engine().CatapultShot(target);
			BeginAni();
		}
	} else if (DynamicCast<iBattleUnit_Turret*>(pCurUnit)) {
		iBattleGroup* pTarget = SelectTurretTarget(m_pBattle->Engine());
		check(pTarget);
		m_pBattle->Engine().TurretShot(pTarget);
		BeginAni();
	}
	return true;
}

void iBattleView::BeginAni()
{
	m_bAni = true;
	EnableControls(Acting);
	m_actTimer = fix32::zero;
	Invalidate();
}

void iBattleView::EndAni()
{
	//gLogMgr.Log(_T("EndAni\r\n"));
	m_bAni = false;
	EnableControls(Acting);
}

// Calculates delta distance between two cells
inline uint32 CellsDelta2(sint32 px1, sint32 py1, sint32 px2, sint32 py2)
{
	px1 = px1 + py1/2;
	px2 = px2 + py2/2;
	uint32 dx = iABS(px1 - px2);
	uint32 dy = iABS(py1 - py2);
	return iMIN(dx,dy) + iDIF(dx,dy);
}



/*uint32 iBattleView::GetNearestOccupiedCell(const iPoint &pos, iPoint &cell) {
uint32 best = 1000;
for (sint32 yy=0; yy<11; ++yy)
for (uint32 xx=0; xx<13; ++xx)
if(pos.GetDelta(Map2Screen(iPoint(xx, yy))) < best) {
best = pos.GetDelta(Map2Screen(iPoint(xx, yy)));
cell = iPoint(xx, yy);
}
return best;

}*/


void iBattleView::PrepareSurface()
{
    iSize sz = gGfxMgr.Dimension(PDGG(COMBAT_BTYPE_BARREN));
    iSize szSky = gGfxMgr.Dimension(PDGG(COMBAT_STYPE_BARREN_MOUNT));

    sz.h += szSky.h;

	m_dibSurf.Cleanup();
	m_dibSurf.Init(sz, iDib::RGB);
	iIsoMetric im(5);

	/*sint32 step_x = im.GetCellStepX()*2;
	sint32 step_y = im.GetCellStepY();
	sint32 cnt_x=10;
	sint32 cnt_y=20;
	// Draw background surface
	for (sint32 yy=0; yy<cnt_y; ++yy) {
	for (sint32 xx=0; xx<cnt_x; ++xx){
	sint32 xpos = xx*step_x;
	sint32 ypos = yy*step_y;
	if (yy%2!=0) xpos -= step_x >> 1;
	iPoint op(xpos-step_x,ypos+20);
	uint32 cseq = CalcCellSeq(iPoint(xx,yy),4);
	// Surf
	gGfxMgr.BlitMasked(PDGG(SURF_TILES) + m_surfType*4 + cseq, PDGG(TRANS_TILES)+14,m_dibSurf,op);
	}
	}*/

	// Background
	gGfxMgr.Blit(PDGG(COMBAT_BTYPE_BARREN) + m_background, m_dibSurf, iPoint(0, gGfxMgr.Dimension(PDGG(COMBAT_STYPE_BARREN_MOUNT)).h));
	// Sky
	gGfxMgr.Blit(PDGG(COMBAT_STYPE_BARREN_MOUNT) + m_sky, m_dibSurf, iPoint(0, 0));

	// Draw moat
	if (const iCastleFort* pFort = m_pBattle->Engine().CastleFort()) {
		if (pFort->HasMoat()) {
#ifdef PC_VERSION
#ifdef OS_MACOS
            iPoint pnt = Map2Screen(iPoint(4,0))+iPoint(-250, -150);
#else
			iPoint pnt = Map2Screen(iPoint(4,0))+iPoint(-100, -75);
#endif
#else
            iPoint pnt;
            if(!gUseIpadUI)
                pnt = Map2Screen(iPoint(8,0)) + iPoint(-15, 10);
            else
                pnt = Map2Screen(iPoint(8,0)) + iPoint(-100, 0);
#endif
			gGfxMgr.Blit(PDGG(COMBAT_MOAT), m_dibSurf, pnt);
		}
	}

	// Draw Sky
	/*m_dibSurf.VGradientRect(iRect(m_Rect.x,m_Rect.y,m_Rect.w,30), SKY_GRAD_COLORS[m_surfType][0], SKY_GRAD_COLORS[m_surfType][1]);
	m_dibSurf.HLine(iPoint(m_Rect.x,m_Rect.y+30), m_Rect.x2(), SKY_GRAD_COLORS[m_surfType][1], 192);
	m_dibSurf.HLine(iPoint(m_Rect.x,m_Rect.y+31), m_Rect.x2(), SKY_GRAD_COLORS[m_surfType][1], 128);
	m_dibSurf.HLine(iPoint(m_Rect.x,m_Rect.y+32), m_Rect.x2(), SKY_GRAD_COLORS[m_surfType][1], 64);*/

	//m_dibSurf.FillRect(iRect(0,0,m_Rect.w,30),cColor_Blue);
}

bool iBattleView::Process(fix32 t)
{
	// actions
	if (m_pBattle->Engine().ActionCount()) {
		m_actTimer += t;
		while (m_pBattle->Engine().ActionCount() && (m_actTimer >= (
#ifdef MULTIPLAYER
			gMPMgr.IsReplaying() ? 0 :
#endif
			m_pBattle->Engine().CurAction()->m_delay
			))) {
				iBattleAct* pAct = m_pBattle->Engine().StepAction();
				check(pAct);
				Invalidate();
				m_actTimer -=
#ifdef MULTIPLAYER
					gMPMgr.IsReplaying() ? 0 :
#endif
					pAct->m_delay;
				delete pAct;

				// No more actions for current group
				if (!m_pBattle->Engine().ActionCount()) {
					EndAni();
					// Check victory conditions
					BATTLE_RESULT br = m_pBattle->Engine().CheckBattleState();
					if (br != BR_NA) {
						EndBattle(iBattleResult(br,DC_DEFEAT));
						return true;
					}
					m_pBattle->Engine().NextGroup();

					// poison damage handled in iBattleGroup::NewRound, so we need recheck
                    br = m_pBattle->Engine().CheckBattleState();
                    if (br != BR_NA) {
                        EndBattle(iBattleResult(br,DC_DEFEAT));
                        return true;
                    }

					if (!OnGroupChanged()) return true;
					Invalidate();
				}
		}
	}

	// highlight blinking
	m_blinkTimer += t;
	if(m_blinkTimer > HIGHLIGHT_BLINK_DELAY) {
		EnableControls(Nothing);
		m_blinkTimer = fix32::zero;
		m_bHighlight = !m_bHighlight;
		Invalidate();
	}

	// events
	iEvtIt it=m_cellEvents.First();
	while(it != m_cellEvents.End()) {
		Invalidate();
		(*it)->m_time -= t;
		if ( (*it)->m_time < fix32::zero){
			it = m_cellEvents.Delete(it);
			continue;
		}
		(*it)->dstate = (uint8)iCLAMP<sint32>(0,255,((*it)->m_time * fix32(255.0f) / CELL_EVT_DELAY).ceil());
		++it;
	}

#ifdef PC_VERSION
    if( m_bAni && !m_bEndAutocombat )
        return true;

    if( SpellTracking() ){

        m_bDefender = false;
        m_bWait = false;
        m_bBeginAutocombat = false;
        m_bEndAutocombat = false;
        m_bSpell = false;
        return true;
    }

    if( m_bBeginAutocombat ){

        m_bBeginAutocombat = false;

        if(IsAnyAutobattle())
            EndAutobattle();
        else
            BeginAutobattle();
    }
    else if( m_bEndAutocombat ){

        m_bEndAutocombat = false;

        if(IsAnyAutobattle())
            EndAutobattle();
    }
    else if(m_bDefender){

        m_bDefender = false;

        SetInfoMode(false);
		//set the flag, so it doesn't select move entry right now
		m_bDontMove = true;

		m_pBattle->Engine().Defend();
		EnableControls(Nothing);
		OnGroupChanged();
    }
    else if(m_bWait){

        m_bWait = false;

        if(m_pBattle->Engine().CanWait()) {

			m_pBattle->Engine().Wait();
			EnableControls(iBattleView::Nothing);
            OnGroupChanged();
		}
		else if(!gUseIpadUI){
			m_pBattle->Engine().Defend();
            OnGroupChanged();
        }
    }
    else if( m_bSpell ){

        m_bSpell = false;

        SpellDlg();
    }
#endif

	return true;
}

void iBattleView::AddLogEvent(const iStringT& msg)
{
	m_log.Add(msg);
}



void iBattleView::AddCellEvent(const iStringT& msg, const iPoint& pos)
{
	m_cellEvents.Add(new iGameEvent(fix32(2.0f), msg, pos));
}

void iBattleView::DoCompose()
{
    gApp.Surface().Fill(cColor_Black);
	// Compose background
    iSize sz = m_dibSurf.GetSize();
    iRect rc = AlignRect(sz, iRect(iPoint(), gApp.Surface().GetSize()), AlignCenter);
    m_dibSurf.CopyToDibXY(gApp.Surface(), rc);
	if (!m_pBattle) return;

    if(gUseIpadUI)
    {
        // Show turn sequence (for current round)
        if(1/*m_bAni && !gApp.ViewMgr().HasModalDlg()*/) {
            iSize glyphSize(70, 70);
            if (m_dibTurnGlyph.GetSize() != glyphSize) m_dibTurnGlyph.Resize(glyphSize);
            const iTurnSeq& turnSeq = m_pBattle->Engine().TurnSeq();
            iRect tseqRect(AlignRect(iSize(glyphSize.w * turnSeq.GetTurnSeqSize(), glyphSize.h), m_Rect, AlignBottom) - iPoint(0, DEF_BTN_HEIGHT + 15), glyphSize);
            for (uint32 xx=0; xx<10; ++xx) {
                gApp.Surface().CopyRectToDibXY(m_dibTurnGlyph, tseqRect, iPoint());
                if (xx < turnSeq.GetTurnSeqSize()) {
                    if (iBattleUnit_CreatGroup* pCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(turnSeq.GetTurnSeqItem(xx))) {
                        ComposeCreatureIcon(m_dibTurnGlyph, iRect(1,1,glyphSize.w-2,glyphSize.h-2), pCreatGroup->GetCreatGroup()->Type(), false,
                                            pCreatGroup->Owner()->GetSide() == iBattleMember::Defender);
                    } else if (iBattleUnit_Catapult* pCatapult = DynamicCast<iBattleUnit_Catapult*>(turnSeq.GetTurnSeqItem(xx))) {
                        // align the actual picture to the bottom of the rect
                        gGfxMgr.Blit(PDGG(SHOOT_AMMO), m_dibTurnGlyph, GetRealSpritePos(PDGG(SHOOT_AMMO), iRect(iPoint(), m_dibTurnGlyph.GetSize()), false, AlignBottom));
                    } else if (iBattleUnit_Turret* pTurret = DynamicCast<iBattleUnit_Turret*>(turnSeq.GetTurnSeqItem(xx))) {
                        gGfxMgr.Blit(PDGG(ARROW_TOWER), m_dibTurnGlyph, GetRealSpritePos(PDGG(ARROW_TOWER), iRect(iPoint(), m_dibTurnGlyph.GetSize()), false, AlignBottom));
                    }
                }
                m_dibTurnGlyph.CopyToDibXY(gApp.Surface(), tseqRect, (uint8)((15-xx)*10));
                tseqRect.x += glyphSize.w;
            }
        }
    }

	iBattleUnit* pCurUnit  = m_pBattle->Engine().TurnSeq().CurUnit();
	if(!pCurUnit || !pCurUnit->Owner() || !pCurUnit->Owner()->IsValid()) return; // the owning hero was already removed from map
	PLAYER_ID pid = pCurUnit->Owner()->Owner();
#ifndef MULTIPLAYER
	bool bInt = !(pid == PID_NEUTRAL || gGame.Map().FindPlayer(pid)->PlayerType() == PT_COMPUTER) && !m_pBattle->Engine().ActionCount() && (m_pBattle->Engine().CheckBattleState() == BR_NA);
#else
	bool bInt = !(pid == PID_NEUTRAL || gGame.Map().FindPlayer(pid)->PlayerType() == PT_COMPUTER || gGame.Map().FindPlayer(pid)->PlayerType() == PT_REMOTE) && !m_pBattle->Engine().ActionCount() && (m_pBattle->Engine().CheckBattleState() == BR_NA);
	bool bRemote = (pid != PID_NEUTRAL && gGame.Map().FindPlayer(pid)->PlayerType() == PT_REMOTE);
#endif

	iPoint op;
	// Draw Grid
		op = anchor;

		for (sint32 yy=0; yy<11; ++yy) {
			for (uint32 xx=0; xx<13; ++xx) {
				if (gSettings.GetEntryValue(CET_COMBATGRID))
					gGfxMgr.BlitEffect(PDGG(GRID_HEX), gApp.Surface(), op,iGfxManager::EfxTransparent);
				// Draw cover map (only for interactive side)
				if (bInt && !SpellTracking()){
					if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattle->Engine().TurnSeq().CurUnit())){
						if (pCurCreatGroup->GetCreatGroup()->CanMove(xx,yy)) {
							gGfxMgr.BlitEffect(PDGG(GRID_HEX), gApp.Surface(), op,iGfxManager::EfxTransparent);
							gGfxMgr.BlitEffect(PDGG(GRID_HEX), gApp.Surface(), op,iGfxManager::EfxTransparent);

						}
					}
				}
				op.x+=sm_hexWidth;
			}
			op.y+=sm_hexHeight;
			op.x=anchor.x;
			if (yy%2 == 0) op.x-=sm_hexHalfWidth;
		}


	// Show path (for debug)
	//if (!m_Actions.GetSize()) for (uint32 pp=0; pp<m_Path.GetSize(); ++pp) gfxMgr.BlitEffect( PDGG(HEXCELLS), dib, Map2Screen(m_Path[pp]), iGfxManager::EfxTransparent);

	// Draw current troop and targets highlighters (only for interactive side)
	if (bInt) {
		if (SpellTracking()) {

			iHero* pCaster = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
			check(pCaster);
			MAGIC_SCHOOL_LEVEL msl = m_pCastSpellToolBar->Spell()->GetSchoolLevel(pCaster);
			if (m_pCastSpellToolBar->Spell()->TargetMode(msl) == STM_CREAT_GROUP) {
				for (uint32 xx=0; xx<m_spellTargets.GetSize(); ++xx) {
					iPoint op = Map2Screen(m_spellTargets[xx]);
					gGfxMgr.Blit( PDGG(HEX_TARGET_CELL), gApp.Surface(), op);
					if (m_spellTargets[xx] != SpellTrackCell()) gGfxMgr.BlitEffect( PDGG(HEX_TARGET_CELL), gApp.Surface(), op, iGfxManager::EfxTransparent);
				}
			} else {
				check(m_pCastSpellToolBar->Spell()->HasCoverArea(msl));
				iSimpleArray<iPoint> spellCovers;
				m_pBattle->Engine().GetSpellCovers(m_pCastSpellToolBar->Spell(), msl, SpellTrackCell(), spellCovers);
				iBattleUnit_Hero caster(m_pBattle->Engine().TurnSeq().CurUnit()->Owner());
				for (uint32 xx=0; xx<spellCovers.GetSize(); ++xx) {
					iPoint op = Map2Screen(spellCovers[xx]);
					if (iBattleGroup* pTarget = m_pBattle->Engine().FindGroup(spellCovers[xx]))	{
						if (m_pCastSpellToolBar->Spell()->CanCast(&caster, pTarget)) {
							gGfxMgr.Blit( PDGG(HEX_TARGET_CELL), gApp.Surface(), op);
						} else {
							gGfxMgr.Blit( PDGG(HEX_CELL), gApp.Surface(), op);
						}
					} else gGfxMgr.Blit( PDGG(HEX_CELL), gApp.Surface(), op);
					//gGfxMgr.BlitEffect( PDGG(HEX_TARGET_CELL), gApp.Surface(), op, iGfxManager::EfxTransparent);
				}

			}
		} else {
			if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit)) {
				if(IsMelee()) {
					for (uint32 xx=0; xx<pCurCreatGroup->GetCreatGroup()->MeleeListCount(); ++xx) {
							iPoint op = Map2Screen(pCurCreatGroup->GetCreatGroup()->GetMeleeEntry(xx)->m_pos);
							gGfxMgr.Blit( PDGG(HEX_TARGET_CELL), gApp.Surface(), op);
							gGfxMgr.BlitEffect( PDGG(HEX_TARGET_CELL), gApp.Surface(), op, iGfxManager::EfxTransparent);
                    }
				}
				if (IsShoot()) {
					for (uint32 xx=0; xx<pCurCreatGroup->GetCreatGroup()->ShootListCount(); ++xx) {
						iPoint op = Map2Screen(pCurCreatGroup->GetCreatGroup()->GetShootEntry(xx)->m_pos);
						gGfxMgr.Blit( PDGG(HEX_TARGET_CELL), gApp.Surface(), op);
						if (pCurCreatGroup->GetCreatGroup()->GetShootEntry(xx)->m_pos != m_trackCell)
                                                    gGfxMgr.BlitEffect( PDGG(HEX_TARGET_CELL), gApp.Surface(), op, iGfxManager::EfxTransparent);
					}
				}
			}
			//if (m_pMeleeTrack)
			//	gGfxMgr.Blit(PDGG(HEX_TARGET_CELL), gApp.Surface(), Map2Screen(m_trackCell));
		}
	}

	if(m_MoveEntry != cInvalidPoint)
		gGfxMgr.Blit(PDGG(HEX_TARGET_CELL), gApp.Surface(), Map2Screen(m_MoveEntry));

#ifdef PC_VERSION
    if( bInt && m_Entered != cInvalidPoint ){

        gGfxMgr.Blit(PDGG(HEX_TARGET_CELL), gApp.Surface(), Map2Screen(m_Entered));
        ComposeSpellCursor();
    }
#endif

	iSortArray<iBatCmpItem*> cmpList;

	// Add castle's fort elements
	if (const iCastleFort* pFort = m_pBattle->Engine().CastleFort()) {
		for (uint32 xx=0; xx<pFort->ElementsCount(); ++xx) {
            bool bGround;
            if(gUseIpadUI)
                bGround = FORT_DECS_IPAD[pFort->GetElement(xx)->type][pFort->GetElement(xx)->state].bGround;
            else
                bGround = FORT_DECS_IPHONE[pFort->GetElement(xx)->type][pFort->GetElement(xx)->state].bGround;
			cmpList.Insert(new iBatCmpItem_FortElement(pFort->GetElement(xx)), (bGround)?0:(iCastleFort::ElCells[xx][1]*sm_hexHalfWidth+iCastleFort::ElCells[xx][0]));
		}
	}

	// Add units
	op = anchor;
	iBattleGroup* pGroup = NULL;
	if (iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(pCurUnit)) pGroup = pCurCreatGroup->GetCreatGroup();
	for (uint32 xx=0; xx<m_pBattle->Engine().AArmy().Count(); ++xx) {
		iPoint pos = m_pBattle->Engine().AArmy()[xx]->ScreenPos() == cInvalidPoint ? m_pBattle->Engine().AArmy()[xx]->Pos() : m_pBattle->Engine().AArmy()[xx]->ScreenPos();
		uint32 val = (pos.y*sm_hexHalfWidth+pos.x);
		iBattleGroup* gr = m_pBattle->Engine().AArmy()[xx];
		if (gr->State() == iBattleGroup::Melee || gr->State() == iBattleGroup::Shooting) val += 2;
		cmpList.Insert(new iBatCmpItem_Creature(gr, ((gr == pGroup && m_bHighlight) || (gr->IsGroupCell(m_highlightedTarget) && gr->IsAlive())) && !SpellTracking(),
			gr->Pos() == m_highlightedGroup, gr->InMoat() && (gr->State() != iBattleGroup::Moving || gr->TransType() != TRANS_FLY)), val);
	}
	for (uint32 xx=0; xx<m_pBattle->Engine().DArmy().Count(); ++xx) {
		iPoint pos = m_pBattle->Engine().DArmy()[xx]->ScreenPos() == cInvalidPoint ? m_pBattle->Engine().DArmy()[xx]->Pos() : m_pBattle->Engine().DArmy()[xx]->ScreenPos();
		uint32 val = (pos.y*sm_hexHalfWidth+pos.x);
		iBattleGroup* gr = m_pBattle->Engine().DArmy()[xx];
		if (gr->State() == iBattleGroup::Melee || gr->State() == iBattleGroup::Shooting) val += 2;
		cmpList.Insert(new iBatCmpItem_Creature(gr, ((gr == pGroup && m_bHighlight) || (gr->IsGroupCell(m_highlightedTarget) && gr->IsAlive())) && !SpellTracking(),
			gr->Pos() == m_highlightedGroup, gr->InMoat() && (gr->State() != iBattleGroup::Moving || gr->TransType() != TRANS_FLY)), val);
	}

	// Add obstacles
	for (uint32 xx=0; xx<m_obstacles.GetSize(); ++xx) {
		uint32 val = (m_obstacles[xx].cell.y*sm_hexHalfWidth+m_obstacles[xx].cell.x);
		cmpList.Insert(new iBatCmpItem_Obstacle(&m_obstacles[xx]), val);
	}

	// add catapult
	if(m_pBattle->Engine().TurnSeq().Catapult())
		cmpList.Insert(new iBatCmpItem_Catapult(m_pBattle->Engine().TurnSeq().Catapult()), 9*sm_hexHalfWidth+0);

	// add flying ammo
	if(m_pBattle->Engine().Ammo().m_pos != cInvalidPoint)
		cmpList.Insert(new iBatCmpItem_Ammo(m_pBattle->Engine().Ammo()), 10000);

	// Draw shadow
	for (uint32 xx=0; xx<cmpList.Size(); ++xx) {
		cmpList[xx].value->ComposeShadow(gApp.Surface(),cmpList[xx].value->m_pos);
	}

	// Draw sprites
	for (uint32 xx=0; xx<cmpList.Size(); ++xx) {
//        if(/*xx==0 ||*/ xx==1 /*|| xx==2 || xx==3 || xx==4 || xx==5 || xx==6 || xx==7 || xx==8 || xx==9*/)
		cmpList[xx].value->Compose(gApp.Surface(),cmpList[xx].value->m_pos, bInt
#ifdef MULTIPLAYER
			|| bRemote
#endif
			);
	}

	for (uint32 xx=0; xx<cmpList.Size(); ++xx) delete cmpList[xx].value;
	cmpList.Cleanup();

#if 0 // Debug dist map
	op = anchor;
	iTextComposer::FontConfig fc(FS_SMALL);
	for (yy=0; yy<11; ++yy) {
		for (uint32 xx=0; xx<13; ++xx) {
			if (m_pBattle->Engine().CurGroup()->CanMove(xx,yy)) gGfxMgr.BlitAlpha(PDGG(GRID_HEX), gApp.Surface(), op, 16);
			if (m_pBattle->Engine(). CurGroup()->DistMap().GetAt(xx,yy).dirs[0] != 0xFF) gTextComposer.TextOut(fc, gApp.Surface(), iPoint(0,0), iFormat(_T("%d"),m_pBattle->Engine().CurGroup()->DistMap().GetAt(xx,yy).dirs[0]), iRect(op.x,op.y,20,19),AlignRight);
			if (m_pBattle->Engine().CurGroup()->DistMap().GetAt(xx,yy).dirs[1] != 0xFF) gTextComposer.TextOut(fc, gApp.Surface(), iPoint(0,0), iFormat(_T("%d"),m_pBattle->Engine().CurGroup()->DistMap().GetAt(xx,yy).dirs[1]), iRect(op.x,op.y,20,19),AlignLeft);
			op.x+=20;
		}
		op.y+=15;
		op.x=anchor.x;
		if (yy%2 == 0) op.x-=10;
	}
#endif

	// Compose cell events
	for (iEvtCIt it=m_cellEvents.First(); it != m_cellEvents.End(); ++it) {
		iPoint pos = Map2Screen((*it)->pos);
		pos.x += sm_hexHalfWidth;
		pos.y -= sm_hexHeight + (255 - (*it)->dstate) / 7;
		uint8 a = (*it)->dstate;
		if (a < 128) a *= 2; else a = 255;

		iTextComposer::FontConfig fontcfg(FS_MEDIUM, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		gTextComposer.TextOut( fontcfg, gApp.Surface(), pos, iFormat(_T("#A%02X"),a)+(*it)->text, iRect(pos,iSize()), AlignTop);
	}

	// Show toolTip
	if (!m_toolTip.Empty()) {
		iTextComposer::FontConfig fontcfg(FS_MEDIUM, iDibFont::ComposeProps(TOOLTIP_COLORS[0][1], TOOLTIP_COLORS[0][0],iDibFont::DecBorder));
		gTextComposer.TextOut( fontcfg, gApp.Surface(), iPoint(), m_toolTip, iRect(0,0,320,30), AlignCenter);
	}

/*	// Draw  cursor and action info
	if (SpellTracking()) {
		ComposeSpellCursor();
	} else
		ComposeBattleCursors();
 */
}

void iBattleView::BeginAutobattle()
{
    /*// add only once
    if (m_pAutoBattleToolBar == NULL)
    {*/
	//AddChild(m_pAutoBattleToolBar = new iAutoBattleToolBar(m_pMgr, this, iRect(0, m_Rect.y2()-DEF_BTN_HEIGHT - 5,m_Rect.w, DEF_BTN_HEIGHT + 5)));

    m_pBattle->Engine().SetAutobattle(true);
	OnGroupChanged();
}

void iBattleView::EndAutobattle()
{
    /*if (!IsAutobattle())
        return;
*/
	//check(m_pAutoBattleToolBar);
	/*RemoveChild(m_pAutoBattleToolBar);
	delete m_pAutoBattleToolBar; // it's here because RemoveChild doesn't call to delete
	m_pAutoBattleToolBar = NULL;
	*/
	m_pAutoBattleToolBar->SetVisible(false);
	m_pBattle->Engine().SetAutobattle(false);
}

void iBattleView::SpellDlg(){

	SetInfoMode(false);
	//set the flag, so it doesn't select move entry right now
	m_bDontMove = true;

	iHero* pHero = m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->SpellCaster();
	check(pHero);
	// Check for Spere of Negation
	if (iHero* pArtOwner = m_pBattle->Engine().GetBattleInfo().HasArtInBattle(ARTT_NEGSPHERE)) {
		iTextDlg dlg(m_pMgr, _T(""), iFormat(gTextMgr[TRID_MSG_BAT_NEGSPHERE], pArtOwner->Name().CStr()), pHero->Owner()->PlayerId());
		dlg.DoModal();
		return;
	}

	// fix for double cast bug: remove the previous spelltrack if any
	EndSpellTrack(cInvalidPoint);		// fix for double cast bug: remove the previous spelltrack if any

	EnableControls(SpellBook);
	iDlg_Spellbook csDlg(m_pMgr, pHero, SPT_MASK_COMBAT, false);

	sint32 res = csDlg.DoModal();

	uint32 flags = CanInfo | CanMelee;
	if (m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->CanCastSpell()) flags |= CanCast;
	if (m_pBattle->Engine().CanWait()) flags |= CanWait;

	if (res == DRC_YES) {
		iBaseSpell* pSpell = gGame.ItemMgr().m_spellMgr.Spell(csDlg.SelSpell());
		check(pSpell && pSpell->Type() == SPT_COMBAT);
		iCombatSpell* pCombatSpell = DynamicCast<iCombatSpell*>(pSpell);
		check(pCombatSpell);
		MAGIC_SCHOOL_LEVEL msl = pSpell->GetSchoolLevel(pHero);
		if (pCombatSpell->NeedTarget(msl)) {
			if (!BeginSpellTrack(pCombatSpell)) {
				iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_SPELL_AFFECT_NO_ONE], pHero->Owner()->PlayerId());
				tdlg.DoModal();
			}
			EnableControls(flags);
		} else {
			if (!m_pBattle->Engine().CastSpell(pCombatSpell, cInvalidPoint, NULL)) {
				iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_SPELL_AFFECT_NO_ONE], pHero->Owner()->PlayerId());
				tdlg.DoModal();
			} else BeginAni();
			EnableControls(flags);
		}
	}
}

void iBattleView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();

	if (m_pCastSpellToolBar && uid == DRC_CANCEL) {
		EndSpellTrack(cInvalidPoint);
	} else if (m_pAutoBattleToolBar && uid == DRC_CANCEL) {
		EndAutobattle();
	} else if (IsAcceptingCmds() && (uid == BCI_ASSAULTER_BTN || uid == BCI_DEFENDER_BTN)) {
			SetInfoMode(false);
			//set the flag, so it doesn't select move entry right now
			m_bDontMove = true;

			iBattleMember* pActMember = (uid == BCI_ASSAULTER_BTN)?m_pBattle->Engine().GetBattleInfo().m_pAssaulter:m_pBattle->Engine().GetBattleInfo().m_pDefender;
			iBattleMember* pFriendlyMember = m_pBattle->Engine().TurnSeq().CurUnit()->Owner();
			iBattleMember* pEnemyMember = (pFriendlyMember->GetSide() == iBattleMember::Defender)?m_pBattle->Engine().GetBattleInfo().m_pAssaulter:m_pBattle->Engine().GetBattleInfo().m_pDefender;

			iHero* pHero = NULL;
			iCastle* pCastle = NULL;
			if (iBattleMember_Hero* pHeroActor = DynamicCast<iBattleMember_Hero*>(pActMember)) {
				pHero = pHeroActor->GetHero();
			} else if (iBattleMember_Castle *pCastleActor = DynamicCast<iBattleMember_Castle*>(pActMember)) {
				pHero = pCastleActor->GetVisitor();
				pCastle = pCastleActor->GetCastle();
			}

			if(!pHero) {
				check(pCastle);
				iDlg_BattleCastle bcDlg(m_pMgr, *pCastle, pActMember->GetSide() == pFriendlyMember->GetSide());
				HighlightMemberButton(uid, true);
				sint32 res = bcDlg.DoModal();
				HighlightMemberButton(uid, false);
				if(res == iDlg_BattleCastle::MainMenu) {
					iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_EXITMM], PID_NEUTRAL);
					if (qdlg.DoModal() == DRC_YES) {
						gGame.MainMenu();
					}
				} else if(res == iDlg_BattleCastle::AutoBattle) {
					BeginAutobattle();
				}
				return;
			}


			BATTLE_RESULT br = (pFriendlyMember->GetSide() == iBattleMember::Defender)?BR_ASSAULTER_WIN:BR_DEFENDER_WIN;


			uint32 flags = 0;
			if (pActMember->GetSide() == pFriendlyMember->GetSide() && !DynamicCast<iCastle*>(pHero->GetLocation())) {
				flags |= iDlg_BattleHero::CanRetreat;
				if (DynamicCast<iBattleMember_Hero*>(pEnemyMember)) flags |= iDlg_BattleHero::CanSurrender;
			}

			HighlightMemberButton(uid, true);
			iDlg_BattleHero bhDlg(m_pMgr, *pHero, pActMember->GetSide() == pFriendlyMember->GetSide(), flags);
			sint32 res = bhDlg.DoModal();
			HighlightMemberButton(uid, false);

			if (res == iDlg_BattleHero::Retreat) {
				// Check for Shackles of War
				if (iHero* pArtOwner = m_pBattle->Engine().GetBattleInfo().HasArtInBattle(ARTT_SHOFWAR)) {
					iTextDlg dlg(m_pMgr, _T(""), iFormat(gTextMgr[TRID_MSG_BAT_SHOFWAR], pArtOwner->Name().CStr()), pHero->Owner()->PlayerId());
					dlg.DoModal();
				} else {
					iDlg_TownList tldlg(m_pMgr, pHero->Owner(),gTextMgr[TRID_SELECT_CASTLE_TO_RETREAT], TRID_RETREAT, true);
					if (tldlg.DoModal() == DRC_OK) {
						check(tldlg.Destination());
						EndBattle(iBattleResult(br,DC_RETREAT,tldlg.Destination()));
					}

				}

			} else if(res == iDlg_BattleHero::MainMenu) {
				iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_EXITMM], PID_NEUTRAL);
				if (qdlg.DoModal() == DRC_YES) {
					gGame.MainMenu();
				}
			} else if(res == iDlg_BattleHero::AutoBattle) {
				BeginAutobattle();
			} else if (res == iDlg_BattleHero::Surrender) {
				// Check for Shackles of War
				if (iHero* pArtOwner = m_pBattle->Engine().GetBattleInfo().HasArtInBattle(ARTT_SHOFWAR)) {
					iTextDlg dlg(m_pMgr, _T(""), iFormat(gTextMgr[TRID_MSG_BAT_SHOFWAR], pArtOwner->Name().CStr()), pHero->Owner()->PlayerId());
					dlg.DoModal();
				} else {
					iBattleMember_Hero* pEnemyHero = DynamicCast<iBattleMember_Hero*>(pEnemyMember);
					check(pEnemyHero);
					uint32 cost = m_pBattle->Engine().GetSurrenderCost(DynamicCast<iBattleMember_Hero*>(pFriendlyMember));
					iDlg_SurrenderCond scdlg(m_pMgr, pEnemyHero->GetHero(), cost);
					if (scdlg.DoModal() == DRC_OK) {
						if (pHero->Owner()->Mineral(MINERAL_GOLD) < cost) {
							iTextDlg nerdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_NOTENOUGHGOLD], pHero->Owner()->PlayerId());
							nerdlg.DoModal();
						} else {
							iDlg_TownList tldlg(m_pMgr, pHero->Owner(),gTextMgr[TRID_SELECT_CASTLE_TO_RETREAT],TRID_SURRENDER, true);
							if (tldlg.DoModal() == DRC_OK) {
								check(tldlg.Destination());
								EndBattle(iBattleResult(br,DC_SURRENDER,tldlg.Destination(), cost));
							}
						}
					}
				}
			}
	} else if (uid == BCI_WAIT_BTN) {
        gTutorial.Trigger(TUT_BATTLE_WAIT);
		SetInfoMode(false);
		//set the flag, so it doesn't select move entry right now
		m_bDontMove = true;

		if(m_pBattle->Engine().CanWait()) {
			m_pBattle->Engine().Wait();
			EnableControls(Nothing);
		}
		else
			m_pBattle->Engine().Defend();
		OnGroupChanged();
	} else if (uid == BCI_DEFEND_BTN) {
        gTutorial.Trigger(TUT_BATTLE_DEFEND);

		SetInfoMode(false);
		//set the flag, so it doesn't select move entry right now
		m_bDontMove = true;

		m_pBattle->Engine().Defend();
		EnableControls(Nothing);
		OnGroupChanged();
	} else if (uid == BCI_CASTSPELL_BTN) {
        gTutorial.Trigger(TUT_BATTLE_CAST);
		SpellDlg();
	} else if (uid == BCI_AUTOBATTLE_BTN) {
		//iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_NOTIMPL], m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->Owner());
		//tdlg.DoModal();
		BeginAutobattle();
	} else if (uid == BCI_MSGLOG_BTN) {
		//iDlg_BattleLog bldlg(m_pMgr, m_pBattle->Engine().TurnSeq().CurUnit()->Owner()->Owner(), m_log);
		//bldlg.DoModal();
#ifdef MULTIPLAYER
	} else if(uid == BCI_KICK_BTN) {
		PLAYER_ID pid = gGame.Map().RealCurPlayer();
		iQuestDlg qdlg(m_pMgr, _T(""), iFormat(gTextMgr[TRID_KICK_CNFRM], gMPMgr.GetColorNickname(pid).CStr())
			, PID_NEUTRAL);
		if (qdlg.DoModal() == DRC_YES) {
			gMPMgr.DoKick(pid);
		}
#endif
	} else if (uid == BCI_SETTINGS_BTN) {
		iSettingsDlg sdlg(&gApp.ViewMgr(), true);
		sint32 res = sdlg.DoModal();
		if (res == GMRC_MAINMENU) {
			// Quit to main menu
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_EXITMM], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES) {
				gGame.MainMenu();
			}
		} else if (res == GMRC_QUIT) {
			// Quit to OS
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_QUIT], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES) {
				gGame.Quit();
			}
		} else if (res == GMRC_LOAD) {
			// Load game
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_RELOAD], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES) {
				iSaveDlg sdlg(&gApp.ViewMgr(), false);
				if (sdlg.DoModal() == DRC_OK) {
					iMapInfo scenProps = sdlg.SelScenario();
					//iScenPropsDlg spdlg(&gApp.ViewMgr(), scenProps, true);
					//if (spdlg.DoModal() == DRC_OK)	{
					scenProps.ReorderPlayers();
					gGame.ExitGame(false);
					gGame.StartNewGame(scenProps, false, false);
					//}
				}
			}
		}
	} else if(uid == BCI_INFO_BTN) {
		if(cmd == CCI_HOLD) {
            gTutorial.Trigger(TUT_BATTLE_INFO);
			SetInfoMode(true);
        }
		else if(cmd == CCI_RELEASE)
			SetInfoMode(false);
	}
}

void iBattleView::Recalc()
{
#ifndef OS_MACOS
	uint16 width = gApp.Surface().GetWidth();
	uint16 height = gApp.Surface().GetHeight();
#else
    uint16 width = GetWindowWidth();
	uint16 height = GetWindowHeight();
#endif
	if(width >= 1024) {
		sm_hexWidth = 72;
		sm_hexHalfWidth = 36;
		sm_hexHeight = 42;
		sm_gridZoom = 2;
	} else if(width >= 480) {
		sm_hexWidth = 36;
		sm_hexHalfWidth = 18;
		sm_hexHeight = 20;
		sm_gridZoom = 1;
	} else {
		sm_hexWidth = 24;
		sm_hexHalfWidth = 13;
		sm_hexHeight = 17;
		sm_gridZoom = 0;
	}

    iPoint offset = iPoint(0,0);
    if(!gUseIpadUI)
    {
        offset.y = -13;
    }

#ifndef PC_VERSION
	anchor = m_Rect.point() + iPoint((width - 13 * sm_hexWidth + sm_hexHalfWidth) / 2, (height - 11 * sm_hexHeight) / 2 + sm_hexHeight) + offset;
#else
    anchor = iPoint((width - 13 * sm_hexWidth + sm_hexHalfWidth) / 2, (height - 11 * sm_hexHeight) / 2 + sm_hexHeight);
#endif
}

void iBattleView::OnActivate(bool bActivate)
{
	std::string battleMusic[4] = {
		("Night of the Owl.mp3"),
		("Firesong.mp3"),
		("Chee Zee Jungle.mp3"),
		("Cartoon Battle.mp3"),
	};
	if(bActivate) {

		bFirstDarken = true;
		iRandomizer r(gGame.Map().GetRandSeed());
		auto music = RelativeFilePath("Music/" + battleMusic[r.Rand(4)]);
		gMusicMgr.Play(music);
	}

}

iPoint iBattleView::FortElemHitPos( iPoint pos, iCastleFort::ElementType type, iCastleFort::ElementState state)
{
    SpriteId sid;
    iPoint offset;
    if(gUseIpadUI)
    {
        sid = FORT_DECS_IPAD[type][state].sid;
        offset = FORT_DECS_IPAD[type][state].offset;
    }
    else
    {
        sid = FORT_DECS_IPHONE[type][state].sid;
        offset = FORT_DECS_IPHONE[type][state].offset;
    }

	return pos + offset + gGfxMgr.Anchor(sid) + iPoint(gGfxMgr.Dimension(sid).w / 2, gGfxMgr.Dimension(sid).h / 2);
}

iPoint iBattleView::FortElemPos( iPoint pos, iCastleFort::ElementType type, iCastleFort::ElementState state )
{

    SpriteId sid;
    iPoint offset;
    if(gUseIpadUI)
    {
        sid = FORT_DECS_IPAD[type][state].sid;
        offset = FORT_DECS_IPAD[type][state].offset;
    }
    else
    {
        sid = FORT_DECS_IPHONE[type][state].sid;
        offset = FORT_DECS_IPHONE[type][state].offset;
    }

	return pos + offset;
}


/*
* Autobattle toolbar
*/
iAutoBattleToolBar::iAutoBattleToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, 0, Enabled|Visible)
{
}

void iAutoBattleToolBar::OnCompose()
{
	iRect rc = GetScrRect();

	//gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rc);
	//ButtonFrame(gApp.Surface(), rc, 0);
	//
	iRect bkrc(rc);
	bkrc.InflateRect(50, 0);
	bkrc.InflateRect(0, 0, 0, 10);
	ComposeGlowBkgnd(gApp.Surface(), bkrc, true, true);//false);
	gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(), rc.point(), gTextMgr[TRID_AUTO_COMBAT], rc, AlignCenter);
}


/*
* Spell toolbar
*/
iCastSpellToolBar::iCastSpellToolBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, iHero* pCaster, iCombatSpell* pSpell)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, 0, Enabled|Visible), m_pSpell(pSpell), m_pCaster(pCaster), m_pTarget(NULL)
{
#ifdef PC_VERSION
	sint32 bw = gTextComposer.GetTextSize(gTextMgr[TRID_CANCEL], btnfc_normal).w + 30;
    iRect rc = AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignRight) - iPoint(3, 0) + iPoint(0, 3);
	AddChild(m_pBtnCancel = new iTextButton(pViewMgr, pCmdHandler, AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignRight) - iPoint(3, 0) + iPoint(0, 3), TRID_CANCEL, 100, Visible|Enabled));
#endif
}

void iCastSpellToolBar::OnCompose()
{
#ifdef PC_VERSION
	iRect rc = GetScrRect();
	//gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rc);
	//ButtonFrame(gApp.Surface(), rc, 0);
	//
	iRect bkrc(rc);
	bkrc.InflateRect(50, 0);
	bkrc.InflateRect(0, 0, 0, 10);

    sint32 w = 0;

	iStringT txt;
	iStringT spname = iFormat(_T("#I%04X %s"), m_pSpell->Icon(), gTextMgr[m_pSpell->NameKey()]);
	if(m_pTarget) {
		txt += iFormat(gTextMgr[TRID_MSG_BAT_CAST_SPELL_TO], spname.CStr(), gTextMgr[TRID_CREATURE_PEASANT_F3+m_pTarget->Type()*3]);
		if(m_pSpell->SpellClass() == SPC_DAMAGE)
		{
			iSpell_Damage* pDmgSpell = (iSpell_Damage*)m_pSpell;
			txt += _T(" ") + iFormat(gTextMgr[TRID_MSG_BAT_DAMAGE1], pDmgSpell->EstimateDamage(m_pCaster, m_pTarget));
		}

        w += gTextComposer.GetTextSize(txt, dlgfc_hdr).w;
	}
	else  {
		txt += iFormat(_T("%s %s"), gTextMgr[TRID_MSG_CHOOSE_TARGET], spname.CStr());
		if(m_pSpell->SpellClass() == SPC_DAMAGE)
		{
			iSpell_Damage* pDmgSpell = (iSpell_Damage*)m_pSpell;
			txt += _T(" ") + iFormat(gTextMgr[TRID_MSG_BAT_DAMAGE1], pDmgSpell->SpellBookDamage(m_pCaster));
		}
        w += gTextComposer.GetTextSize(txt, dlgfc_hdr).w;
	}

    bkrc.w = w + gTextComposer.GetTextSize(gTextMgr[TRID_CANCEL], btnfc_normal).w + 30 + 50*2 +10;

    sint32 offset = 571 - bkrc.w/2;

    iRect rect = m_pBtnCancel->GetRect();
    rect.x = w + 20 + offset;
    m_pBtnCancel->SetRect(rect);

    bkrc.x += offset;
    ComposeGlowBkgnd(gApp.Surface(), bkrc, true, true );//false);

    rc.x += offset;
    gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(), rc.point(), txt, rc, AlignLeft);
#endif
}
