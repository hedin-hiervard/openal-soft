/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.palmkingdoms.pk2_remastered

import android.app.Activity
import android.media.MediaPlayer
import android.net.Uri
import android.opengl.GLSurfaceView
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.View
import java.nio.ByteBuffer

/*
 * There is not much here, but at the bottom, it setups immersive Mode, so that only the app
 * shows, and the use needs to swipe up from the bottom to get the navigation buttons to appear.
 */

class MainActivity : Activity() {

    private lateinit var mGLView: MyGLSurfaceView
    lateinit var textureBuffer: ByteBuffer
    private lateinit var musicHelper: MusicHelper

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        FileAccessor.activity = this;
        native_onStart()

        // Create a GLSurfaceView instance and set it
        // as the ContentView for this Activity
        mGLView = MyGLSurfaceView(this)
        mGLView.activity = this
        setContentView(mGLView)
        musicHelper = MusicHelper(applicationContext)
    }

    override fun onPause() {
        super.onPause()
        // The following call pauses the rendering thread.
        // If your OpenGL application is memory intensive,
        // you should consider de-allocating objects that
        // consume significant memory here.
        mGLView!!.onPause()
    }

    override fun onResume() {
        super.onResume()
        // The following call resumes a paused rendering thread.
        // If you de-allocated graphic objects for onPause()
        // this is a good place to re-allocate them.
        mGLView!!.onResume()
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_FULLSCREEN
                        or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)  //this line is api 19+
            } else {
                window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_FULLSCREEN)
            }
        }
    }

    private fun updateSurface() {
        mGLView.queueEvent(Runnable {
            mGLView.requestRender()
        })
    }

    private fun getSurface(): ByteBuffer {
        return textureBuffer;
    }

    private fun quit() {
        finish()
    }

    private fun log(text: String) {
        Log.i("PK2", text)
    }

//    private fun getFileContent(relPath: String): ByteArray {
//
//    }

    fun isTablet(): Boolean {
        val xdpi = resources.displayMetrics.xdpi
        val ydpi = resources.displayMetrics.ydpi
        val width = resources.displayMetrics.widthPixels
        val height = resources.displayMetrics.heightPixels
        val x = Math.pow((width / xdpi).toDouble(), 2.0)
        val y = Math.pow((height / ydpi).toDouble(), 2.0)
        val screenInches = Math.sqrt(x + y)
            return screenInches >= 8 && width >= 1024 && height >= 768
    }

    fun musicPlay(path: String) {
        val resPath = path.removePrefix("assets/")
        musicHelper.playBackgroundMusic(resPath, true)
    }

    fun musicStop() {
        musicHelper.stopBackgroundMusic()
    }

    fun musicGetPos(): Int {
        return musicHelper.backgroundMusicPosition;
    }

    fun musicSetPos(pos: Int) {
        musicHelper.backgroundMusicPosition = pos
    }

    fun musicSetVolume(vol: Float) {
        musicHelper.backgroundVolume = vol
    }


    external fun native_onMouseDown(x: Int, y: Int, mID: Int, mbID: Int)
    external fun native_onMouseUp(x: Int, y: Int, mID: Int, mbID: Int)
    external fun native_onMouseMove(x: Int, y: Int, mID: Int, mbID: Int)
    external fun native_onSurfaceCreated(width: Int, height: Int)
    external fun native_onStart()
    external fun native_onQuit()

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}