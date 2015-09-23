#include <jni.h>
#include <stdlib.h>
#include "JNIUtil.h"

JavaVM* JNIUtil::m_sJVM = NULL;
void JNIUtil::SetJavaVm(JavaVM*)
{
	m_sJVM = NULL;
}
JNIUtil::JNIUtil() :
		m_bAttach(false)
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
