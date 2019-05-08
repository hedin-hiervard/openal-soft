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

extern iItemMgr gItemMgr;

/*
 *	Prototypes (nodes)
 */
iVisCnst* iBaseVisCnstNode::CreateObject(const iPoint& pos)
{ 
	if(iVisCnstNode_NewTeleport *pNode = DynamicCast<iVisCnstNode_NewTeleport*>(this))
		return new iNewTeleport(pos, pNode);
	else
		return new iVisCnst(pos, this); 
}
//
void iBaseVisCnstNode::Serialize(iDynamicBuffer& buff)
{
	buff.Write((uint16)m_Type);
	buff.Write(gItemMgr.m_TextMgr.GetEntry(m_NameKey).idx);
	buff.Write(gItemMgr.m_TextMgr.GetEntry(m_msgKey).idx);
	buff.Write(gItemMgr.m_TextMgr.GetEntry(m_vmsgKey).idx);
}
//
void iVisCnstNode_Stable::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	buff.Write((sint16)m_ActPoints);
}
//
void iVisCnstNode_ManaSource::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	buff.Write((sint16)m_ManaPoints);
}
//
void iVisCnstNode_MLModifier::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	buff.Write(m_moraleModifier);
	buff.Write(m_luckModifier);
}
//
void iVisCnstNode_PSModifier::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	::Serialize(buff,m_psModifier);
}
//
void iVisCnstNode_Dwelling::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	buff.Write((uint16)m_creatType);
}
//
void iVisCnstNode_Shrine::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	buff.Write((uint8)m_level);
}
//
void iVisCnstNode_Treasury::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	m_vars.Serialize(buff);
}
//
void iVisCnstNode_Teleport::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	buff.Write((uint8)m_mode);
	buff.Write((uint8)m_index);
}

//
iNewTeleport* iVisCnstNode_NewTeleport::CreateObject(const iPoint& pos)
{ return new iNewTeleport(pos, this); }
//

//
void iVisCnstNode_Keymaster::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	buff.Write((uint8)m_key);
}
//
void iVisCnstNode_WeeklyMin::Serialize(iDynamicBuffer& buff)
{
	iBaseVisCnstNode::Serialize(buff);
	::Serialize(buff, m_minerals);
}

//////////////////////////////////////////////////////////////////////////
iOwnCnst* iBaseOwnCnstNode::CreateObject(const iPoint& pos, PLAYER_ID owner)
{ return new iOwnCnst(pos,owner,this); }
//
void iBaseOwnCnstNode::Serialize(iDynamicBuffer& buff)
{
	buff.Write((uint16)m_Type);
	buff.Write((uint8)m_Scouting);
	buff.Write(gItemMgr.m_TextMgr.GetEntry(m_NameKey).idx);
	buff.Write(gItemMgr.m_TextMgr.GetEntry(m_msgKey).idx);
	if (m_msgIcn.Empty()) buff.Write((uint16)0xFFFF);
	else {
		const iSpriteMgr& spMgr = gItemMgr.m_SpriteMgr;
		buff.Write((uint16)spMgr[m_msgIcn].id);
	}
}
//
void iOwnCnstNode_FurtSkill::Serialize(iDynamicBuffer& buff)
{
	iBaseOwnCnstNode::Serialize(buff);
	::Serialize(buff, m_furtSkills);
}
//////////////////////////////////////////////////////////////////////////
void iCastle::iCtlCnsts::Init(iCastle* pCastle)
{
	for (uint32 xx=0; xx<CTLCNST_COUNT; ++xx){
		if (CTLCNSTS_DESC[xx].caps.Support(pCastle->Proto()->Type(),pCastle->Proto()->Size())) {
			m_cnsts.Add(iCnstEntry((CTL_CONSTR)xx));
		}
	}
}

void iCastle::iCtlCnsts::SetState(CTL_CONSTR type, uint32 state)
{
	for (uint32 xx=0; xx<m_cnsts.GetSize(); ++xx) {
		if (m_cnsts[xx].type == type) {
			m_cnsts[xx].state = state;
			return;
		}
	}
}

const iStringT& iCastle::Name() const
{ 
	/*if (!m_CustomName.Empty()) return m_CustomName; 
	else */return gItemMgr.m_TextMgr[m_pProto->NameKey()]; 
}

iCastle* iCastleNode::CreateObject(const iPoint& pos, PLAYER_ID owner)
{ return new iCastle(pos, owner, this); }

/*
 *	Objects
 */
