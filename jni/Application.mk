APP_PLATFORM = android-9
APP_ABI := armeabi
APP_STL := stlport_static
APP_CPPFLAGS:=-frtti -fexceptions 
LOCAL_CFLAGS += -pie -fPIE 
LOCAL_LDFLAGS += -pie -fPIE
APP_OPTIM := debug