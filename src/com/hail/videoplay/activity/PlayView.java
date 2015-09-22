package com.hail.videoplay.activity;


import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.hail.videoplay.jni.PlayNative;

public class PlayView extends SurfaceView implements SurfaceHolder.Callback{

	public static String TAG = "TestView"; 
	private  PlayNative mNative = null;
	private SurfaceHolder holder;
	private Context mContext;
	private int width = 0;
	private int height = 0;
	String filePath = "";
	public PlayView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		holder = getHolder();
		holder.addCallback(this);
		mContext = context;
		mNative = new PlayNative();
		mNative.Test();
	}
	public PlayView(Context context,AttributeSet attrs)
	{
		super(context,attrs);
		holder = getHolder();
		holder.addCallback(this);
		mContext = context;
		mNative = new PlayNative();
	}
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		Log.i(TAG, "surfaceCreated");
	}
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub
		Log.i(TAG, "surfaceChanged");
		mNative.Init(holder.getSurface());
	}
	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		Log.i(TAG, "surfaceDestroyed");
		mNative.Destroye();
	}
	public Boolean openFile(String path)
	{
		filePath = path;
		return true;
	}
}