package com.hail.videoplay.activity;

import com.hail.videoplay.R;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;

public class MainActivity extends Activity{
	PlayView playview = null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main_layout);
		playview = new PlayView(this);
		LinearLayout linearlayout = (LinearLayout) findViewById(R.id.video);
		LinearLayout.LayoutParams layoutParam = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT);
		linearlayout.addView(playview,layoutParam);
		Button btnPlay = (Button) findViewById(R.id.btn_play);
		btnPlay.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				playview.Play();
			}
		});
		Button btnPause = (Button) findViewById(R.id.btn_pause);
		btnPause.setOnClickListener(new OnClickListener() {
					
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				playview.Pause();
			}
		});
		Button btnStop = (Button) findViewById(R.id.btn_stop);
		btnStop.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				playview.Stop();
			}
		});
	}
}
