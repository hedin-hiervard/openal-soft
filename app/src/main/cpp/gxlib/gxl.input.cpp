#include "gxl.inc.h"
#ifdef OS_WINCE
//#include <gx.h>
#include "gxl.window.h"
#include "gxl.display.h"
#include "gapi_help.h"
#endif //OS_WINCE
#include "gxl.window.h"
#include "gxl.ctr.cbuff.h"
#include "gxl.input.h"


#ifdef OS_WINCE
// TODO:: Make just one instance of it!!!
#include "gxl.gxdyn.h"
#ifndef GXL_NOGAPI
extern iGAPILoader gapi;
#endif
/////////////////////////
#endif

// from winuserm.h
#define VK_TSOFT1   VK_F1               // Softkey 1
#define VK_TSOFT2   VK_F2               // Softkey 2
#define VK_TTALK    VK_F3               // Talk
#define VK_TEND     VK_F4               // End
#define VK_THOME    VK_LWIN             // Home
#define VK_TBACK    VK_ESCAPE           // Back

#define VK_TACTION      VK_RETURN       // Action
#define VK_TRECORD      VK_F10          // Record
#define VK_TFLIP        VK_F17          // Flip
#define VK_TPOWER       VK_F18          // Power
#define VK_TVOLUMEUP    VK_F6           // Volume Up
#define VK_TVOLUMEDOWN  VK_F7           // Volume Down

#define VK_TUP      VK_UP               // Up
#define VK_TDOWN    VK_DOWN             // Down
#define VK_TLEFT    VK_LEFT             // Left
#define VK_TRIGHT   VK_RIGHT            // Right

#define VK_T0       L'0'                // 0 .. 9 keys
#define VK_T1       L'1'
#define VK_T2       L'2'
#define VK_T3       L'3'
#define VK_T4       L'4'
#define VK_T5       L'5'
#define VK_T6       L'6'
#define VK_T7       L'7'
#define VK_T8       L'8'
#define VK_T9       L'9'
#define VK_TSTAR    VK_F8               // *
#define VK_TPOUND   VK_F9               // #

#define VK_SYMBOL   VK_F11              // Symbol (SYM) key

#define VK_REDKEY   VK_F19		// Sent by a keypad that has a special red function key
#define VK_ROCKER   VK_F20              // Arrow keys came from a Rocker
#define VK_DPAD     VK_F21              // Arrow keys came from a DPAD
#define VK_ACTION   VK_F23              // Sent with VK_RETURN when doing Action on PPC rockers


// Chorded keys.  DONE is VK_RWIN + VK_F6 and MOJI is VK_RWIN + VK_F7
#define VK_DONE     VK_F6                 // <OK/Close> hotkey code
#define MOD_DONE    (MOD_WIN | MOD_KEYUP) // <OK/Close> hotkey modifiers

#define VK_MOJI     VK_F7                  // Key used to switch between FE language layouts
#define MOD_MOJI    (MOD_WIN | MOD_KEYUP)  //Modifiers so MOJI goes to the shell


// App keys
#define VK_APP1     0xC1
#define VK_APP2     0xC2
#define VK_APP3     0xC3
#define VK_APP4     0xC4
#define VK_APP5     0xC5
#define VK_APP6     0xC6


//////////////////////////////////////////////////////////////////////////
iInput::iInput() : m_bInited(false)
{
	ClearKeyState();
}

iInput::~iInput()
{
	check(!m_bInited);
}

void iInput::ClearKeyState()
{
	memset(m_KeysState,0,sizeof(m_KeysState));
}

bool iInput::Init(const iSize& scr_siz, uint32 flags)
{
	check(!m_bInited);
	m_dimScreen = scr_siz;
	m_Flags = flags;
#ifdef OS_WINCE
	if ( !gGapi.Init(false) ) return false;
	if (!gGapi.GXOpenInput()) return false;
	m_KeyList = gGapi.GXGetDefaultKeys(GX_NORMALKEYS);

	SetOrientation( (flags & GXLF_LANDSCAPE) != 0, (flags & GXLF_LHANDER) != 0);

#endif //OS_WINCE

	return m_bInited = true;
}

