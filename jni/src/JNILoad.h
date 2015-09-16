#ifndef JNILOAD_H_
#define JNILOAD_H_

#include <jni.h>



#ifdef __cplusplus
extern "C" {
#endif

int jniRegisterNativeMethods(JNIEnv* env, const char* className,
       const JNINativeMethod* gMethods, int numMethods);

#ifdef __cplusplus
}
#endif

#endif /* JNILOAD_H_ */
