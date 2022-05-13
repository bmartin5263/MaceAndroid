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

import android.app.Application;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.util.Log;
import android.widget.Toast;

public class SpacegameApplication extends Application {
    public void onCreate(){
        super.onCreate();	
        Log.i("native-activity", "Application Created");
        Toast.makeText(this, getAppName(), Toast.LENGTH_SHORT).show();
    }

    private CharSequence getAppName() {
        var packageManager = getApplicationContext().getPackageManager();
        ApplicationInfo appInfo;
        try {
            appInfo = packageManager.getApplicationInfo( this.getPackageName(), 0);
            return packageManager.getApplicationLabel(appInfo);
        } catch (NameNotFoundException e) {
            return "(unknown)";
        }
    }
}
