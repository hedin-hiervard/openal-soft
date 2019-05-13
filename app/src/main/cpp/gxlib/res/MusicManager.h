#ifndef GXLIB_RES_MUSICMANAGER_H_
#define GXLIB_RES_MUSICMANAGER_H_

#include "FileAccessor/FileAccessor.h"

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
	void Play(const fileaccessor::RelativeFilePath& path, bool restorPos= false);
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
	void RecordPos(const fileaccessor::RelativeFilePath& path, uint32 pos);
	uint32 GetRecordedPos(const fileaccessor::RelativeFilePath& path);
private:
	iMusicManager();
	~iMusicManager();

	struct posRec {
		const fileaccessor::RelativeFilePath& path;
		uint32 pos;
	};
#ifdef OS_WIN32
	audiere::OutputStreamPtr sound;
	audiere::AudioDevicePtr device;
	float volume;
#endif
	const fileaccessor::RelativeFilePath nowPlaying;
	iSimpleArray<posRec> m_posHist;
#if defined( OS_MACOS )
	uint32 curPos;
	bool  isPause;
#endif
};

#endif //GXLIB_RES_MUSICMANAGER_H_
