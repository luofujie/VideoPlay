#include "jniLoad.h"
#include "Stdafx.h"
#include <stdlib.h>
int RegisterAllNativeMethods(JNIEnv *env);

int jniRegisterNativeMethods(JNIEnv* env, const char* className,
        const JNINativeMethod* gMethods, int numMethods) {

    jclass clazz;
    clazz = env->FindClass(className);

    if (clazz == NULL) {
        return JNI_ERR;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_ERR;
    }
    return JNI_OK;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = JNI_ERR;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }

    if(RegisterAllNativeMethods(env) != JNI_OK) {
        LOGE("RegisterAllNativeMethods ERROR! ");
        goto end;
    }
    result = JNI_VERSION_1_4;
    LOGI("RegisterAllNativeMethods Success! ");
end:
    return result;
}


