#ifndef PHEROES_GAME_MAP_H_
#define PHEROES_GAME_MAP_H_

/*
 *  Map information
 */
struct iMapInfo {
    enum GameMode {
        GM_UNDEFINED = 0,
        GM_SPLAYER,
        GM_HOTSEAT
#ifdef MULTIPLAYER
        , GM_NETWORK
#endif
    };

    struct iPlayerInfo {
        iPlayerInfo(PLAYER_ID pid, PLAYER_TYPE_MASK ptypemask, PLAYER_TYPE ptype, CTL_TYPE ntype) 
        : m_Id(pid), m_TypeMask(ptypemask), m_Type(ptype), m_Nation(ntype) {}

        iMineralSet         m_Minerals;
        PLAYER_ID           m_Id;
        PLAYER_TYPE_MASK    m_TypeMask;
        PLAYER_TYPE         m_Type;
        CTL_TYPE            m_Nation;
        uint16              m_curHeroId;
        uint16              m_curCastleIdx;
        uint8               m_keys;
        uint16              m_curVisitorId;
    };

    void Serialize(iDynamicBuffer& dbuff) const;
    void Unserialize(iDynamicBuffer& dbuff);
    
    bool ReadMapInfo(iFileI* pFile, bool checkver = true);
    void ReorderPlayers();
    void PrepareForNetworkPlay(PLAYER_ID ownId, bool host);
    inline uint32 TotalPlayers() const { return m_Players.GetSize(); }
    inline uint32 HumanPlayers() const { uint32 res = 0; for (uint32 xx=0; xx<m_Players.GetSize(); ++xx) if (m_Players[xx].m_TypeMask == PTM_HUMAN_ONLY || m_Players[xx].m_TypeMask == PTM_HUMAN_OR_COMPUTER) res++; return res; }
    const iStringT& MapName() const;
    const iStringT& MapEnglishName() const;
    const iStringT& MapDescription() const;
    const iStringT& Id() const { return m_Id; }

    bool Supported() const;

	bool NotOfAnyType() const;
	
    uint16                      m_encType;
    bool                        m_bNewGame;
    uint32                      m_saveTime;
    GameMode                    m_gameMode;
    fileaccessor::RelativeFilePath                    m_FileName;
    DIFFICULTY_LEVEL            m_Difficulty;
    iSimpleArray<iPlayerInfo>   m_Players;
    MAP_SIZE                    m_Size;
    iStringT                    m_Id;
    iStringT                    m_Name[GLNG_COUNT]; // we must have a copy of this string for all supported languages to show in map selection dlg
    iStringT                    m_Description[GLNG_COUNT];

    uint32                      m_supportedLang; // mask of supported languages
    iStringT                    m_Version;
    iStringT                    m_Author;

    uint32                      m_curDay;
    PLAYER_ID                   m_curPlayerId;
    uint32                      m_rseed;
    iSize                       m_metrics;
    uint32                      m_globalProperties;
};

/*
 *  
 */
struct iCell {
    iCell() : surf(0), avatar(0) {}

    inline uint32 SurfNode(uint32 idx) const
    { check(idx<4); return (surf >> ((3-idx)*4)) & 0xF; }

    inline bool Solid() const
    { return (SurfNode(0) == SurfNode(1)) && (SurfNode(1) == SurfNode(2)) && (SurfNode(2) == SurfNode(3)); }


    uint16 surf;
    uint16 avatar;
};

/*
 *  
 */
class iGameMap : public iMapT<iCell>
{
public:
    typedef iIList<iVisCnst>        iVisCnstList;
    typedef iVisCnstList::Iterator  iVCIt;

    typedef iIList<iOwnCnst>        iOwnCnstList;
    typedef iOwnCnstList::Iterator  iOCIt;
    typedef iOwnCnstList::ConstIterator iCOCIt;

    typedef iIList<iCastle>         iCastleList;
    typedef iCastleList::Iterator   iCtIt;

    typedef iIList<iMapItem>        iMapItemList;
    typedef iMapItemList::Iterator  iMIt;

    typedef iIList<iMapEvent>       iMapEventList;
    typedef iMapEventList::Iterator iEIt;

    typedef iIList<iMapMarker>      iMapMarkerList;
    typedef iMapMarkerList::Iterator    iMrkIt;


    typedef iIList<iMapGuard>       iMapGuardList;
    typedef iMapGuardList::Iterator iGIt;

    typedef iIList<iPlayer>         iPlayerList;
    typedef iPlayerList::Iterator   iPLIt;
    typedef iPlayerList::ConstIterator  iCPLIt;

    typedef iIList<iHero>           iHeroList;
    typedef iHeroList::Iterator     iHLIt;

    typedef iIList<iShip>           iShipList;
    typedef iShipList::Iterator     iSLIt;

public:
    iGameMap();
    ~iGameMap();

    struct iSurfCell {
        uint8 lowestLayer;
        uint8 layers[STYPE_COUNT];
    };

    bool GetCellSurf(const iPoint& pos, iSurfCell& cell)
    {
        memset(&cell, 0, sizeof(iSurfCell));
        iCell surf = GetAt(pos.x,pos.y);

        // solid
        if (surf.SurfNode(0) == surf.SurfNode(1) && surf.SurfNode(1) == surf.SurfNode(2) && surf.SurfNode(2) == surf.SurfNode(3)) {
            cell.lowestLayer = (uint8)surf.SurfNode(0);
            return true;
        }

        // calculate underlying surface type
        cell.lowestLayer = (uint8)iMIN(surf.SurfNode(0),iMIN(surf.SurfNode(1),iMIN(surf.SurfNode(2),surf.SurfNode(3))));
        // setup config masks for all type of surfaces
        for (uint32 xx=0; xx<4; ++xx) {
            if (surf.SurfNode(xx) != cell.lowestLayer) cell.layers[surf.SurfNode(xx)] |= (1 << xx);
        }

        return false;

        /*
        //  1 bit   : solid surface  flag
        //  3 bits  : underlying surface type
        //  28 bits : 4 bits per surface transition config 4*7=28 bits
        iCell cell = GetAt(pos.x,pos.y);
        uint32 ccfg = 0;

        if ( cell.Solid() ) {
            // setup solid flag and surface type
            ccfg |= (1<<31) | ( cell.SurfNode(0) << 28);
            return ccfg;
        }

        uint8 uls = (uint8)iMIN(cell.SurfNode(0),iMIN(cell.SurfNode(1),iMIN(cell.SurfNode(2),cell.SurfNode(3))));
        ccfg |= (uls<<28);
        for (uint32 xx=0; xx<4; ++xx){
            if (cell.SurfNode(xx) != uls) ccfg |= (1 << (cell.SurfNode(xx)*4+xx));
        }

        return ccfg;
        */
    }

};


#endif //PHEROES_GAME_MAP_H_
