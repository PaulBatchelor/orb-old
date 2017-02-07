package com.paulbatchelor.orb;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.content.res.AssetManager;
import android.os.Build;
import android.content.Context;
import android.media.AudioManager;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.view.MotionEvent;
import android.view.View;
import android.util.DisplayMetrics;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("native-audio-jni");
    }

    static int width, height;
    static AssetManager assetManager;
    GL2JNIView mView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mView = new GL2JNIView(getApplication());
        setContentView(mView);

        //setContentView(R.layout.activity_main);


        View decorView = getWindow().getDecorView();
        // Hide both the navigation bar and the status bar.
        // SYSTEM_UI_FLAG_FULLSCREEN is only available on Android 4.1 and higher, but as
        // a general rule, you should design your app to hide the status bar whenever you
        // hide the navigation bar.
        int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                      | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                      | View.SYSTEM_UI_FLAG_FULLSCREEN;
        decorView.setSystemUiVisibility(uiOptions);

        assetManager = getAssets();

        // initialize native audio system
        createEngine();

        int sampleRate = 0;
        int bufSize = 0;
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            AudioManager myAudioMgr = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
            String nativeParam = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            sampleRate = Integer.parseInt(nativeParam);
            nativeParam = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
            bufSize = Integer.parseInt(nativeParam);
        }
        createBufferQueueAudioPlayer(sampleRate, bufSize);
        Log.i("orb", "starting soundpipe!\n");

        Context myContext;
        myContext = getApplicationContext();
        DisplayMetrics metrics = new DisplayMetrics();
        ((WindowManager) myContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getMetrics(metrics);
        Log.i("orb", "Getting dimmensions.");
        width = metrics.widthPixels;
        height = metrics.heightPixels;

        /* orb needs dimensions sooner rather than later */
        setDimensions(width, height);

        createSoundpipe(assetManager, sampleRate, bufSize);
        startYourEngines();


    }

    @Override
    protected void onDestroy()
    {
        shutdown();
        super.onDestroy();
    }
    @Override protected void onPause() {
        super.onPause();
        //mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        //mView.onResume();
    }
    public boolean onTouchEvent(MotionEvent e) {
        if(e.getAction() == MotionEvent.ACTION_DOWN) {
            float x = e.getX() / width;
            float y = e.getY() / height;
            pos(x, y);
        }
        return true;
    }

    public native String stringFromJNI();
    public static native void createEngine();
    public static native void createBufferQueueAudioPlayer(int sampleRate, int samplesPerBuf);
    public static native void createSoundpipe(AssetManager assetManager, int sampleRate, int samplesPerBuf);
    public static native void shutdown();
    public static native boolean startYourEngines();
    public static native void pos(float x, float y);
    public static native void setDimensions(int width, int height);


}