void iInput::Destroy()
{
	check(m_bInited);
#ifdef OS_WINCE
	gGapi.GXCloseInput();
#endif //OS_WINCE
	m_Entries.Reset();

	m_bInited = false;
}

void iInput::SetOrientation(bool bLandscape, bool bLeftHander)
{
	if (bLandscape && (m_Flags & GXLF_LANDSCAPE) == 0) m_Flags |= GXLF_LANDSCAPE;
	else if (!bLandscape && (m_Flags & GXLF_LANDSCAPE) != 0) m_Flags ^= GXLF_LANDSCAPE;

	if (bLeftHander && (m_Flags & GXLF_LHANDER) == 0) m_Flags |= GXLF_LHANDER;
	else if (!bLeftHander && (m_Flags & GXLF_LHANDER) != 0) m_Flags ^= GXLF_LHANDER;

	if (m_Flags & GXLF_LANDSCAPE) {
		//if ( (m_Flags & GXLF_DEV_LANDSCAPE) || (m_Flags & GXLF_LHANDER)) {
		//	m_mo = (sint16)(m_dimScreen.w) - 1;
		//} else {
		m_mo = (sint16)(m_dimScreen.h) - 1;
		//}
	} else {
		m_mo = 0;
	}

	// now prepare translate key map
#ifdef OS_WINCE
#define RIDX( kcode, kofs ) ((kcode+kofs)&0x03)

	size_t rotateKeys = 0;
//#if !defined(HPC_JORNADA)
	if ( m_Flags & GXLF_LANDSCAPE ) {
		if ( m_Flags & GXLF_LHANDER ) {
			rotateKeys = 3;
		} else {
			rotateKeys = 1;
		}
	}
//#endif


	m_Translate[ RIDX( 0, rotateKeys ) ].vk	= m_KeyList.vkUp;
	m_Translate[ RIDX( 1, rotateKeys ) ].vk	= m_KeyList.vkRight;
	m_Translate[ RIDX( 2, rotateKeys ) ].vk	= m_KeyList.vkDown;
	m_Translate[ RIDX( 3, rotateKeys ) ].vk	= m_KeyList.vkLeft;
	m_Translate[ 0 ].kc = KEY_UP;
	m_Translate[ 1 ].kc = KEY_RIGHT;
	m_Translate[ 2 ].kc = KEY_DOWN;
	m_Translate[ 3 ].kc = KEY_LEFT;

	m_Translate[ 4 ].vk = 0x86; // VK_ACTION m_KeyList.vkStart;
	m_Translate[ 4 ].kc = KEY_ENTER;
	m_Translate[ 5 ].vk = m_KeyList.vkA; // VK_APP1
	m_Translate[ 5 ].kc = KEY_VKA;
	m_Translate[ 6 ].vk = m_KeyList.vkB; // VK_APP2
	m_Translate[ 6 ].kc = KEY_VKB;
	m_Translate[ 7 ].vk = m_KeyList.vkC; // VK_APP3
	m_Translate[ 7 ].kc = KEY_VKC;
	// patch for asus (and co)
	m_Translate[ 8 ].vk = VK_RETURN;
	m_Translate[ 8 ].kc = KEY_ENTER;

	// find fourth hardware key
	// TODO:: refactor to the separate proc
	uint32 foundKey4 = VK_F15; // unused by default
	uint32 fourKeys[4] = { VK_APP1, VK_APP2, VK_APP3, VK_APP4 };
	for( size_t nk = 5; nk != 8; nk++ ) {
		uint32 passedCode = m_Translate[nk].vk;
		if ( passedCode >= VK_APP1 && passedCode <= VK_APP4 ) {
			fourKeys[ passedCode - VK_APP1 ] = 0; // mark as used
		}
	}
	for( size_t nk = 0; nk !=4; nk++ ) {
		if ( fourKeys[nk] != 0 ) {
			foundKey4=fourKeys[nk];
			break;
		}
	}
	m_Translate[ 9 ].vk = foundKey4; //FindFourthHWKey();
	m_Translate[ 9 ].kc = KEY_VKD;

//	m_Translate[ 9 ].vk = VK_APP4;
//	m_Translate[ 9 ].kc = KEY_VKD;
	m_Translate[10 ].vk = VK_APP5;
	m_Translate[10 ].kc = KEY_VKE;
	m_Translate[11 ].vk = VK_APP6;
	m_Translate[11 ].kc = KEY_VKF;
	// record key (if any)
	m_Translate[12 ].vk = VK_TRECORD;
	m_Translate[12 ].kc = KEY_REC;
	// smartphone specific keys
	m_Translate[13 ].vk = VK_TTALK;
	m_Translate[13 ].kc = KEY_TALK;
	m_Translate[14 ].vk = VK_TEND;
	m_Translate[14 ].kc = KEY_END;
	m_Translate[15 ].vk = VK_TBACK;
	m_Translate[15 ].kc = KEY_BACK;
	// - this scan code sent on every now and then for now apparent reason
	// ( The VK_APP keys will only be recognized by the system if they are wrapped with VK_LWIN. The following code snippet demonstrates how to do this programmatically. )
	// keybd_event(VK_LWIN, 0, KEYEVENTF_SILENT, 0);
	// GenKeyEvent(VK_APP1);
	// keybd_event(VK_LWIN, 0, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, 0);
	m_Translate[16 ].vk = 0x99ff; //VK_THOME;
	m_Translate[16 ].kc = KEY_HOME;
	m_Translate[17 ].vk = VK_TSOFT1;
	m_Translate[17 ].kc = KEY_SOFT1;
	m_Translate[18 ].vk = VK_TSOFT2;
	m_Translate[18 ].kc = KEY_SOFT2;

	m_TranslateSize     = 19;

#undef RIDX
#endif //OS_WINCE
}

