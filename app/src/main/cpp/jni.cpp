#include <memory>
#include <jni.h>
#include <android/log.h>
#include "pythonthread.h"

#define TAG "2d-print-system"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__)

static std::unique_ptr<PythonThread> s_PythonThread;

extern "C" JNIEXPORT jint JNICALL
Java_com_example_epython_MainActivity_startPythonThread(
    JNIEnv* env,
    jobject /* this */,
    jstring jApkDir,
    jstring jFilesDir,
    jstring jExternalFilesDir) {
    if ( !s_PythonThread ) {
        const char *apkDir = env->GetStringUTFChars(jApkDir, NULL);
        const char *filesDir = env->GetStringUTFChars(jFilesDir, NULL);
        const char *externalFilesDir = env->GetStringUTFChars(jExternalFilesDir, NULL);

        s_PythonThread = std::make_unique<PythonThread>(apkDir, filesDir, externalFilesDir);
        s_PythonThread->start();

        env->ReleaseStringUTFChars(jApkDir, apkDir);
        env->ReleaseStringUTFChars(jFilesDir, filesDir);
        env->ReleaseStringUTFChars(jExternalFilesDir, externalFilesDir);
    } else {
        LOGW("Python Thread already started.");
    }
    return 0;
}
