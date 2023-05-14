package com.geareonix.engine;
import android.annotation.TargetApi;
import android.app.NativeActivity;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

import org.fmod.FMOD;

public class MainActivity extends NativeActivity {

    @TargetApi(Build.VERSION_CODES.KITKAT)
    final static int VISIBILITY_OPTS = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        FMOD.init(this);
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

    public void showKeyboard()
    {
        InputMethodManager imm = ( InputMethodManager )getSystemService( Context.INPUT_METHOD_SERVICE );
        if (imm != null) {
            imm.showSoftInput( this.getWindow().getDecorView(), InputMethodManager.SHOW_FORCED );
        }
    }

    public void hideKeyboard()
    {
        InputMethodManager imm = ( InputMethodManager )getSystemService( Context.INPUT_METHOD_SERVICE );
        if (imm != null) {
            imm.hideSoftInputFromWindow( this.getWindow().getDecorView().getWindowToken(), 0 );
        }
    }

    static {
        System.loadLibrary("fmod");
    }
}
