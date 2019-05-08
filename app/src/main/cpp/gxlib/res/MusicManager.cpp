
#include "gxl.inc.h"
#include "gxl.ctr.array.h"
#include "MusicManager.h"

#ifdef OS_IPHONE
#include "iphone_musicmgr.h"
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

iMusicManager::~iMusicManager()
{
	
}

bool iMusicManager::Init()
{
#ifdef OS_WIN32
	device = audiere::OpenDevice(); 
#endif
#ifdef OS_APPLE
	return iphoneMusicMgr_Init();
#endif
	return true;
}

void iMusicManager::Cleanup()
{
#ifdef OS_APPLE
	iphoneMusicMgr_Deinit();
#endif
}
		
void iMusicManager::SetVolume(uint32 newvol)
{
#ifdef OS_WIN32
	volume = (double)newvol / 10.0;
	if( sound )
		sound->setVolume(volume);
#endif
#ifdef OS_APPLE
	iphoneMusicMgr_SetVolume((double)newvol / 10.0);
#endif
}

void iMusicManager::Play(iStringT& filename, bool restorePos)
{
	if(nowPlaying == filename) return;
	iStringA tmp = iStringA(CvtT2A<>(filename.CStr()));
#ifdef OS_WIN32
	Stop();
	sound = audiere::OpenSound(device, tmp.CStr(), true);
	sound->setVolume( volume );
	sound->setRepeat(true); 
	sound->play();
#endif

#ifdef OS_APPLE
	RecordPos(nowPlaying, iphoneMusicMgr_GetCurPos());
	iphoneMusicMgr_Play(tmp.CStr());
	if(restorePos)
		iphoneMusicMgr_SetCurPos(GetRecordedPos(filename));
#endif
	nowPlaying = filename;
	
}

void iMusicManager::Stop()
{
#ifdef OS_WIN32
	if(sound)
		sound->stop();
#endif

#ifdef OS_APPLE
	iphoneMusicMgr_Stop();
#endif	
}

void iMusicManager::RecordPos(iStringT& track, uint32 pos)
{
	for(uint32 xx=0; xx<m_posHist.GetSize(); xx++) 
		if(m_posHist[xx].track == track) {
			m_posHist[xx].pos = pos;
			return;
		}
	
	posRec rec;
	rec.track = track;
	rec.pos = pos;
	m_posHist.Add(rec);
}
uint32 iMusicManager::GetRecordedPos(iStringT &track)
{
	for(uint32 xx=0; xx<m_posHist.GetSize(); xx++) 
		if(m_posHist[xx].track == track) {
			return m_posHist[xx].pos;
		}
	return 0;
}

#if defined( OS_MACOS )
void iMusicManager::Play(){

	if( isPause ){
		
		isPause = false;
	
		if( nowPlaying.Length() ){
		
			iStringA tmp = iStringA(CvtT2A<>(nowPlaying.CStr()));
			iphoneMusicMgr_Play(tmp.CStr());
			iphoneMusicMgr_SetCurPos(curPos);
		}
	}
}

void iMusicManager::Pause(){

	if( !isPause ){
	
		isPause = true;
		curPos = iphoneMusicMgr_GetCurPos();
		iphoneMusicMgr_Stop();
	}
}
#endif

