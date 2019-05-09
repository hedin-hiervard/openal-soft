#include "iosdk/adomatic/android/AdomaticHelper.h"
#include "JniHelper.h"
#include <jni.h>


using namespace iosdk;
using namespace std;
using namespace jnihelper;

AdomaticHelper* AdomaticHelper::sm_instance = nullptr;
const string marketingUrl = "http://adomatic.palmkingdoms.com";



void AdomaticHelper::init(string productId, string secretKey)
{
 JniMethodInfo t;
 if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Adomatic", "init", "(Ljava/lang/String;Ljava/lang/String;)V")) {
  jstring jpid = t.env->NewStringUTF(productId.c_str());
  jstring jsecretKey = t.env->NewStringUTF(secretKey.c_str());
  t.env->CallStaticVoidMethod(t.classID, t.methodID, jpid, jsecretKey);

  t.env->DeleteLocalRef(jpid);
  t.env->DeleteLocalRef(jsecretKey);
  t.env->DeleteLocalRef(t.classID);
}
}

void AdomaticHelper::requestMoreGames(string forOS)
{
 JniMethodInfo t;
 if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Adomatic", "requestMoreGames", "(Ljava/lang/String;)V")) {
  jstring jforos = t.env->NewStringUTF(forOS.c_str());
  t.env->CallStaticVoidMethod(t.classID, t.methodID, jforos);
  t.env->DeleteLocalRef(t.classID);
  t.env->DeleteLocalRef(jforos);
}
}

void AdomaticHelper::requestFullscreenAd(std::string testCountry)
{
  JniMethodInfo t;
  if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Adomatic", "requestFullscreenAd", "(Ljava/lang/String;)V")) {
    jstring jcountry = t.env->NewStringUTF(testCountry.c_str());
    t.env->CallStaticVoidMethod(t.classID, t.methodID, jcountry);

    t.env->DeleteLocalRef(jcountry);
    t.env->DeleteLocalRef(t.classID);
  }
}
