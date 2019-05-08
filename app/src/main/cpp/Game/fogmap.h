#ifndef PHEROES_GAME_FOGMAP_H_
#define PHEROES_GAME_FOGMAP_H_

/*
 *	Fog Map
 */
class iFogMap : private iMapT<uint8>
{
public:
	bool InitFogMap(uint32 w, uint32 h, bool bFogMap);
	void CleanupFogMap() { Cleanup(); }
	void ResetFog();
	void DiscoverMap(sint32 x, sint32 y, sint32 radius, bool bHide);
	bool GetFogCfg(const iPoint& pos, uint8& dcfg, uint8& fcfg) const;
	bool IsInvisible(const iPoint& pos) const;
	bool IsHidden(const iPoint& pos) const;

	inline bool IsVisBound(const iPoint& pos) const
	{ return IsVisBound(pos.x, pos.y); }

	inline bool IsVisBound(sint32 x, sint32 y) const
	{
		return 
			((IsValidPos(x+16,y+16) && GetAt(x+16,y+16) == 0) || (IsValidPos(x+16+1,y+16) && GetAt(x+16+1,y+16) == 0) ||
			(IsValidPos(x+16,y+16+1) && GetAt(x+16,y+16+1) == 0) || (IsValidPos(x+16+1,y+16+1) && GetAt(x+16+1,y+16+1) == 0));
	}

	inline uint8* GetFogMapPtr() { return m_Buff; }
	inline uint32 GetFogMapDataSize() { return m_Width * m_Height; }

	inline bool IsValidCell(const iPoint& pos) const
	{ return IsValidPos(pos.x+16,pos.y+16); }

private:
	bool	m_bFogMap;
};


#endif //PHEROES_GAME_FOGMAP_H_
