#ifdef MULTIPLAYER

#include "stdafx.h"
#include "Credits.h"
#include "MenuView.h"
#include <string>

#include "BattleView.h"
#include "OverlandView.h"
#include "ScopePtr.h"

#include "InteractBattle.h"

#include "Dlg_MPStatus.h"
#include "helpers.h"
#include "Dlg_ScenProps.h"

#include "serialize.h"

#include "PHSocket.h"
#include "MPGameSetupView.h"
#include "MPLobbyView.h"

#include "HttpGetSocket.h"

#include "Remote_Player.h"

#include "shellapi.h"

const iCharA* delim = (":");
const iCharT* MP_CONFIG_FILE = _T("Multiplayer.cfg");
const iCharT* MP_STATS_FILE = _T("Traffic.sta");

void DebugReset();
void Debug(iStringA str);
void Debug(iStringT str);

#ifndef _DEBUG
const char* MP_SERVER = "173.203.98.85";
const uint32  MP_PORT = 1024;

const char* MP_UPDATE_SERVER = "http://palmheroesteam.com/update";
uint32 MP_UPDATE_PORT = 80;
#else
const char* MP_SERVER = "173.203.98.85";
//const char* MP_SERVER = "192.168.1.148";
//const char* MP_SERVER = "localhost";
const uint32  MP_PORT = 1024;

const char* MP_UPDATE_SERVER = "http://palmheroesteam.com/update";
uint32 MP_UPDATE_PORT = 80;
#endif

class FileLog : public StdLog
{
public:
    void error(ISocketHandler *,Socket *sock,const std::string& call,int err,const std::string& sys_err,loglevel_t lvl) {       
        FILE *f = fopen("error.txt", "a+");
        struct tm tp;
#if defined(_WIN32) && !defined(UNDER_CE)
        time_t t = time(NULL);
        memcpy(&tp, localtime(&t), sizeof(tp));
#elif !defined(UNDER_CE)
        time_t t = time(NULL);
        localtime_r(&t, &tp);
#endif
        std::string level;

        switch (lvl)
        {
        case LOG_LEVEL_WARNING:
            level = "Warning";
            break;
        case LOG_LEVEL_ERROR:
            level = "Error";
            break;
        case LOG_LEVEL_FATAL:
            level = "Fatal";
            break;
        case LOG_LEVEL_INFO:
            level = "Info";
            break;
        }
        if (sock)
        {
            fprintf(f, "%d-%02d-%02d %02d:%02d:%02d :: fd %d :: %s: %d %s (%s)\n",
                tp.tm_year + 1900,
                tp.tm_mon + 1,
                tp.tm_mday,
                tp.tm_hour,tp.tm_min,tp.tm_sec,
                sock -> GetSocket(),
                call.c_str(),err,sys_err.c_str(),level.c_str());
        }
        else
        {
            fprintf(f, "%d-%02d-%02d %02d:%02d:%02d :: %s: %d %s (%s)\n",
                tp.tm_year + 1900,
                tp.tm_mon + 1,
                tp.tm_mday,
                tp.tm_hour,tp.tm_min,tp.tm_sec,
                call.c_str(),err,sys_err.c_str(),level.c_str());
        }
        fclose(f);
    }
};

inline bool NeedCreateBattle(iPlayer* att, iPlayer* def) {
    // either we are one of participants OR one of the AI's is computed on our machine
    return  att != def && (att == gGame.Map().ActPlayer() || (def && def == gGame.Map().ActPlayer()) || 
        att->PlayerType() == PT_COMPUTER || (def && def->PlayerType() == PT_COMPUTER) ||
        (att && def && gMPMgr.GetHostType() == iMPManager::HT_HOST));
}
void iMPManager::OnUpload(uint32 count)
{
    m_curTraf.up += count;
    m_totalTraf.up += count;
    if(gGame.ActViewType() == iChildGameView::CV_OVERLAND)
        gGame.MainView()->Invalidate();
}

void iMPManager::OnDownload(uint32 count)
{
    m_curTraf.down += count;
    m_totalTraf.down += count;
    if(gGame.ActViewType() == iChildGameView::CV_OVERLAND)
        gGame.MainView()->Invalidate();
}

void iMPManager::OnConnect()
{
    // clean data
    m_pBattleWrapper = NULL;

    if(m_statusDlg)
        HideStatusDlg();        
    gGame.ShowView(iChildGameView::CV_MPLOBBY);
    ((iMPLobbyView*)gGame.ActView())->SetActive(false);
    if(m_authkey == ("")) {
        m_socket->Send(("2\n"));
        m_state = LS_AWT_REG;   
    } else {        
        m_socket->Send(("3"));          
        m_socket->Send(m_authkey);
        m_socket->Send(("\n"));
        m_state = LS_AWT_LOGCNF;
    }
}

void iMPManager::OnConnectFailed()
{
    if(m_statusDlg)
        HideStatusDlg();        
    if(m_socket) {
        m_socket = NULL;
        iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_FAILURE], gTextMgr[TRID_FAILED_CONNECTING], PID_NEUTRAL);       
        m_bBlock = true;
        dlg.DoModal();
        m_bBlock = false;
        gGame.RealMainMenu();   
    }
}

void iMPManager::OnDisconnect()
{       
    if(m_statusDlg) {
        HideStatusDlg();        
    }
    if(m_socket) {
        m_socket = NULL;
        m_bIsCombat = false;    
        gGame.RealMainMenu();           
    }
}

void iMPManager::OnConnectTimeout()
{
    if(m_statusDlg)
        HideStatusDlg();
    m_socket = NULL;
    iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_FAILURE], gTextMgr[TRID_FAILED_CONNECTING], PID_NEUTRAL);
    m_bBlock = true;
    dlg.DoModal();
    m_bBlock = false;
    gGame.RealMainMenu();   
}

void iMPManager::OnRawData(const char *data, size_t n)
{   
    if(m_state != LS_RCV_SAVE && m_state != LS_RCV_CMDDATA)
        return; 
    m_rcvdCnt -= n; 
    m_buff.Write(data, n);
    if(m_statusDlg)
        m_statusDlg->SetPercent((sint32)100*(m_buff.GetSize())/(m_buff.GetSize() + m_rcvdCnt));
    if(m_rcvdCnt == 0) {
        if(m_statusDlg)
            HideStatusDlg();
        if(m_state == LS_RCV_SAVE) {
            m_state = LS_AWT_GAMECMD;       
            m_socket->SetLineProtocol();        
            iFile::Delete(gSavePath + _T("mpsave.phs"));
            iFilePtr pFile( CreateWin32File( gSavePath + _T("mpsave.phs")));                
            pFile.get()->Write(m_buff.GetData(), m_buff.GetSize());     
            pFile.reset();

            iFilePtr read(OpenWin32File(gSavePath + _T("mpsave.phs")));
            iMapInfo mapInfo;
            mapInfo.m_bNewGame = false;
            mapInfo.m_FileName = gSavePath + _T("mpsave.phs");              
            uint32 fourcc; read.get()->Read(&fourcc,sizeof(fourcc));                
            if (mapInfo.ReadMapInfo(read.get())) {          
                mapInfo.PrepareForNetworkPlay(m_ownColor, m_hostType == HT_HOST);
                gGame.StartNewGame(mapInfo, false, false);
                m_socket->Send(("1\n"));
				iFile::Delete(gSavePath + _T("mpsave.phs"));
                return;                 
            }       			
        } else if(m_state == LS_RCV_CMDDATA) {          
            

            m_socket->SetLineProtocol();        
            
            m_cmdQueue.GetLast().data = m_buff;
            m_buff.Rewind();
            m_socket->Send(("1\n")); // cmd data received, please proceed
            if(--m_counter == 0) {
                m_state = LS_AWT_GAMECMD;       
                ExecuteCmdQueue();
            } else
                m_state = LS_AWT_GAMECMDMONAD;
        }       

    }
}

FileLog flog;

iMPManager::iMPManager(): MPLogic(), m_authkey(("")), m_pra(false), m_pBattleWrapper(NULL), m_bBlock(false), m_bReplaying(false),
    m_bPlayingAutobattle(false), m_lastActivity(GetTickCount()), m_bIsCombat(false), m_pingTimer(fix32::zero),
    m_bDisconFlag(false)
{

    m_handler = new PHSocketHandler(&m_log);    
    m_curTraf.down = 0; m_curTraf.up = 0;
}

iMPManager::~iMPManager() {
    #ifndef OS_IPHONE
    iFileStream pConfigFile(CreateWin32File(gRootPath + MP_STATS_FILE));
#else
    iFileStream pConfigFile(CreateWin32File(gSavePath + MP_STATS_FILE));    
#endif
    pConfigFile.Write(m_totalTraf.down);
    pConfigFile.Write(m_totalTraf.up);
}

