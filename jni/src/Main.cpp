#include "JNIUtil.h"
#include "PlayNative.h"
/**
 * JNI registration.
 */
typedef int (*RegisterFunc)(JNIEnv *env);

RegisterFunc g_funcs[] =
{ RegisterVideoPlayMethods };

int RegisterAllNativeMethods(JNIEnv *env)
{
	//注册所有类的native函数
	int nRet = JNI_ERR;
	for (int i = 0; i < sizeof(g_funcs) / sizeof(RegisterFunc); i++)
	{
		nRet = g_funcs[i](env);
		if (nRet != JNI_OK)
			break;
	}
	return nRet;
}
