package com.hail.videoplay.activity;

import com.hail.videoplay.jni.VideoPlayNative;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity{
	VideoPlayNative mPlay = new VideoPlayNative();
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		mPlay.Test();
		mPlay.Init();
	}
}
