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
#include "zip/unzip.h"
#include "ifaces.h"

extern iItemMgr gItemMgr;

/*
 *  
 */
class iResXmlReader : public Sax::PathHandler
{
public:
    iResXmlReader(iItemMgr& itemMgr, const iStringT& path, bool packed = false)
    : m_ItemMgr(itemMgr), m_Path(path), m_pCtlNode(NULL), m_ctlCfg(1024), m_Packed(packed) {}

    iBaseVisCnstNode* ParseVisCnst(const Attributes& attr)
    {
        sint32 tidx = FindStr(VCNSTTYPE_ID,VCNST_COUNT,attr[_T("type")].CStr());
        if (tidx == -1) {
            OutputDebugString(attr[_T("type")].CStr());
            check(0);
        }

        if (tidx == VCNST_BASIC) {
            iBaseVisCnstNode* pRes = new iBaseVisCnstNode(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")],VCNST_BASIC, false, true);
            return pRes;
        } else if (tidx == VCNST_STABLE) {
            check(attr.Exists(_T("actpts")));
            sint32 act_pts = attr[_T("actpts")].GetInt();
            iVisCnstNode_Stable* pRes = new iVisCnstNode_Stable(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")],act_pts);
            return pRes;
        } else if (tidx == VCNST_GAZEBO) {
            iVisCnstNode_Gazebo* pRes = new iVisCnstNode_Gazebo(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_MANASOURCE) {
            check(attr.Exists(_T("manapts")));
            sint32 mana_pts = attr[_T("manapts")].GetInt();
            iVisCnstNode_ManaSource* pRes = new iVisCnstNode_ManaSource(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], mana_pts);
            return pRes;
        } else if (tidx == VCNST_MLMODIFIER) {
            check(attr.Exists(_T("morale")) || attr.Exists(_T("luck")));
            sint8 mp = (sint8)attr[_T("morale")].GetInt();
            sint8 lp = (sint8)attr[_T("luck")].GetInt();
            iVisCnstNode_MLModifier* pRes = new iVisCnstNode_MLModifier(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], mp, lp);
            return pRes;
        } else if (tidx == VCNST_PSMODIFIER) {
            iPrSkills ps;
            for (uint32 xx=0; xx<PRSKILL_COUNT; ++xx) if (attr.Exists(PRSKILL_ID[xx])) ps.val[xx] = attr[PRSKILL_ID[xx]].GetInt();
            iVisCnstNode_PSModifier* pRes = new iVisCnstNode_PSModifier(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], ps);
            return pRes;
        } else if (tidx == VCNST_OBELISK) {
            iVisCnstNode_Obelisk* pRes = new iVisCnstNode_Obelisk(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_SIGN) {
            iVisCnstNode_Sign* pRes = new iVisCnstNode_Sign(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_DWELLING) {
            check(attr.Exists(_T("ctype")));
            sint32 ctype = FindStr(CREAT_ID,CREAT_COUNT,attr[_T("ctype")].CStr());
            check(ctype != -1);
            iVisCnstNode_Dwelling* pRes = new iVisCnstNode_Dwelling(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], (CREATURE_TYPE)ctype);
            return pRes;
        } else if (tidx == VCNST_WITCHHUT) {
            iVisCnstNode_WitchHut* pRes = new iVisCnstNode_WitchHut(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_SHRINE) {
            uint32 level = attr[_T("level")].GetUInt();
            check(level >= 1 && level <= 5);
            level--;
            iVisCnstNode_Shrine* pRes = new iVisCnstNode_Shrine(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], (SPELL_LEVEL)level);
            return pRes;
        } else if (tidx == VCNST_TREASURY) {		
            iVisCnstNode_Treasury* pRes = new iVisCnstNode_Treasury(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_TELEPORT) {
            iVisCnstNode_Teleport* pRes = new iVisCnstNode_Teleport(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], (uint8)attr[_T("mode")].GetUInt(), attr[_T("index")].GetUInt());
            return pRes;
        } else if (tidx == VCNST_HARBOR) {
            iVisCnstNode_Harbor* pRes = new iVisCnstNode_Harbor(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_WARACADEMY) {
            iVisCnstNode_WarAcademy* pRes = new iVisCnstNode_WarAcademy(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_NEWTELEPORT) {
            iVisCnstNode_NewTeleport* pRes = new iVisCnstNode_NewTeleport(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], 0, 0);
            return pRes;
        } else if (tidx == VCNST_KEYMASTER) {
            iVisCnstNode_Keymaster* pRes = new iVisCnstNode_Keymaster(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")], (uint8)attr[_T("key")].GetUInt());
            return pRes;
        } else if (tidx == VCNST_KNWLTREE) {
            iVisCnstNode_TreeOfKnowledge* pRes = new iVisCnstNode_TreeOfKnowledge(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_WINDMILL) {
            iVisCnstNode_Windmill* pRes = new iVisCnstNode_Windmill(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            return pRes;
        } else if (tidx == VCNST_WEEKLYMIN) {
            iVisCnstNode_WeeklyMin* pRes = new iVisCnstNode_WeeklyMin(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("vmsg")]);
            pRes->Minerals();
            for (uint32 minId=0; minId<MINERAL_COUNT; ++minId) {
                if (sint32 val = attr[MINERAL_ID[minId]].GetInt()) {
                    pRes->Minerals().quant[minId] = val;
                }
            }
            
            return pRes;
        }

        check(0);
        return NULL;
    }

    iBaseOwnCnstNode* ParseOwnCnst(const Attributes& attr)
    {
        sint32 tidx = FindStr(OCNSTTYPE_ID,OCNST_COUNT,attr[_T("type")].CStr());
        check(tidx!=-1);
        check(attr.Exists(_T("scouting")));
        uint8 scouting= (uint8)attr[_T("scouting")].GetUInt();
        if (!scouting) scouting = 4;
        if (tidx == OCNST_BASIC) {
            iBaseOwnCnstNode* pRes = new iBaseOwnCnstNode(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("msgIcn")], OCNST_BASIC, scouting);
            return pRes;
        } else if (tidx == OCNST_FURTSKILL) {
            iFurtSkills fs;
            for (uint32 xx=0; xx<FSK_COUNT; ++xx) if (attr.Exists(FURTSKILL_ID[xx])) fs.Value((FURTHER_SKILLS)xx) = attr[FURTSKILL_ID[xx]].GetInt();
            iOwnCnstNode_FurtSkill* pRes = new iOwnCnstNode_FurtSkill(attr[_T("id")], attr[_T("name")], attr[_T("msg")], attr[_T("msgIcn")], fs, scouting);
            return pRes;
        }

        check(0);
        return NULL;
    }

    iArtifactNode* ParseArtifact(const Attributes& attr)
    {
        sint32 tidx = FindStr(ARTTYPE_ID,ARTT_COUNT,attr[_T("type")].CStr());
        check(tidx!=-1);
        check(attr.Exists(_T("level")));
        ART_LEVEL_TYPE level = (ART_LEVEL_TYPE)attr[_T("level")].GetUInt();
        check(attr.Exists(_T("assign")));
        ART_ASSIGN_TYPE assign = (ART_ASSIGN_TYPE)FindStr(ARTASSIGN_ID,ART_ASSIGN_COUNT,attr[_T("assign")].CStr());
        check(assign != -1);
        check(attr.Exists(_T("icon")));
        SpriteId icon = iStringT(_T("artifacts.")) + attr[_T("icon")];
        uint32 reqLevel = attr[_T("reqlvl")].GetUInt();
        SECONDARY_SKILLS reqSecSkill = SECSK_NONE;
        if (attr.Exists(_T("reqss"))) {
            sint32 ssidx = FindStr(SECSKILL_ID,SECSK_COUNT,attr[_T("reqss")].CStr());
            check(ssidx!=-1);
            reqSecSkill = (SECONDARY_SKILLS)(ssidx);
        }
        iCreatBonus cb;
        cb.ct = CREAT_UNKNOWN;
        if(attr.Exists(_T("creatures"))) {
            cb.ct = (CREATURE_TYPE)FindStr(CREAT_ID,CREAT_COUNT,attr[_T("creatures")].CStr());
            for(uint32 xx=0; xx<CBT_COUNT;xx++) {
                if(attr.Exists(CREAT_BONUS_ID[xx])) {
                    cb.bt = (CREAT_BONUS_TYPE)xx;
                    cb.bonus = attr[CREAT_BONUS_ID[xx]].GetInt();
                    break;      
                }
            }
            check(cb.bt != -1 && cb.ct != -1);
        }
            
        if (tidx == ARTT_FURTSKILL) {
            iFurtSkills fs;
            for (uint32 xx=0; xx<FSK_COUNT; ++xx) if (attr.Exists(FURTSKILL_ID[xx])) fs.Value((FURTHER_SKILLS)xx) = attr[FURTSKILL_ID[xx]].GetInt();
            return new iArtifactNode_FurtSkill(attr[_T("id")], attr[_T("name")], attr[_T("desc")], level, assign, reqLevel, reqSecSkill, icon, fs, cb);
        } else if (tidx == ARTT_NEGSPHERE){
            return new iArtifactNode(attr[_T("id")], attr[_T("name")], attr[_T("desc")], ARTT_NEGSPHERE, level, assign, reqLevel, reqSecSkill, icon);
        } else if (tidx == ARTT_SHOFWAR){
            return new iArtifactNode(attr[_T("id")], attr[_T("name")], attr[_T("desc")], ARTT_SHOFWAR, level, assign, reqLevel, reqSecSkill, icon);
        } else if (tidx == ARTT_CURSWORD){
            return new iArtifactNode(attr[_T("id")], attr[_T("name")], attr[_T("desc")], ARTT_CURSWORD, level, assign, reqLevel, reqSecSkill, icon);
        } else if (tidx == ARTT_ICEAMULET){
            return new iArtifactNode(attr[_T("id")], attr[_T("name")], attr[_T("desc")], ARTT_ICEAMULET, level, assign, reqLevel, reqSecSkill, icon);
        } else if (tidx == ARTT_ARROWS){
            return new iArtifactNode(attr[_T("id")], attr[_T("name")], attr[_T("desc")], ARTT_ARROWS, level, assign, reqLevel, reqSecSkill, icon);
        } else if (tidx == ARTT_FASTCROSSBOW){
            return new iArtifactNode(attr[_T("id")], attr[_T("name")], attr[_T("desc")], ARTT_FASTCROSSBOW, level, assign, reqLevel, reqSecSkill, icon);
        } else if (tidx == ARTT_MAGICSPHERE){           
            for(uint32 xx=0; xx<MST_COUNT; xx++)
                if(attr.Exists(MAGSCHOOL_ID[xx]))
                    return new iArtifactNode_MagicSphere(attr[_T("id")], attr[_T("name")], attr[_T("desc")], level, assign, reqLevel, reqSecSkill, icon, xx);       
        }else {
            check(0);
        }

        check(0);
        return NULL;
    }

    bool OnEndDocument()
    {
        return true;
    }

    void ParseTreasuryGuard(const Attributes& attr)
    {
        check(m_pVisCnstNode->Type() == VCNST_TREASURY);
        iVisCnstNode_Treasury* pTreasury = (iVisCnstNode_Treasury*)m_pVisCnstNode;
        check(attr.Exists(_T("type")));
        sint32 ctype = FindStr(CREAT_ID,CREAT_COUNT,attr[_T("type")].CStr());
        check(ctype != -1);
        check(attr.Exists(_T("quant")));
        sint32 quant = attr[_T("quant")].GetUInt();
        pTreasury->m_vars.GetLastVariant().guards.AddGroup((CREATURE_TYPE)ctype, quant);
    }

    void ParseTreasuryReward(const Attributes& attr)
    {
        check(m_pVisCnstNode->Type() == VCNST_TREASURY);
        iVisCnstNode_Treasury* pTreasury = (iVisCnstNode_Treasury*)m_pVisCnstNode;
        check(attr.Exists(_T("type")));
        sint32 rtype = FindStr(REWARD_TYPE_ID, RIT_COUNT,attr[_T("type")].CStr());
        check(rtype != -1);
        sint32 fparam = 0, sparam = 0;
        if (rtype == RIT_MINERAL) {
            fparam = FindStr(MINERAL_ID,MINERAL_COUNT,attr[_T("fparam")].CStr());
            check(fparam != -1);
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_EXPERIENCE) {
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_MANAPTS) {
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_TRAVELPTS) {
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_MORALE) {
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_LUCK) {
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_FURTSKILL) {
            fparam = FindStr(FURTSKILL_ID,FSK_COUNT,attr[_T("fparam")].CStr());
            check(fparam != -1);
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_SECSKILL) {
            fparam = FindStr(SECSKILL_ID,SECSK_COUNT,attr[_T("fparam")].CStr());
            check(fparam != -1);
            sparam = attr[_T("sparam")].GetInt();
        } else if (rtype == RIT_ARTIFACT) {
            fparam = attr[_T("fparam")].GetInt();
            check(fparam >= 0 && fparam <= 4);
        } else if (rtype == RIT_MAGICSPELL) {
            fparam = attr[_T("fparam")].GetInt();
            check(fparam >= 0 && fparam <= 5);
        } else if (rtype == RIT_CREATGROUP) {
            fparam = FindStr(CREAT_ID,CREAT_COUNT,attr[_T("fparam")].CStr());
            check(fparam != -1);
            sparam = attr[_T("sparam")].GetInt();
        } else {
            check("Invalid Treasury reward type" == 0);
        }

        pTreasury->m_vars.GetLastVariant().rewards.Add(iRewardItem((REWARD_ITEM_TYPE)rtype, fparam, sparam));
    }

    bool OnEnterElement( const iStringT& path, const Attributes& attr )
    {
     //   OutputDebugString(path.CStr());
    //    OutputDebugString(L"\r\n");

        // Common set props
        if (path == _T("HMMConfig")) {
            m_ItemMgr.m_SetName = attr[_T("name")];
        // Init text manager
        } else if (path == _T("HMMConfig.LanguageFile")) {
            bool bPrimary = attr[_T("primary")].GetUInt() > 0;
            sint32 glang = FindStr(LANG_NAME, GLNG_COUNT, attr[_T("lang")].CStr());
            if (glang < GLNG_ENGLISH || glang >= GLNG_COUNT) {
                MessageBox(NULL, iFormat(_T("Invalid language file entry <%s>!"), attr[_T("lang")].CStr()).CStr(), NULL, MB_OK);
            } else {
                if(m_Packed) {
                    iStringT name = attr[_T("file")];
                    name.Replace(_T("\\"), _T("/"));
                    int length;
                    char* buffer = gItemMgr.GetZipBuffer(name, length);
                    //if (!AddLanguageFile(m_Path + attr[_T("file")], (GAME_LANG)glang, m_ItemMgr.m_TextMgr, bPrimary)) return false;
                    if (!AddLanguageFile(buffer, length, (GAME_LANG)glang, m_ItemMgr.m_TextMgr, bPrimary)) return false;
                }
                else {
                    if (!AddLanguageFile(m_Path + attr[_T("file")], (GAME_LANG)glang, m_ItemMgr.m_TextMgr, bPrimary)) return false;
                }
            }
        // Init sprite file
        } else if (path == _T("HMMConfig.SpriteFile")) {
            if (!m_ItemMgr.m_SpriteMgr.Init(m_Path, attr[_T("config")])) {
                return false;
            }
        } 
#ifdef BUILD_EXPORTER		
		// Init sound file
		else if (path == _T("HMMConfig.SoundFile")) {
            if(!m_Packed) {
                if (!m_ItemMgr.m_SoundMgr.Init(m_Path + attr[_T("config")])) {
                    return false;
                }
            }
        } 
		// Add font file
		else if (path == _T("HMMConfig.AddFont")) {
            if(!m_Packed) {
                m_ItemMgr.m_fontFiles.Add(m_Path + attr[_T("file")]);                           
            }		
        }
#endif
		// Add big pix
		else if (path == _T("HMMConfig.AddPix")) {
            if(!m_Packed) {
                m_ItemMgr.m_pixFiles.Add(iPair<iStringT, bool>(m_Path + attr[_T("file")], attr[_T("alpha")] == _T("1") ? true : false));                            
            }
        
        // Avatars
        } else if (path == _T("HMMConfig.Avatars.SeqItem")) {
            if (attr.Exists(_T("sequence"))) {
                uint32 seq_cnt = attr[_T("sequence")].GetUInt();
                check(seq_cnt>0);
                for (uint32 xx=0; xx<seq_cnt; ++xx) {
                    iStringT aid = iFormat(_T("%s_%04d"),attr[_T("id")].CStr(), xx);
                    iStringT aname = attr[_T("name")];
                    iStringT sid = iFormat(_T("%s_%04d"),attr[_T("spriteSeq")].CStr(), xx);
                    sint8 pmod = (sint8)attr[_T("passModifier")].GetInt();
                    SPRITE_LEVEL slvl = SPL_PLANTS;
                    if (attr.Exists(_T("level"))) slvl = (SPRITE_LEVEL)attr[_T("level")].GetUInt();
                    iAvatar* pAvatar = m_ItemMgr.m_AvatarMgr.AddAvatar(aid, aname);
                    pAvatar->AddSprite(sid,slvl,iPoint(0,0));
                    pAvatar->AddPassMod(iPoint(0,0),pmod);
                }
            } else {
                check(0);
            }
        } else if (path == _T("HMMConfig.Avatars.Item")) {
            m_pAvatar = m_ItemMgr.m_AvatarMgr.AddAvatar(attr[_T("id")], attr[_T("name")]);
        } 
#ifdef BUILD_EXPORTER			
		else if  (path == _T("HMMConfig.Avatars.Item.Sound")) {
            if(!m_Packed) {
                check(m_ItemMgr.m_SoundMgr.FindItem(attr[_T("id")]) != -1);
                m_pAvatar->SetSoundId(attr[_T("id")]);
            }
        } 
#endif
		else if  (path == _T("HMMConfig.Avatars.Item.Sprite")) {
            SPRITE_LEVEL slvl = SPL_PLANTS;
            if (attr.Exists(_T("level"))) slvl = (SPRITE_LEVEL)attr[_T("level")].GetUInt();
            m_pAvatar->AddSprite(attr[_T("id")],slvl,ParsePnt(attr[_T("offset")]));
        } else if  (path == _T("HMMConfig.Avatars.Item.PassModifier")) {
            m_pAvatar->AddPassMod(ParsePnt(attr[_T("offset")]),(sint8)attr[_T("value")].GetInt());
        } else if  (path == _T("HMMConfig.Avatars.Item.Flag")) {
            m_pAvatar->AddFlag(ParsePnt(attr[_T("offset")]));
        // Combat obstacles
        } else if (path == _T("HMMConfig.CombatObstacles.Item")) {
            m_ItemMgr.m_battleObstacles.Add(iBattleObstacle(attr[_T("sprite")], ParsePnt(attr[_T("offset")]), (uint8)attr[_T("size")].GetUInt(), (uint8)attr[_T("level")].GetUInt()));
        } else if (path == _T("HMMConfig.CombatObstacles.Item.Surf")) {
            check(m_ItemMgr.m_battleObstacles.GetSize());
            sint32 surfType = FindStr(SURF_ID,STYPE_COUNT,attr[_T("id")].CStr());
            check(surfType != -1);
            m_ItemMgr.m_battleObstacles.GetLast().m_surfMask |= SURF_TYPE_MASK[surfType];
        // Decorations
        } else if (path == _T("HMMConfig.Decorations.Group")) {
            m_pGroupNode = new iGroupNode(attr[_T("id")]);
            m_ItemMgr.m_pDecorNode->Add(m_pGroupNode);
        } else if (path == _T("HMMConfig.Decorations.Group.Item")) {
            m_pDecorNode = new iDecorNode(attr[_T("id")], attr[_T("name")]);
            m_pGroupNode->Add(m_pDecorNode);
        } else if (path == _T("HMMConfig.Decorations.Group.Item.Avatar")) {
            m_pDecorNode->AddAvatar(attr[_T("id")],ParsePnt(attr[_T("offset")]));
        // Path Elements
        } else if (path == _T("HMMConfig.PathElements.Item")) {
            SPRITE_LEVEL slvl = SPL_PLANTS;
            if (attr.Exists(_T("level"))) slvl = (SPRITE_LEVEL)attr[_T("level")].GetUInt();
            m_ItemMgr.m_pPathElNode->Add(new iPathElNode(attr[_T("id")],attr[_T("name")], (PATH_ELEMENT_TYPE)attr[_T("type")].GetInt(), attr[_T("sprite")], slvl, attr[_T("pass")].GetInt()));
        // Visitables
        } else if (path == _T("HMMConfig.Visitables.Item")) {
            m_ItemMgr.m_pVisCnstNode->Add(m_pVisCnstNode = ParseVisCnst(attr));
        } else if (path == _T("HMMConfig.Visitables.Item.Avatar")) {
            m_pVisCnstNode->AddAvatar(attr[_T("id")],ParsePnt(attr[_T("offset")]));
        // Treasuries
        } else if (path == _T("HMMConfig.Visitables.Item.TrVariant")) {
            check(m_pVisCnstNode->Type() == VCNST_TREASURY);
            iVisCnstNode_Treasury* pTreasury = (iVisCnstNode_Treasury*)m_pVisCnstNode;
            check(attr.Exists(_T("prob")));
            uint32 prob = attr[_T("prob")].GetUInt();
            check(prob>0 && prob <= 100);
            pTreasury->m_vars.AddVariant(prob);
        } else if (path == _T("HMMConfig.Visitables.Item.TrVariant.Guard")) {
            ParseTreasuryGuard(attr);
        } else if (path == _T("HMMConfig.Visitables.Item.TrVariant.Reward")) {
            ParseTreasuryReward(attr);
        // Ownerables
        } else if (path == _T("HMMConfig.Ownerables.Item")) {
            m_ItemMgr.m_pOwnCnstNode->Add(m_pOwnCnstNode = ParseOwnCnst(attr));
        } else if (path == _T("HMMConfig.Ownerables.Item.Avatar")) {
            m_pOwnCnstNode->AddAvatar(attr[_T("id")],ParsePnt(attr[_T("offset")]));
        // Castles
        } else if (path == _T("HMMConfig.Castles")) {
            for (uint32 cc=0; cc<CTLT_COUNT; ++cc) {
                m_ItemMgr.m_pCastleNode->Add(new iObjGroupNode(gItemMgr.m_TextMgr[CASTLE_NAMEKEY[cc]]));
            }
        } else if (path == _T("HMMConfig.Castles.Item")) {
            check(!m_pCtlNode);
            uint32 csiz = attr[_T("size")].GetUInt();
            check(csiz>0 && csiz<CTLS_COUNT);
            uint32 corient  = attr[_T("orient")].GetUInt();
            check(corient >= 0 && corient <= 1);
            m_pCtlNode = new iCastleNode(attr[_T("id")], CTLT_CITADEL, (CTL_SIZE)csiz, (CTL_ORIENT)corient, m_ctlCfg++);
        } else if (path == _T("HMMConfig.Castles.Item.Avatar")) {
            check(m_pCtlNode);
            m_pCtlNode->AddAvatar(iFormat(_T("_%s"),attr[_T("id")].CStr()),ParsePnt(attr[_T("offset")]));
        // Heroes
        } else if (path == _T("HMMConfig.Heroes.Item")) {
            sint32 n = FindStr(HERO_TYPE_ID,HERO_TYPE_COUNT,attr[_T("type")].CStr());
            check(n != -1);
            m_pHeroNode = new iHeroNode(attr[_T("id")],attr[_T("name")], (HERO_TYPE)n, attr[_T("icon32")], attr[_T("icon48")], attr[_T("icon46")]);
            m_ItemMgr.m_pHeroesNode->Add(m_pHeroNode);
        // Sea
        } else if (path == _T("HMMConfig.Sea.Item")) 
        {
			/*if( attr[_T("id")] == _T("harbor") ){
				
				m_pVisCnstNode = ParseVisCnst( attr );
				m_ItemMgr.m_pShipsNode->Add(m_pVisCnstNode);
			}else{
*/
				m_pShipNode = new iShipNode(attr[_T("id")],attr[_T("name")], attr[_T("type")].GetUInt());			
				m_ItemMgr.m_pShipsNode->Add(m_pShipNode);
			//}
		 } else if (path == _T("HMMConfig.Sea.Item.Avatar")) {
		        m_pVisCnstNode->AddAvatar(attr[_T("id")],ParsePnt(attr[_T("offset")]));
        // Treasuries
        } else if (path == _T("HMMConfig.Heroes.Item.Cost")) {
            iMineralSet cost;
            cost.Reset();
            for (uint32 minId=0; minId<MINERAL_COUNT; ++minId) {
                if (sint32 val = attr[MINERAL_ID[minId]].GetInt()) {
                    cost.quant[minId] = val;
                }
            }
            m_pHeroNode->Cost() = cost;
        } else if (path == _T("HMMConfig.Heroes.Item.FurtSkill")) {
            sint32 n = FindStr(FURTSKILL_ID,FSK_COUNT,attr[_T("id")].CStr());
            check(n != -1);
            sint32 val = attr[_T("value")].GetInt();
            m_pHeroNode->FurtSkills().Value((FURTHER_SKILLS)n) += val;
        } else if (path == _T("HMMConfig.Heroes.Item.SecSkill")) {
            sint32 n = FindStr(SECSKILL_ID,SECSK_COUNT,attr[_T("id")].CStr());
            sint32 lvl = attr[_T("level")].GetInt();
            check(n != -1 && lvl > 0);
            m_pHeroNode->SecSkills().Add(iSecSkillEntry((SECONDARY_SKILLS)n, (SECSKILL_LEVEL)(lvl-1)));
        } else if (path == _T("HMMConfig.Heroes.Item.Spell")) {
            sint32 n = FindStr(MAGIC_SPELL_ID,MSP_COUNT,attr[_T("id")].CStr());
            check(n != -1);
            m_pHeroNode->Spells().Add((uint16)n);
        // Artifacts
        } else if (path == _T("HMMConfig.Artifacts.Item")) {
            m_ItemMgr.m_pArtifactNode->Add(m_pArtifactNode = ParseArtifact(attr));
        } 

        return true;
    }

    bool OnLeaveElement( const iStringT& path ) 
    { 
        if (path == _T("HMMConfig.Castles.Item")) {
            check(m_pCtlNode);
            iObjGroupNode::Iterator cgIt = m_ItemMgr.m_pCastleNode->First();
            for (sint32 ctlt = CTLT_CITADEL; ctlt < CTLT_COUNT; ++ctlt, ++cgIt) {
                iCastleNode* pCtlNode = new iCastleNode(m_pCtlNode->Id(), (CTL_TYPE)ctlt, m_pCtlNode->Size(), m_pCtlNode->Orient(), m_pCtlNode->Config());
                for (uint32 xx=0; xx<m_pCtlNode->AvatarsCount(); ++xx) {
                    pCtlNode->AddAvatar(iStringT(CASTLE_PREFIXES[pCtlNode->Type()])+m_pCtlNode->Avatar(xx).id,m_pCtlNode->Avatar(xx).offset);
                }
                (*cgIt)->Add(pCtlNode);
            }

            delete m_pCtlNode;
            m_pCtlNode = NULL;
        }

        return true; 
    }
    
    iStringT        m_Path;
    iAvatar*        m_pAvatar;
    iGroupNode*     m_pGroupNode;

    iCastleNode*        m_pCtlNode;
    uint16              m_ctlCfg;
    iDecorNode*         m_pDecorNode;
    iBaseVisCnstNode*   m_pVisCnstNode;
    iBaseOwnCnstNode*   m_pOwnCnstNode;
    iHeroNode*          m_pHeroNode;
    iShipNode*          m_pShipNode;
    iArtifactNode*      m_pArtifactNode;

    bool            m_Hdr;
    bool            m_Packed;
    iItemMgr&       m_ItemMgr;
};

/*
 *  
 */

char* iItemMgr::GetZipBuffer(iStringT name, int& length) {
     int s = m_ZipItems.GetSize();
     for(int i = 0; i < s; i ++) {
         if(!name.CompareIgnoreCase(m_ZipItems[i].path)) {
             length = m_ZipItems[i].length;
             return m_ZipItems[i].buffer;
         }
     }
     return NULL;
} 

iItemMgr::~iItemMgr() {
    int s = m_ZipItems.GetSize();
    for(int i = 0; i < s; i ++) {
        delete m_ZipItems[i].buffer;
    }
}

/*
bool iItemMgr::InitRaw(const iStringT& fname, const iStringT &path)
{
    try {
        //      

        // Create item groups
        m_pResNode = new iObjGroupNode(_T("Map objects"));
        Add(m_pResNode);
        m_pArtifactNode = new iObjGroupNode(_T("Artifacts"));
        Add(m_pArtifactNode);
        m_pDecorNode = new iObjGroupNode(_T("Decorations"));
        Add(m_pDecorNode);
        m_pPathElNode = new iObjGroupNode(_T("Path Elements"));
        Add(m_pPathElNode);
        m_pVisCnstNode = new iObjGroupNode(_T("Visitables"));
        Add(m_pVisCnstNode);
        m_pOwnCnstNode = new iObjGroupNode(_T("Ownerables"));
        Add(m_pOwnCnstNode);
        m_pCastleNode = new iObjGroupNode(_T("Castles"));
        Add(m_pCastleNode);
        m_pHeroesNode = new iObjGroupNode(_T("Heroes"));
        Add(m_pHeroesNode);
        m_pGuardsNode = new iObjGroupNode(_T("Guards"));
        Add(m_pGuardsNode);
        
        // Read config
        iResXmlReader reader(*this,path);
        if (!Sax::Parse( fname, &reader )) throw 0;

        // Predefined resources
        sint32 xx;
        for (xx=-1; xx<MINERAL_COUNT; ++xx) m_pResNode->Add(new iMineralNode((MINERAL_TYPE)xx));
        m_pResNode->Add(new iManaCrystalNode());
        m_pResNode->Add(new iCampFireNode());
        m_pResNode->Add(new iChestNode());
        m_pResNode->Add(new iSpellScrollNode());
        m_pResNode->Add(new iLampNode());
        m_pResNode->Add(new iMapEventNode());
        m_pResNode->Add(new iMapMarkerNode());
        m_pResNode->Add(new iUltimateArtNode());
        for (xx=0; xx<6; ++xx) m_pResNode->Add(new iKeyGuardNode((uint8)xx));

        // Predefined creatures
        for (uint32 cid=0; cid<CREAT_COUNT; ++cid) m_pGuardsNode->Add(new iGuardNode(CREATURE_TYPE(cid)));
        // Add random guards
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L1));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L2));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L3));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L4));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L5));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L6));
        

        // Normalize indeces
        uint16 idx=0;
        for (iObjGroupNode::Iterator hit = m_pHeroesNode->First(); hit != m_pHeroesNode->End(); ++hit){
            ((iHeroNode*)(*hit))->Idx() = idx++;
        }
        idx=0;
        for (iObjGroupNode::Iterator vcit = m_pVisCnstNode->First(); vcit != m_pVisCnstNode->End(); ++vcit){
            ((iBaseVisCnstNode*)(*vcit))->Idx() = idx++;
        }
        idx=0;
        for (iObjGroupNode::Iterator ocit = m_pOwnCnstNode->First(); ocit != m_pOwnCnstNode->End(); ++ocit){
            ((iBaseOwnCnstNode*)(*ocit))->Idx() = idx++;
        }

        idx=0;
        iObjGroupNode::Iterator cit = m_pCastleNode->First();
        while (cit != m_pCastleNode->End()){
            if (iCastleNode* pCastle = DynamicCast<iCastleNode*>((iBaseNode*)(*cit))){
                pCastle->Idx() = idx++;
            }
            cit = cit.WalkNext();
        }       
        idx=0;
        for (iObjGroupNode::Iterator arit = m_pArtifactNode->First(); arit != m_pArtifactNode->End(); ++arit){
            ((iArtifactNode*)(*arit))->Idx() = idx++;
        }
        idx=0;
        for (iObjGroupNode::Iterator peit = m_pPathElNode->First(); peit != m_pPathElNode->End(); ++peit){
            ((iPathElNode*)(*peit))->Idx() = idx++;
        }


        // Random artifacts
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND, _T("RandomArtifact"), _T("RAND_ARTIFACT"),_T("artifacts.rand_artifacts_0000")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L1, _T("RandomArtifact1"), _T("RAND_ARTIFACT1"),_T("artifacts.rand_artifacts_0001")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L2, _T("RandomArtifact2"), _T("RAND_ARTIFACT2"),_T("artifacts.rand_artifacts_0002")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L3, _T("RandomArtifact3"), _T("RAND_ARTIFACT3"),_T("artifacts.rand_artifacts_0003")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L4, _T("RandomArtifact4"), _T("RAND_ARTIFACT4"),_T("artifacts.rand_artifacts_0004")));

    } catch (...) {
        return false;
    }

    m_bInited = true;
    return m_bInited;
}
*/

bool iItemMgr::Init(const iStringT& xmlFileName, const iStringT &packagePath, bool packed)
{
	m_bPacked = packed;
#ifdef OS_WIN32
	if( !m_bPacked )
		if (GetFileAttributes(xmlFileName.CStr()) == -1)
		{
			iStringT istr = iStringT(_T("Can't find: ")) + xmlFileName;
			MessageBox(0, istr.CStr(), 0, MB_TOPMOST | MB_ICONSTOP);
			return false;
		}
#endif
	    
    if(m_bPacked) m_SpriteMgr.SetPacked();

    try {
        if(m_bPacked) {
            HZIP zip;
            zip = OpenZip(packagePath.CStr(), 0);
            ZIPENTRY ze;
            GetZipItem(zip, -1, &ze); // -1 gives overall information about the zip file
            int num = ze.index;


            for(int i = 0; i < num; i ++) {
                GetZipItem(zip, i, &ze);
                ZipItem zi;
                if(ze.attr & FILE_ATTRIBUTE_DIRECTORY) {
                    continue;
                }
                zi.path = ze.name;
                zi.path.Trim();
                zi.buffer = new char[ze.unc_size];
                zi.length = ze.unc_size;
                UnzipItem(zip, i, zi.buffer, ze.unc_size);
                m_ZipItems.Add(zi);
            }

            CloseZip(zip);
        }

        // Create item groups
        m_pResNode = new iObjGroupNode(_T("Map objects"));
        Add(m_pResNode);
        m_pArtifactNode = new iObjGroupNode(_T("Artifacts"));
        Add(m_pArtifactNode);
        m_pDecorNode = new iObjGroupNode(_T("Decorations"));
        Add(m_pDecorNode);
        m_pPathElNode = new iObjGroupNode(_T("Path Elements"));
        Add(m_pPathElNode);
        m_pVisCnstNode = new iObjGroupNode(_T("Visitables"));
        Add(m_pVisCnstNode);
        m_pOwnCnstNode = new iObjGroupNode(_T("Ownerables"));
        Add(m_pOwnCnstNode);
        m_pCastleNode = new iObjGroupNode(_T("Castles"));
        Add(m_pCastleNode);
        m_pHeroesNode = new iObjGroupNode(_T("Heroes"));
        Add(m_pHeroesNode);
        m_pShipsNode = new iObjGroupNode(_T("Sea"));
        Add(m_pShipsNode);
        m_pGuardsNode = new iObjGroupNode(_T("Guards"));
        Add(m_pGuardsNode);

        // Read config
		iResXmlReader reader(*this, packagePath, true);

        if(m_bPacked) {
            int length;
            char* buffer = gItemMgr.GetZipBuffer(xmlFileName, length);

            if (!Sax::Parse(buffer, length, &reader)) throw 0;
        }
        else {
            // Read config
            iResXmlReader reader(*this, packagePath);
            if (!Sax::Parse(xmlFileName, &reader )) throw 0;
        }


        // Predefined resources
        sint32 xx;
        for (xx=-1; xx<MINERAL_COUNT; ++xx) m_pResNode->Add(new iMineralNode((MINERAL_TYPE)xx));
        m_pResNode->Add(new iManaCrystalNode());
        m_pResNode->Add(new iCampFireNode());
        m_pResNode->Add(new iChestNode());
        m_pResNode->Add(new iSpellScrollNode());
        m_pResNode->Add(new iLampNode());
        m_pResNode->Add(new iMapEventNode());
        m_pResNode->Add(new iMapMarkerNode());
        m_pResNode->Add(new iUltimateArtNode());
        for (xx=0; xx<6; ++xx) m_pResNode->Add(new iKeyGuardNode((uint8)xx));

        // Predefined creatures
        for (uint32 cid=0; cid<CREAT_COUNT; ++cid) m_pGuardsNode->Add(new iGuardNode(CREATURE_TYPE(cid)));
        // Add random guards
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L1));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L2));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L3));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L4));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L5));
        m_pGuardsNode->Add(new iGuardNode(CREAT_RANDOM_L6));


        // Normalize indeces
        uint16 idx=0;
        for (iObjGroupNode::Iterator hit = m_pHeroesNode->First(); hit != m_pHeroesNode->End(); ++hit){
            ((iHeroNode*)(*hit))->Idx() = idx++;
        }
        idx=0;
        for (iObjGroupNode::Iterator vcit = m_pVisCnstNode->First(); vcit != m_pVisCnstNode->End(); ++vcit){
            ((iBaseVisCnstNode*)(*vcit))->Idx() = idx++;
        }

		/*for (iObjGroupNode::Iterator scit = m_pShipsNode->First(); scit != m_pShipsNode->End(); ++scit){
            
			if( ((iBaseVisCnstNode*)(*scit))->NameKey() == _T("OBJ_HARBOR") )
				((iBaseVisCnstNode*)(*scit))->Idx() = idx++;
        }*/
        idx=0;
        for (iObjGroupNode::Iterator ocit = m_pOwnCnstNode->First(); ocit != m_pOwnCnstNode->End(); ++ocit){
            ((iBaseOwnCnstNode*)(*ocit))->Idx() = idx++;
        }

        idx=0;
        iObjGroupNode::Iterator cit = m_pCastleNode->First();
        while (cit != m_pCastleNode->End()){
            if (iCastleNode* pCastle = DynamicCast<iCastleNode*>((iBaseNode*)(*cit))){
                pCastle->Idx() = idx++;
            }
            cit = cit.WalkNext();
        }       
        idx=0;
        for (iObjGroupNode::Iterator arit = m_pArtifactNode->First(); arit != m_pArtifactNode->End(); ++arit){
            ((iArtifactNode*)(*arit))->Idx() = idx++;
        }
        idx=0;
        for (iObjGroupNode::Iterator peit = m_pPathElNode->First(); peit != m_pPathElNode->End(); ++peit){
            ((iPathElNode*)(*peit))->Idx() = idx++;
        }


        // Random artifacts
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND, _T("RandomArtifact"), _T("RAND_ARTIFACT"),_T("artifacts.rand_artifacts_0000")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L1, _T("RandomArtifact1"), _T("RAND_ARTIFACT1"),_T("artifacts.rand_artifacts_0001")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L2, _T("RandomArtifact2"), _T("RAND_ARTIFACT2"),_T("artifacts.rand_artifacts_0002")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L3, _T("RandomArtifact3"), _T("RAND_ARTIFACT3"),_T("artifacts.rand_artifacts_0003")));
        m_pArtifactNode->Add(new iArtifactNode(ARTT_RAND_L4, _T("RandomArtifact4"), _T("RAND_ARTIFACT4"),_T("artifacts.rand_artifacts_0004")));

    } catch (...) {
        return false;
    }

    m_bInited = true;
    return m_bInited;
}


void iItemMgr::Reset()
{
    for (iObjGroupNode::Iterator hit = m_pHeroesNode->First(); hit != m_pHeroesNode->End(); ++hit){
        iHeroNode* pHeroNode = (iHeroNode*)*hit;
        pHeroNode->Reset();
    }
}