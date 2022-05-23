/*
 * Copyright 2013 The Android Open Source Project
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

package dev.bdon.spacegame;

import android.annotation.SuppressLint;
import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.WindowManager.LayoutParams;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.sample.spacegame.R;

import java.util.Objects;

public class SpacegameActivity extends NativeActivity {

    PopupWindow popupWindow;
    TextView fpsLabel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d("native-activity", "Activity Created");

        //Hide toolbar
        View decorView = getWindow().getDecorView();
        decorView.setOnSystemUiVisibilityChangeListener(visibility -> hideTopBar());
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();
        Log.d("native-activity", "attach to window " + Objects.toString(getWindow()));
        showUI();
    }

    protected void onResume() {
        super.onResume();
        Log.d("native-activity", "onResume");
        hideTopBar();
    }

    protected void onPause() {
        super.onPause();
        Log.d("native-activity", "onPause");
    }

    // Removes the top bar to make view appear fullscreen
    void hideTopBar() {
        Log.d("native-activity", "setImmersiveSticky");
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    @SuppressLint("InflateParams")
    public void showUI() {
        if (popupWindow == null) {
            Log.i("native-activity", "Creating popup ui");
            this.runOnUiThread(this::createPopupUi);
        }
    }

    @SuppressLint("DefaultLocale")
    public void updateFPS(final float fFPS) {
        if (fpsLabel != null) {
            this.runOnUiThread(() -> fpsLabel.setText(String.format("%2.2f FPS", fFPS)));
        }
    }

    @SuppressLint("InflateParams")
    public void createPopupUi() {
        Log.d("native-activity", "popup stuff");
        var layoutInflater = (LayoutInflater) getBaseContext().getSystemService(LAYOUT_INFLATER_SERVICE);

        View popupView = layoutInflater.inflate(R.layout.widgets, null);
        popupWindow = new PopupWindow(popupView, LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);

        LinearLayout mainLayout = new LinearLayout(this);
        MarginLayoutParams params = new MarginLayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        params.setMargins(0, 0, 0, 0);
        setContentView(mainLayout, params);

        // Show our UI over NativeActivity window
        popupWindow.showAtLocation(mainLayout, Gravity.TOP | Gravity.START, 10, 10);
        popupWindow.update();

        fpsLabel = popupView.findViewById(R.id.textViewFPS);
    }
}


