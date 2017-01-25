package com.example.paul.androidsoundpipe;

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
import android.util.DisplayMetrics;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("native-audio-jni");
    }

    static int width, height;
    static AssetManager assetManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

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
        createSoundpipe(assetManager, sampleRate, bufSize);
        startYourEngines();

        Context myContext;
        myContext = getApplicationContext();
        DisplayMetrics metrics = new DisplayMetrics();
        ((WindowManager) myContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getMetrics(metrics);
        width = metrics.widthPixels;
        height = metrics.heightPixels;
    }

    @Override
    protected void onDestroy()
    {
        shutdown();
        super.onDestroy();
    }

    public boolean onTouchEvent(MotionEvent e) {
        float x = e.getX() / width;
        float y = e.getY() / height;
        setX(x);
        setY(y);
        return true;
    }

    public native String stringFromJNI();
    public static native void createEngine();
    public static native void createBufferQueueAudioPlayer(int sampleRate, int samplesPerBuf);
    public static native void createSoundpipe(AssetManager assetManager, int sampleRate, int samplesPerBuf);
    public static native void shutdown();
    public static native boolean startYourEngines();
    public static native void setX(float x);
    public static native void setY(float y);
}
