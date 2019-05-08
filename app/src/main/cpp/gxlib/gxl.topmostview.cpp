#include "gxl.inc.h"
#ifdef OS_WINCE
//#include <gx.h>
#include "gapi_help.h"
#endif //OS_WINCE
#include "gxl.ctr.cbuff.h"
#include "gxl.ctr.array.h"
#include "gxl.window.h"
#include "gxl.input.h"
#include "gxl.timer.h"
#include "gxl.display.h"
#include "gxl.view.h"
#include "gxl.dialog.h"
#include "gxl.topmostview.h"
#include "gxl.viewmgr.h"
#include "gxl.application.h"

iTopmostView::iTopmostView(iViewMgr* pViewMgr)
: iView(pViewMgr, pViewMgr->Metrics(), GENERIC_VIEWPORT, 0, Visible|Enabled) {}

// Message handler
bool iTopmostView::ProcessMessage(const iInput::iEntry& msg)
{
	switch(msg.taskType) {
		case iInput::iEntry::MouseMove: MouseTrack(iPoint(msg.px,msg.py), msg.mID, msg.mbID); break;
		case iInput::iEntry::MouseDown: MouseDown(iPoint(msg.px,msg.py), msg.mID, msg.mbID); break;
		case iInput::iEntry::MouseUp: MouseUp(iPoint(msg.px,msg.py), msg.mID, msg.mbID); break;
		case iInput::iEntry::KeyDown: KeyDown(msg.key); break;
		case iInput::iEntry::KeyUp: KeyUp(msg.key); break;
	}
	return true;
}

bool iTopmostView::KeyDown(sint32 key)
{
	return OnKeyDown(key);
}

bool iTopmostView::KeyUp(sint32 key)
{
	return OnKeyUp(key);
}

