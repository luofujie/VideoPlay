#include <jni.h>
#include <stdlib.h>
#include "JNIUtil.h"

JavaVM* JNIUtil::m_sJVM = NULL;
jclass JNIUtil::m_strClass = NULL;
jmethodID JNIUtil::m_ctorID = 0;
jmethodID JNIUtil::m_getByteID = 0;
void JNIUtil::Init(JNIEnv* env)
{
	env->GetJavaVM(&m_sJVM);
	jclass tmp = env->FindClass("java/lang/String");
	m_strClass = (jclass)env->NewGlobalRef(tmp);
	m_ctorID = env->GetMethodID(m_strClass, "<init>",
				"([BLjava/lang/String;)V");
	m_getByteID = env->GetMethodID(m_strClass, "getBytes",
			"(Ljava/lang/String;)[B");
}
void JNIUtil::CleanUp(JNIEnv* env)
{
	env->DeleteGlobalRef(m_strClass);
	m_sJVM = NULL;
	m_strClass = NULL;
	m_getByteID =0;
	m_ctorID = 0;
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

string JNIUtil::Jstring2String(jstring jstr)
{
	JNIEnv *env = NULL;
	env = GetJNIEnv();
	char* rtn = NULL;
	jstring strencode = env->NewStringUTF("utf-8");
	jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, m_getByteID, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0)
	{
		rtn = new char[alen+1];
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	string strRet(rtn);
	delete[] rtn;
	return strRet;
}

//char* to jstring
jstring JNIUtil::String2Jstring(const char* pat)
{
	JNIEnv *env = NULL;
	env = GetJNIEnv();
	jbyteArray bytes = env->NewByteArray(strlen(pat));
	env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*) pat);
	jstring encoding = env->NewStringUTF("utf-8");
	return (jstring)env->NewObject(m_strClass, m_ctorID, bytes, encoding);
}
