#include "gxl.inc.h"
#include "gxl.audio.h"

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AudioToolbox/AudioToolbox.h>

//////////////////////////////////////////////////////////////////////////
const sint32 DEF_STREAM_VOLUME = 256;

iSoundPlayer::iStream::iStream() 
: m_state(Available), m_pBuff(NULL), m_bufLen(0), m_smpLen(0), m_smpPos(0), m_volume(DEF_STREAM_VOLUME), m_newVol(DEF_STREAM_VOLUME), m_bNeedStop(false)
{

}

iSoundPlayer::iStream::~iStream() 
{
	Cleanup();
}

void iSoundPlayer::iStream::Cleanup()
{
	if (m_pBuff) {
		free(m_pBuff);
		m_bufLen = m_smpLen = m_smpPos = 0;
	}

	m_state = Available;
}

void iSoundPlayer::iStream::Reset()
{
	m_smpLen = m_smpPos = 0;
	m_state = Available;
}

bool iSoundPlayer::iStream::PushSample(sint8* pSample, uint32 smpLen, uint32 flags)
{
	check(m_state == Available);
	check(smpLen);
	if (m_bufLen < smpLen) {
		Cleanup();
		m_pBuff = (sint8*)malloc(smpLen);
		m_bufLen = smpLen;
	}

	memcpy(m_pBuff, pSample, smpLen);
	m_smpPos = 0;
	m_smpLen = smpLen;
	m_flags = flags;
	m_state = Busy;
	m_volume = m_newVol = DEF_STREAM_VOLUME;
	m_bNeedStop = false;

	return true;
}

void iSoundPlayer::iStream::Stop()
{
	m_bNeedStop = true;
	m_newVol = 0;
}

uint32 iSoundPlayer::iStream::FillBuffer(sint32* pProcBuff, uint32 bufSamples)
{
	check(m_pBuff);

	sint8* src = m_pBuff + m_smpPos;
	sint32* dst = pProcBuff;

	uint32 copied = 0;
	while (1) {
		uint32 toCopy = iMIN<uint32>(bufSamples-copied, m_smpLen - m_smpPos);
		sint32 volDif = m_newVol - m_volume;
		uint32 toFade = iMIN<uint32>(iABS(volDif), toCopy);
		sint32 fadeStep = iSIGN(volDif);

		uint32 bb = 0;
		for (; bb<toFade; ++bb) {
			m_volume += fadeStep;
			*dst += *src * m_volume;
			dst++;
			src++;

		}
		for (; bb<toCopy; ++bb) {
			*dst += *src * m_volume;
			dst++;
			src++;
		}

		m_smpPos += toCopy;
		copied += toCopy;

		if (m_flags & PlayLoop) {
			if (m_bNeedStop && m_volume == 0) {
				Reset();
				break;
			} else if (copied < bufSamples) {
				src = m_pBuff;
				m_smpPos = 0;
			} else break;
		} else {
			if (m_smpPos == m_smpLen) {
				Reset();
			}
			break;
		}
	}
	return copied;
}

void iSoundPlayer::iStream::SetVolume(sint32 volume)
{
	m_newVol = volume;
}

//////////////////////////////////////////////////////////////////////////
const uint32 STREAM_COUNT	= 8;	// streams count
const uint32 BUFFER_COUNT	= 3;	// buffers count
const uint32 SAMPLE_SIZE	= 2;	// size of sample in bytes
const uint32 BUFFER_LENGTH	= 2000;	// length of buffer in samples

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
DWORD iSoundPlayer::MixerThread(LPVOID pData)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

iSoundPlayer::iSoundPlayer(): mDevice(NULL), mContext(NULL), m_volume(1.0f)
{
}

iSoundPlayer::~iSoundPlayer()
{
	Close();
}

bool iSoundPlayer::HasAudioDev()
{
	return true;
}

bool iSoundPlayer::Init()
{
	Close();
	mDevice = alcOpenDevice(NULL); // select the "preferred device"
	if (mDevice) {
		// use the device to make a context
		mContext=alcCreateContext(mDevice,NULL);
		// set my context to the currently active one
		alcMakeContextCurrent(mContext);
		return true;
	}
	return false;
}

void iSoundPlayer::Close()
{
	if(mContext) 	alcDestroyContext(mContext);
	if(mDevice)		alcCloseDevice(mDevice);
}

void iSoundPlayer::SetVolume(sint32 volume)
{
	ALfloat newvol = (float)volume / 100;
	//ALfloat coef = m_volume ? (newvol / m_volume) : 0; 
	m_volume = newvol;
	for(uint32 x = 0; x < sourcePool.GetSize(); x++)
	{
		/*ALfloat cur;
		alGetSourcef(sourcePool[x], AL_GAIN, &cur);
		
		cur *= coef;
		 */
		alSourcef(sourcePool[x], AL_GAIN, m_volume);		
	}
}

sint32 iSoundPlayer::GetStreamVolume(sint32 streamId)
{
	ALfloat res;
	alGetSourcef(streamId, AL_GAIN, &res);
	return (sint32)(res * 100);
}

void iSoundPlayer::SetStreamVolume(sint32 streamId, sint32 volume)
{
	alSourcef(streamId, AL_GAIN, (float)(volume) / 100.0f * m_volume);
}

bool iSoundPlayer::IsBusyStream(sint32 streamId) const
{
	ALint res;
	alGetSourcei(streamId, AL_SOURCE_STATE, &res);
	return res != AL_PLAYING;
}

uint32 iSoundPlayer::LoadSample(sint8* pSample, uint32 smpLen, uint32 flags)
{
	uint32 bufferID;
	ALenum  format = AL_FORMAT_MONO8;
	ALsizei size = smpLen;
	ALsizei freq = 22050;
	// grab a buffer ID from openAL
	alGenBuffers(1, &bufferID);

	//signed -> unsigned conversion (for OpenAL)
	uint8* unsbuff = (uint8*)malloc(smpLen);
	for(uint32 x=0; x<smpLen; x++)
		unsbuff[x] = (uint8)(pSample[x] + 128);
	// load the awaiting data blob into the openAL buffer.
    alBufferData(bufferID,format,unsbuff,size,freq); 
	free(unsbuff);
	return bufferID;
}

uint32 iSoundPlayer::AcquireFreeSource()
{
	for(uint32 x=0; x<sourcePool.GetSize(); x++) {
		ALint st;
		alGetSourcei(sourcePool[x], AL_SOURCE_STATE, &st);
		if(st == AL_STOPPED)
			return sourcePool[x];
	}
	ALuint n;
	alGenSources(1, &n);
	alSourcef(n, AL_PITCH, 1.0f);
	alSourcef(n, AL_GAIN, 1.0f);
	sourcePool.Add(n);
	return n;
}

uint32 iSoundPlayer::PlaySample(uint32 sid, uint32 flags)
{
	
	uint32 res = AcquireFreeSource();
	
	// attach the buffer to the source
	alSourcei(res, AL_BUFFER, sid);
	alSourcei (res, AL_LOOPING,  flags);
	alSourcef(res, AL_GAIN, m_volume);		
	alSourcePlay(res);
	return res;
}

uint32 iSoundPlayer::ReplaceSample(uint32 streamId, uint32 sampleId)
{
	alSourceStop(streamId);
	alSourcei(streamId, AL_BUFFER, sampleId);
	alSourcePlay(streamId);
	return streamId;
}


void iSoundPlayer::Stop(uint32 sid)
{
	alSourceStop(sid);
}
