#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_palmkingdoms_pk2_1remastered_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++1111";
    return env->NewStringUTF(hello.c_str());
}
