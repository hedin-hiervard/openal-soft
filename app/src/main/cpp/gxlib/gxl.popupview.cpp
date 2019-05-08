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
#include "gxl.popupview.h"
#include "gxl.viewmgr.h"
#include "gxl.application.h"

iPopupView::iPopupView(iViewMgr* pViewMgr)
: iView(pViewMgr, iRect(0,0,0,0), GENERIC_VIEWPORT, 0, Enabled)
{
}

void iPopupView::TrackPopup(const iPoint& pos, const iRect& bound, Alignment al)
{
	iSize nsiz = PopupViewSize();
	iSize msiz = PopupViewMinSize();
	nsiz.w = iMAX<uint32>(nsiz.w,msiz.w);
	nsiz.h = iMAX<uint32>(nsiz.h,msiz.h);

	iRect orc = bound;
	
	if (al & AlignRight) {
		orc.w = pos.x - bound.x;
	} else if (al & AlignLeft) {
		orc.x = pos.x;
	}

	if (al & AlignBottom) orc.h = pos.y - bound.y;
	else if (al & AlignTop) orc.y = pos.y;

	m_Rect = AlignRect(nsiz, orc, al);


	if (m_Rect.x < bound.x) m_Rect.x = bound.x;
	else if (m_Rect.x + m_Rect.w > bound.x + bound.w) m_Rect.x = bound.x + bound.w - m_Rect.w;
	if (m_Rect.y < bound.y) m_Rect.y = bound.y;
	else if (m_Rect.y + m_Rect.h > bound.y + bound.h) m_Rect.y = bound.y + bound.h - m_Rect.h;

	OnTrackPopup(m_Rect);
	SetVisible(true);
}

void iPopupView::HidePopup()
{
	OnHidePopup();
	SetVisible(false);
}
