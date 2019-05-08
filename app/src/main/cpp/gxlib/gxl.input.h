#ifndef GXLIB_GXL_INPUT_H_
#define GXLIB_GXL_INPUT_H_

#include "gxl.window.h"

enum iKbdKey {
	KEY_INVALID = -1,
#ifdef OS_WINCE
	KEY_SOFT1,
	KEY_SOFT2,
	KEY_VKA,
	KEY_VKB,
	KEY_VKC,
	KEY_VKD,
	KEY_VKE,
	KEY_VKF,
	KEY_REC,
	KEY_TALK,
	KEY_BACK,
#endif
	KEY_ENTER = 0,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_END,
	KEY_HOME,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_SPACE,
	KEY_ESC,
	KEY_TAB,
	KEY_E,
#ifdef OS_MACOS
	KEY_CMD_S,
	KEY_CMD_L,
	KEY_CMD_SHIFT_S,
	KEY_CMD_SHIFT_L,
	KEY_CMD_F,
#endif
	KEY_COUNT
};

class iInput : public iInputMsgHnd
{
public:
	struct iEntry {
		enum Type {
			Unknown = 0,
			MouseMove,
			MouseDown,
			MouseUp,
			KeyDown,
			KeyUp,
			MouseEntered,
			MouseLeave,
			MouseWheelDown,
			MouseWheelUp,
            MouseRightDown,
            MouseRightUp
		};

		iEntry() : taskType(Unknown), key(0) {}
		iEntry(Type t, sint16 x, sint16 y, MouseId mid, MouseButtonId mbid ) : taskType(t), px(x), py(y), mID(mid), mbID(mbid) {}
		iEntry(Type t, iKbdKey k) : taskType(t), key(k) {}

		Type	taskType;
		union {
			struct {
				sint16 px,py;
				MouseId mID;
				MouseButtonId mbID;
			};
			sint32	key;
		};
	};

private:
#ifdef OS_WINCE
	GXKeyList			m_KeyList;
#endif //OS_WINCE
	bool				m_KeysState[KEY_COUNT];
	uint32				m_Flags;
	sint16				m_mo;
	iSize				m_dimScreen;
	iCircBuff<iEntry>	m_Entries;
	bool				m_bInited;

	struct KeyEntry {
		uint32 vk;	// virtual key code
		uint32 kc;  // key code
	};

	KeyEntry			m_Translate[19];
	uint32				m_TranslateSize;
public:

	iInput();
	~iInput();
	bool Init(const iSize& scr_siz, uint32 flags);
	void Destroy();

	void SetOrientation(bool bLandscape, bool bLeftHander);
	inline uint32 EntryCount() const { return m_Entries.Count(); }
	inline iEntry& Get() { return m_Entries.Get(); }
	inline void Reset() { m_Entries.Reset(); }

	iKbdKey KeyCode2Key(sint16 key_code);
	
	void CvtScrCoor(sint16& px, sint16& py);

	// iInputMsgHnd 
	void msg_OnKeyDown(sint16 key);
	void msg_OnKeyUp(sint16 key);
	void msg_OnMouseMove(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID);
	void msg_OnMouseDown(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID);
	void msg_OnMouseUp(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID);
	void msg_OnMouseEntered(sint16 px, sint16 py, MouseId mID);
	void msg_OnMouseLeave(sint16 px, sint16 py, MouseId mID);
	void msg_OnMouseWheelDown(sint16 line, MouseId mID);
	void msg_OnMouseWheelUp(sint16 line, MouseId mID);
    void msg_OnMouseRightDown(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID);
	void msg_OnMouseRightUp(sint16 px, sint16 py, MouseId mID, MouseButtonId mbID);
    
	void ClearKeyState();
};


#endif //GXLIB_GXL_INPUT_H_
