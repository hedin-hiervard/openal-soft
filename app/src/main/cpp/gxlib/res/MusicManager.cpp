
#include "gxl.inc.h"
#include "gxl.ctr.array.h"
#include "MusicManager.h"
#include "FileAccessor/FileAccessor.h"

#ifdef OS_android
#include "android_musicmgr.h"
#endif

#if defined(OS_MACOS)
#include "macos_musicmgr.h"
#endif

iMusicManager::iMusicManager()
#if defined( OS_MACOS )
: curPos(0)
, isPause( false )
#endif
{

}

using namespace fileaccessor;

void androidMusicMgr_Play(const char *filename);
void androidMusicMgr_Stop();
void androidMusicMgr_SetVolume(double newvol);
unsigned int androidMusicMgr_GetCurPos();
void androidMusicMgr_SetCurPos(unsigned int);

iMusicManager::~iMusicManager()
{

}

bool iMusicManager::Init()
{
#ifdef OS_WIN32
	device = audiere::OpenDevice();
#endif
#ifdef OS_APPLE
	return androidMusicMgr_Init();
#endif
	return true;
}

void iMusicManager::Cleanup()
{

}

void iMusicManager::SetVolume(uint32 newvol)
{
	androidMusicMgr_SetVolume((double)newvol / 10.0);
}

void iMusicManager::Play(const fileaccessor::RelativeFilePath& path, bool restorePos)
{
	if(nowPlaying == path) return;

	 RecordPos(nowPlaying, androidMusicMgr_GetCurPos());
	 auto tmp = FileAccessor::sharedFileAccessor()->resolve(path);
	 androidMusicMgr_Play(tmp.to_string().c_str());
	 nowPlaying = path;
	 if(restorePos)
		androidMusicMgr_SetCurPos(GetRecordedPos(path));
}

void iMusicManager::Stop()
{
	androidMusicMgr_Stop();
}

void iMusicManager::RecordPos(const fileaccessor::RelativeFilePath& path, uint32 pos)
{
	 for(uint32 xx=0; xx<m_posHist.GetSize(); xx++)
	 	if(m_posHist[xx].path == path) {
	 		m_posHist[xx].pos = pos;
	 		return;
	 	}

	 posRec rec;
	 rec.path = path;
	 rec.pos = pos;
	 m_posHist.Add(rec);
}
uint32 iMusicManager::GetRecordedPos(const fileaccessor::RelativeFilePath& path)
{
	 for(uint32 xx=0; xx<m_posHist.GetSize(); xx++)
	 	if(m_posHist[xx].path == path) {
	 		return m_posHist[xx].pos;
	 	}
	return 0;
}

