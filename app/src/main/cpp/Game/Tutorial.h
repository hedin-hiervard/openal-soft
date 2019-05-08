//
//  Tutorial.h
//  pheroes
//
//  Created by Andrey Kiryushkin on 1/30/12.
//  Copyright (c) 2012 Palm Heroes Team. All rights reserved.
//

#ifndef pheroes_Tutorial_h
#define pheroes_Tutorial_h

#ifdef OS_IPHONE
enum TUT_MSG
{
    TUT_MAP_CONTROLS,
    TUT_NO_PATH,
    TUT_MAP_ITEMS,
    TUT_VISITABLES,
    TUT_OWNERABLES,
    TUT_CASTLE,
    TUT_CASTLE_BUILDING,
    TUT_CASTLE_RECRUIT,
    TUT_CASTLE_TAVERN,
    TUT_CASTLE_MAGIC,
    TUT_CASTLE_MARKET,
    TUT_HERO,
    TUT_HERO_STATS,
    TUT_HERO_ARTIFACTS,
    TUT_HERO_MAGIC,
    TUT_HERO_MEETING,
    TUT_HERO_MEETING_ARMY,
    TUT_HERO_MEETING_ARTIFACTS,
    TUT_BATTLE,
    TUT_NEXT_TURN,
    TUT_HERO_CASTLE_SELECT,
    TUT_MINIMAP,
    TUT_INFO_MODE,
    TUT_TAP_TO_CONFIRM,
    TUT_HIRED_HERO,
    TUT_DOUBLE_CLICK_TO_ENTER,
    TUT_HORSE_BUTTON,
    TUT_CASTLE_SELECTED,
    TUT_HERO_TIRED,
    TUT_GAME_SENSE,
    TUT_BATTLE_INFO,
    TUT_BATTLE_DEFEND,
    TUT_BATTLE_WAIT,
    TUT_BATTLE_CAST,
    TUT_COUNT
};
#else
enum TUT_MSG
{
    TUT_MAP_CONTROLS,
    TUT_NO_PATH,
    TUT_MAP_ITEMS,
    TUT_VISITABLES,
    TUT_OWNERABLES,
    TUT_CASTLE,
    TUT_CASTLE_BUILDING,
    TUT_CASTLE_RECRUIT,
    TUT_CASTLE_TAVERN,
    TUT_CASTLE_MAGIC,
    TUT_CASTLE_MARKET,
    TUT_HERO,
    TUT_HERO_MAGIC,
    TUT_HERO_MEETING,
    TUT_BATTLE,
    TUT_NEXT_TURN,
    TUT_HERO_CASTLE_SELECT,
    TUT_INFO_MODE,
    TUT_TAP_TO_CONFIRM,
    TUT_HIRED_HERO,
    TUT_DOUBLE_CLICK_TO_ENTER,
    TUT_HORSE_BUTTON,
    TUT_CASTLE_SELECTED,
    TUT_HERO_TIRED,
    TUT_GAME_SENSE,
    TUT_BATTLE_INFO,
    TUT_BATTLE_DEFEND,
    TUT_BATTLE_WAIT,
    TUT_BATTLE_CAST,
    TUT_COUNT
};
#endif

class iTutorial
{
public:
    iTutorial() {}
    void Trigger(TUT_MSG msg);
    void MarkDontShow(TUT_MSG msg);
    void MarkDontShowAll();
    void Init();
    void ShowFingerHelp();
    void ResetDontShow();
private:
    void Save();
    
private:
    bool m_bDisabled[TUT_COUNT];
};

#endif