void iMPManager::ExecuteCmd(iPlayer *plr, iCharA cmd, iStringA recvd, iDynamicBuffer& data)
{
    Debug(iFormat(("Executing cmd: %c, recvd: %s"), cmd, recvd.CStr()));
    m_lastActivity = GetTickCount();
    m_bIsCombat = false;
    if(cmd == MPC_ENDTURN) { // turn ended
        gGame.Map().NextPlayer();   
    } else if(cmd == '2') { // end replay
            // make sure the anim is done
#ifdef MULTIPLAYER
            Debug("entering executecmd endreplay animation");
#endif
            m_pra = true;
            while(gGame.MainView()->Composer().AniObj() && gApp.Cycle());
            m_pra = false;
#ifdef MULTIPLAYER
            Debug("exiting executecmd endreplay animation");
#endif
            m_bReplaying = false;                   
            // we call on select here for the AI player to recalc his strategy based on the most current situation after replay
            gGame.Map().CurPlayer()->OnSelect(false, true);

            // unblock the interface if needed
            gGame.OnPlayerChanged(gGame.Map().CurPlayer(), gGame.Map().CurPlayer () == gGame.Map().ActPlayer());    

            // if combat, check if we need to move units
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();
    } else if(cmd == MPC_HERO_MOVED) { 
        iPoint from, to;    
        from.x = recvd.Token(delim).GetUInt();
        from.y = recvd.Token(delim).GetUInt();
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();
        uint32 moves = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(from);
        iHero *pHero = DynamicCast<iHero*>(pMapObj);
        check(pHero);           
        
        if(!pHero) {                        
            Debug(iFormat("failed moving hero from [%d, %d] to [%d, %d]", from.x, from.y, to.x, to.y));                     
            return;
        }       
        
        iPath path(pHero->Pos());
        path.AddMoveToPoint(to, 0);
        path.SetTargetAction((iPath::Action)(pHero->GetTarget(to.x, to.y, true).action));
        pHero->SetPath(path);
        m_pra = true;       
        pHero->Step();          
#ifdef MULTIPLAYER
        Debug("entering hero step animation");
#endif
        while(pHero->Moving()) gApp.Cycle();
#ifdef MULTIPLAYER
        Debug("exiting hero step animation");
#endif
        pHero->SetMoves(moves);
        m_pra = false;                      
    } else if(cmd == MPC_OBJ_DISAPPEARED) { // object disappeared
        iPoint from;                    
        from.x = recvd.Token(delim).GetUInt();
        from.y = recvd.Token(delim).GetUInt();                                              
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(from);
        if(DynamicCast<iMapItem_Mineral*>(pMapObj) || DynamicCast<iMapItem_ManaCrystal*>(pMapObj) || DynamicCast<iMapItem_CampFire*>(pMapObj) || DynamicCast<iMapItem_Artifact*>(pMapObj) || DynamicCast<iMapItem_Chest*>(pMapObj) || DynamicCast<iMapItem_SpellScroll*>(pMapObj) || DynamicCast<iMapItem_KeyGuard*>(pMapObj) || DynamicCast<iMapItem_Lamp*>(pMapObj)) {        
            iMapItem *pMapItem = (iMapItem*)(pMapObj);
            m_pra = true;
            gGame.Map().RemoveMapItem(pMapItem);
            gGame.MainView()->Invalidate();
            m_pra = false;
        } else if(iMapGuard* pMapGuard = DynamicCast<iMapGuard*>(pMapObj)) {        
            m_pra = true;
            gGame.Map().RemoveMapGuard(pMapGuard);
            gGame.MainView()->Invalidate();
            m_pra = false;
        }
    } else if(cmd == MPC_HERO_ATTACKED) { // hero attacked 
        iPoint from, to;    
        from.x = recvd.Token(delim).GetUInt();
        from.y = recvd.Token(delim).GetUInt();
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();                                
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(from);
        iHero *pAttacker = DynamicCast<iHero*>(pMapObj);                
        pMapObj = gGame.Map().GetCellObj(to);                       
        iHero *pDefender = DynamicCast<iHero*>(pMapObj);                        
        if(!pAttacker || !pDefender) return;
        iCastle *pCastle = DynamicCast<iCastle*>(pDefender->GetLocation());
        m_pra = true;
        if(pCastle && NeedCreateBattle(pAttacker->Owner(), gGame.Map().FindPlayer(pCastle->Owner()))) {
            pAttacker->SiegeCastle(pCastle);
            if(m_pBattleWrapper)
                m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();
        }
        else if(NeedCreateBattle(pAttacker->Owner(), pDefender->Owner())) {
            pAttacker->AttackHero(pDefender);       
            if(m_pBattleWrapper)
                m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();
        }
        m_pra = false;                              
    } 
    else if(cmd == MPC_OWNCNST_ATTACKED) { 
        iPoint from, to;    
        from.x = recvd.Token(delim).GetUInt();
        from.y = recvd.Token(delim).GetUInt();
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();                                
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(from);
        iHero *pAttacker;
        pAttacker = DynamicCast<iHero*>(pMapObj);                       
        pMapObj = gGame.Map().GetCellObj(to);   
        iOwnCnst *pCnst;        
        if(iHero *pHero = DynamicCast<iHero*>(pMapObj))
            pCnst = DynamicCast<iOwnCnst*>(pHero->GetLocation());
        else
            pCnst = DynamicCast<iOwnCnst*>(pMapObj);

        // this is our construction --- defend!!
        if(NeedCreateBattle(pAttacker->Owner(), gGame.Map().FindPlayer(pCnst->Owner()))) {
            m_pra = true;
            iBattleInfo bi(pAttacker, pCnst);
            gGame.BeginBattle(bi);  
            if(m_pBattleWrapper)
                m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();            
        }
        m_pra = false;
    } else if(cmd == MPC_VISCNST_ATTACKED) { 
        iPoint from, to;    
        from.x = recvd.Token(delim).GetUInt();
        from.y = recvd.Token(delim).GetUInt();
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();                                
        iHero *pAttacker = NULL;
        for (iPlayer::iCHLIt hit = gGame.Map().CurPlayer()->HeroFirst(); hit != gGame.Map().CurPlayer()->HeroEnd(); ++hit) 
            if(hit->Pos() == from) {
                pAttacker = (iHero*)*hit;
                break;
            }
        iBaseMapObject *pMapObj = gGame.Map().GetCellObj(to);   
        iVisCnst *pCnst;
        if(iHero *pHero = DynamicCast<iHero*>(pMapObj))
            pCnst = DynamicCast<iVisCnst*>(pHero->GetLocation());
        else
            pCnst = DynamicCast<iVisCnst*>(pMapObj);
        
        if(NeedCreateBattle(pAttacker->Owner(), NULL)) {
            m_pra = true;
            iBattleInfo bi(pAttacker, pCnst);
            gGame.BeginBattle(bi);  
            if(m_pBattleWrapper)
                m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();            
        }
        m_pra = false;
    } else if(cmd == MPC_MAPGUARD_ATTACKED) { 
        iPoint from, to;    
        from.x = recvd.Token(delim).GetUInt();
        from.y = recvd.Token(delim).GetUInt();
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();                                
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(from);
        iHero *pAttacker = DynamicCast<iHero*>(pMapObj);                
        pMapObj = gGame.Map().GetCellObj(to);   
        iMapGuard *pGuard = DynamicCast<iMapGuard*>(pMapObj);
        
        if(NeedCreateBattle(pAttacker->Owner(), NULL)) {
            m_pra = true;
            iBattleInfo bi(pAttacker, pGuard);
            gGame.BeginBattle(bi);  
            if(m_pBattleWrapper)
                m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();            
        }
        m_pra = false;
    } else if(cmd == MPC_CASTLE_SIEGED) {
        iPoint from, to;    
        from.x = recvd.Token(delim).GetUInt();
        from.y = recvd.Token(delim).GetUInt();
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();                                
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(from);
        iHero *pAttacker = DynamicCast<iHero*>(pMapObj);                
        pMapObj = gGame.Map().GetCellObj(to);                               
        iCastle *pCastle;
        if(iHero *pHero = DynamicCast<iHero*>(pMapObj)) {
            if(NeedCreateBattle(pAttacker->Owner(), pHero->Owner())) {
                m_pra = true;
                pAttacker->SiegeCastle(DynamicCast<iCastle*>(pHero->GetLocation()));                
                if(m_pBattleWrapper)
                    m_pBattleWrapper->PlayAnim();
                m_pra = false;
                if(m_pBattleWrapper)
                    m_pBattleWrapper->OnNextGroup();                
                return;
            }
        }
        else {          
            pCastle = DynamicCast<iCastle*>(pMapObj);
            if(NeedCreateBattle(pAttacker->Owner(), gGame.Map().FindPlayer(pCastle->Owner()))) {
                m_pra = true;
                pAttacker->SiegeCastle(pCastle);                            
                if(m_pBattleWrapper)
                    m_pBattleWrapper->PlayAnim();
                m_pra = false;
                if(m_pBattleWrapper)
                    m_pBattleWrapper->OnNextGroup();                            
            }
            return;

        }       
    } else if(cmd == MPC_CBT_MOVE) { // move in combat
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        iPoint to;  
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();
        iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattleWrapper->Engine().TurnSeq().CurUnit());
        if(!pCurCreatGroup) return;
        iBattleGroup* pCurGroup = pCurCreatGroup->GetCreatGroup();
        m_pra = true;
        m_pBattleWrapper->Engine().Move(to, pCurGroup->Orient());       
        if(m_pBattleWrapper) {
            m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)                
                m_pBattleWrapper->OnNextGroup();        
        }
        m_pra = false;
    } else if(cmd == MPC_CBT_ATTACK) { // attack in combat
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        iPoint to;  
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();
        uint16 mdir = recvd.Token(delim).GetUInt();
        iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattleWrapper->Engine().TurnSeq().CurUnit());
        if(!pCurCreatGroup) return;
        iBattleGroup* pCurGroup = pCurCreatGroup->GetCreatGroup();
        m_pra = true;
        m_pBattleWrapper->Engine().Melee(to, mdir);         
        if(m_pBattleWrapper) {
            m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();                    
        }
        m_pra = false;
    } else if(cmd == MPC_CBT_WAIT) { // wait
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        m_pra = true;
        m_pBattleWrapper->Engine().Wait();              
        m_pBattleWrapper->PlayAnim();
        m_pra = false;
        m_pBattleWrapper->OnNextGroup();
    } else if(cmd == MPC_CBT_DEFEND) { // defend
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        m_pra = true;
        m_pBattleWrapper->Engine().Defend();    
        m_pBattleWrapper->PlayAnim();
        m_pra = false;
        m_pBattleWrapper->OnNextGroup();
    } else if(cmd == MPC_CBT_SHOOT) { // shoot      
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        iPoint to;  
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();
        iBattleUnit_CreatGroup* pCurCreatGroup = DynamicCast<iBattleUnit_CreatGroup*>(m_pBattleWrapper->Engine().TurnSeq().CurUnit());
        if(!pCurCreatGroup) return;
        iBattleGroup* pCurGroup = pCurCreatGroup->GetCreatGroup();
        m_pra = true;
        const iBattleGroup::iShootEntry *pShootTrack = pCurGroup->GetShootEntry(to);
        m_pBattleWrapper->Engine().Shot(to, pShootTrack->m_penalty);        
        if(m_pBattleWrapper) {
            m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();                
        }
        m_pra = false;
    } else if(cmd == MPC_RSEED) { // random seed changed
        m_pra = true;
        gGame.Map().SetRandSeed((uint32)recvd.GetInt());
        m_pra = false;
    } else if(cmd == MPC_CBT_END) { // fight ended
        m_bIsCombat = false;
        m_pra = true;   
        if(m_pBattleWrapper) {
            PLAYER_ID winnerId = (PLAYER_ID)recvd.Token(delim).GetUInt();
            DEFEAT_CAUSE cause = (DEFEAT_CAUSE)recvd.Token(delim).GetUInt();        
            BATTLE_RESULT br;
            iBattleResult bres;     
            br = m_pBattleWrapper->Engine().GetBattleInfo().m_pAssaulter->Owner() == winnerId ? BR_ASSAULTER_WIN : BR_DEFENDER_WIN;                 
            bres = iBattleResult(br, cause);        
            if(cause == DC_RETREAT || cause == DC_SURRENDER) {
                iPoint pos;
                pos.x = recvd.Token(delim).GetUInt();
                pos.y = recvd.Token(delim).GetUInt();
                iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pos);
                iCastle *pCastle = DynamicCast<iCastle*>(pMapObj);
                check(pCastle);                             
                if(!pCastle) return;
                bres.m_pDest = pCastle;                     
            }                               
            if(gGame.ActViewType() == iChildGameView::CV_BATTLE)
                ((iBattleView*)gGame.ActView())->EndBattle(bres);                       
            else {
                m_pBattleWrapper->Engine().PrepareBattleResult(bres);   
                m_pBattleWrapper->EndBattle(bres);          
                m_pBattleWrapper = NULL;
            }
        }   
        m_pra = false;
    } else if(cmd == MPC_CBT_CATAPULT) { // catapult shot
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        iCastleFort::ElementType target = (iCastleFort::ElementType)recvd.Token(delim).GetUInt();
        m_pra = true;
        m_pBattleWrapper->Engine().CatapultShot(target);
        if(m_pBattleWrapper) {
            m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();            
        }
        m_pra = false;
    } else if(cmd == MPC_CBT_TURRET) { // turret shot
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        iPoint to;  
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();        
        iBattleGroup* pCurGroup = m_pBattleWrapper->Engine().FindGroup(to);
        if(!pCurGroup) return;
        m_pra = true;
        m_pBattleWrapper->Engine().TurretShot(pCurGroup);       
        if(m_pBattleWrapper) {
            m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();        
        }
        m_pra = false;      
    } else if(cmd == MPC_CBT_SPELLCAST) { // spell cast 
        m_bIsCombat = true;
        /* if not in combat, ignore this command. somebody else fights */
        if(!m_pBattleWrapper) return;

        MAGIC_SPELL msp = (MAGIC_SPELL)recvd.Token(delim).GetUInt();
        iPoint to;  
        to.x = recvd.Token(delim).GetUInt();
        to.y = recvd.Token(delim).GetUInt();        

        uint32 sid = recvd.Token(delim).GetUInt();
        iBattleGroup *pTarget = NULL;
        if(sid < 2) { // target specified
            iBattleMember::Side side = (iBattleMember::Side)sid;

            iBattleArmy &army = side == iBattleMember::Assaulter ? m_pBattleWrapper->Engine().AArmy() : 
                m_pBattleWrapper->Engine().DArmy();
            pTarget = (iBattleGroup*)army.FindGroupByIdx(recvd.Token(delim).GetUInt());
        }
        m_pra = true;
        m_pBattleWrapper->Engine().CastSpell((iCombatSpell*)gGame.ItemMgr().m_spellMgr.Spell(msp), to, pTarget);    
        if(m_pBattleWrapper) {
            m_pBattleWrapper->PlayAnim();
            m_pra = false;
            if(m_pBattleWrapper)
                m_pBattleWrapper->OnNextGroup();        
        }
        m_pra = false;
    } else if(cmd == MPC_RECRUIT) { // recruit hero         
        iCastle *pCastle;
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        pCastle = DynamicCast<iCastle*>(pMapObj);
        if(!pCastle) return;

        m_pra = true;
        iHero *newh = gGame.ItemMgr().HeroesMgr().Get(recvd.Token(delim).GetUInt());
        if(newh)
            plr->UpdateTavernVisitor(newh);
        /*plr->RecruitTavernVisitor(pCastle);       */
        ((iTavernCtlCnst*)pCastle->FindCnst(CTLCNST_TAVERN))->RecruitVisitor();
        m_pra = false;
    } else if(cmd == MPC_DIG) {
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        m_pra = true;
        gGame.Map().Dig(pt);
        m_pra = false;
    } else if(cmd == MPC_UPDATE_HERO) { // update hero 
        iHero* pHero;       
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        pHero = DynamicCast<iHero*>(pMapObj);
        if(!pHero) {
            Debug(iFormat("failed to update hero in point [%d, %d]", pt.x, pt.y));
            return;
        }
        uint16 proto;
        data.Rewind();
        data.Read(proto);
        //plr->RemoveHero(pHero);
        m_pra = true;
	pHero->Unserialize(data, false, false, false);
        gGame.Map().SetHeroPos(pHero, pHero->Pos());
        m_pra = false;
        Debug(_T("end updating hero"));
    } else if(cmd == MPC_UPDATE_CASTLE) { // update castle
        iCastle* pCastle;       
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        if(iHero *pHero = DynamicCast<iHero*>(pMapObj)) 
            pCastle = DynamicCast<iCastle*>(pHero->GetLocation());
        else
            pCastle = DynamicCast<iCastle*>(pMapObj);
        
        if(!pCastle) return;
        data.Rewind();
        m_pra = true;
        Unserialize(data, pCastle->Garrison());
        // Can build
        uint8 canBuild; data.Read(canBuild); pCastle->SetBuilt(canBuild>0);
        // Constructions        
        pCastle->Cnsts().Unserialize(data);
        m_pra = false;
    } else if(cmd == MPC_UPDATE_MINERALS) { // update minerals
        data.Rewind();
        PLAYER_ID pid = (PLAYER_ID)recvd.Token(delim).GetUInt();
        iPlayer *p = gGame.Map().FindPlayer(pid);
        Unserialize(data, p->Minerals());           
    } else if(cmd == MPC_UPDATE_MAPGUARD) { // update guards
        iMapGuard *pMapGuard;
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        pMapGuard = DynamicCast<iMapGuard*>(pMapObj);
        
        if(!pMapGuard) return;
        data.Rewind();
        Unserialize(data, pMapGuard->Creatures());      
    } else if(cmd == MPC_UPDATE_OWNCNST) { 
        iOwnCnst *pCnst;
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        if(iHero *pHero = DynamicCast<iHero*>(pMapObj))
            pCnst = DynamicCast<iOwnCnst*>(pHero->GetLocation());
        else
            pCnst = DynamicCast<iOwnCnst*>(pMapObj);
                
        if(!pCnst) return;
        data.Rewind();
        Unserialize(data, pCnst->Guard());      
    } else if(cmd == MPC_UPDATE_VISCNST) { 
        iVisCnst *pCnst;
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        if(iHero *pHero = DynamicCast<iHero*>(pMapObj))
            pCnst = DynamicCast<iVisCnst*>(pHero->GetLocation());
        else
            pCnst = DynamicCast<iVisCnst*>(pMapObj);
        
        
        if(!pCnst) return;
        data.Rewind();
        m_pra = true;
        pCnst->Unserialize(data);
        m_pra = false;
    }   else if(cmd == MPC_REMOVE_MAPEVENT) { 
        iMapEvent *pEvent;
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();        
        pEvent = gGame.Map().FindMapEvent(pt, PID_NEUTRAL);
        
        if(!pEvent) return;
        m_pra = true;
        gGame.Map().RemoveMapEvent(pEvent);
        m_pra = false;
    }  else if(cmd == MPC_UPDATE_MAPITEM) { 
        iMapItem *pMapItem;
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        pMapItem = (iMapItem*)(pMapObj);
        
        if(!pMapItem) return;
        data.Rewind();
        Unserialize(data, pMapItem->Guard());
        if(pMapItem->Type() == MAPITEM_LAMP) {
            sint32 r;
            data.Read(r);
            ((iMapItem_Lamp*)pMapItem)->SetCount(r);
        }
    } else if(cmd == MPC_CHANGE_OWNER) { // change owner
        iOwnCnst *pCnst;
        iPoint pt;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);
        if(iHero *pHero = DynamicCast<iHero*>(pMapObj)) 
            pCnst = DynamicCast<iOwnCnst*>(pHero->GetLocation());
        else
            pCnst = DynamicCast<iOwnCnst*>(pMapObj);
        
        if(!pCnst) return;  
        PLAYER_ID newowner = (PLAYER_ID)recvd.Token(delim).GetUInt();
        if(pCnst->Owner() != PID_NEUTRAL) {
            iPlayer* oldPlayer = gGame.Map().FindPlayer(pCnst->Owner());        
            oldPlayer->m_Cnsts.Remove(iPlayer::iCLIt(pCnst));
            oldPlayer->UpdateFogMap();
        } else {
            gGame.Map().m_OwnCnsts.Remove(iGameWorld::iOCIt(pCnst));
        }
        m_pra = true;
        pCnst->SetOwner(newowner);
        iPlayer* newPlayer = gGame.Map().FindPlayer(newowner);          
        newPlayer->AddCnst(pCnst);
        m_pra = false;
        
    } else if(cmd == MPC_UPDATE_KEYS) {
        plr->SetKeys(recvd.GetUInt());
    }  else if(cmd == MPC_UPDATE_OBELISKS) {
        //plr->
    } else if(cmd == MPC_PLAYER_DROPPED) {      
        PLAYER_ID pid = (PLAYER_ID)(recvd.Token(delim).GetUInt());      
        iPlayer *plr = gGame.Map().FindPlayer(pid);
        if(!plr || plr->PlayerType() == PT_COMPUTER) return; // we don't have him anyway... forget it       
        
        if(!m_bReplaying) {
            bool was_replaying = m_bReplaying;
            m_bReplaying = false;
            m_bBlock = true;
                
            if(pid == gGame.Map().ActPlayer()->PlayerId()) {// oops. we've been kicked
                iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_PLAYER_DROPPED], gTextMgr[TRID_KICKED], PID_NEUTRAL);
                dlg.DoModal();  
            } else{ 
                iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_PLAYER_DROPPED], iFormat(gTextMgr[TRID_PLAYER_DROPPED_MSG], gTextMgr[TRID_PLAYER_RED+pid]), PID_NEUTRAL);
                dlg.DoModal();  
            }
            m_bBlock = false;
            m_bReplaying = was_replaying;       
        }
        
        // remove from game list
        GameUserList newl;      
        for(uint32 xx=0; xx<m_gameUsers.GetSize(); xx++)
            if(m_gameUsers[xx].m_color != plr->PlayerId()) {
                newl.Add(m_gameUsers[xx]);
        }
        m_gameUsers = newl;

        if(gMPMgr.GetHostType() != HT_HOST) return; //not our problem               

        iAI_Player *n = new iAI_Player(*plr);       
        gGame.Map().ReplacePlayer(plr, n);
        // we call on select here for the AI player to recalc his strategy based on the most current situation after replay
        if(gGame.Map().CurPlayer()->PlayerType() == PT_COMPUTER)
            gGame.Map().CurPlayer()->OnSelect(false, true);

        // unblock the interface if needed
        gGame.OnPlayerChanged(gGame.Map().CurPlayer(), gGame.Map().CurPlayer () == gGame.Map().ActPlayer());    

        // if combat, check if we need to move units
        if(m_pBattleWrapper)
            m_pBattleWrapper->OnNextGroup();
    } else if(cmd == MPC_PLAYER_JUMPED_IN) {        
        PLAYER_ID pid = (PLAYER_ID)(recvd.Token(delim).GetUInt());      
        iPlayer *plr = gGame.Map().FindPlayer(pid);
        if(!plr || plr == gGame.Map().ActPlayer()) return; // we don't have him anyway... forget it     
                                
        GameUserData ud;
        ud.m_id = (UserId)(recvd.Token(delim).GetUInt());
        ud.m_color = pid;
        ud.m_nickname = recvd.Token(delim);         
        m_gameUsers.Add(ud);

        if(!m_bReplaying) {
            bool was_replaying = m_bReplaying;
            m_bReplaying = false;
            m_bBlock = true;

            iStringT nick = iStringT(CvtA2T<>(ud.m_nickname.CStr()));
            iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_PLAYER_JUMPED], 
                iFormat(gTextMgr[TRID_PLAYER_JUMPED_MSG], gTextMgr[TRID_PLAYER_RED+pid], nick.CStr()), PID_NEUTRAL);
            dlg.DoModal();  
        
            m_bBlock = false;
            m_bReplaying = was_replaying;       
        }

        if(gMPMgr.GetHostType() != HT_HOST) return; //not our problem               
            
        iPlayer *n = new iRemote_Player(*plr);
        gGame.Map().ReplacePlayer(plr, n);

        /*iAI_Player *n = new iAI_Player(*plr);     
        gGame.Map().ReplacePlayer(plr, n);
        // we call on select here for the AI player to recalc his strategy based on the most current situation after replay
        gGame.Map().CurPlayer()->OnSelect(false, false);
        */

        // unblock the interface if needed
        gGame.OnPlayerChanged(gGame.Map().CurPlayer(), gGame.Map().CurPlayer () == gGame.Map().ActPlayer());    

        // if combat, check if we need to move units
        if(m_pBattleWrapper)
            m_pBattleWrapper->OnNextGroup();
    } 
    else if(cmd == MPC_GAME_DROPPED) {      
        iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_GAME_DROPPED], gTextMgr[TRID_GAME_DROPPED_MSG], PID_NEUTRAL);       
        bool was_replaying = m_bReplaying;
        m_bReplaying = false;
        dlg.DoModal();  
        //gGame.MainMenu();
        DisconnectFromServer();
        return;
    } 
    else if(cmd == MPC_PLAYER_WON) {
        
    } else if(cmd == MPC_CHARM_ADDED) {
        iPoint pt;      
        sint32 flag;
        MAGIC_SPELL spell;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        spell = (MAGIC_SPELL)recvd.Token(delim).GetUInt();
        flag = (sint32)recvd.Token(delim).GetUInt();

        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);   
        iHero *pHero = DynamicCast<iHero*>(pMapObj);
        if(!pHero)
            return;
        m_pra = true;
        pHero->AddCharm(spell, flag);
        m_pra = false;              
    } else if(cmd == MPC_REMOVE_HERO) {
        iPoint pt;
        bool bResetArmy;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        bResetArmy = (bool)(recvd.Token(delim).GetUInt());

        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);   
        iHero *pHero = DynamicCast<iHero*>(pMapObj);
        if(!pHero)
            return;
        if(!m_pBattleWrapper) {
            m_pra = true;
            gGame.Map().RemoveHero(pHero, bResetArmy);
            m_pra = false;
        }
    } else if(cmd == MPC_CASTLE_CAPTURED) {
        iPoint pt;
        PLAYER_ID newowner;
        pt.x = recvd.Token(delim).GetUInt();
        pt.y = recvd.Token(delim).GetUInt();
        newowner = (PLAYER_ID)recvd.Token(delim).GetUInt();

        iBaseMapObject* pMapObj = gGame.Map().GetCellObj(pt);   
        iCastle *pCastle = DynamicCast<iCastle*>(pMapObj);
        if(!pCastle)
            return;
        if(pCastle->Owner() == newowner) return;
        m_pra = true;
        iPlayer *oldPlayer = NULL, *newplayer = gGame.Map().FindPlayer(newowner);
        if (pCastle->Owner() != PID_NEUTRAL) {
            oldPlayer = gGame.Map().FindPlayer(pCastle->Owner());
            check(oldPlayer);
            oldPlayer->RemoveCastle(pCastle);
        } else {
            gGame.Map().m_Castles.Remove(iGameWorld::iCtIt(pCastle));
        }

        pCastle->SetOwner(newowner);
        newplayer->AddCastle(pCastle);
        newplayer->OnCaptureCastle(pCastle);

        if (oldPlayer && !oldPlayer->DefeatState()) gGame.Map().RemovePlayer(oldPlayer);

        m_pra = false;
    }
}


