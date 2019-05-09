#ifndef PHEROES_GAME_SHIP_H_
#define PHEROES_GAME_SHIP_H_
//////////////////////////////////////////////////////////////////////////
// Ship.cpp
//////////////////////////////////////////////////////////////////////////

#ifndef _HMM_GAME_SHIP_H_
#define _HMM_GAME_SHIP_H_


class iShip : public iIListNode, public iBaseMapObject
{
private:
    IMPL_TYPEAWARE( iShip );


public:
    iShip(const iPoint& pos, uint32 img_type);
    iShip();
    ~iShip();
    void Serialize(iDynamicBuffer& dbuff) const;
    void Unserialize(iDynamicBuffer& dbuff, bool bInit, bool bAfterLoad, bool bSetOwner, iHero* pHero);

    inline uint8& Angle() { return m_Angle; }
    inline uint8 Angle() const { return m_Angle; }
    inline void SetAngle(uint8 angle) { m_Angle = angle;};
    void SetPos(const iPoint& pos);
    
    inline uint32 GetImgType(){ return m_img_type;}; // get image type, different ships are different only by images
    inline bool IsLoaded() const { return m_pHero != 0; };     // is hero inside ship    
	inline iHero* Hero() { return m_pHero; }	    // return passenger or NULL if no
    void Occupy(iHero *pHero);
    void Leave(iHero *pHero, const iPoint &pt);
    void HeroDeath(iHero *pHero);

	/// returns iVisCnst_Harbor object, ship is parked at or NULL if none
	iVisCnst_Harbor* GetHarborCnst(uint32& dock) const;

	/// if ship is in iVisCnst_Harbor, orients it the proper way
	void OrientInHarbor();
	
private:
    iHero*      m_pHero; ///< current hero on ship
    uint8		m_Angle; ///< angle ship is rotated
    uint32      m_img_type; ///< the sprite type of the ship
};


#endif // _HMM_GAME_SHIP_H_
// END OF FILE
#endif //PHEROES_GAME_SHIP_H_
