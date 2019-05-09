#include "iosdk/analytics/Analytics.h"
#include <jni.h>

namespace iosdk
{
  Analytics* Analytics::sm_sharedInstance = nullptr;
  void Analytics::Init(const char* trackerID)
  {
    // JniMethodInfo t;
    // if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Analytics", "Init", "(Ljava/lang/String;)V")) {
    //   jstring jid = t.env->NewStringUTF(trackerID);
    //   t.env->CallStaticVoidMethod(t.classID, t.methodID, jid);

    //   t.env->DeleteLocalRef(jid);
    //   t.env->DeleteLocalRef(t.classID);
    // }
  }

  void Analytics::SetCustomDimension(int idx, const char* value)
  {
    // JniMethodInfo t;
    // if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Analytics", "SetCustomDimension", "(ILjava/lang/String;)V")) {
    //   jstring jval = t.env->NewStringUTF(value);
    //   t.env->CallStaticVoidMethod(t.classID, t.methodID, idx, jval);

    //   t.env->DeleteLocalRef(jval);
    //   t.env->DeleteLocalRef(t.classID);
    // }
  }

  void Analytics::SetCustomMetricsInt(int idx, int value)
  {
    // JniMethodInfo t;
    // if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Analytics", "SetCustomMetricsInt", "(II)V")) {
    //   t.env->CallStaticVoidMethod(t.classID, t.methodID, idx, value);
    //   t.env->DeleteLocalRef(t.classID);
    // }
  }


  void Analytics::TrackEvent(const char* category, const char* action, const char* label, int value)
  {
  //  JniMethodInfo t;
  //  if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Analytics", "TrackEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V"))
  //  {
  //   jstring jcat = t.env->NewStringUTF(category),
  //   jact = t.env->NewStringUTF(action),
  //   jlabel = t.env->NewStringUTF(label);

  //   t.env->CallStaticVoidMethod(t.classID, t.methodID, jcat, jact, jlabel, value);

  //   t.env->DeleteLocalRef(jcat);
  //   t.env->DeleteLocalRef(jact);
  //   t.env->DeleteLocalRef(jlabel);
  //   t.env->DeleteLocalRef(t.classID);
  // }
}

void Analytics::TrackEvent(const char* category, const char* action)
{
  // JniMethodInfo t;
  // if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Analytics", "TrackEvent", "(Ljava/lang/String;Ljava/lang/String;)V"))
  // {
  //   jstring jcat = t.env->NewStringUTF(category),
  //   jact = t.env->NewStringUTF(action);

  //   t.env->CallStaticVoidMethod(t.classID, t.methodID, jcat, jact);

  //   t.env->DeleteLocalRef(jcat);
  //   t.env->DeleteLocalRef(jact);
  //   t.env->DeleteLocalRef(t.classID);
  // }
}

void Analytics::TrackEvent(const char* category, const char* action, const char* label)
{
  // JniMethodInfo t;
  // if (JniHelper::getStaticMethodInfo(t, "com/palmheroes/iosdk/Analytics", "TrackEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
  // {
  //   jstring jcat = t.env->NewStringUTF(category),
  //   jact = t.env->NewStringUTF(action),
  //   jlabel = t.env->NewStringUTF(label);

  //   t.env->CallStaticVoidMethod(t.classID, t.methodID, jcat, jact, jlabel);

  //   t.env->DeleteLocalRef(jcat);
  //   t.env->DeleteLocalRef(jact);
  //   t.env->DeleteLocalRef(jlabel);
  //   t.env->DeleteLocalRef(t.classID);
  // }
}
}