iKbdKey iInput::KeyCode2Key(sint16 key_code)
{
	// following keys also should be assigned to their
	// system-default actions (on Smartphone platform)
	// VK_TTALK - none or user action
	// VK_TEND  - quit application
	// VK_TBACK - switch to previous application (or minimize)
	// VK_THOME - switch to the desktop (or minimize)

#ifdef OS_WINCE

    for( size_t n = 0; n != m_TranslateSize; ++n ) {
    	if ( key_code == m_Translate[n].vk )
    		return (iKbdKey)m_Translate[n].kc;
    }
    return KEY_INVALID;

#endif //OS_WINCE
#ifdef OS_WIN32
	if (key_code == VK_RETURN ) return KEY_ENTER;
	else if (key_code == VK_UP ) return KEY_UP;
	else if (key_code == VK_DOWN ) return KEY_DOWN;
	else if (key_code == VK_LEFT ) return KEY_LEFT;
	else if (key_code == VK_RIGHT ) return KEY_RIGHT;
//	else if (key_code == (unsigned char)'Q') return KEY_VKA;
//	else if (key_code == (unsigned char)'W') return KEY_VKB;
//	else if (key_code == (unsigned char)'E') return KEY_VKC;
	else if (key_code == VK_F8) return KEY_F8;
	return KEY_INVALID;
#endif // OS_WIN32

#ifdef OS_MACOS

#endif
}


void iInput::msg_OnKeyDown(sint16 key)
{
	iKbdKey kkey = KeyCode2Key(key);
	if (kkey != KEY_INVALID && !m_KeysState[kkey]){
		m_KeysState[kkey] = true;
		m_Entries.Put(iEntry(iEntry::KeyDown,kkey));
	} else {
	}
}

void iInput::msg_OnKeyUp(sint16 key)
{
	iKbdKey kkey = KeyCode2Key(key);
	if (kkey != KEY_INVALID && m_KeysState[kkey]){
		m_KeysState[kkey] = false;
		m_Entries.Put(iEntry(iEntry::KeyUp,kkey));
	} else {
	}
}


