package com.geareonix.engine;
import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends NativeActivity {

    final static int VISIBILITY_OPTS = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setDecor();
    }

    @Override
    protected void onResume() {
        super.onResume();
        setDecor();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        setDecor();
    }

    void setDecor() {
         getWindow().getDecorView().setSystemUiVisibility(VISIBILITY_OPTS);
    }
}
