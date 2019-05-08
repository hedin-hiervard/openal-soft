#include "stdafx.h"
#include "common.h"
#include "cm_creatures.h"
#include "Army.h"
#include "cm_magic.h"
#include "serialize.h"
#include "treasury_cnst.h"


void iTrVarCtr::AddVariant(uint32 probability)
{
	m_items.Add(Item(probability));
}

void iTrVarCtr::Serialize(iDynamicBuffer& dbuff) const
{
	uint16 quant = (uint16)m_items.GetSize();
	dbuff.Write(quant);
	for (uint16 xx=0; xx<quant; ++xx) {
		dbuff.Write((uint8)m_items[xx].probability);
		::Serialize(dbuff, m_items[xx].guards);
		::Serialize(dbuff, m_items[xx].rewards);
	}
}

void iTrVarCtr::Unserialize(iDynamicBuffer& dbuff)
{
	m_items.RemoveAll();
	uint16 quant;
	dbuff.Read(quant);
	while (quant--) {
		uint8 prob;
		dbuff.Read(prob);
		m_items.Add(Item(prob));
		::Unserialize(dbuff, m_items.GetLast().guards);
		::Unserialize(dbuff, m_items.GetLast().rewards);
	}
}

