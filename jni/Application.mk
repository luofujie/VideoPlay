APP_PLATFORM = android-9
APP_ABI := armeabi-v7a
APP_STL := stlport_static
APP_OPTM := debug  
NDK_TOOLCHAIN_VERSION  := 4.8
LOCAL_CFLAGS += -pie -fPIE 
LOCAL_LDFLAGS += -pie -fPIE
APP_CPPFLAGS:=-frtti -fexceptions 