void iMPManager::SortGamesList()
{   
    iSortArray<GameSessionData> sort;
    sint32 idx;
    for (uint32 xx=0; xx<m_games.GetSize(); ++xx) {
        idx = m_games[xx].m_lastActivity;
        if(m_games[xx].m_state == GS_PLAYING)
            idx += 0x10000000;
        sort.Insert(m_games[xx], idx);
    }                       
    for (uint32 xx=0; xx<m_games.GetSize(); ++xx) m_games[xx] = sort.Value(xx);
}

void iMPManager::ExecuteCmdQueue()
{   
    iStringA recvd;
        
    for(uint32 xx=0; xx<m_cmdQueue.GetSize(); xx++) {
        recvd = m_cmdQueue[xx].cmd;
        iDynamicBuffer& buf = m_cmdQueue[xx].data;
        PLAYER_ID color = (PLAYER_ID)recvd.Token(delim).GetUInt();
        recvd.Del(0, 1);
        if(color != PID_NEUTRAL) {
            iPlayer* player = gGame.Map().FindPlayer(color);            
            iCharA cmd = recvd[0];
            recvd.Del(0, 1);
            ExecuteCmd(player, cmd, recvd, buf);
        }
    }
    m_cmdQueue.RemoveAll(); 
}

void iMPManager::OnLine(const iStringA &line)
{           
    if(!m_socket) return;
    iStringA recvd = line;  
    if(m_state == LS_AWT_REG) {
        m_authkey = recvd.Token(delim);
        SaveCredentials();
        m_nickname = recvd.Token(delim);            
        m_id = recvd.Token(delim).GetUInt();
        m_state = LS_AWT_LOBBYCMD;          
    }
    else if(m_state == LS_AWT_LOGCNF) {     
        if(recvd[0] == '0') { // error logging in -- our credentials are corrupted
            iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_UNABLE_TO_LOG_IN], gTextMgr[TRID_USER_ID_INVALID], PID_NEUTRAL);
            m_bBlock = true; // do nothing while dialog stays           
            dlg.DoModal();
            m_bBlock = false;   
            //gGame.MainMenu();
            return;
        }
        m_nickname = recvd.Token(delim);    
        m_id = recvd.Token(delim).GetUInt();
        uint8 hasgame = recvd.Token(delim).GetUInt();
        m_state = LS_AWT_LOBBYCMD;
        if(hasgame) {
            uint8 ishost = recvd.Token(delim).GetUInt();
            m_hostType = ishost ? HT_HOST : HT_JOINED;
            m_ownColor = (PLAYER_ID)recvd.Token(delim).GetUInt();
            iQuestDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_RESUME_QSTN], gTextMgr[TRID_RESUME_QSTN_MSG], PID_NEUTRAL);
            uint8 res = dlg.DoModal();
            if(res == DRC_YES) {
                m_bReplaying = true;
                m_socket->Send(("1\n"));
                //m_state = LS_AWT_SAVEOFFER; // waiting for savegame
            }
            else {
                m_socket->Send(("0\n"));
                ((iMPLobbyView*)gGame.ActView())->SetActive(true);
            }
        } else
            ((iMPLobbyView*)gGame.ActView())->SetActive(true);
    }else if(m_state == LS_AWT_LOBBYCMD) {
        iCharA cmd = recvd[0];
        recvd.Del(0, 1);
        if(cmd == MPC_FAILED_TO_JOIN) { // failed to join
            uint8 code = recvd.GetUInt();
            iStringT msg;
            if(code == 0) {
                msg = gTextMgr[TRID_GAME_INVALID];
            } else if(code == 1) {
                msg = gTextMgr[TRID_GAME_FULL];
            }  
            iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_UNABLE_TO_JOIN], msg, 
                PID_NEUTRAL);
            m_bBlock = true;
            dlg.DoModal();
            m_bBlock = false;
        } else if(cmd == 'i') { // failed to create
            iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_UNABLE_TO_CREATE], _T(""), 
                PID_NEUTRAL);
            m_bBlock = true;
            dlg.DoModal();
            m_bBlock = false;
        }
        else if(cmd == '1') { // user list output started
            m_lobbyUsers.RemoveAll();
            m_counter = recvd.Token(delim).GetUInt();
            if(m_counter > 0)
                m_state = LS_AWT_USR;           
        } else if(cmd == '2') { // new user added
            UserData ud;
            ud.m_id = (UserId)recvd.Token(delim).GetUInt();
            ud.m_nickname = recvd.Token(delim);
            m_lobbyUsers.Add(ud);
            gGame.ActView()->Invalidate();
        } else if(cmd == '3') { // user left
            UserList newList;
            UserId id = (UserId)recvd.Token(delim).GetUInt();
            for(uint32 xx=0;xx<m_lobbyUsers.GetSize(); xx++)
                if(m_lobbyUsers[xx].m_id != id)
                    newList.Add(m_lobbyUsers[xx]);
            m_lobbyUsers = newList;
            gGame.ActView()->Invalidate();
        } else if(cmd == '5') { // created a game
            m_gameId = (GSessionId)recvd.Token(delim).GetUInt();
            m_hostType = (HostType)recvd.Token(delim).GetUInt(); // Host or Joined?
            m_ownColor = (PLAYER_ID)recvd.Token(delim).GetUInt(); // own color
            if(m_hostType == HT_JOINED) {
                for(uint32 xx=0; xx<PID_COUNT; xx++)
                    m_bPlayersPresent[xx] = false;
                while(recvd.Length() > 1) {
                    PLAYER_ID pid = (PLAYER_ID)recvd.Token(delim).GetUInt();
                    m_bPlayersPresent[pid] = true;
                }                   
            }
            m_state = LS_AWT_SETUPCMD;
            m_gameUsers.RemoveAll();
            gGame.ShowView(iChildGameView::CV_MPGAMESETUP);
            ((iMPGameSetupView*)gGame.ActView())->SetActive(false);
        } else if(cmd == '7') { // game list output started
            m_games.RemoveAll();
            ((iMPLobbyView*)gGame.ActView())->SetActive(true);
            m_counter = recvd.Token(delim).GetUInt();
            if(m_counter > 0)
                m_state = LS_AWT_GAME;  
        } else if(cmd == '8') { // game added
            GameSessionData d;          
            d.m_id = (GSessionId)(recvd.Token(delim).GetUInt());                        
            d.m_hostname = recvd.Token(delim);                      
            d.m_mapname = recvd.Token(delim);                       
            d.m_freeSlots = recvd.Token(delim).GetUInt();
            d.m_totalSlots = recvd.Token(delim).GetUInt();
            d.m_state = (GameState)(recvd.Token(delim).GetUInt());
            d.m_lastActivity = recvd.Token(delim).GetUInt();

            bool found = false;

            for(uint32 xx=0; xx<m_games.GetSize(); xx++)
                if(m_games[xx].m_id == d.m_id) {
                    found = true;
                    m_games[xx] = d;
                    break;
                }

                if(!found) {
                    m_games.Add(d);
                }
                SortGamesList();
                gGame.ActView()->Invalidate();              
        } else if (cmd == '9') { // game removed
            GSessionList newl;
            GSessionId id = (GSessionId)(recvd.Token(delim).GetUInt());         

            for(uint32 xx=0; xx<m_games.GetSize(); xx++)
                if(m_games[xx].m_id != id) {
                    newl.Add(m_games[xx]);
                }           
                m_games = newl;
                gGame.ActView()->Invalidate();
        } else if(cmd == 's') { // immediately start the game
            m_gameId = (GSessionId)recvd.Token(delim).GetUInt();
            m_hostType = (HostType)recvd.Token(delim).GetUInt(); // Host or Joined?
            m_ownColor = (PLAYER_ID)recvd.Token(delim).GetUInt(); // own color
            m_state = LS_AWT_RESUMEDCSN;    
            m_gameUsers.RemoveAll();
            gSfxMgr.PlaySound(CSND_PICKUP01);
        } else if(cmd == MPC_EVENT) {
            EVENT_TYPE type = (EVENT_TYPE)recvd.Token(delim).GetUInt();
            switch(type) {
                case ET_GAME_DROPPED:
                    m_bBlock = true;
                    iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_GAME_DROPPED], gTextMgr[TRID_GAME_TERMINATED], PID_NEUTRAL);
                    dlg.DoModal();
                    m_bBlock = false;
                    break;
            };
        }
    } else if(m_state == LS_AWT_RESUMEDCSN) {
        iCharA cmd = recvd[0];
        recvd.Del(0, 1);
        if(cmd == '1') {
            UserId id;
            iStringA nick;
            PLAYER_ID color;
            CTL_TYPE castle;

            id = (UserId)(recvd.Token(delim).GetUInt());
            nick = recvd.Token(delim);
            color = (PLAYER_ID)(recvd.Token(delim).GetUInt());
            castle = (CTL_TYPE)(recvd.Token(delim).GetUInt());

            GameUserData ud;
            ud.m_id = id;
            ud.m_color = color;
            ud.m_nickname = nick;
            ud.m_castle = castle;

            bool found = false;

            for(uint32 xx=0; xx<m_gameUsers.GetSize(); xx++)
                if(m_gameUsers[xx].m_id == id) {
                    found = true;
                    m_gameUsers[xx] = ud;
                    break;
                }

                if(!found) {
                    m_gameUsers.Add(ud);
                }
        } else if(cmd = MPC_SAVE_FOLLOWS) {
            m_state = LS_AWT_SAVEOFFER; // game info transfered, awaiting savegame
        }
    } else if(m_state == LS_AWT_USR) {
        UserData ud;
        ud.m_id = (UserId)recvd.Token(delim).GetUInt();
        ud.m_nickname = recvd.Token(delim);
        m_lobbyUsers.Add(ud);
        gGame.ActView()->Invalidate();
        if(--m_counter <= 0)
        {
            m_state = LS_AWT_LOBBYCMD;
            return;
        }

    } else if(m_state == LS_AWT_GAME) {
        GameSessionData gd;
        gd.m_id = (UserId)recvd.Token(delim).GetUInt();
        gd.m_hostname = recvd.Token(delim);
        gd.m_mapname = recvd.Token(delim);
        gd.m_freeSlots = recvd.Token(delim).GetUInt();
        gd.m_totalSlots = recvd.Token(delim).GetUInt();
        gd.m_state = (GameState)(recvd.Token(delim).GetUInt());
        gd.m_lastActivity = recvd.Token(delim).GetUInt();

        //ud.m_nickname = recvd.Token(delim);
        m_games.Add(gd);
        gGame.ActView()->Invalidate();
        if(--m_counter <= 0)
        {
            SortGamesList();
            m_state = LS_AWT_LOBBYCMD;
            return;
        }

    } else if(m_state == LS_AWT_SETUPCMD) {
        iCharA cmd = recvd[0];
        recvd.Del(0, 1);
        if(cmd == '0') { //leave game and return to lobby
            m_state = LS_AWT_LOBBYCMD;
            gGame.ShowView(iChildGameView::CV_MPLOBBY);
        } else if(cmd == '1') { // player joined
            UserId id;
            iStringA nick;
            PLAYER_ID color;
            CTL_TYPE castle;

            id = (UserId)(recvd.Token(delim).GetUInt());
            nick = recvd.Token(delim);
            color = (PLAYER_ID)(recvd.Token(delim).GetUInt());
            castle = (CTL_TYPE)(recvd.Token(delim).GetUInt());

            GameUserData ud;
            ud.m_id = id;
            ud.m_color = color;
            ud.m_nickname = nick;
            ud.m_castle = castle;

            bool found = false;

            for(uint32 xx=0; xx<m_gameUsers.GetSize(); xx++)
                if(m_gameUsers[xx].m_id == id) {
                    found = true;
                    m_gameUsers[xx] = ud;
                    break;
                }

                if(!found) {
                    m_gameUsers.Add(ud);
                }
                ((iMPGameSetupView*)gGame.ActView())->UpdatePlayers();
            
            gSfxMgr.PlaySound(CSND_PICKUP01);
        } else if(cmd == '2') { // player left
            UserId id;
            GameUserList newl;

            id = (UserId)(recvd.Token(delim).GetUInt());


            for(uint32 xx=0; xx<m_gameUsers.GetSize(); xx++)
                if(m_gameUsers[xx].m_id != id) {
                    newl.Add(m_gameUsers[xx]);
                }
                m_gameUsers = newl;
                ((iMPGameSetupView*)gGame.ActView())->UpdatePlayers();
        } else if(cmd == '3') { // game started
            if(m_hostType == HT_HOST) { //launch the game, make the save and send it
                for(uint32 xx=0; xx<m_gameUsers.GetSize(); xx++)
                    if(m_gameUsers[xx].m_id == m_id) {          
                        m_mapInfo.PrepareForNetworkPlay(m_gameUsers[xx].m_color, true);
                        break;
                    }                   
                    gGame.StartNewGame(m_mapInfo, true, true);                  
                    // randoms have changed. update now                 
                    SaveGameFile(gSavePath + _T("mpsave.phs"), gGame.Map(), true);
                    //gGame.Map().UpdateTavernVisitors();
                    /*gGame.ExitGame(false);
                    iFilePtr pSaveFile(OpenWin32File(gSavePath + _T("mpsave.phs")));
                    if (pSaveFile) {
                        iMapInfo* pMapInfo = new iMapInfo;
                        pMapInfo->m_bNewGame = false;
                        pMapInfo->m_FileName = gSavePath + _T("mpsave.phs");
                        uint32 fourcc; pSaveFile->Read(&fourcc,sizeof(fourcc));
                        if (fourcc == GMAP_FILE_HDR_KEY && pMapInfo->ReadMapInfo(pSaveFile.get())) {                            
                            pMapInfo->PrepareForNetworkPlay(m_ownColor, true);
                            gGame.StartNewGame(*pMapInfo, false, true);
							iFile::Delete(gSavePath + _T("mpsave.phs"));
                            pSaveFile.reset();                          
                        }
                        delete pMapInfo;
                    }
                    */
                    //gGame.Map().UpdateTavernVisitors();
                    m_state = LS_AWT_SAVEACCEPT;
                    iFileStream pFile(OpenWin32File(gSavePath + _T("mpsave.phs")));
                    m_socket->Send(iFormat(("%d\n"), pFile.Size()));
                    iFile::Delete(gSavePath + _T("mpsave.phs"));

            } else 
                m_state = LS_AWT_SAVEOFFER;
        }
    } else if(m_state == LS_AWT_SAVEACCEPT) {
        if(recvd.GetUInt() == 1) { //accepted
            // if was send through queue, remove the entry
            if(m_sendQueue.GetSize() > 0)
                m_sendQueue.RemoveAt(0);
            iFileStream pFile(OpenWin32File(gSavePath + _T("mpsave.phs")));
            iDynamicBuffer buf(pFile.Size());
            buf.SetSize(pFile.Size());
            pFile.Read(buf.GetData(), pFile.Size());
            ShowStatusDlg(gTextMgr[TRID_UPLOADING_SAVEGAME], _T(""), true);
            m_socket->SendBuf((const char*)buf.GetData(), pFile.Size());                                    
            m_state = LS_AWT_SAVECNFRM;
        }
    } else if(m_state == LS_AWT_SAVECNFRM) {
        if(recvd.GetUInt() == 1) {
            m_state = LS_AWT_GAMECMD;
            if(m_statusDlg)
                HideStatusDlg();
            // unblock the interface
            gGame.MainView()->SetEnabled(true);

        }
    } else if(m_state == LS_AWT_SAVEOFFER) {
        m_rcvdCnt = recvd.GetUInt();
        if(m_rcvdCnt > 0) {
            m_state = LS_RCV_SAVE;
            m_socket->SetLineProtocol(false);
            ShowStatusDlg(gTextMgr[TRID_DOWNLOADING_SAVEGAME], _T(""), true);
            m_statusDlg->SetPercent(0);
            m_socket->Send(("1\n"));
            m_buff.ReInit(m_rcvdCnt);
            m_buff.Reset();
            m_bReplaying = true;                    
        }
    } else if(m_state == LS_AWT_GAMECMD) {
        iCharA cmd = recvd[0];
        recvd.Del(0, 1);
        if(cmd == MPC_ENDTURN) { // a turn ended
            gGame.Map().NextPlayer();   
        } else if(cmd == '1') { // cmd list start
            m_counter = recvd.GetUInt();
            if(m_counter > 0) {
                m_state = LS_AWT_GAMECMDMONAD;
                m_cmdQueue.RemoveAll();             
            }   
        }
    } else if(m_state == LS_AWT_GAMECMDMONAD) {             
        Debug(iFormat("cmd received: %s", recvd.CStr()));
        iStringA copy = recvd;
        iStringA prefix = copy.Token(delim);        
        copy.Del(0, 1);
        /* if command is binary data */
        for(uint32 xx=0; xx<BINARY_CMDS / sizeof(char); xx++)
            if(char(copy[0]) == binary_cmds[xx]) {              
                Debug(iFormat("enqueing binary cmd: %s", recvd.CStr()));
                iCharA ch = copy[0];
                copy.Del(0, 1);
                m_state = LS_RCV_CMDDATA;
                m_socket->SetLineProtocol(false);               
                //ShowStatusDlg(_T("Downloading data"), _T(""));

                //m_statusDlg->SetPercent(0);
                m_rcvdCnt = copy.Token(delim).GetUInt();                        
                m_buff.ReInit(m_rcvdCnt);
                m_buff.Rewind();
                m_socket->Send(("1\n"));
                GameCmd cmd;    
                copy.Del(0, 1);
                cmd.cmd = iFormat(("%s:%c%s"), prefix.CStr(), ch, copy.CStr());
                m_cmdQueue.Add(cmd);        
                return;
            }

        GameCmd cmd;
        cmd.cmd = recvd;
        m_cmdQueue.Add(cmd);        
        
        if(--m_counter == 0) {
            m_state = LS_AWT_GAMECMD;
            ExecuteCmdQueue();              
        }
    } else if(m_state == LS_AWT_BINACCEPT) {
        if(recvd.GetUInt() == 1) {
            GameCmd& cmd = m_sendQueue.At(0);
            m_socket->SendBuf((char*)cmd.data.GetData(), cmd.data.GetSize()); // send the top cmd data and wait for confirm
        
            m_sendQueue.RemoveAt(0); // remove the top cmd
            m_state = LS_AWT_BINCNFRM;          
        }
    } else if(m_state == LS_AWT_BINCNFRM) {
        //HideStatusDlg();
        if(recvd.GetUInt() == 1)
            StepSendCmdQueue();
    }   
}

