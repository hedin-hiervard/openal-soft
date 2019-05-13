#include "stdafx.h"
#include "IntroDlg.h"
#include "lzo/lzo.h"

#include "FileAccessor/FileAccessor.h"

using namespace fileaccessor;

iIntroDlg::iIntroDlg(iViewMgr* pViewMgr)
: iDialog(pViewMgr), m_dibLogo(iSize(320,240), iDib::RGB), m_aniStep(0)
{
	iFilePtr sdataFile = OpenWin32File(RelativeFilePath("game.pix"));
	if (!sdataFile) return;
	// disabled for now
	check(0);
	uint32 tmpBuffLen;
	sdataFile->Read(&tmpBuffLen, sizeof(tmpBuffLen));
	unsigned char* pTmpBuff = (unsigned char*)malloc(tmpBuffLen);
	sdataFile->Read(pTmpBuff, tmpBuffLen);
	iDynamicBuffer tmpData;
	LZO::Decompress(pTmpBuff, tmpBuffLen, tmpData );
	memcpy(m_dibLogo.GetPtr(), tmpData.GetData(), tmpData.GetSize());
	m_pMgr->SetTimer(1000, 1, this);
}

void iIntroDlg::OnTimer(uint32 tid)
{
	if (m_aniStep < 255) {
		m_aniStep += 25;
		m_pMgr->SetTimer(30, 1, this);
		Invalidate();
	} else {
		EndDialog(DRC_OK);
	}
}

void iIntroDlg::OnCompose()
{
	if (m_aniStep < 255) {
		m_dibLogo.CopyToDibXY(gApp.Surface(), iPoint());
		gApp.Surface().Fade((uint8)(255 - m_aniStep));
	}
}

iSize iIntroDlg::GetDlgMetrics()
{
	return iSize(10,10);
}
