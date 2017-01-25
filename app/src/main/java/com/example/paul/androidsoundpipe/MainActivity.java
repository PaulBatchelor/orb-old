package com.example.paul.androidsoundpipe;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.content.res.AssetManager;
import android.os.Build;
import android.content.Context;
import android.media.AudioManager;
import android.util.Log;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("native-audio-jni");
    }

    static AssetManager assetManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

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
    }

    @Override
    protected void onDestroy()
    {
        shutdown();
        super.onDestroy();
    }

    public native String stringFromJNI();
    public static native void createEngine();
    public static native void createBufferQueueAudioPlayer(int sampleRate, int samplesPerBuf);
    public static native void createSoundpipe(AssetManager assetManager, int sampleRate, int samplesPerBuf);
    public static native void shutdown();
    public static native boolean startYourEngines();
}