void iMPManager::LoadCredentials()
{
#ifndef OS_IPHONE
    iFileStream pConfigFile(OpenWin32File(gRootPath + MP_CONFIG_FILE));
#else
    iFileStream pConfigFile(OpenWin32File(gSavePath + MP_CONFIG_FILE)); 
#endif  
    if (!pConfigFile.IsValid()) return;
    iStringT temp;
    pConfigFile.Read(temp);
    m_authkey = iStringA(CvtT2A<>(temp.CStr()));
}

void iMPManager::SaveCredentials()
{
#ifndef OS_IPHONE
    iFileStream pConfigFile(CreateWin32File(gRootPath + MP_CONFIG_FILE));
#else
    iFileStream pConfigFile(CreateWin32File(gSavePath + MP_CONFIG_FILE));   
#endif
    if (!pConfigFile.IsValid()) return;
    iStringT temp;
    temp = iStringT(CvtA2T<>(m_authkey.CStr()));
    pConfigFile.Write(temp);
}

void iMPManager::DisconnectFromServer()
{       
    m_bDisconFlag = true;
    gGame.RealMainMenu();
    if(!m_socket) return;   
    if(!m_socket->ErasedByHandler()) {      
        m_socket->SetCloseAndDelete();      
        m_socket = NULL;
    }
    
    //m_handler->Remove(m_socket);
    //m_socket->SetCloseAndDelete();
}


