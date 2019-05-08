#ifndef GXLIB_RES_MUSICMANAGER_H_
#define GXLIB_RES_MUSICMANAGER_H_

#ifdef OS_WIN32
#include "audiere.h"
//#pragma common(lib, "audiere.lib")
//using namespace audiere; 
#endif

class iMusicManager
{
public:
	bool Init();
	void Cleanup();
		
	//
	void Play(iStringT& filename, bool restorPos= false);
#if defined( OS_MACOS )
	void Play();
	void Pause();
#endif
	void Stop();
	void SetVolume(uint32 newvol);
	static iMusicManager& Instance()
		{
			static iMusicManager theSingleInstance;
			return theSingleInstance;
		}
private:
	void RecordPos(iStringT& track, uint32 pos);
	uint32 GetRecordedPos(iStringT &track);
private:
	iMusicManager();
	~iMusicManager();

	struct posRec {
		iStringT track;
		uint32 pos;
	};
#ifdef OS_WIN32
	audiere::OutputStreamPtr sound;
	audiere::AudioDevicePtr device;
	float volume;
#endif
	iStringT nowPlaying;
	iSimpleArray<posRec> m_posHist;
#if defined( OS_MACOS )
	uint32 curPos;
	bool  isPause;
#endif
};

#endif //GXLIB_RES_MUSICMANAGER_H_
