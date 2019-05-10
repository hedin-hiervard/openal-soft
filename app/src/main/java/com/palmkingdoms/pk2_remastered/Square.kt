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

import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import java.nio.ShortBuffer

import android.opengl.GLES20
import com.palmkingdoms.pk2_remastered.MyGLRenderer

/**
 * A two-dimensional square for use as a drawn object in OpenGL ES 2.0.
 */
class Square {

    private val vertexShaderCode =
        """
            uniform mat4 uMVPMatrix;

            varying vec2 vTexCoords;

            attribute vec4 aPosition;
            attribute vec4 aTexPosition;

            void main() {
              vTexCoords = aTexPosition.xy;
              gl_Position = uMVPMatrix * aPosition;
            }
        """

    private val fragmentShaderCode =
        """
            precision mediump float;

            uniform vec4 uColor;
            uniform sampler2D uTexture;

            varying vec2 vTexCoords;

            void main() {
                gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
               // gl_FragColor = texture2D(uTexture, vTexCoords);
            }
        """

    private val vertexBuffer: FloatBuffer
    private val texBuffer: FloatBuffer
    private val drawListBuffer: ShortBuffer
    private val mProgram: Int
    private var mPositionHandle: Int = 0
    private var mTexPositionHandle: Int = 0
    private var mColorHandle: Int = 0
    private var mTextureHandle: Int = 0
    private var mMVPMatrixHandle: Int = 0

    private val drawOrder = shortArrayOf(0, 1, 2, 0, 2, 3) // order to draw vertices

    private val vertexStride = COORDS_PER_VERTEX * 4 // 4 bytes per vertex

    internal var color = floatArrayOf(0.2f, 0.709803922f, 0.898039216f, 1.0f)

    /**
     * Sets up the drawing object data for use in an OpenGL ES context.
     */
    init {
        // initialize vertex byte buffer for shape coordinates
        val bb = ByteBuffer.allocateDirect(
            // (# of coordinate values * 4 bytes per float)
            squareCoords.size * 4
        )
        bb.order(ByteOrder.nativeOrder())
        vertexBuffer = bb.asFloatBuffer()
        vertexBuffer.put(squareCoords)
        vertexBuffer.position(0)

        // initialize texture buffer
        val bb2 = ByteBuffer.allocateDirect(
            // (# of coordinate values * 4 bytes per float)
            texCoords.size * 4
        )
        bb2.order(ByteOrder.nativeOrder())
        texBuffer = bb2.asFloatBuffer()
        texBuffer.put(texCoords)
        texBuffer.position(0)

        // initialize byte buffer for the draw list
        val dlb = ByteBuffer.allocateDirect(
            // (# of coordinate values * 2 bytes per short)
            drawOrder.size * 2
        )
        dlb.order(ByteOrder.nativeOrder())
        drawListBuffer = dlb.asShortBuffer()
        drawListBuffer.put(drawOrder)
        drawListBuffer.position(0)

        // prepare shaders and OpenGL program
        val vertexShader = MyGLRenderer.loadShader(
            GLES20.GL_VERTEX_SHADER,
            vertexShaderCode
        )
        val fragmentShader = MyGLRenderer.loadShader(
            GLES20.GL_FRAGMENT_SHADER,
            fragmentShaderCode
        )

        mProgram = GLES20.glCreateProgram()             // create empty OpenGL Program
        GLES20.glAttachShader(mProgram, vertexShader)   // add the vertex shader to program
        GLES20.glAttachShader(mProgram, fragmentShader) // add the fragment shader to program
        GLES20.glLinkProgram(mProgram)                  // create OpenGL program executables
    }

    /**
     * Encapsulates the OpenGL ES instructions for drawing this shape.
     *
     * @param mvpMatrix - The Model View Project matrix in which to draw
     * this shape.
     */
    fun draw(mvpMatrix: FloatArray, texture: Int) {
        // Add program to OpenGL environment
        GLES20.glUseProgram(mProgram)

        // POSITION
        // get handle to vertex shader's vPosition member
        mPositionHandle = GLES20.glGetAttribLocation(mProgram, "aPosition")
        GLES20.glEnableVertexAttribArray(mPositionHandle)
        GLES20.glVertexAttribPointer(
            mPositionHandle, COORDS_PER_VERTEX,
            GLES20.GL_FLOAT, false,
            vertexStride, vertexBuffer
        )

        // TEX POSITION
        mTexPositionHandle = GLES20.glGetAttribLocation(mProgram, "aTexPosition")
        GLES20.glEnableVertexAttribArray(mTexPositionHandle)
        GLES20.glVertexAttribPointer(
            mTexPositionHandle, COORDS_PER_VERTEX,
            GLES20.GL_FLOAT, false,
            vertexStride, texBuffer
        )

        // COLOR
        mColorHandle = GLES20.glGetUniformLocation(mProgram, "uColor")
        GLES20.glUniform4fv(mColorHandle, 1, color, 0)

        // TEXTURE
        mTextureHandle = GLES20.glGetUniformLocation(mProgram, "uTexture")
        GLES20.glUniform1i(mTextureHandle, texture)

        // get handle to shape's transformation matrix
        mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix")
        MyGLRenderer.checkGlError("glGetUniformLocation")

        // Apply the projection and view transformation
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0)
        MyGLRenderer.checkGlError("glUniformMatrix4fv")

        // Draw the square
        GLES20.glDrawElements(
            GLES20.GL_TRIANGLES, drawOrder.size,
            GLES20.GL_UNSIGNED_SHORT, drawListBuffer
        )

        // Disable vertex array
        GLES20.glDisableVertexAttribArray(mPositionHandle)
    }

    companion object {

        // number of coordinates per vertex in this array
        internal val COORDS_PER_VERTEX = 3
        internal var squareCoords = floatArrayOf(
            -0.5f, 0.5f, 0.0f, // top left
            -0.5f, -0.5f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f, // bottom right
            0.5f, 0.5f, 0.0f
        ) // top right

        internal var texCoords = floatArrayOf(
            0.0f, 0.0f, 0.0f, // top left
            0.0f, 1.0f, 0.0f, // bottom left
            1.0f, 1.0f, 0.0f, // bottom right
            1.0f, 0.0f, 0.0f
        ) // top right
    }

}