#ifdef OS_WINCE
#include "initguid.h"
#include "connmgr.h"

BOOL ConnectToNetwork(int timeoutsecs)
{
  // handle to connection to start
  HANDLE hConnection = NULL;

  // stores return value identifying status of connection attempt
  DWORD dwStatus;

  // initialise connection info structure
  CONNMGR_CONNECTIONINFO pConnectionInfo;
  ZeroMemory(&pConnectionInfo, sizeof(CONNMGR_CONNECTIONINFO));

  // set structure size
  pConnectionInfo.cbSize = sizeof(CONNMGR_CONNECTIONINFO);

  // set priority to identify that a user initiated this request
  // and the GUI is waiting on the creation of the connection
  pConnectionInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;

  // identify the network to connect to
  pConnectionInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
  pConnectionInfo.guidDestNet = IID_DestNetInternet;

  // specify that other applications can use this connection
  pConnectionInfo.bExclusive = FALSE;

  // specify that a connection should be made
  pConnectionInfo.bDisabled = FALSE;

  // request connection
  HRESULT hr = ConnMgrEstablishConnectionSync(&pConnectionInfo,
                                              &hConnection,
                                              timeoutsecs * 1000,
                                              &dwStatus);
  if (hr == S_OK && dwStatus == CONNMGR_STATUS_CONNECTED)
  {
    return true;
  }
  else
  {    
    return false;
  }
}
#endif

