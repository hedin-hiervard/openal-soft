# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(source
  ./Common/lang_cze.cpp
  ./Common/lang_eng.cpp
  ./Common/lang_fr.cpp
  ./Common/lang_ger.cpp
  ./Common/lang_pl.cpp
  ./Common/lang_rus.cpp
  ./Common/lang_slov.cpp
 ./complib/lzo/lzo.cpp
 ./complib/lzo/minilzo.c
  # ./complib/sha/sha.cpp
  # ./complib/sha/sha1.cpp
  # ./complib/xau/cpcm_tst.cpp
  # ./complib/xau/cpnm_tst.cpp
  ./complib/xau/xau.audio.cpp
  ./complib/xau/xjp.block.cpp
  ./complib/xau/xjp.codec.cpp
  ./complib/xau/xjp.dct.cpp
  ./complib/xau/xjp.huff.cpp
  ./complib/xau/xjp.quant.cpp
  # ./complib/xxc/wce.dyncode.cpp
  # ./complib/xxc/wce.wm5id.cpp
  # ./complib/xxc/xauthorize.cpp
  # ./complib/xxc/xserials.cpp
  # ./complib/xxc/xxc.app.cpp
  # ./complib/xxc/xxc.big.cpp
  # ./complib/xxc/xxc.bloom.cpp
  # ./complib/xxc/xxc.cipher.cpp
  # ./complib/xxc/xxc.ecp.cpp
  # ./complib/xxc/xxc.file.cpp
  # ./complib/xxc/xxc.gf2.cpp
  # ./complib/xxc/xxc.rnd.cpp
  # ./complib/xxc/xxc.sdh.cpp
  # ./complib/xxc/xxc.security.cpp
  # ./complib/xxc/xxc.shash.cpp
  # ./complib/xxc/xxc.store.cpp
  # ./complib/xxc/xxc.sysinfo.cpp
  # ./complib/xxc/xxc.wbox.cpp
  # ./complib/zip/unzip.cpp
  ./dlg_touch/Dlg_ArmyRoom.touch.cpp
  ./dlg_touch/Dlg_ArtInfo.cpp
  ./dlg_touch/Dlg_BattleCastle.cpp
  ./dlg_touch/Dlg_BattleHero.touch.cpp
  ./dlg_touch/Dlg_BattleLog.cpp
  ./dlg_touch/Dlg_BattleResult.cpp
  ./dlg_touch/Dlg_CapArt.cpp
  ./dlg_touch/Dlg_Chest.cpp
  ./dlg_touch/Dlg_ChooseBattleGroup.cpp
  ./dlg_touch/Dlg_CreatInfo.cpp
  ./dlg_touch/Dlg_FightGuard.cpp
  ./dlg_touch/Dlg_GameLanguage.cpp
  # ./dlg_touch/Dlg_HallOfFame.cpp
  ./dlg_touch/dlg_kingdomoverview.touch.cpp
  ./dlg_touch/Dlg_LeaveGuards.touch.cpp
  ./dlg_touch/Dlg_Level.touch.cpp
  ./dlg_touch/Dlg_Message.cpp
  ./dlg_touch/Dlg_MLInfo.cpp
  ./dlg_touch/Dlg_MPStatus.cpp
  ./dlg_touch/Dlg_NewWeek.cpp
  ./dlg_touch/dlg_puzzlemap.touch.cpp
  ./dlg_touch/Dlg_RandomMap.cpp
  ./dlg_touch/dlg_recruit.touch.cpp
  ./dlg_touch/Dlg_Register.cpp
  ./dlg_touch/Dlg_Rewards.cpp
  ./dlg_touch/Dlg_RiseSkeletons.cpp
  ./dlg_touch/Dlg_Save.touch.cpp
  ./dlg_touch/Dlg_ScenList.touch.cpp
  ./dlg_touch/Dlg_ScenProps.cpp
  ./dlg_touch/Dlg_Settings.cpp
  ./dlg_touch/Dlg_SkillInfo.cpp
  ./dlg_touch/Dlg_Spellbook.cpp
  ./dlg_touch/Dlg_SpellInfo.cpp
  ./dlg_touch/dlg_splitcreatures.touch.cpp
  ./dlg_touch/Dlg_SurrenderCond.cpp
  ./dlg_touch/Dlg_TimeEvent.cpp
  ./dlg_touch/Dlg_TownList.touch.cpp
  ./dlg_touch/Dlg_Tutorial.cpp
  ./dlg_touch/Dlg_WitchHut.cpp
  ./Game/ai.cpp
  ./Game/ai.dmap.cpp
  ./Game/ai.explore.cpp
  ./Game/ai.scanner.cpp
  ./Game/animation.cpp
  ./Game/AniObject.cpp
  ./Game/ArmyOwner.cpp
  ./Game/Artifacts.cpp
  ./Game/AutoBattle.cpp
  ./Game/AvatarMgr.cpp
  ./Game/BattleAI.cpp
  ./Game/BattleEngine.cpp
  ./Game/BattleFort.cpp
  ./Game/BattleInfo.cpp
  ./Game/BattleObstacles.cpp
  ./Game/BattlePF.cpp
  ./Game/BattleUnit.cpp
  ./Game/CastleCnsts.cpp
  ./Game/Castles.cpp
  ./Game/Composer.cpp
  ./Game/Constructions.cpp
  ./Game/Credits.cpp
  ./Game/dbg.cpp
  ./Game/fogmap.cpp
  ./Game/Game.cpp
  ./Game/GameWorld.cpp
  ./Game/GfxHlp.cpp
  ./Game/helpers.cpp
  ./Game/hero.cpp
  ./Game/HiddenBattle.cpp
  ./Game/hmm.cpp
  ./Game/InteractBattle.cpp
  ./Game/IntroDlg.cpp
  ./Game/ItemMgr.cpp
  ./Game/lang.cpp
  ./Game/Magic.cpp
  ./Game/Map.cpp
  ./Game/MapEnum.cpp
  ./Game/MapShop.cpp
  ./Game/MPManager.cpp
  ./Game/objects.cpp
  ./Game/Observer.cpp
  ./Game/PathFinding.cpp
  ./Game/player.cpp
  ./Game/PopupDlg.cpp
  ./Game/Remote_Player.cpp
  ./Game/RMG.cpp
  ./Game/Settings.cpp
  ./Game/Ship.cpp
  ./Game/SoundMap.cpp
  ./Game/Splash.cpp
  ./Game/SpriteDef.cpp
  ./Game/stdafx.cpp
  ./Game/TextManager.cpp
  ./Game/Tutorial.cpp
  ./gxlib/gxl.application.cpp
  ./gxlib/gxl.audio.cpp
  ./gxlib/gxl.cmdline.cpp
  ./gxlib/gxl.common.def.cpp
  ./gxlib/gxl.config.file.cpp
  ./gxlib/gxl.dialog.cpp
  ./gxlib/gxl.dib.cpp
  ./gxlib/gxl.dib.fx.cpp
  ./gxlib/gxl.dib.loader.cpp
  ./gxlib/gxl.dib.transform.cpp
  ./gxlib/gxl.dibfont.cpp
  ./gxlib/gxl.display.cpp
  ./gxlib/gxl.file.cpp
  ./gxlib/gxl.input.cpp
  ./gxlib/gxl.logmgr.cpp
  ./gxlib/gxl.math.cpp
  ./gxlib/gxl.memdbg.cpp
  ./gxlib/gxl.metrics.cpp
  ./gxlib/gxl.popupview.cpp
  ./gxlib/gxl.stdctrl.cpp
  ./gxlib/gxl.string.std.cpp
  ./gxlib/gxl.tables.cpp
  ./gxlib/gxl.topmostview.cpp
  ./gxlib/gxl.view.cpp
  ./gxlib/gxl.viewmgr.cpp
  #./gxlib/gxl.winapp.cpp
  ./gxlib/gxl.window.cpp
  # ./gxlib/android_specific/osys_iphone.cpp
  ./gxlib/android_specific/touchdb.cpp
  ./gxlib/android_specific/android_common.cpp
  ./gxlib/android_specific/AndroidView.cpp
  ./gxlib/res/GfxManager.cpp
  ./gxlib/res/MusicManager.cpp
  ./gxlib/res/SfxManager.cpp
  ./gxlib/res/sprite.cpp
  ./gxlib/res/TextComposer.cpp
  # ./gxlib/xxl.arena.cpp
  ./PH_Common/Army.cpp
  ./PH_Common/cm_castles.cpp
  ./PH_Common/cm_constructions.cpp
  ./PH_Common/cm_creatures.cpp
  ./PH_Common/cm_events.cpp
  ./PH_Common/cm_magic.cpp
  ./PH_Common/cm_ships.cpp
  ./PH_Common/common.cpp
  ./PH_Common/MPLogic.cpp
  ./PH_Common/PHSocket.cpp
  ./PH_Common/SpanMap.cpp
  ./PH_Common/treasury_cnst.cpp
  # editor ? ./reslib/Artifacts.cpp
  # editor ? ./reslib/AvatarMgr.cpp
  # editor ? ./reslib/Constructions.cpp
  # editor ? ./reslib/defines.cpp
  # editor ? ./reslib/objects.cpp
  # editor ? ./reslib/resmgr.cpp
  # editor ? ./reslib/SoundMgr.cpp
  # editor ? ./reslib/SpriteMgr.cpp
  # editor ? ./reslib/TextMgr.cpp
  ./screens_generic/ArmyList.cpp
  ./screens_generic/ArmyList.touch.cpp
  ./screens_generic/ArtifactList.cpp
  ./screens_generic/ArtifactList.touch.cpp
  ./screens_generic/BattleView.cpp
  ./screens_generic/BattleView.touch.cpp
  ./screens_generic/CastleViewCommons.cpp
  ./screens_generic/CommonControls.cpp
  ./screens_generic/CommonDialogs.cpp
  # ./screens_generic/Ctl_MapShop.cpp
  ./screens_generic/Ctl_Save.cpp
  ./screens_generic/Ctl_ScenList.cpp
  ./screens_generic/Ctl_ScenProps.cpp
  ./screens_generic/Ctl_Settings.cpp
  ./screens_generic/CtrlTransitionView.cpp
  ./screens_generic/FingerHelp.cpp
  ./screens_generic/GameMenuDlg.cpp
  ./screens_generic/GameMenuView.cpp
  ./screens_generic/GameViewInterface.cpp
  ./screens_generic/MenuView.cpp
  ./screens_generic/MinimapView.touch.cpp
  ./screens_generic/MPGameSetupView.cpp
  ./screens_generic/MPLobbyView.cpp
  ./screens_generic/MPLoginView.cpp
  ./screens_generic/OverlandView.cpp
  ./screens_generic/OverlandView.touch.cpp
  ./screens_generic/ScreenSlider.cpp
  ./screens_generic/ScrollingView.cpp
  ./screens_generic/TransitionView.cpp
  ./screens_ipad/CastleView.cpp
  ./screens_ipad/GameResultView.cpp
  ./screens_ipad/HarborView.cpp
  ./screens_ipad/HeroView.cpp
  ./screens_ipad/MainMenuView.cpp
  ./screens_ipad/MeetView.cpp
  ./screens_ipad/OverlandUI.cpp
  ./screens_ipad/UpgradeView.cpp
  ./screens_iphone/CastleView.cpp
  ./screens_iphone/GameResultView.cpp
  ./screens_iphone/HarborView.cpp
  ./screens_iphone/HeroView.cpp
  ./screens_iphone/MainMenuView.cpp
  ./screens_iphone/MeetView.cpp
  ./screens_iphone/OverlandUI.cpp
  ./screens_iphone/UpgradeView.cpp
  ./native-lib.cpp
  ./iosdk/analytics/android/Analytics.cpp
)

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${source})

target_include_directories(
    native-lib
    PRIVATE
    ./
    ./Game/
    ./gxlib
    ./Common
    ./PH_Common
    ./gxlib/res
    ./screens_generic
    ./screens_iphone
    ./screens_ipad
    ./dlg_touch
    ./complib
    ./reslib
  )

add_compile_definitions(
  OS_ANDROID
  FULL_VERSION
  HMM_GAME_2X
  IPHONE_DEBUG
  UNICODE_
  _UNICODE_
  UNICODE
  _UNICODE
  _HMM_GAME_
  SINGLEPLAYER
  DEVICE_IPHONE
)

# full version flags
# FULL_VERSION SHOP_PRODUCTION HMM_GAME_2X UNICODE_ _UNICODE_ UNICODE _UNICODE OS_IPHONE _HMM_GAME_ SINGLEPLAYER DEVICE_IPHONE HAS_STOREKIT

target_compile_options(
  native-lib
  PRIVATE
  -w
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
