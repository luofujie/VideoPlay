#ifndef JNI_UTIL_H
#define JNI_UTIL_H
#include <jni.h>
#include <android/log.h>
#include <string>
using namespace std;
#define LOG_TAG "JNI_LOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
class JNIUtil
{

public:
	/* 注意Init和CleanUp必须在主线程中执行*/
	static void Init(JNIEnv* env);
	static void CleanUp(JNIEnv* env);
public:
	JNIUtil();
	~JNIUtil();
	JNIEnv* GetJNIEnv();
	string Jstring2String(jstring jstr);
	jstring String2Jstring(const char* str);
private:
	static jclass m_strClass;
	static jmethodID m_ctorID;
	static jmethodID m_getByteID;
	void Detach();
	static JavaVM* m_sJVM;
public:
	bool m_bAttach;
};


#endif