bool iMPManager::ConnectToServer()
{   
    m_bDisconFlag = false;
#ifdef OS_WINCE 
    ShowStatusDlg(_T(""), gTextMgr[TRID_CONNECTING_TO_SERVER], true);       
    if(!ConnectToNetwork(20)) { 
        HideStatusDlg();        
        return false;
    }
    HideStatusDlg();
#endif  

#if !defined(OS_IPHONE) && !defined(OS_WIN32)
    if(!gMPMgr.CheckUpdates()) {        
        gGame.RealMainMenu();
        return false;
    }   
#endif  
    
    ShowStatusDlg(_T(""), gTextMgr[TRID_CONNECTING_TO_SERVER], true);   
    if(m_socket && !m_socket->ErasedByHandler()) {
        m_socket->SetCloseAndDelete();              
    }
    m_socket = new PHSocket(*m_handler, this);  
    m_socket->SetCallOnConnect();   
    m_socket->SetLineProtocol();
    m_socket->SetDeleteByHandler();
    m_socket->SetConnectTimeout(4);
    //m_socket->SetTimeout(0);  
    //m_socket->SetConnectionRetry(1);  
    m_socket->Open(MP_SERVER, MP_PORT); 
    m_socket->SetTcpNodelay();  
    m_handler->Add(m_socket);
    m_handler->Select(0,0); 

    m_lobbyUsers.RemoveAll();
    m_games.RemoveAll();
    DebugReset();
    return true;
}

void iMPManager::Init()
{
    LoadCredentials();
#ifndef OS_IPHONE
    iFileStream pConfigFile(OpenWin32File(gRootPath + MP_STATS_FILE));
#else
    iFileStream pConfigFile(OpenWin32File(gSavePath + MP_STATS_FILE));  
#endif  
    if (!pConfigFile.IsValid()) return;
    pConfigFile.Read(m_totalTraf.down);
    pConfigFile.Read(m_totalTraf.up);   
}   

bool iMPManager::PollSockets(fix32 t)
{   
    if(!m_handler || !m_socket || m_socket->ErasedByHandler()) return 0;
    if(m_pra) return 0; // a great fix: if we are playing remote animation DO NOT POLL SOCKETS!!
    if(m_bBlock) return 0; // we are showing a dlg that is related to sockets problem, do not poll
    if(gGame.View(iChildGameView::CV_BATTLE) && ((iBattleView*)gGame.View(iChildGameView::CV_BATTLE))->IsAni()) return 0; // we are playing battle anim. please wait
    m_handler->Select(0,0);
    m_handler->CheckSanity();

    if(m_state == LS_AWT_BINACCEPT || m_state == LS_AWT_BINCNFRM || m_state ==LS_AWT_SAVEACCEPT ||
        m_state == LS_AWT_SAVECNFRM || m_state == LS_AWT_SAVEOFFER) {
            return true;
    }
    else {
        m_pingTimer += t;
        if(m_pingTimer >= fix32(60.0f))
        {
            m_pingTimer = fix32::zero;
            if(m_socket)
                m_socket->Send("\n");
        }
        return false;
    }   
}

void iMPManager::CreateGame(iMapInfo& mapInfo)
{
    check(m_state == LS_AWT_LOBBYCMD);
    m_mapInfo = mapInfo;
    mapInfo.m_Name[GLNG_ENGLISH].Replace(_T(":"), _T(".")); // ":" is a service symbol, replace it
    m_socket->Send(iFormat(("1%s%s%d%s"), CvtT2A<>(mapInfo.m_Name[GLNG_ENGLISH].CStr()).str_, delim, mapInfo.m_Players[0].m_Id, delim));
    m_socket->Send(iFormat(("%d%s"), mapInfo.HumanPlayers(), delim));
    for(uint32 xx=0; xx<mapInfo.TotalPlayers(); xx++) {
        if(mapInfo.m_Players[xx].m_TypeMask == PTM_HUMAN_ONLY || 
            mapInfo.m_Players[xx].m_TypeMask == PTM_HUMAN_OR_COMPUTER)
            m_socket->Send(iFormat(("%d%s"), mapInfo.m_Players[xx].m_Id, delim));
    }
    m_socket->Send(("\n"));
}

void iMPManager::LaunchGame()
{
    m_socket->Send(("1\n"));
}

void iMPManager::JoinGame(iMPManager::GSessionId game) 
{
    check(m_state == LS_AWT_LOBBYCMD);
    m_socket->Sendf("2%d\n", game);
}

void iMPManager::LeaveGame() 
{
    //check(m_state == LS_SETTING_UP);
    m_socket->Send(("0\n"));
}

void iMPManager::MarkReady(bool bReady)
{
    //check(m_state == LS_SETTING_UP);
    if(bReady)
        m_socket->Send(("11\n"));
    else
        m_socket->Send(("10\n"));
}

void iMPManager::ShowStatusDlg(const iStringT header, const iStringT txt, bool bCancelBtn)
{   
    m_statusDlg = new iMPStatusDlg(&gApp.ViewMgr(), header, txt, PID_NEUTRAL);  
    m_statusDlg->Show(bCancelBtn);
}

void iMPManager::HideStatusDlg()
{
    m_statusDlg->Hide();    
    gApp.ViewMgr().ReleaseViewCapture();
    delete m_statusDlg;
    m_statusDlg = NULL;
}

void iMPManager::EnqueueAndRun(const iStringA& string)
{
    GameCmd cmd;        
    cmd.cmd = string;
    m_sendQueue.Add(cmd);   
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();
}

void iMPManager::SendImpersonated(const iStringA& string)
{   
    if(m_hostType == HT_HOST)
        m_socket->Send(iFormat(("1%d%s%s\n"), gGame.Map().CurPlayer()->PlayerId(), 
        delim, string.CStr()));
    else
        m_socket->Send(iFormat(("1%s\n"), string.CStr()));      
}

