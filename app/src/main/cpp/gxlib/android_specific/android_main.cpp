#include <jni.h>
#include <string>

#include "stdafx.h"
#include <pthread.h>

#define VOID_METHOD(name) JNIEXPORT void JNICALL Java_com_palmkingdoms_pk2_1remastered_MainActivity_native_1##name

uint getSizeNextPOT(uint size);
void pheroes_main();

JavaVM* vm = NULL;
jobject obj = NULL;
jmethodID log_mid;
jmethodID update_surface_mid;

u_int32_t surface_width;
u_int32_t surface_height;

char* _textureBuffer = NULL;
int _textureWidth;
int _textureHeight;

void Android_initSurface() {
    _textureWidth = getSizeNextPOT(surface_width);
    _textureHeight = getSizeNextPOT(surface_height);

    int textureSize = _textureWidth * _textureHeight * 2;
    _textureBuffer = (char*)malloc(textureSize);
    memset(_textureBuffer, 1424, textureSize);
}

void log(const std::string& msg) {
    void *p;
    JNIEnv *env = (JNIEnv*)p;
    vm->AttachCurrentThread(&env, NULL);
    jstring jmsg = env->NewStringUTF(msg.c_str());
    env->CallVoidMethod(obj, log_mid, jmsg);
    env->DeleteLocalRef(jmsg);
}

void updateSurface() {
    void *p;
    vm->GetEnv(&p, JNI_VERSION_1_6);
    JNIEnv *env = (JNIEnv*)p;
    int size = _textureWidth * _textureHeight * 2;
    jbyteArray jdata = env->NewByteArray(size);
    env->SetByteArrayRegion(jdata, 0, size, (jbyte*)_textureBuffer);
    env->CallVoidMethod(obj, update_surface_mid, jdata);
    env->DeleteLocalRef(jdata);
}

u_int32_t GetWindowWidth() {
  return surface_width;
}

u_int32_t GetWindowHeight(){
  return surface_height;
}

void * Android_getSurface() {
    return _textureBuffer;
}

extern "C" {

VOID_METHOD(onSurfaceCreated)(
        JNIEnv *env,
        jobject, /* this */
        jint width,
        jint height
    )
{
    surface_width = width;
    surface_height = height;

    log("surface created: " + std::to_string(width) + "x" + std::to_string(height));
    Android_initSurface();
   // updateSurface();
}

VOID_METHOD(onMouseDown)(
        JNIEnv *env,
        jobject, /* this */
        jint x,
        jint y,
        jint mID,
        jint mbID)
{

}

VOID_METHOD(onMouseUp)(
        JNIEnv *env,
        jobject, /* this */
        jint x,
        jint y,
        jint mID,
        jint mbID)
{

}

VOID_METHOD(onMouseMoved)(
        JNIEnv *env,
        jobject, /* this */
        jint x,
        jint y,
        jint mID,
        jint mbID)
{

}

void* main_thread(void* param) {
    pheroes_main();
    log("main exited");
    return NULL;
}

VOID_METHOD(onStart)(
        JNIEnv *env,
        jobject t
)
{
    env->GetJavaVM(&vm);
    obj = env->NewGlobalRef(t);
    jclass cls = env->GetObjectClass(t);
    log_mid = env->GetMethodID(cls, "log", "(Ljava/lang/String;)V");
    update_surface_mid = env->GetMethodID(cls, "updateSurface", "([B)V");
    log("started, running main loop");
    pthread_t thread;
    pthread_create(&thread, NULL, &main_thread, NULL);
}

VOID_METHOD(onQuit)(
        JNIEnv *env,
        jobject /* this */
)
{

}
}
