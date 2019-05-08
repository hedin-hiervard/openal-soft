#include "he/he.std.h"
#include "he/he.tables.h"
#include "he/he.container.map.h"
#include "he/he.container.cache.h"
#include "he/he.container.simple.array.h"
#include "he/he.file.h"
#include "he/he.file.name.h"

#include "pa/pa.xml.builder.h"
#include "pa/pa.map.h"
#include "pa/pa.sax.pathhandler.h"

#include "xe/xe.std.h"
#include "xe/xe.dib.h"
#include "xe/xe.dc.hlp.h"

#include "si/si.std.h"
#include "si/si.wctrl.tree.h"


#include "common.h"
#include "cm_magic.h"
#include "cm_creatures.h"
#include "cm_castles.h"
#include "cm_constructions.h"
#include "Army.h"
#include "rtti.h"
#include "defines.h"
#include "objects.h"
#include "nodes.h"
#include "Artifacts.h"
#include "treasury_cnst.h"
#include "Constructions.h"

#include "SpriteMgr.h"
#include "AvatarMgr.h"
#include "TextMgr.h"
#include "SoundMgr.h"

#include "resmgr.h"

#include "ifaces.h"

extern iItemMgr gItemMgr;

//////////////////////////////////////////////////////////////////////////
iMapItem::iMapItem(const iPoint& pos,  MAP_ITEM_TYPE mit, const SpriteId& sid)
: iBaseMapObject(pos), m_mapItemType(mit), m_spriteId(sid) {}

inline iStringT MineralSprite(MINERAL_TYPE mt)
{
	iStringT sm(MINERAL_ID[mt]);
	sm.Lower();
	return iFormat(_T("resources.%s"),sm.CStr());
}

//////////////////////////////////////////////////////////////////////////
iMapItem_Mineral::iMapItem_Mineral(const iPoint& pos, MINERAL_TYPE mt, sint32 count)
: iMapItem(pos,MAPITEM_MINERAL, (mt==MINERAL_UNKNOWN)?_T("resources.rand_res_icn"):iFormat(_T("resources.icons_%04d"),mt)), m_mineralType(mt), m_count(count) {}
//////////////////////////////////////////////////////////////////////////
iMapItem_ManaCrystal::iMapItem_ManaCrystal(const iPoint& pos, sint32 count)
: iMapItem(pos,MAPITEM_MANACRYSTAL, _T("resources.mcryst")), m_count(count) {}
//////////////////////////////////////////////////////////////////////////
iMapItem_CampFire::iMapItem_CampFire(const iPoint& pos)
: iMapItem(pos, MAPITEM_CAMPFIRE, _T("resources.campfire")) {}
//////////////////////////////////////////////////////////////////////////
iMapItem_Chest::iMapItem_Chest(const iPoint& pos)
: iMapItem(pos, MAPITEM_CHEST, _T("resources.chest")) {}
//////////////////////////////////////////////////////////////////////////
iMapItem_SpellScroll::iMapItem_SpellScroll(const iPoint& pos, uint16 spellId)
: iMapItem(pos, MAPITEM_SPELLSCROLL, _T("resources.spell_scroll")), m_spellId(spellId) {}
//////////////////////////////////////////////////////////////////////////
iMapItem_Lamp::iMapItem_Lamp(const iPoint& pos)
: iMapItem(pos, MAPITEM_LAMP, _T("resources.lamp")) {}
//////////////////////////////////////////////////////////////////////////
iMapItem_KeyGuard::iMapItem_KeyGuard(const iPoint& pos, uint8 key)
: iMapItem(pos, MAPITEM_KEYGUARD, iFormat(_T("resources.key_guards_%04d"),key)), m_key(key) {}

//////////////////////////////////////////////////////////////////////////
iMapGuard::iMapGuard(const iPoint& pos, CREATURE_TYPE ct, sint32 count, GUARD_DISPOS disp, bool bNotGrow)
: iBaseMapObject(pos), m_creat(ct, count), m_disp(disp), m_bNotGrow(bNotGrow), m_okRewBeh(GRB_USUAL), m_permFailRewBeh(GRB_USUAL)
{
	if (ct < CREAT_RANDOM) m_spriteId = iFormat(_T("resources.minimon_%04d"),ct);
	else m_spriteId = iFormat(_T("resources.rand_guards_%04d"),ct-CREAT_RANDOM);
}

const iStringT& iMapGuard::Title() const
{
	return gItemMgr.m_TextMgr[(m_creat.Type() < CREAT_RANDOM)?CREAT_NAMEKEY[m_creat.Type()]:RAND_GUARDS_NAMEKEY[m_creat.Type()-CREAT_RANDOM]];
}
//////////////////////////////////////////////////////////////////////////
iMapEvent::iMapEvent(const iPoint& pos)
: iBaseMapObject(pos), m_playerMask(PIM_ALL), m_flags(MEF_HUMAN_ENABLED | MEF_AI_ENABLED)
{
}

//////////////////////////////////////////////////////////////////////////
iMapMarker::iMapMarker(const iPoint& pos)
: iBaseMapObject(pos), m_inIndex(1)
{
}

//////////////////////////////////////////////////////////////////////////
iHero::iHero(const iPoint& pos, iHeroNode* proto, PLAYER_ID owner)
: iBaseMapObject(pos), m_pProto(proto), m_Owner(owner), m_Experience(0), m_Level(1), m_bCustomSpells(false), m_noExping(false) {}

//////////////////////////////////////////////////////////////////////////
iShip::iShip(const iPoint& pos, iShipNode* proto)
: iBaseMapObject(pos), m_pProto(proto) {}


// END OF FILE