void iMPManager::OnHeroMoved(const iPoint& from, const iPoint& to, uint32 moves)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bReplaying) return;  
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d%s%d"), MPC_HERO_MOVED, from.x, delim,
        from.y, delim, to.x, delim, to.y, delim, moves));   
}

void iMPManager::OnEndTurn()
{
    if(!m_socket || !m_socket->IsConnected()) return;   
    EnqueueAndRun(iFormat(("%c%d"), MPC_RSEED, gGame.Map().GetRandSeed())); 
    EnqueueAndRun(iFormat(("%c\n"), MPC_ENDTURN));  
}

void iMPManager::OnDisapObject(const iPoint& from)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d%s%d"), MPC_OBJ_DISAPPEARED, from.x, delim, from.y));
}

void iMPManager::OnNewTurn(PLAYER_ID pid) {
    if(!m_socket || !m_socket->IsConnected() ) return;
    
    if(m_hostType == HT_HOST) {
        iPlayer *plr = gGame.Map().FindPlayer(pid);
        if(!gGame.Map().IsFirstPlayer(plr)) return;

        // block the interface
        gGame.MainView()->SetEnabled(false);

        // save the game & upload
        SaveGameFile(gSavePath + _T("mpsave.phs"), gGame.Map(), true);
        iFileStream pFile(OpenWin32File(gSavePath + _T("mpsave.phs")));     
        EnqueueAndRun(iFormat(("2%d\n"), pFile.Size())); // sending savegame                
        //if(gGame.Map().GameDay() % 7 == 1 && IsMultiplayer())
        //  gGame.Map().UpdateTavernVisitors();
    }           
}

void iMPManager::OnAttackVisCnst(iHero* pAttacker, iVisCnst* pVisCnst)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
        
    EnqueueAndRun(iFormat(("%c%d\n"), MPC_RSEED, gGame.Map().GetRandSeed()));
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d\n"), MPC_VISCNST_ATTACKED,
        pAttacker->Pos().x, delim, pAttacker->Pos().y, delim,
        pVisCnst->Pos().x, delim, pVisCnst->Pos().y));              
}

void iMPManager::OnAttackMapGuard(iHero* pAttacker, iMapGuard *pMapGuard)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
        
    EnqueueAndRun(iFormat(("%c%d\n"), MPC_RSEED, gGame.Map().GetRandSeed()));
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d\n"), MPC_MAPGUARD_ATTACKED,
        pAttacker->Pos().x, delim, pAttacker->Pos().y, delim,
        pMapGuard->Pos().x, delim, pMapGuard->Pos().y));                
}   

void iMPManager::OnAttackHero(iHero* pAttacker, iHero* pDefender)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    // we (or our computer) attacked human, or a computer on a host or our machine
    //if(pDefender->Owner()->PlayerType() == PT_REMOTE || 
    //  pDefender->Owner()->PlayerType() == PT_COMPUTER) {
        EnqueueAndRun(iFormat(("%c%d\n"), MPC_RSEED, gGame.Map().GetRandSeed()));
        EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d\n"), MPC_HERO_ATTACKED,
            pAttacker->Pos().x, delim, pAttacker->Pos().y, delim,
            pDefender->Pos().x, delim, pDefender->Pos().y));            
    //}
}

void iMPManager::OnAttackOwnCnst(iHero* pAttacker, iOwnCnst *pCnst)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    
    //if(gGame.Map().FindPlayer(pCnst->Owner())->PlayerType() == PT_REMOTE) {
        EnqueueAndRun(iFormat(("%c%d\n"), MPC_RSEED, gGame.Map().GetRandSeed()));
        EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d\n"), MPC_OWNCNST_ATTACKED,
            pAttacker->Pos().x, delim, pAttacker->Pos().y, delim,
            pCnst->Pos().x, delim, pCnst->Pos().y));            
    //}
}



void iMPManager::OnSiegeCastle(iHero* pAttacker, iCastle* pCastle)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    //if(gGame.Map().FindPlayer(pCastle->Owner())->PlayerType() == PT_REMOTE) {
        EnqueueAndRun(iFormat(("%c%d\n"), MPC_RSEED, gGame.Map().GetRandSeed()));
        EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d\n"), MPC_CASTLE_SIEGED,
            pAttacker->Pos().x, delim, pAttacker->Pos().y, delim,
            pCastle->Pos().x, delim, pCastle->Pos().y));            
    //}

}

void iMPManager::OnCombatMove(const iPoint& to)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bPlayingAutobattle) return;  

    SendImpersonated(iFormat(("%c%d%s%d\n"), MPC_CBT_MOVE,
        to.x, delim, to.y));
    m_socket->Send(("4\n")); // send last cmd from queue to all
}

void iMPManager::OnCombatAttack(const iPoint &to, sint32 meleedir)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bPlayingAutobattle) return;  

    SendImpersonated(iFormat(("%c%d%s%d%s%d\n"), MPC_CBT_ATTACK,
        to.x, delim, to.y, delim, meleedir));
    m_socket->Send(("4\n")); // send last cmd from queue to all
}

void iMPManager::OnCombatShoot(const iPoint& to)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bPlayingAutobattle) return;  

    SendImpersonated(iFormat(("%c%d%s%d\n"), MPC_CBT_SHOOT,
        to.x, delim, to.y));
    m_socket->Send(("4\n")); // send last cmd from queue to all
}

void iMPManager::OnCombatWait()
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bPlayingAutobattle) return;  
    SendImpersonated(iFormat(("%c\n"), MPC_CBT_WAIT));
    m_socket->Send(("4\n")); // send last cmd from queue to all
}

void iMPManager::OnCombatDefend()
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bPlayingAutobattle) return;  
    SendImpersonated(iFormat(("%c\n"), MPC_CBT_DEFEND));
    m_socket->Send(("4\n")); // send last cmd from queue to all
}

void iMPManager::OnCombatCastSpell(iCombatSpell* pSpell, const iPoint& pos, iBattleGroup *pSpellTarget)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bPlayingAutobattle) return;      
    SendImpersonated(iFormat(("%c%d%s%d%s%d%s%d%s%d"), MPC_CBT_SPELLCAST,
        pSpell->Id(), delim, pos.x, delim, pos.y, delim, 
        pSpellTarget ? pSpellTarget->Owner()->GetSide() : 2,  delim, pSpellTarget ? pSpellTarget->ArmyGroupIndex() : 0));   
}


void iMPManager::OnCombatEnd(iBattleResult& br)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra || m_bPlayingAutobattle) return;  
    iStringA tosend = iFormat(("%c%d%s%d%s"), MPC_CBT_END, br.m_pWinner->Owner(), delim, br.m_defCause, delim); 

    if(br.m_defCause == DC_RETREAT || br.m_defCause == DC_SURRENDER) {
        tosend += iFormat(("%d%s%d%s"), br.m_pDest->Pos().x, delim, br.m_pDest->Pos().y, delim);        
    }
    EnqueueAndRun(tosend); // send battle end info for participants
    
    if(br.m_defCause == DC_RETREAT || br.m_defCause == DC_SURRENDER && br.m_pLoser->SpellCaster()) {
        // if battle ended in retreat or surrender,
        // non participants will see only hero teleporting
        EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d%s"), MPC_HERO_MOVED, br.m_pLoser->SpellCaster()->Pos().x, delim,
            br.m_pLoser->SpellCaster()->Pos().y, delim, br.m_pDest->Pos().x, delim, br.m_pDest->Pos().y, delim));   
    }

    /*
        that should update from another place
    if(br.m_pWinner->SpellCaster())
        OnHeroUpdate(br.m_pWinner->SpellCaster());
    if(br.m_pLoser->SpellCaster())
        OnHeroUpdate(br.m_pLoser->SpellCaster());
        */  
}

void iMPManager::OnCombatCatapultShot(iCastleFort::ElementType target)
{
    /*
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    SendImpersonated(iFormat(("t%d\n"), target));
    */

}

void iMPManager::OnCombatTurretShot(const iPoint& pos)
{
    /*  if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    SendImpersonated(iFormat(("u%d%s%d\n"), pos.x, delim, pos.y));
    */
}

void iMPManager::OnDig(const iPoint& cell)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d%s%d"), MPC_DIG, cell.x, delim, cell.y));   
}   

void iMPManager::OnHeroUpdate(iHero* pHero)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    if(pHero->Pos() == cInvalidPoint) return;

    GameCmd cmd;    
    pHero->Serialize(cmd.data);
    cmd.cmd = iFormat(("%c%d%s%d%s%d"), MPC_UPDATE_HERO, cmd.data.GetSize(), delim, pHero->Pos().x, delim, pHero->Pos().y);
    m_sendQueue.Add(cmd);
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();
}

void iMPManager::OnCastleUpdate(iCastle *pCastle)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    GameCmd cmd;    

    Serialize(cmd.data, pCastle->Garrison());
    // Can build
    cmd.data.Write((uint8)pCastle->CanBuild());
    // Constructions
    pCastle->Cnsts().Serialize(cmd.data);

    cmd.cmd = iFormat(("%c%d%s%d%s%d"), MPC_UPDATE_CASTLE, cmd.data.GetSize(), delim, pCastle->Pos().x, delim, pCastle->Pos().y);
    m_sendQueue.Add(cmd);
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();
}

void iMPManager::OnRecruitHero(iHero* pHero, iCastle* pCastle)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    EnqueueAndRun(iFormat(("%c%d"), MPC_RSEED, gGame.Map().GetRandSeed())); 
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d"), MPC_RECRUIT, pCastle->Pos().x, delim, pCastle->Pos().y, delim,
        pHero->Proto()->m_protoId));
}

void iMPManager::OnResourcesUpdate(PLAYER_ID pid, iMineralSet& newres)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    GameCmd cmd;    
    Serialize(cmd.data, newres);
    cmd.cmd = iFormat(("%c%d%s%d"), MPC_UPDATE_MINERALS, cmd.data.GetSize(), delim, pid);
    m_sendQueue.Add(cmd);
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();
}

void iMPManager::StepSendCmdQueue()
{
    while(m_sendQueue.GetSize() > 0) {
        GameCmd cmd = m_sendQueue.At(0);        
        if(cmd.cmd[0] != MPC_ENDTURN && cmd.cmd[0] != '2' && cmd.cmd[0] != '3')
            SendImpersonated(cmd.cmd);
        else
            m_socket->Send(cmd.cmd);

        if(cmd.cmd[0] == '2') {
            m_state = LS_AWT_SAVEACCEPT;        
            return;
        }
        else if(cmd.data.GetSize() > 0) {
            //ShowStatusDlg(_T("Sending data..."), _T(""));                     
            m_state = LS_AWT_BINACCEPT;
            return;
        } else 
            m_sendQueue.RemoveAt(0); // if no binary, just remove the cmd
    }       
    m_state = LS_AWT_GAMECMD;
}

