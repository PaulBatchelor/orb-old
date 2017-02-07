/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.paulbatchelor.orb;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import android.view.Window;
import android.view.WindowManager;
import android.view.MotionEvent;
import android.util.DisplayMetrics;
import android.content.Context;

import java.io.File;


public class GL2JNIActivity extends Activity {

    GL2JNIView mView;
    static int width, height;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new GL2JNIView(getApplication());
        setContentView(mView);

        Context myContext;
        myContext = getApplicationContext();
        DisplayMetrics metrics = new DisplayMetrics();
        //myContext.getWindowManger().getDefaultDisplay().getMetrics(metrics);

        ((WindowManager) myContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getMetrics(metrics);
        width = metrics.widthPixels;
        height = metrics.heightPixels;
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }

    public static native void pos(float x, float y);

    public boolean onTouchEvent(MotionEvent e) {
        float x = e.getX() / width;
        float y = e.getY() / height;
        pos(x,y);
        return true;
    }
}
