#include "gxl.inc.h"
#include "gxl.ctr.array.h"
#include "gxl.ctr.cbuff.h"
#include "gxl.ctr.array.h"
#include "gxl.ctr.map.h"
#include "gxl.input.h"
#include "gxl.timer.h"
#include "gxl.display.h"

#include "gxl.view.h"
#include "gxl.dialog.h"
#include "gxl.topmostview.h"
#include "gxl.popupview.h"
#include "gxl.stdctrl.h"

#include "gxl.viewmgr.h"
#include "gxl.application.h"
#include "SfxManager.h"
#include "xau/xau.audio.h"


iSfxManager::iSfxManager()
: m_pDataFile(NULL), m_pPcmBuff(NULL), m_pcmBuffLen(0), m_pXauBuff(NULL), m_xauBuffLen(0)

{

}

iSfxManager::~iSfxManager()
{
	Cleanup();
}

bool iSfxManager::Init(const fileaccessor::RelativeFilePath& path)
{
	Cleanup();
	m_pDataFile = OpenWin32File(path);
	if (!m_pDataFile) return false;
	uint32 cnt;
	m_pDataFile->Read(&cnt, sizeof(uint32));
	while (cnt--) {
		uint32 pcmLen, xauLen;
		m_pDataFile->Read(&pcmLen, sizeof(uint32));
		m_pDataFile->Read(&xauLen, sizeof(uint32));
		if (m_xauBuffLen < xauLen) {
			free(m_pXauBuff);
			m_pXauBuff = malloc(xauLen);
			m_xauBuffLen = xauLen;
		}
		m_pDataFile->Read(m_pXauBuff, xauLen);

		m_pPcmBuff = (sint8*)malloc(pcmLen);
		m_pcmBuffLen = pcmLen;

		xau_pcm8m_decode( m_pXauBuff, xauLen, m_pPcmBuff, pcmLen );
		m_entries.Add(iGXApp::Instance().SndPlayer().LoadSample(m_pPcmBuff, pcmLen, 0));
		// !! do not free m_pPcmBuff right now, it is used internally by SndPlayer and will be released by it when needed
		// it doesn't free...
//		free(m_pPcmBuff);
	}
	/*m_pcmBuffLen = m_xauBuffLen = 16*1024;
	m_pPcmBuff = (sint8*)malloc(m_pcmBuffLen);
	m_pXauBuff = malloc(m_xauBuffLen);
	*/
	return true;
}

void iSfxManager::Cleanup()
{
/*
	m_entries.RemoveAll();
	if (m_pPcmBuff) {
		free(m_pPcmBuff);
		m_pPcmBuff = NULL;
		m_pcmBuffLen = 0;
	}
	if (m_pXauBuff) {
		free(m_pXauBuff);
		m_pXauBuff = NULL;
		m_xauBuffLen = 0;
	}
	if (m_pDataFile) {
		m_pDataFile.reset();
		m_pDataFile = NULL;
	}
*/
}

sint32 iSfxManager::PlaySound(uint16 idx, bool bLoop)
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) return -1;
#endif

	if(idx == (uint16)(-1)) return -1;
	if (!iGXApp::Instance().SndPlayer().Inited()) return -1;
	return iGXApp::Instance().SndPlayer().PlaySample(m_entries[idx], (bLoop)?iSoundPlayer::PlayLoop:0);
}

sint32 iSfxManager::ReplaceSound(sint32 channel, uint16 idx)
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) return -1;
#endif

	if (!iGXApp::Instance().SndPlayer().Inited()) return -1;
	return iGXApp::Instance().SndPlayer().ReplaceSample(channel, m_entries[idx]);
}

