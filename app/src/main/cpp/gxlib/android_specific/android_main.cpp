#include <jni.h>
#include <string>

#include "stdafx.h"
#include <pthread.h>

#define VOID_METHOD(name) JNIEXPORT void JNICALL Java_com_palmkingdoms_pk2_1remastered_MainActivity_native_1##name

uint getSizeNextPOT(uint size);
void pheroes_main();

JavaVM* JVM = NULL;
jobject activity_obj = NULL;
u_int32_t surface_width;
u_int32_t surface_height;

jmethodID log_java_method_id;
jmethodID get_apk_path_method_id;
jmethodID get_cache_dir_method_id;
jmethodID get_library_dir_method_id;
jmethodID get_documents_dir_method_id;
jmethodID get_log_dir_method_id;
jclass class_fa;

char* _textureBuffer = NULL;
int _textureWidth;
int _textureHeight;


JNIEnv* getJNIEnv() {
    JNIEnv *env;
    JVM->AttachCurrentThread(&env, NULL);
    return env;
}

std::string jstring2string(jstring jstr)
{
    if (jstr == NULL)
    {
        return "";
    }

    JNIEnv *env = getJNIEnv();

    const char* chars = env->GetStringUTFChars(jstr, NULL);
    std::string ret(chars);
    env->ReleaseStringUTFChars(jstr, chars);

    return ret;
}

void Android_initSurface() {
    _textureWidth = getSizeNextPOT(surface_width);
    _textureHeight = getSizeNextPOT(surface_height);

    int textureSize = _textureWidth * _textureHeight * 2;
    _textureBuffer = (char*)malloc(textureSize);
    memset(_textureBuffer, 1424, textureSize);
}

void log(const std::string& msg) {
    JNIEnv* env = getJNIEnv();
    jstring jmsg = env->NewStringUTF(msg.c_str());
    env->CallVoidMethod(activity_obj, log_java_method_id, jmsg);
    env->DeleteLocalRef(jmsg);
}

void updateSurface() {
    // void *p;
    // vm->GetEnv(&p, JNI_VERSION_1_6);
    // JNIEnv *env = (JNIEnv*)p;
    // int size = _textureWidth * _textureHeight * 2;
    // jbyteArray jdata = env->NewByteArray(size);
    // env->SetByteArrayRegion(jdata, 0, size, (jbyte*)_textureBuffer);
    // env->CallVoidMethod(obj, update_surface_mid, jdata);
    // env->DeleteLocalRef(jdata);
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

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JVM = vm;
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);

    return JNI_VERSION_1_6;
}

VOID_METHOD(onStart)(
        JNIEnv *env,
        jobject t
) {
    activity_obj = env->NewGlobalRef(t);
    jclass cls = env->GetObjectClass(t);
    log_java_method_id = env->GetMethodID(cls, "log", "(Ljava/lang/String;)V");

    class_fa = env->FindClass("com/palmkingdoms/pk2_remastered/FileAccessor");
    class_fa = (jclass)env->NewGlobalRef(class_fa);

    get_apk_path_method_id = env->GetStaticMethodID(class_fa, "getApkPath", "()Ljava/lang/String;");
    get_library_dir_method_id = env->GetStaticMethodID(class_fa, "getLibraryDirectory", "()Ljava/lang/String;");
    get_cache_dir_method_id = env->GetStaticMethodID(class_fa, "getCacheDirectory", "()Ljava/lang/String;");
    get_documents_dir_method_id = env->GetStaticMethodID(class_fa, "getDocumentsDirectory", "()Ljava/lang/String;");
    get_log_dir_method_id = env->GetStaticMethodID(class_fa, "getLogDirectory", "()Ljava/lang/String;");

    log("started, running main loop");

    pthread_t thread;
    pthread_create(&thread, NULL, &main_thread, NULL);
}

VOID_METHOD(onQuit)(
        JNIEnv *env,
        jobject /* this */
) {}

}
