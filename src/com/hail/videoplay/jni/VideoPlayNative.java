package com.hail.videoplay.jni;


public class VideoPlayNative {
	static {


    	System.loadLibrary("ffmpeg");
    	System.loadLibrary("VideoPlay");

	}
	public native void Test();
	public native int Init();
}
