#include <jni.h>
#include <stdlib.h>
#include "JNIUtil.h"

JavaVM* JNIUtil::m_sJVM = NULL;
void JNIUtil::SetJavaVm(JavaVM* vm)
{
	m_sJVM = vm;
}
JNIUtil::JNIUtil() :m_bAttach(false)
{

}
JNIUtil::~JNIUtil()
{
	Detach();
}
JNIEnv* JNIUtil::GetJNIEnv()
{
	if (m_sJVM == NULL)
		return NULL;
	int status = 0;
	JNIEnv *env = NULL;
	status = m_sJVM->GetEnv((void**) &env, JNI_VERSION_1_4);
	if (status < 0)
	{
		status = m_sJVM->AttachCurrentThread(&env, NULL);
		if (status < 0)
		{
			return NULL;
		}
		m_bAttach = true;
	}
	return env;
}
void JNIUtil::Detach()
{
	if (m_bAttach)
	{
		m_sJVM->DetachCurrentThread();
		m_bAttach = false;
	}
}

char* JNIUtil::Jstring2String(jstring jstr)
{
	JNIEnv *env = NULL;
	env = GetJNIEnv();
	char* rtn = NULL;
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("utf-8");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes",
			"(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0)
	{
		rtn = (char*) malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	return rtn;
}

//char* to jstring
jstring JNIUtil::String2Jstring(const char* pat)
{
	JNIEnv *env = NULL;
	env = GetJNIEnv();
	jclass strClass = env->FindClass("Ljava/lang/String;");
	jmethodID ctorID = env->GetMethodID(strClass, "<init>",
			"([BLjava/lang/String;)V");
	jbyteArray bytes = env->NewByteArray(strlen(pat));
	env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*) pat);
	jstring encoding = env->NewStringUTF("utf-8");
	return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}
