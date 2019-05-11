package com.palmkingdoms.pk2_remastered;

import android.content.Context;
import android.app.Activity;
import java.lang.reflect.Field;
import android.util.ArrayMap;
import android.annotation.TargetApi;
import  android.os.Build;
import android.os.Environment;
import java.io.FilenameFilter;
import android.util.Log;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.io.RandomAccessFile;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;


public class FileAccessor
{
  public static Activity activity;

  public static String getLibraryDirectory()
  {
    try 
    {      
      File f = activity.getDir("store", Context.MODE_PRIVATE);
      return f.getAbsolutePath();      
    } 
    catch(Throwable x)
    {
      return null;
    }
  }

  public static String getDocumentsDirectory()
  {
    try 
    {
      File f = activity.getDir("save", Context.MODE_PRIVATE);
      return f.getAbsolutePath();      

    } catch(Throwable x)
    {
      return null;
    }
  }

  public static String getLogDirectory()
  {
    try 
    {
      File f = activity.getExternalFilesDir("log");
      if(f != null) return f.getAbsolutePath();
      f = activity.getDir("log", Context.MODE_PRIVATE);
      return f.getAbsolutePath();
    } catch(Throwable x)
    {
      return null;
    }
  }

  public static String getApkPath()
  {
    String packageName = activity.getPackageName();
    PackageManager pm = activity.getPackageManager();
    try {
      ApplicationInfo ai = pm.getApplicationInfo(packageName, 0);
      String apk = ai.publicSourceDir;
      return apk;
    } catch (Throwable x) 
    {
      return null;
    }    
  }
  
  public static String getCacheDirectory()
  {
    try
    {
      File f = activity.getExternalFilesDir("cache");
      if(f != null) return f.getAbsolutePath();
      f = activity.getExternalCacheDir();
      if(f != null) return f.getAbsolutePath();
      f = activity.getCacheDir();
      return f.getAbsolutePath();
    } catch(Throwable x)
    {
      return null;
    }
  }
 
}