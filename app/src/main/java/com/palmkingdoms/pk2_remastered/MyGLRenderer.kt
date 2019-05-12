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

import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.opengl.Matrix
import android.util.Log
import com.palmkingdoms.pk2_remastered.Square
import java.nio.ByteBuffer
import java.nio.IntBuffer

/**
 * Provides drawing instructions for a GLSurfaceView object. This class
 * must override the OpenGL ES drawing lifecycle methods:
 *
 *  * [android.opengl.GLSurfaceView.Renderer.onSurfaceCreated]
 *  * [android.opengl.GLSurfaceView.Renderer.onDrawFrame]
 *  * [android.opengl.GLSurfaceView.Renderer.onSurfaceChanged]
 *
 */
class MyGLRenderer : GLSurfaceView.Renderer {
    public var activity: MainActivity? = null
    private var mSquare: Square? = null

    // mMVPMatrix is an abbreviation for "Model View Projection Matrix"
    private val mMVPMatrix = FloatArray(16)
    private val mProjectionMatrix = FloatArray(16)
    private val mViewMatrix = FloatArray(16)
    private val mRotationMatrix = FloatArray(16)
    private var mainTexture: Int = -1
    private var textureWidth: Int = 0
    private var textureHeight: Int = 0

    /**
     * Returns the rotation angle of the triangle shape (mTriangle).
     *
     * @return - A float representing the rotation angle.
     */
    /**
     * Sets the rotation angle of the triangle shape (mTriangle).
     */
    var angle: Float = 0.toFloat()

    fun createTexture(surfaceWidth: Int, surfaceHeight: Int) {
        textureWidth = Math.max(1, Integer.highestOneBit(surfaceWidth - 1) shl 1)
        textureHeight = Math.max(1, Integer.highestOneBit(surfaceHeight - 1) shl 1)

        Log.i("PK2", "texture size: %d x %d".format(textureWidth, textureHeight))

        if(mainTexture == -1) {
            val textures = IntBuffer.allocate(1)
            GLES20.glGenTextures(1, textures)
            mainTexture = textures[0]
        }
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mainTexture)

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT)

        GLES20.glEnable(GLES20.GL_TEXTURE_2D)
        activity?.textureBuffer = ByteBuffer.allocateDirect(textureHeight * textureWidth * 2)

        activity?.textureBuffer?.position(0)
        for(i in 0 .. textureHeight * textureWidth * 2 - 1) {
            activity?.textureBuffer?.put(120.toByte())
        }
    }

    fun updateTexture() {


        activity?.textureBuffer?.position(0)

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mainTexture)
        GLES20.glTexImage2D(
            GLES20.GL_TEXTURE_2D,
            0,
            GLES20.GL_RGB,
            textureWidth,
            textureHeight,
            0,
            GLES20.GL_RGB,
            GLES20.GL_UNSIGNED_SHORT_5_6_5,
            activity?.textureBuffer
        )
    }

    override fun onSurfaceCreated(unused: GL10, config: EGLConfig) {
        // Set the background frame color
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f)
    }

    override fun onDrawFrame(unused: GL10) {
        val scratch = FloatArray(16)

        // Draw background color
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT)

        // Set the camera position (View matrix)
//        Matrix.setLookAtM(mViewMatrix, 0, 0f, 0f, -3f, 0f, 0f, 0f, 0f, 1.0f, 0.0f)
        Matrix.setIdentityM(mViewMatrix, 0)

        // Calculate the projection and view transformation
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mViewMatrix, 0)

        // Draw square
        mSquare!!.draw(mMVPMatrix, mainTexture)
    }

    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int) {
        // Adjust the viewport based on geometry changes,
        // such as screen rotation
        GLES20.glViewport(0, 0, width, height)

        val ratio = width.toFloat() / height

        // this projection matrix is applied to object coordinates
        // in the onDrawFrame() method
//        Matrix.frustumM(mProjectionMatrix, 0, -ratio, ratio, -1f, 1f, 3f, 7f)
        Matrix.orthoM(
            mProjectionMatrix,
            0,
            0.0f,
            width.toFloat(),
            height.toFloat(),
            0.0f,
            -1.0f,
            1.0f
        )

        createTexture(width, height)
        mSquare = Square(
            width.toFloat(),
            height.toFloat(),
            width.toFloat() / textureWidth,
            height.toFloat() / textureHeight
        )
        activity?.native_onSurfaceCreated(width, height);
    }

    companion object {

        private val TAG = "MyGLRenderer"

        /**
         * Utility method for compiling a OpenGL shader.
         *
         *
         * **Note:** When developing shaders, use the checkGlError()
         * method to debug shader coding errors.
         *
         * @param type - Vertex or fragment shader type.
         * @param shaderCode - String containing the shader code.
         * @return - Returns an id for the shader.
         */
        fun loadShader(type: Int, shaderCode: String): Int {

            // create a vertex shader type (GLES20.GL_VERTEX_SHADER)
            // or a fragment shader type (GLES20.GL_FRAGMENT_SHADER)
            val shader = GLES20.glCreateShader(type)

            // add the source code to the shader and compile it
            GLES20.glShaderSource(shader, shaderCode)
            GLES20.glCompileShader(shader)

            return shader
        }

        /**
         * Utility method for debugging OpenGL calls. Provide the name of the call
         * just after making it:
         *
         * <pre>
         * mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
         * MyGLRenderer.checkGlError("glGetUniformLocation");</pre>
         *
         * If the operation is not successful, the check throws an error.
         *
         * @param glOperation - Name of the OpenGL call to check.
         */
        fun checkGlError(glOperation: String) {

        }
    }

}