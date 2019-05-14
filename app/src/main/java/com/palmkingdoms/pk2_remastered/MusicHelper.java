package com.palmkingdoms.pk2_remastered;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.MediaPlayer;
import android.util.Log;

public class MusicHelper {
  // ===========================================================
  // Fields
  // ===========================================================

  private final Context mContext;
  private MediaPlayer mBackgroundMediaPlayer;
  private float mLeftVolume;
  private float mRightVolume;
  private boolean mPaused;
  private String mCurrentPath;

  // ===========================================================
  // Constructors
  // ===========================================================

  public MusicHelper(final Context pContext) {
    this.mContext = pContext;

    this.initData();
  }

  // ===========================================================
  // Getter & Setter
  // ===========================================================

  // ===========================================================
  // Methods for/from SuperClass/Interfaces
  // ===========================================================

  // ===========================================================
  // Methods
  // ===========================================================


  public void preloadBackgroundMusic(final String pPath) {
    if ((this.mCurrentPath == null) || (!this.mCurrentPath.equals(pPath))) {
      // preload new background music

      // release old resource and create a new one
      if (this.mBackgroundMediaPlayer != null) {
        this.mBackgroundMediaPlayer.release();
      }

      this.mBackgroundMediaPlayer = this.createMediaplayerFromAssets(pPath);

      // record the path
      this.mCurrentPath = pPath;
    }
  }

  public void playBackgroundMusic(final String pPath, final boolean isLoop) {
    if (this.mCurrentPath == null) {
      // it is the first time to play background music or end() was called
      this.mBackgroundMediaPlayer = this.createMediaplayerFromAssets(pPath);
      this.mCurrentPath = pPath;
    } else {
      if (!this.mCurrentPath.equals(pPath)) {
        // play new background music

        // release old resource and create a new one
        if (this.mBackgroundMediaPlayer != null) {
          this.mBackgroundMediaPlayer.release();
        }
        this.mBackgroundMediaPlayer = this.createMediaplayerFromAssets(pPath);

        // record the path
        this.mCurrentPath = pPath;
      }
    }

    if (this.mBackgroundMediaPlayer == null) {
    } else {
      // if the music is playing or paused, stop it
      this.mBackgroundMediaPlayer.stop();

      this.mBackgroundMediaPlayer.setLooping(isLoop);

      try {
        this.mBackgroundMediaPlayer.prepare();
        this.mBackgroundMediaPlayer.seekTo(0);
        this.mBackgroundMediaPlayer.start();

        this.mPaused = false;
      } catch (final Exception e) {

      }
    }
  }

  public void stopBackgroundMusic() {
    if (this.mBackgroundMediaPlayer != null) {
      this.mBackgroundMediaPlayer.stop();

      // should set the state, if not, the following sequence will be error
      // play -> pause -> stop -> resume
      this.mPaused = false;
    }
  }

  public void pauseBackgroundMusic() {
    if (this.mBackgroundMediaPlayer != null && this.mBackgroundMediaPlayer.isPlaying()) {
      this.mBackgroundMediaPlayer.pause();
      this.mPaused = true;
    }
  }

  public void resumeBackgroundMusic() {
    if (this.mBackgroundMediaPlayer != null && this.mPaused) {
      this.mBackgroundMediaPlayer.start();
      this.mPaused = false;
    }
  }

  public void rewindBackgroundMusic() {
    if (this.mBackgroundMediaPlayer != null) {
      this.mBackgroundMediaPlayer.stop();

      try {
        this.mBackgroundMediaPlayer.prepare();
        this.mBackgroundMediaPlayer.seekTo(0);
        this.mBackgroundMediaPlayer.start();

        this.mPaused = false;
      } catch (final Exception e) {

      }
    }
  }

  public boolean isBackgroundMusicPlaying() {
    boolean ret = false;

    if (this.mBackgroundMediaPlayer == null) {
      ret = false;
    } else {
      ret = this.mBackgroundMediaPlayer.isPlaying();
    }

    return ret;
  }

  public void end() {
    if (this.mBackgroundMediaPlayer != null) {
      this.mBackgroundMediaPlayer.release();
    }

    this.initData();
  }

  public int getBackgroundMusicPosition() {
    if(this.mBackgroundMediaPlayer == null)
      return 0;
    return this.mBackgroundMediaPlayer.getCurrentPosition();
  }

  public void setBackgroundMusicPosition(int position) {
    if(this.mBackgroundMediaPlayer == null)
      return;
    this.mBackgroundMediaPlayer.seekTo(position);
  }

  public float getBackgroundVolume() {
    if (this.mBackgroundMediaPlayer != null) {
      return (this.mLeftVolume + this.mRightVolume) / 2;
    } else {
      return 0.0f;
    }
  }

  public void setBackgroundVolume(float pVolume) {
    if (pVolume < 0.0f) {
      pVolume = 0.0f;
    }

    if (pVolume > 1.0f) {
      pVolume = 1.0f;
    }

    this.mLeftVolume = this.mRightVolume = pVolume;
    if (this.mBackgroundMediaPlayer != null) {
      this.mBackgroundMediaPlayer.setVolume(this.mLeftVolume, this.mRightVolume);
    }
  }

  private void initData() {
    this.mLeftVolume = 0.5f;
    this.mRightVolume = 0.5f;
    this.mBackgroundMediaPlayer = null;
    this.mPaused = false;
    this.mCurrentPath = null;
  }

  /**
   * create mediaplayer for music
   *
   * @param pPath
   *            the pPath relative to assets
   * @return
   */
  private MediaPlayer createMediaplayerFromAssets(final String pPath) {
    MediaPlayer mediaPlayer = new MediaPlayer();

    try {
      if (pPath.startsWith("/")) {
        mediaPlayer.setDataSource(pPath);
      } else {
        final AssetFileDescriptor assetFileDescritor = this.mContext.getAssets().openFd(pPath);
        mediaPlayer.setDataSource(assetFileDescritor.getFileDescriptor(), assetFileDescritor.getStartOffset(), assetFileDescritor.getLength());
      }

      mediaPlayer.prepare();

      mediaPlayer.setVolume(this.mLeftVolume, this.mRightVolume);
    } catch (final Exception e) {
      mediaPlayer = null;

    }

    return mediaPlayer;
  }

  // ===========================================================
  // Inner and Anonymous Classes
  // ===========================================================
}