// HACK: from gxl.display.cpp
extern bool bDisplayNativeLandscape;
extern bool bDisplayFullScreenMode;

#ifdef OS_WINCE
extern iDisplay* main_display;
#endif

void iInput::CvtScrCoor(sint16& px, sint16& py)
{
    // HACK... all data stored in iDisplay class, so the best variant to call it.
#ifdef OS_WINCE
	if (main_display)
    {
        main_display->CvtScrCoor(px, py);
    }
#elif defined(OS_WIN32)
	if (m_Flags & GXLF_DOUBLESIZE) { px /= 2; py /= 2; }
#elif defined(OS_IPHONE) || defined(OS_ANDROID)
    if(m_Flags & GXLF_DOUBLESIZE) {
		 px /= 2;
		 py /= 2;
	} else  if(m_Flags & GXLF_QUADSIZE) {
		px /= 4;
		py /= 4;
	}
//	sint16 npx;
//	sint16 npy;
//	iSize sz = m_dimScreen;
//	if(m_Flags & GXLF_DOUBLESIZE) {
//		sz.w /= 2;
//		sz.h /= 2;
//		// px /= 2;
//		// py /= 2;
//	}
//	if(!(m_Flags & GXLF_LHANDER)) {
//		npx = sz.w - py;
//		npy = px;
//	}
//	else {
//		npx = py;
//		npy = sz.h - px;
//	}
//	px = npx;
//	py = npy;
#endif

}



void iInput::msg_OnMouseMove(sint16 px, sint16 py, MouseId mID, MouseButtonId mbid)
{
	CvtScrCoor(px,py);
	m_Entries.Put(iEntry(iEntry::MouseMove,px,py, mID, mbid));
}

void iInput::msg_OnMouseDown(sint16 px, sint16 py, MouseId mID, MouseButtonId mbid)
{
//    NKDbgPrintfW(L"Down1: %d %d\n", px, py);
	CvtScrCoor(px,py);
//    NKDbgPrintfW(L"Down2: %d %d\n", px, py);
	m_Entries.Put(iEntry(iEntry::MouseDown,px,py, mID, mbid));
}

void iInput::msg_OnMouseUp(sint16 px, sint16 py, MouseId mID, MouseButtonId mbid)
{
//    NKDbgPrintfW(L"Up1: %d %d\n", px, py);
	CvtScrCoor(px,py);
 //   NKDbgPrintfW(L"Up2: %d %d\n", px, py);
	m_Entries.Put(iEntry(iEntry::MouseUp,px,py, mID, mbid));
}

void iInput::msg_OnMouseEntered(sint16 px, sint16 py, MouseId mID){

	CvtScrCoor(px,py);

	m_Entries.Put(iEntry(iEntry::MouseEntered,px,py, mID, 0));
}

void iInput::msg_OnMouseLeave(sint16 px, sint16 py, MouseId mID){

	CvtScrCoor(px,py);

	m_Entries.Put(iEntry(iEntry::MouseLeave,px,py, mID, 0));
}

void iInput::msg_OnMouseWheelDown(sint16 line, MouseId mID){

	m_Entries.Put(iEntry(iEntry::MouseWheelDown,line,0, mID, 0));
}

void iInput::msg_OnMouseWheelUp(sint16 line, MouseId mID){

	m_Entries.Put(iEntry(iEntry::MouseWheelUp,line,0, mID, 0));
}

void iInput::msg_OnMouseRightDown(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID){
#ifdef OS_APPLE
    CvtScrCoor(px,py);
#endif
	m_Entries.Put(iEntry(iEntry::MouseRightDown,px,py, mID, mbID));
}

void iInput::msg_OnMouseRightUp(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID){
#ifdef OS_APPLE
    CvtScrCoor(px,py);
#endif
	m_Entries.Put(iEntry(iEntry::MouseRightUp,px,py, mID, mbID));
}
