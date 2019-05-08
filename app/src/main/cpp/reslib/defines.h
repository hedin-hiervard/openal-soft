#ifndef RESLIB_DEFINES_H_
#define RESLIB_DEFINES_H_

/*
 *  
 */
#define RGB16(r,g,b) (uint16)((r&0xF8)<<8 | (g&0xFC)<<3 | (b&0xF8)>>3)


template <class KeyType>
struct SpriteElT {
    SpriteElT(const KeyType& _spriteId, SPRITE_LEVEL _level, const iPoint& _anchor)
    : spriteId(_spriteId), level(_level), anchor(_anchor) {}

    KeyType         spriteId;
    SPRITE_LEVEL    level;
    iPoint          anchor;
};

struct PassMod {
    PassMod(const iPoint& _offset, sint8 _mod)
    : offset(_offset), mod(_mod) {}
    iPoint  offset;
    sint8   mod;
};

typedef iStringT    SpriteId;
typedef SpriteElT<SpriteId>     SpriteEl;
typedef iSimpleArray<SpriteEl>  SpriteList;

typedef iSimpleArray<PassMod>   PassModList;
typedef iSimpleArray<iPoint>    FlagList;

//
class iAvatar
{
public:
    iAvatar(const iStringT& id, const iStringT& nameKey, uint32 _idx)
    : m_id(id), m_nameKey(nameKey), m_idx(_idx) {}

    // Index
    void SetIdx(uint32 nIdx)
    { m_idx = nIdx;}
    inline uint32 Idx() const
    { return m_idx; }

    // Id
    void SetId(const iStringT& nId)
    { m_id = nId; }
    inline const iStringT& Id() const
    { return m_id; }

    // Name key
    void SetNameKey(const iStringT& nNameKey)
    { m_nameKey = nNameKey; }
    inline const iStringT& NameKey() const
    { return m_nameKey; }
    inline bool HasName() const
    { return !m_nameKey.Empty(); } 

    // Pass mods
    void AddPassMod(const iPoint& pos, sint8 mod)
    { m_passMods.Add(PassMod(pos,mod)); }
    inline uint32 PassModsCount() const
    { return m_passMods.GetSize(); }
    inline const PassMod& PassModEntry(uint32 idx) const
    { return m_passMods[idx]; }

    // Sound
    void SetSoundId(const iStringT& soundId) { m_soundId = soundId; }
    const iStringT& GetSoundId() const { return m_soundId; }

    // Sprites
    void AddSprite(const SpriteId& sid, SPRITE_LEVEL level, const iPoint& anchor)
    { m_sprites.Add(SpriteEl(sid,level, anchor)); }
    inline uint32 SpritesCount() const
    { return m_sprites.GetSize(); }
    inline const SpriteEl& SpriteEntry(uint32 idx) const
    { return m_sprites[idx]; }

    // Flags
    void AddFlag(const iPoint& pos)
    { m_flags.Add(pos); }
    inline uint32 FlagsCount() const
    { return m_flags.GetSize(); }
    inline const iPoint& FlagEntry(uint32 idx) const
    { return m_flags[idx]; }

protected:
    FlagList    m_flags;
    SpriteList  m_sprites;
    PassModList m_passMods;
    uint32      m_idx;
    iStringT    m_soundId;
    iStringT    m_id;
    iStringT    m_nameKey;
};
//
typedef iStringT    AvatarId;
struct AvatarEntry {
    AvatarEntry(const iPoint& _offset, const AvatarId& _id) : offset(_offset), id(_id) {}
    iPoint      offset;
    AvatarId    id;
};
typedef iSimpleArray<AvatarEntry>   AvatarList;
//
struct iVisualObject {
    void AddAvatar(const AvatarId& aid, const iPoint& offset)
    { m_AvatarList.Add(AvatarEntry(offset,aid)); }
    void AddAvatar(const AvatarEntry& avt)
    { m_AvatarList.Add(avt); }

    inline uint32 AvatarsCount() const
    { return m_AvatarList.GetSize(); }
    inline const AvatarEntry Avatar(uint32 idx) const
    { check(idx<m_AvatarList.GetSize()); return m_AvatarList[idx]; }

private:
    AvatarList  m_AvatarList;
};

//
class iIdxItem
{
public:
    iIdxItem(const iStringT& id, uint16 idx = 0xFFFF) : m_Id(id), m_Idx(idx) {}

    inline const iStringT& Id() const
    { return m_Id; }
    inline uint16 Idx() const
    { return m_Idx; }
    inline uint16& Idx()
    { return m_Idx; }

private:
    iStringT    m_Id;
    uint16      m_Idx;
};
/*
 *  
 */

