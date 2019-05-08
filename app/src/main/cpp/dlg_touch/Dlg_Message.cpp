#include "stdafx.h"
#include "Dlg_Message.h"

iDlg_Message::iDlg_Message(iViewMgr* pViewMgr, iHero* pHero, iMapItem* pMapItem)
: iTextDlg(pViewMgr,gTextMgr[TRID_MAPRES_GOLD+pMapItem->Type()], pMapItem->Message(),pHero->Owner()->PlayerId()), m_pHero(pHero), m_pMapItem(pMapItem)
{}


