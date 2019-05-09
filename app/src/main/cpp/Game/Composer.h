#ifndef PHEROES_GAME_COMPOSER_H_
#define PHEROES_GAME_COMPOSER_H_

/*
 *	
 */
struct ComposeItem {
	inline ComposeItem(SpriteId sid, const iPoint& pos, uint8 shadow, SPRITE_LEVEL level, sint8 a, uint8 ht, uint8 crop)
	: m_sid(sid), m_pos(pos), m_shadow(shadow), m_level(level), m_alpha(a), m_ht(ht), m_crop(crop) {}

	inline bool Solid() const
	{ return m_alpha == 255; }

	SpriteId	m_sid;
	iPoint		m_pos;
	uint8		m_shadow;
	uint8		m_level;
	uint8		m_alpha;
	sint8		m_ht;
	uint8		m_crop;
};

#ifndef BUILD_EXPORTER

inline SpriteId GetHeroSprite(iHero *pHero)
{ return PDGG(HEROES)+pHero->Angle()*9 + pHero->Proto()->m_hType*72; }


inline SpriteId GetShipSprite(iShip *pShip)
{ return SHIP_TYPE_DESC[pShip->GetImgType()].sprite + pShip->Angle() * 6;  }


// workaround for bad pictures order in ship sprite
/*
static uint8 arrAngleToShipAngle[8] = 
{
    3, 4, 1, 7, 2, 6, 0, 5    
};
*/

inline SpriteId GetHeroInShipSprite(iHero *pHero)
{       
    return SHIP_TYPE_DESC[pHero->GetShip()->GetImgType()].sprite + pHero->Angle() * 6; 
}

#endif
/*
 *	
 */
class iComposeList : public iSortArray<ComposeItem>
{
public:
	iComposeList() {}

	void AddItem(SpriteId sid, const iPoint& pos, uint8 shadow, SPRITE_LEVEL level, uint32 snum, uint8 a=255, sint8 ht = -1, uint8 cropped=0);
	void Cleanup()
	{
		m_Array.RemoveAll();
		m_ComposeRect = cInvalidRect;
	}

	void SetComposeRect(const iRect& rect)
	{
		m_ComposeRect = rect;
	}

private:
	iRect				m_ComposeRect;
};

/*
 *	
 */
class iOLComposer: public IAniCB
{
public:
	iOLComposer(const iRect& rc, iView* pOwner);
	~iOLComposer();

	bool Init();
	void Cleanup();

	// Metrics
	inline sint32 MapPixW() const { return gGame.Map().GetWidth() * 40; }
	inline sint32 MapPixH() const { return gGame.Map().GetHeight() * 20; }

	// Anchor
	void SetAnchor(const iPoint& pos);
	inline const iPoint& GetAnchor() const { return m_Anchor; }
	iPoint ActAnchor(const iPoint& pos) const;
	iPoint GetCenterCell(const iPoint& anchor) const;
	iPoint GetCenterViewAnchor(const iPoint& cell, const iPoint& offset = iPoint());
	
	iPoint GetCenterCell() const;
	void CenterView(const iPoint& cell, const iPoint& offset = iPoint());
	
	bool IsOnScreen(const iPoint& pos);
	
	// Animation CB
	void AniDone();
	bool StepAni(fix32 t);
	void SetAniObj(iAniObj* pObj);
	iAniObj* RemoveAniObj();
	inline iAniObj* AniObj() { return m_pAniObj; }
	void AddCellMsg(const iStringT& msg, const iPoint& pos);

	// Target
	inline const iPoint& GetTargetPoint() const { return m_ptTarget; }
	inline SpriteId GetTargetCursor() const { return m_crTarget; }
	inline void SetTarget(const iPoint& ptTarget, SpriteId crTarget) { m_ptTarget = ptTarget; m_crTarget = crTarget; }

	void Invalidate() {m_lastCompAnchor = cInvalidPoint;}
    void SetRect(const iRect& rect) { m_Rect = rect; }

	void Compose(iDib* outDib, fix32 t);
	static void Compose(iDib& outDib, const iPoint& anchor, bool bPuzzleMap = false, const iPoint& tgtCell = cInvalidPoint);

	inline void SetRedCross(const iPoint& rc) { m_RedCross = rc; }
	inline void SetZoom(uint32 grade) { m_zoom = grade; Invalidate(); }
	inline uint32 GetZoom() { return m_zoom;  }
	
private:
	iView*			m_pOwner;
	iRect			m_Rect;
	iPoint			m_Anchor;
	iPoint			m_lastCompAnchor;
	iDib			m_dibSurfLayer;
	iDib			m_dibSurveyBuff;
	iComposeList	m_ComposeList;
	bool			m_bSurveyMode;

	iPoint			m_ptTarget;
	SpriteId		m_crTarget;

	iAniObj*		m_pAniObj;
	iEventList		m_cellEvents;
	iPoint			m_RedCross;
	uint32			m_zoom;
};


#endif //PHEROES_GAME_COMPOSER_H_
