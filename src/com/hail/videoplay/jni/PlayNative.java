package com.hail.videoplay.jni;

import android.view.Surface;


public class PlayNative {
	static {


    	System.loadLibrary("ffmpeg");
    	System.loadLibrary("VideoPlay");

	}
	public native void Test();
	public native int Init(Surface surface);
	public native int Open(String path);
	public native void Play();
	public native void Pause();
	public native void Stop();
	public native void Destroye();
	
}
