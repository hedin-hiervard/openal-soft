#ifndef GXLIB_GXL_AUDIO_H_
#define GXLIB_GXL_AUDIO_H_

#ifdef OS_WIN32
#include "mmsystem.h"
//#include <AudioToolbox/AudioToolbox.h>
#else

#include "AL/al.h"
#include "AL/alc.h"
#include <gxl.common.def.h>
#include <gxl.ctr.array.h>

#endif

class iSoundPlayer
{
public:
	// Playback flags
	enum PlayFlags {
		PlayLoop	= 0x01,
	};

	// Stream
	class iStream
	{
	public:
		friend class iSoundPlayer;

		// Stream state
		enum State {
			Available = 0,
			Busy
		};

	public:
		iStream();
		~iStream();
		bool PushSample(sint8* pSample, uint32 smpLen, uint32 flags);
		void Stop();

	protected:
		void Reset();
		void Cleanup();
		uint32 FillBuffer(sint32* pProcBuff, uint32 bufSamples);
		sint32 GetVolume() const;
		void SetVolume(sint32 volume);

	protected:
		bool		m_bNeedStop;
		uint32		m_flags;
		uint32		m_bufLen;
		uint32		m_smpLen;
		uint32		m_smpPos;
		State		m_state;
		sint32		m_volume;
		sint32		m_newVol;
		sint8*		m_pBuff;
	};

public:
	friend class iGXApp;

	// c-tor, d-tor
	iSoundPlayer();
	~iSoundPlayer();

	//
	static bool HasAudioDev();

	// Init/close
	bool Init();
	void Close();
#ifdef OS_WIN32
	bool Inited() const { return m_hWaveOut != NULL; }
#else
	bool Inited() const { return true; }

#endif
	bool IsBusyStream(sint32 streamId) const;

	// Volume
	sint32 GetVolume() const;
	void SetVolume(sint32 volume);
	sint32 GetStreamVolume(sint32 streamId);
	void SetStreamVolume(sint32 streamId, sint32 volume);

	// Playback
	uint32 PlaySample(uint32 sid, uint32 flags = 0);
	uint32 ReplaceSample(uint32 streamId, uint32 sid);
	uint32 LoadSample(sint8* pSample, uint32 smpLen, uint32 flags = 0);
	void Stop(uint32 streamId);


protected:

	// Mixer thread
#ifdef OS_WIN32
	static DWORD __stdcall MixerThread(LPVOID pData);
#else
	static DWORD  MixerThread(LPVOID pData);

#endif
private:
	uint32		m_nextBuffer;
#ifdef OS_WIN32
	typedef struct {
		sint8* data;
		uint32 smpLen;
	} sampleInfo;

	sint32		m_volume;
	WAVEHDR*	m_buffers;
	sint16*		m_samples;
	HWAVEOUT	m_hWaveOut;
	iStream*	m_streams;
	HANDLE		m_hWorkThread;
	DWORD		m_idWorkThread;
	CRITICAL_SECTION 	m_csGuard;
	iSimpleArray<sampleInfo> m_sampleLibrary;
#else
 	ALCcontext* mContext; // stores the context (the 'air')
 	ALCdevice* mDevice; // stores the device
 	iSimpleArray<uint32> sourcePool;
 	ALfloat		m_volume;
 	//NSMutableArray * bufferStorageArray; // stores the buffer ids from openAL
 	//NSMutableDictionary * soundDictionary; // stores our soundkeys
	uint32 AcquireFreeSource();
#endif

};


#endif //GXLIB_GXL_AUDIO_H_
