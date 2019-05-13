#include <jni.h>
#include <string>

#include "stdafx.h"
#include <pthread.h>

#define VOID_METHOD(name) JNIEXPORT void JNICALL Java_com_palmkingdoms_pk2_1remastered_MainActivity_native_1##name

uint getSizeNextPOT(uint size);
int preload_resources();
void pheroes_main();

JavaVM* JVM = NULL;
jobject activity_obj = NULL;
u_int32_t surface_width;
u_int32_t surface_height;
pthread_mutex_t buffer_mutex;

jmethodID log_java_method_id;
jmethodID quit_java_method_id;
jmethodID update_surface_java_method_id;
jmethodID get_surface_java_method_id;
jmethodID get_apk_path_method_id;
jmethodID get_cache_dir_method_id;
jmethodID get_library_dir_method_id;
jmethodID get_documents_dir_method_id;
jmethodID get_log_dir_method_id;
jclass class_fa;

char* _textureBuffer = NULL;
int _textureWidth;
int _textureHeight;
bool launched = false;


int Android_textureWidth() { return _textureWidth; }
int Android_textureHeight() { return _textureHeight; }
void Android_surfaceUnlock();
void Android_surfaceLock();
void log(const std::string& msg);
void onMouseDown(int x, int y, MouseId mID, MouseButtonId mbID);
void onMouseMove(int x, int y, MouseId mID, MouseButtonId mbID);
void onMouseUp(int x, int y, MouseId mID, MouseButtonId mbID);

JNIEnv* getJNIEnv() {
    JNIEnv *env;
    JVM->AttachCurrentThread(&env, NULL);
    return env;
}

void* main_thread(void* param) {
    preload_resources();
    pheroes_main();
    log("main exited");
    JNIEnv* env = getJNIEnv();
    env->CallVoidMethod(activity_obj, quit_java_method_id);
    return NULL;
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
    pthread_mutex_lock(&buffer_mutex);

    _textureWidth = getSizeNextPOT(surface_width);
    _textureHeight = getSizeNextPOT(surface_height);

    int textureSize = _textureWidth * _textureHeight * 2;
    _textureBuffer = (char*)malloc(textureSize);
    memset(_textureBuffer, 5600, textureSize);

    pthread_mutex_unlock(&buffer_mutex);
}

void log(const std::string& msg) {
    JNIEnv* env = getJNIEnv();
    jstring jmsg = env->NewStringUTF(msg.c_str());
    env->CallVoidMethod(activity_obj, log_java_method_id, jmsg);
    env->DeleteLocalRef(jmsg);
}

void Android_surfaceLock() {
    pthread_mutex_lock(&buffer_mutex);
}

void Android_surfaceUnlock() {
    pthread_mutex_unlock(&buffer_mutex);
}

void Android_updateSurface() {

    JNIEnv* env = getJNIEnv();
    jobject buf =  env->CallObjectMethod(activity_obj, get_surface_java_method_id);
    char* ptr = (char*)env->GetDirectBufferAddress(buf);
    int textureSize = _textureWidth * _textureHeight * 2;

    memcpy(ptr, _textureBuffer, textureSize);
    env->DeleteLocalRef(buf);


    env->CallVoidMethod(activity_obj, update_surface_java_method_id);
}

char* Android_getSurface() {
  return _textureBuffer;
}

// void Android_updateScreenRect(unsigned short* screen, int x1, int y1, int x2, int y2) {
//   int y;
//   for (y = y1; y < y2; ++y) {
//     memcpy(&_textureBuffer[(y * _textureWidth + x1 )* 2], &screen[y * _width + x1], (x2 - x1) * 2);
//   }
//   updateSurface();
// }


u_int32_t GetWindowWidth() {
  return surface_width;
}

u_int32_t GetWindowHeight(){
  return surface_height;
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

    if(!launched) {
        pthread_t thread;
        pthread_create(&thread, NULL, &main_thread, NULL);
        launched = true;
    }
}

VOID_METHOD(onMouseDown)(
        JNIEnv *env,
        jobject, /* this */
        jint x,
        jint y,
        jint mID,
        jint mbID)
{
  onMouseDown(x, y, mID, mbID);
}

VOID_METHOD(onMouseUp)(
        JNIEnv *env,
        jobject, /* this */
        jint x,
        jint y,
        jint mID,
        jint mbID)
{
  onMouseUp(x, y, mID, mbID);
}

VOID_METHOD(onMouseMove)(
        JNIEnv *env,
        jobject, /* this */
        jint x,
        jint y,
        jint mID,
        jint mbID)
{
  onMouseMove(x, y, mID, mbID);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JVM = vm;
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);
    pthread_mutex_init(&buffer_mutex, NULL);

    return JNI_VERSION_1_6;
}

VOID_METHOD(onStart)(
        JNIEnv *env,
        jobject t
) {
    activity_obj = env->NewGlobalRef(t);
    jclass cls = env->GetObjectClass(t);
    log_java_method_id = env->GetMethodID(cls, "log", "(Ljava/lang/String;)V");
    quit_java_method_id = env->GetMethodID(cls, "quit", "()V");
    update_surface_java_method_id = env->GetMethodID(cls, "updateSurface", "()V");
    get_surface_java_method_id = env->GetMethodID(cls, "getSurface", "()Ljava/nio/ByteBuffer;");

    class_fa = env->FindClass("com/palmkingdoms/pk2_remastered/FileAccessor");
    class_fa = (jclass)env->NewGlobalRef(class_fa);

    get_apk_path_method_id = env->GetStaticMethodID(class_fa, "getApkPath", "()Ljava/lang/String;");
    get_library_dir_method_id = env->GetStaticMethodID(class_fa, "getLibraryDirectory", "()Ljava/lang/String;");
    get_cache_dir_method_id = env->GetStaticMethodID(class_fa, "getCacheDirectory", "()Ljava/lang/String;");
    get_documents_dir_method_id = env->GetStaticMethodID(class_fa, "getDocumentsDirectory", "()Ljava/lang/String;");
    get_log_dir_method_id = env->GetStaticMethodID(class_fa, "getLogDirectory", "()Ljava/lang/String;");

}

VOID_METHOD(onQuit)(
        JNIEnv *env,
        jobject /* this */
) {}

}
