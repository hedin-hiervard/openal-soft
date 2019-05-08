#ifdef MULTIPLAYER

#include "stdafx.h"
#include "MPLoginView.h"

iMPLoginView::iMPLoginView(): iChildGameView(false, iChildGameView::CV_MENU) 
{
}

iMPLoginView::~iMPLoginView() {}

void iMPLoginView::OnCompose()
{
	gApp.Surface().Fill(cColor_Black);
}

void iMPLoginView::iCMDH_ControlCommand(iView *pView, CTRL_CMD_ID cmd, sint32 param)
{

}

#endif //MULTIPLAYER