void iMPManager::OnMapGuardUpdate(iMapGuard* pMapGuard)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    GameCmd cmd;    

    Serialize(cmd.data, pMapGuard->Creatures());
    
    cmd.cmd = iFormat(("%c%d%s%d%s%d"), MPC_UPDATE_MAPGUARD, cmd.data.GetSize(), delim, pMapGuard->Pos().x, delim, pMapGuard->Pos().y);
    m_sendQueue.Add(cmd);
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();
}

void iMPManager:: OnOwnCnstUpdate(iOwnCnst *pOwnCnst)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    GameCmd cmd;    

    Serialize(cmd.data, pOwnCnst->Guard()); 
    
    cmd.cmd = iFormat(("%c%d%s%d%s%d"), MPC_UPDATE_OWNCNST, cmd.data.GetSize(), delim, pOwnCnst->Pos().x, delim, pOwnCnst->Pos().y);
    m_sendQueue.Add(cmd);
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();
}

void iMPManager::OnVisCnstUpdate(iVisCnst *pVisCnst)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    GameCmd cmd;    

    pVisCnst->Serialize(cmd.data);
    
    cmd.cmd = iFormat(("%c%d%s%d%s%d"), MPC_UPDATE_VISCNST, cmd.data.GetSize(), delim, pVisCnst->Pos().x, delim, pVisCnst->Pos().y);
    m_sendQueue.Add(cmd);
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();

}

void iMPManager::OnMapItemUpdate(iMapItem *pMapItem)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  

    GameCmd cmd;    

    Serialize(cmd.data, pMapItem->Guard());
    if(pMapItem->Type() == MAPITEM_LAMP)
        pMapItem->Serialize(cmd.data);
    
    cmd.cmd = iFormat(("%c%d%s%d%s%d"), MPC_UPDATE_MAPITEM, cmd.data.GetSize(), delim, pMapItem->Pos().x, delim, pMapItem->Pos().y);
    m_sendQueue.Add(cmd);
    if(m_state == LS_AWT_GAMECMD)  // if nothing to do send first cmd
        StepSendCmdQueue();

}

void iMPManager::OnMapEventRemove(iMapEvent *pMapEvent)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    
    EnqueueAndRun(iFormat(("%c%d%s%d%s"), MPC_REMOVE_MAPEVENT, pMapEvent->Pos().x, delim,
        pMapEvent->Pos().y, delim));    
}
    

void iMPManager::OnOwnCnstChangedOwner(iOwnCnst *pCnst)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d"), MPC_CHANGE_OWNER, pCnst->Pos().x, delim,
        pCnst->Pos().y, delim, pCnst->Owner()));
}

void iMPManager::OnKeysUpdate(uint8 newkeys)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d"), MPC_UPDATE_KEYS, newkeys));
}

void iMPManager::OnOpenObelisksChanged(uint16 newobelisks)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d"), MPC_UPDATE_OBELISKS, newobelisks)); 
}

void iMPManager::OnCastleCaptured(iCastle* pCastle, PLAYER_ID newowner)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d"), MPC_CASTLE_CAPTURED, pCastle->Pos().x, delim, pCastle->Pos().y, delim, (uint32)newowner));  
}

void iMPManager::OnHeroDefeated(iHero* pHero, bool bResetArmy)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d"), MPC_REMOVE_HERO, pHero->Pos().x, delim, pHero->Pos().y, delim, (uint32)bResetArmy));    
}

void iMPManager::OnPlayerDropped(PLAYER_ID pid)
{
    if(!m_socket || !m_socket->IsConnected()) return;   
    EnqueueAndRun(iFormat(("%c%d"), MPC_PLAYER_DROPPED, pid));  
}

void iMPManager::OnPlayerWon(PLAYER_ID pid)
{
    if(!m_socket || !m_socket->IsConnected()) return;   
    EnqueueAndRun(iFormat(("%c%d"), MPC_PLAYER_WON, pid));  
}

void iMPManager::OnPlayerDefeated(PLAYER_ID pid)
{
    if(!m_socket || !m_socket->IsConnected()) return;   
    EnqueueAndRun(iFormat(("%c%d"), MPC_PLAYER_DEFEATED, pid)); 
}

void iMPManager::OnCharmAdded(iHero* pHero, MAGIC_SPELL spell, sint32 flag)
{
    if(!m_socket || !m_socket->IsConnected() || m_pra) return;  
    EnqueueAndRun(iFormat(("%c%d%s%d%s%d%s%d"), MPC_CHARM_ADDED, pHero->Pos().x, delim, pHero->Pos().y, delim, (uint32)spell, delim, (uint32)flag));
}

void iMPManager::DoKick(PLAYER_ID pid)
{
    if(!m_socket || !m_socket->IsConnected() || m_hostType != HT_HOST) return;  
    EnqueueAndRun(iFormat(("3%d\n"), pid)); 
}

iStringT iMPManager::GetColorNickname(PLAYER_ID color)
{
    for(uint32 xx=0; xx<m_gameUsers.GetSize(); xx++)
        if(m_gameUsers[xx].m_color == color) {
            return iStringT(CvtA2T<>(m_gameUsers[xx].m_nickname.CStr()));                               
        }
    return _T("");
}

iMPManager::PHHttpGetSocket::PHHttpGetSocket(ISocketHandler& h,const std::string& url,const std::string& to_file):
HttpGetSocket(h, url, to_file) {}

void iMPManager::PHHttpGetSocket::OnConnect() {
    SetMethod( "GET" );
    AddResponseHeader( "Accept", "text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1");
    AddResponseHeader( "Accept-Language", "en-us,en;q=0.5");
    AddResponseHeader( "Accept-Encoding", "deflate");
    AddResponseHeader( "Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    AddResponseHeader( "User-agent", MyUseragent() );

    if (GetUrlPort() != 80 && GetUrlPort() != 443)
        AddResponseHeader( "Host", GetUrlHost() + ":" + Utility::l2string(GetUrlPort()) );
    else
        AddResponseHeader( "Host", GetUrlHost() );
    SendRequest();
}


void iMPManager::PHHttpGetSocket::OnData(const char *buf,size_t len) {
    HttpGetSocket::OnData(buf, len);    
    if(gMPMgr.m_statusDlg && GetContentLength() > 0) {
        gMPMgr.m_statusDlg->SetPercent((sint32)100*GetContentPtr()/GetContentLength());
    }
}
    

uint32 GetUIntFromFile(iStringT filename)
{
    iFilePtr pFile( OpenWin32File(filename));               
    if(pFile->IsOpen()) {
        char buf[256];
        pFile->Read(buf, pFile->GetSize());
        buf[pFile->GetSize()] = 0;
        iStringA str = iStringA(buf);       
        return str.GetUInt();
    }
    return 0;
}

bool iMPManager::CheckUpdates()
{
    iStringA path = iStringA(CvtT2A<>((gRootPath + _T("newver.txt")).CStr()));
    PHSocketHandler h(&m_log);
    PHHttpGetSocket sock(h, iFormat("%s/version.txt", MP_UPDATE_SERVER).CStr(), path.CStr());                       
    h.Add(&sock);
    ShowStatusDlg(_T(""), gTextMgr[TRID_CHECKING_UPDATE], true);            
    while(!sock.Complete() && !m_bDisconFlag) {
        gApp.Cycle();
        h.Select(0,0);
        h.CheckSanity();
    }       
    
    if(m_bDisconFlag) {
        gGame.RealMainMenu();
        return false;
    }

    HideStatusDlg();
        
    
    bool need_update = true;

    if(!iFile::Exists(gRootPath + _T("newver.txt")) || 
        (iFile::Exists(gRootPath + _T("version.txt")) && 
        GetUIntFromFile(gRootPath + _T("newver.txt")) <= GetUIntFromFile(gRootPath + _T("version.txt")))) {
        need_update = false;
    }   
    iFile::Delete(gRootPath + _T("newver.txt"));

    if(!need_update) {      
        return true;
    }
        
    iQuestDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_UPDATE_AVAILABLE], gTextMgr[TRID_WOULD_YOU_LIKE_TO_UPDATE], PID_NEUTRAL);
    uint8 res = dlg.DoModal();
    if(res != DRC_YES) return false;
    
    ShowStatusDlg(_T(""), gTextMgr[TRID_UPDATING], true);   
        
    path = iStringA(CvtT2A<>((gRootPath + _T("PH_MP.zip")).CStr()));
    PHHttpGetSocket sock2(h, iFormat(
#if defined(OS_WINCE)
        "%s/PH_MP.zip"
#elif defined(OS_WIN32)
        "%s/PH_PC.zip"
#endif
        ,MP_UPDATE_SERVER).CStr(), path.CStr());                    
    h.Add(&sock2);
    while(!sock2.Complete() && !m_bDisconFlag) {        
        gApp.Cycle();
        h.Select(0,0);
        h.CheckSanity();
    }       

    if(m_bDisconFlag) {
        gGame.RealMainMenu();
        return false;
    }


    HideStatusDlg();
    
    if(!iFile::Exists(gRootPath + _T("PH_MP.zip"))) {       
        return false;
    }
    
    iTextDlg d(&gApp.ViewMgr(), _T(""), gTextMgr[TRID_PLEASE_RESTART], PID_NEUTRAL);
    d.DoModal();

    iStringT lan = _T("-l=en");
    if(gSettings.GetLanguage() == GLNG_RUSSIAN)
        lan = _T("-l=ru");
    iStringT updPath = gRootPath + _T("updater.exe");
    SHELLEXECUTEINFO cShellExecuteInfo;
    memset(&cShellExecuteInfo, 0, sizeof(cShellExecuteInfo)); // Set up memory block
    cShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    cShellExecuteInfo.fMask = 0;//SEE_MASK_NOCLOSEPROCESS;
    cShellExecuteInfo.hwnd = NULL;
    cShellExecuteInfo.lpVerb = _T("Open");
    cShellExecuteInfo.lpFile = updPath.CStr();
    cShellExecuteInfo.nShow = SW_SHOWNORMAL;
    cShellExecuteInfo.lpParameters = lan.CStr();
    ShellExecuteEx(&cShellExecuteInfo); 
    
    gGame.Quit();
    return true;
}

#endif //MULTIPLAYER