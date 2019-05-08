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



#include "serialize.h"

//////////////////////////////////////////////////////////////////////////
iArtifactNode::iArtifactNode(const iStringT& id, const iStringT& nameKey, const iStringT& descKey, ART_TYPE type, ART_LEVEL_TYPE level, ART_ASSIGN_TYPE assign, uint32 reqLevel, SECONDARY_SKILLS reqSecSkill, const SpriteId& spriteId)
: iBaseNode(nameKey)
, iIdxItem(id), m_descKey(descKey), m_Type(type), m_Level(level), m_assign(assign), m_reqLevel(reqLevel), m_reqSecSkill(reqSecSkill), m_SpriteId(spriteId) {}

iArtifactNode::iArtifactNode(RANDOM_ARTIFACTS type,const iStringT& id,  const iStringT& nameKey, const SpriteId& spriteId)
: iBaseNode(nameKey)
, iIdxItem(id, type), m_descKey(_T("VOID")), m_SpriteId(spriteId) {}

iMapItem_Artifact* iArtifactNode::CreateObject(const iPoint& pos)
{ return new iMapItem_Artifact(pos, this); }

void iArtifactNode::Serialize(iDynamicBuffer& buff)
{
	buff.Write((uint16)m_Type);
	buff.Write((uint8)m_Level);
	buff.Write((uint8)m_assign);
	buff.Write((uint16)m_reqLevel);
	buff.Write((sint8)m_reqSecSkill);
}

//
iArtifactNode_FurtSkill::iArtifactNode_FurtSkill(const iStringT& id, const iStringT& nameKey, const iStringT& descKey, ART_LEVEL_TYPE level, ART_ASSIGN_TYPE assign, uint32 reqLevel, SECONDARY_SKILLS reqSecSkill, const SpriteId& spriteId, const iFurtSkills& furtSkills, const iCreatBonus &cb)
: iArtifactNode(id, nameKey, descKey, ARTT_FURTSKILL, level, assign, reqLevel, reqSecSkill, spriteId), m_furtSkills(furtSkills), m_creatBonus(cb) {}

void iArtifactNode_FurtSkill::Serialize(iDynamicBuffer& buff)
{
	iArtifactNode::Serialize(buff);
	::Serialize(buff, m_furtSkills);
	::Serialize(buff, m_creatBonus);
}

//
iArtifactNode_MagicSphere::iArtifactNode_MagicSphere(const iStringT& id, const iStringT& nameKey, const iStringT& descKey, ART_LEVEL_TYPE level, ART_ASSIGN_TYPE assign, uint32 reqLevel, SECONDARY_SKILLS reqSecSkill, const SpriteId& spriteId, sint32 type)
: iArtifactNode(id, nameKey, descKey, ARTT_MAGICSPHERE, level, assign, reqLevel, reqSecSkill, spriteId), m_type(type) {}

void iArtifactNode_MagicSphere::Serialize(iDynamicBuffer& buff)
{
	iArtifactNode::Serialize(buff);
	buff.Write(m_type);
}


//////////////////////////////////////////////////////////////////////////
iMapItem_Artifact::iMapItem_Artifact(const iPoint& pos, iArtifactNode* pProto)
: iMapItem(pos, MAPITEM_ARTIFACT, pProto->Sprite()), m_pProto(pProto) {}
