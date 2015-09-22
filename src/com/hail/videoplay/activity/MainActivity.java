package com.hail.videoplay.activity;

import android.app.Activity;
import android.os.Bundle;
import android.widget.LinearLayout;

public class MainActivity extends Activity{
	PlayView playview = null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		playview = new PlayView(this);
		LinearLayout linearlayout = new LinearLayout(this);
		LinearLayout.LayoutParams layoutParam = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,600);
		linearlayout.addView(playview,layoutParam);
		setContentView(linearlayout);
	}
}
