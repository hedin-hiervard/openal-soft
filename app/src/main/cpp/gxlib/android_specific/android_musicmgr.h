#ifndef GXLIB_android_SPECIFIC_android_MUSICMGR_H_
#define GXLIB_android_SPECIFIC_android_MUSICMGR_H_

bool androidMusicMgr_Init();
void androidMusicMgr_Play(const char *filename);
void androidMusicMgr_Stop();
void androidMusicMgr_SetVolume(double newvol);
void androidMusicMgr_Deinit();
unsigned int androidMusicMgr_GetCurPos();
void androidMusicMgr_SetCurPos(unsigned int pos);
#endif //GXLIB_android_SPECIFIC_android_MUSICMGR_H_