enum TextEntryType {
    TET_DEFAULT = 0,
    TET_MAP_NAME,       
    TET_MAP_DESC,       
    TET_TEVENT_TEXT,    
    TET_HERO_CNAME,     
    TET_SHIP_CNAME,     
    TET_MAPITEM_MSG,    
    TET_MAPGUARD_INTROMSG,   
    TET_MEVENT_REWMSG,  
    TET_MEVENT_ATTMSG,  
    TET_MEVENT_RFAILMSG, 
    TET_VIS_INTROMSG,        
    TET_CASTLE_CNAME, 
	TET_MAPGUARD_OKREWMSG,
	TET_MAPGUARD_CURFAILMSG,
	TET_MAPGUARD_PERMFAILMSG,
	TET_VIS_OKREWMSG,
	TET_VIS_CURFAILMSG,
	TET_VIS_PERMFAILMSG,
	TET_OWN_INTROMSG,
	TET_OWN_OKREWMSG,
	TET_OWN_CURFAILMSG,
	TET_OWN_PERMFAILMSG,
    TET_COUNT
};

extern bool TEXT_ENTRY_ML[TET_COUNT];

iStringT MakeTextEntryName(TextEntryType t, const void* pObj);
GAME_LANG GetLangNameId( iStringT& lang );

extern LPCTSTR LANG_NAME[GLNG_COUNT];
extern LPCTSTR SURF_ID[STYPE_COUNT];
extern LPCTSTR SURF_NAMEKEY[STYPE_COUNT];

extern LPCTSTR MAP_SIZ_SNAME[MSIZ_COUNT];
extern LPCTSTR MAP_SIZ_NAMEKEY[MSIZ_COUNT];

extern LPCTSTR DIFF_NAMEKEY[DFC_COUNT];
extern LPCTSTR VICTCOND_NAMEKEY[VC_COUNT];
extern LPCTSTR LOSECOND_NAMEKEY[LC_COUNT];
extern LPCTSTR PLAYER_NAMEKEY[PID_COUNT];
extern LPCTSTR PLAYER_TYPE_NAMEKEY[PT_COUNT];
extern LPCTSTR PLAYER_TYPE_MASK_NAMEKEY[PTM_COUNT];

extern LPCTSTR HERO_TYPE_ID[HERO_TYPE_COUNT];
extern LPCTSTR HERO_TYPE_NAMEKEY[HERO_TYPE_COUNT];

extern LPCTSTR VCNSTTYPE_ID[VCNST_COUNT];
extern LPCTSTR OCNSTTYPE_ID[OCNST_COUNT];
extern LPCTSTR ARTTYPE_ID[ARTT_COUNT];
extern LPCTSTR ARTASSIGN_ID[ART_ASSIGN_COUNT];

extern LPCTSTR CASTLE_PREFIXES[CTLT_COUNT];
extern LPCTSTR CASTLE_NAMEKEY[CTLT_COUNT];
extern LPCTSTR CTLCNST_NAMEKEY[CTLCNST_COUNT];

extern LPCTSTR FURTSKILL_ID[FSK_COUNT];
extern LPCTSTR PRSKILL_ID[PRSKILL_COUNT];
extern LPCTSTR SECSKILL_ID[SECSK_COUNT];
extern LPCTSTR PRSKILL_NAMEKEY[PRSKILL_COUNT];
extern LPCTSTR SECSK_NAMEKEY[SECSK_COUNT];
extern LPCTSTR SSLVL_NAMEKEY[SSLVL_COUNT];
extern LPCTSTR SPELL_NAMEKEY[MSP_COUNT];

extern LPCTSTR MINERAL_ID[MINERAL_COUNT];
extern LPCTSTR MINERAL_NAMEKEY[MINERAL_COUNT];
extern LPCTSTR RESOURCE_NAMEKEY[MINERAL_COUNT];
extern LPCTSTR RAND_GUARDS_NAMEKEY[7];

extern LPCTSTR ART_LEVEL_ID[ART_LEVEL_COUNT];

extern LPCTSTR CREAT_ID[CREAT_COUNT];
extern LPCTSTR CREAT_NAMEKEY[CREAT_COUNT];

extern LPCTSTR MAGIC_SPELL_ID[MSP_COUNT];
extern LPCTSTR MAGIC_SPELL_NAMEKEY[MSP_COUNT];

extern LPCTSTR REWARD_TYPE_ID[RIT_COUNT];
extern LPCTSTR MAGSCHOOL_ID[MST_COUNT];
extern LPCTSTR CREAT_BONUS_ID[CBT_COUNT];

extern LPCTSTR COLOR_KEYMASTER[6];
extern LPCTSTR COLOR_KEYMASTER_SNOW[6];
extern uint32 COLOR_KEYMASTER_CODE[6];

#endif //RESLIB_DEFINES_H_
