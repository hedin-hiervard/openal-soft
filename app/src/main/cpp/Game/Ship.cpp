//////////////////////////////////////////////////////////////////////////
// Ship.cpp
//////////////////////////////////////////////////////////////////////////
// basic ideas:
// 
// each ship iShip contains iHero inside. owner of ship (iPlayer) detected by owner of hero
// 
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dlg_Level.h"
#include "ArtifactList.h"
#include "serialize.h"
#include "helpers.h"   
#include "OverlandView.h"



iShip::iShip(const iPoint& pos, uint32 img_type) 
: iBaseMapObject(pos, true), m_pHero(NULL), m_img_type(img_type), m_Angle(0)
{

}

iShip::iShip() 
: iBaseMapObject(cInvalidPoint, true), m_pHero(NULL), m_img_type(0), m_Angle(0)
{

}


iShip::~iShip()
{

}

// Serialize / Deserialize ship object
void iShip::Serialize(iDynamicBuffer& dbuff) const
{
    // Position
    ::Serialize(dbuff, m_Pos);
    // Angle
    dbuff.Write((sint8)m_Angle);
    // Type of art image
    dbuff.Write((sint8)m_img_type);
}

void iShip::Unserialize(iDynamicBuffer& dbuff, bool bInit, bool bAfterLoad, bool bSetOwner, iHero* pHero)
{
    sint8 v;

    m_pHero = pHero;
    // Position
    ::Unserialize(dbuff, m_Pos);
    // Angle
    dbuff.Read(v);
    m_Angle = v;
    // Type of art image
    dbuff.Read(v);
    m_img_type = v;
}


void iShip::Occupy(iHero *pHero)
{
    if (IsLoaded()) return;
    m_pHero = pHero; 

    m_pHero->SetShip(this);
    gGame.Map().m_CoverMap.GetAt(Pos().x, Pos().y) = NULL;    
    // we need restore object in cover map, otherwise object will disappear
    gGame.Map().MoveHero(m_pHero->Pos(), Pos());
    m_pHero->SetPos(Pos());    
	m_pHero->Owner()->UpdateFogMap();
}

void iShip::Leave(iHero *pHero, const iPoint &pt)
{
    if (!IsLoaded()) return;
    if (m_pHero != pHero) return;
    iPoint oldpos = m_pHero->Pos();

    m_pHero = NULL;
    pHero->SetShip(NULL);
    // move hero to ground
    gGame.Map().MoveHero(oldpos, pt);
    // put ship to cover map
    gGame.Map().m_CoverMap.GetAt(oldpos.x, oldpos.y) = this;        
    pHero->SetPos(pt);
	OrientInHarbor();
}


void iShip::SetPos(const iPoint& pos)
{
	//if(m_Pos != cInvalidPoint)
	//	gGame.Map().m_CoverMap.GetAt(m_Pos.x, m_Pos.y) = NULL;        
    iBaseMapObject::SetPos(pos);
	OrientInHarbor();
	//gGame.Map().m_CoverMap.GetAt(pos.x, pos.y) = this;        
   // m_beached = gGame.Map().IsCellNearBeach(Pos());
}

void iShip::HeroDeath(iHero *pHero)
{
    if (!IsLoaded()) return;
    if (m_pHero != pHero) return;

    m_pHero = NULL;
    pHero->SetShip(NULL);
    // put ship to cover map
    gGame.Map().m_CoverMap.GetAt(Pos().x, Pos().y) = this;    
}

/// Scans the surrounding square of 3x3 size to find harbor object, then scans its docks to check if our ship is parked there.
iVisCnst_Harbor* iShip::GetHarborCnst( uint32& dock ) const
{
	uint32 x = Pos().x, y = Pos().y;
	iBaseMapObject* pMapObj = NULL;
	iVisCnst_Harbor* hrb = NULL;
	for(sint32 xx=-3; xx<=3; xx++) {
		for(sint32 yy=-3; yy<=3; yy++) {
			pMapObj = gGame.Map().m_CoverMap.IsValidPos(x + xx, y + yy) ? gGame.Map().m_CoverMap.GetAt(x + xx, y + yy) : NULL;		
			if(iHero* pHero = dynamic_cast<iHero*>(pMapObj))
				pMapObj = pHero->GetLocation();
			if(dynamic_cast<iVisCnst_Harbor*>(pMapObj) && pMapObj->Pos() == iPoint(x + xx, y + yy)) {
				hrb = dynamic_cast<iVisCnst_Harbor*>(pMapObj);
				// check docks for our ship
				bool found = false;
				for(uint32 xx=0; xx<hrb->NumOfDocks(); xx++)
					if(hrb->GetShip(xx) == this) {
						found = true;
						dock = xx;
						break;
					}
					if(!found)
						hrb = NULL;
			}
		}
	}	
	return hrb;
}

void iShip::OrientInHarbor()
{
	uint32 dock;
	iVisCnst_Harbor* hrb;
	if(!(hrb = GetHarborCnst(dock))) return; // no action needed
	if(dock == 0) {
		m_Angle = 5; // horizontally
	} else if(dock == 1) {
		m_Angle = 3; // vertically
	} else 
		check(0);
}

// END OF FILE