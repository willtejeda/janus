package org.janus;

import android.Manifest;

import android.R;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager.LayoutParams;
import android.widget.FrameLayout;

public class JanusActivity extends org.qtproject.qt5.android.bindings.QtActivity
                           implements JoystickView.JoystickListener, ButtonView.ButtonListener
{
        private String launch_url = "";
        private boolean launch_in_vr = false;

        //WebView Manager
        private WebViewManager webViewManager = null;

        //Splash/Control handlers
        private SplashView splash_screen;
        private JoystickView walk_joystick;
        private JoystickView view_joystick;
        private ButtonView talk_button;
        private ButtonView jump_button; //Also flight button
        private float walk_x = 0;
        private float walk_y = 0;
        private float view_x = 0;
        private float view_y = 0;
        private boolean show_view_joystick = false;
        private boolean running = false;
        private boolean talking = false;
        private boolean always_talking = false;
        private boolean jumping = false;
        private boolean flying = false;

        //GoogleVR
        GVRManager gvrManager = null;

        //GearVR
        GearManager gearManager = null;

        public void onCreate(Bundle savedInstanceState){
            super.onCreate(savedInstanceState);

            //Intent example:
            //      <a href="intent:#Intent;action=org.janusvr.launchfrombrowser;category=android.intent.category.DEFAULT;category=android.intent.category.BROWSABLE;category=com.google.intent.category.DAYDREAM;category=com.google.intent.category.CARDBOARD;S.url=http://demos.janusvr.com/joseph/android/images/index.html;end">Click here to launch sample app</a>
            Intent intent = getIntent();
            if(intent != null && intent.getAction() != null && intent.getAction().equals("org.janusvr.launchfrombrowser")){
                Bundle bundle = intent.getExtras();
                if(bundle != null){
                        launch_url = bundle.getString("url");
                }
            }
            if(intent != null && intent.getBooleanExtra("android.intent.extra.VR_LAUNCH", false)){
                launch_in_vr = true;
            }

            webViewManager = new WebViewManager(JanusActivity.this);

            View decorView = getWindow().getDecorView();
            //int uiOptions = View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY| View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
            int uiOptions = View.SYSTEM_UI_FLAG_LAYOUT_STABLE; //| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
            decorView.setSystemUiVisibility(uiOptions);

            DisplayMetrics displayMetrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
            int size =  Math.min(displayMetrics.heightPixels, displayMetrics.widthPixels) / 3;


            FrameLayout mainLayout = (FrameLayout) findViewById(R.id.content);
            FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT);

            splash_screen = new SplashView(this);
            splash_screen.setZOrderOnTop(true);
            splash_screen.setBitmaps(this,"assets/splash/splash.png","assets/splash/janusvr.png");
            splash_screen.getHolder().setFormat(PixelFormat.TRANSLUCENT);
            splash_screen.getHolder().setFixedSize(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT);

            params.leftMargin = 0;
            params.bottomMargin = 0;
            params.gravity = Gravity.TOP | Gravity.LEFT;

            mainLayout.addView(splash_screen, params);

            walk_joystick = new JoystickView(this);
            walk_joystick.setZOrderOnTop(true);
            walk_joystick.getHolder().setFormat(PixelFormat.TRANSLUCENT);
            walk_joystick.getHolder().setFixedSize(size, size);

            params = new FrameLayout.LayoutParams(size, size);
            params.leftMargin = 0;
            params.bottomMargin = 0;
            params.gravity = Gravity.BOTTOM | Gravity.LEFT;

            mainLayout.addView(walk_joystick, params);

            talk_button = new ButtonView(this);
            talk_button.setZOrderOnTop(true);
            talk_button.setStates(this,"assets/icons/talk_pressed.png","assets/icons/talk_unpressed.png");
            talk_button.setDoubleTapState(this, "assets/icons/always_talk_unpressed.png", "assets/icons/always_talk_pressed.png");
            talk_button.getHolder().setFormat(PixelFormat.TRANSLUCENT);
            talk_button.getHolder().setFixedSize(3*size/4, 3*size/4);

            params = new FrameLayout.LayoutParams(3*size/4, 3*size/4);
            params.rightMargin = displayMetrics.widthPixels/2;
            params.bottomMargin = 0;
            params.gravity = Gravity.BOTTOM | Gravity.RIGHT;

            mainLayout.addView(talk_button, params);

            jump_button = new ButtonView(this);
            jump_button.setZOrderOnTop(true);
            jump_button.setStates(this, "assets/icons/jump_pressed.png","assets/icons/jump_unpressed.png");
            jump_button.setDoubleTapState(this, "assets/icons/flight_unpressed.png", "assets/icons/flight_pressed.png");
            jump_button.getHolder().setFormat(PixelFormat.TRANSLUCENT);
            jump_button.getHolder().setFixedSize(3*size/4, 3*size/4);

            params = new FrameLayout.LayoutParams(3*size/4, 3*size/4);
            params.rightMargin = displayMetrics.widthPixels/2 - 3*size/4;
            params.bottomMargin = 0;
            params.gravity = Gravity.BOTTOM | Gravity.RIGHT;

            mainLayout.addView(jump_button, params);

            view_joystick = new JoystickView(this);
            view_joystick.setZOrderOnTop(true);
            view_joystick.getHolder().setFormat(PixelFormat.TRANSLUCENT);
            view_joystick.getHolder().setFixedSize(size, size);

            params = new FrameLayout.LayoutParams(size, size);
            params.rightMargin = 0;
            params.bottomMargin = 0;
            params.gravity = Gravity.BOTTOM | Gravity.RIGHT;

            mainLayout.addView(view_joystick, params);

            splash_screen.setVisibility(View.VISIBLE);
            walk_joystick.setVisibility(View.INVISIBLE);
            view_joystick.setVisibility(View.INVISIBLE);
            talk_button.setVisibility(View.INVISIBLE);
            jump_button.setVisibility(View.INVISIBLE);

            gvrManager = new GVRManager(JanusActivity.this, splash_screen);
            gearManager = new GearManager(JanusActivity.this, splash_screen);
        }

        public void onDestroy(){
            super.onDestroy();
            gvrManager.onDestroy();
        }

        @Override
        public void onPause() {
            if (gvrManager != null)
                gvrManager.onPause();
            if (gearManager != null)
                gearManager.onPause();

            nativePause();
            webViewManager.setPaused(true);

            super.onPause();
        }

        @Override
        public void onResume() {
            super.onResume();

            nativeResume();
            webViewManager.setPaused(false);

            if (gvrManager != null)
                gvrManager.onResume();
            if (gearManager != null)
                gearManager.onResume();
        }

        @Override
        public void onBackPressed() {
            gvrManager.onBackPressed();
            gearManager.onBackPressed();

            super.onBackPressed();
        }

        public String getLaunchURL()
        {
            return launch_url;
        }

        public boolean getLaunchInVR()
        {
            return launch_in_vr;
        }

        public boolean getShowingVR()
        {
            return gearManager.getShowingVR() || gvrManager.getShowingVR();
        }

        //============================================================================================================
        //Splash
        //============================================================================================================

        public void setProgressBar(int i){
            if (splash_screen != null){
                if (i >= 95){
                    splash_screen.setProgress(100);
                }
                else if (i < 95){
                    splash_screen.setProgress(i);
                }
            }
        }

        public void setVRModeEnabled(boolean b){
            if (splash_screen != null){
                splash_screen.setVRModeEnabled(b);
            }
        }

        public void hideSplash(){
            Message msg = new Message();
            msg.what = 0;

            hideSplashHandler.sendMessage(msg);
        }

        protected Handler hideSplashHandler = new Handler() {
            public void handleMessage(Message msg) {
                if (splash_screen != null && msg.what == 0){
                    splash_screen.setVisibility(View.GONE);
                    FrameLayout mainLayout = (FrameLayout) findViewById(R.id.content);
                    if (splash_screen.getParent() == mainLayout) {
                        mainLayout.removeView(splash_screen);

                        splash_screen.stop();
                    }
                }
            }
        };

        //============================================================================================================
        //Permissions
        //============================================================================================================

        private boolean answered_permissions = false;

        @Override
        public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
            switch (requestCode) {
                case 0: {
                    answered_permissions = true;
                    return;
                }
            }
        }

        public boolean getPermissions(){
            return this.checkSelfPermission(Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED &&
                this.checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED &&
                this.checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
        }

        public boolean getAnsweredPermissions(){
            return answered_permissions;
        }

        public void askPermissions(){
            Message msg = new Message();
            msg.what = 0;

            askPermissionsHandler.sendMessage(msg);
        }

        protected Handler askPermissionsHandler = new Handler() {
            public void handleMessage(Message msg) {
                if (msg.what == 0){
                    //Ask for permissions
                    if (!getPermissions()){
                        JanusActivity.this.requestPermissions(
                                new String[]{
                                    Manifest.permission.RECORD_AUDIO,
                                    Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE},
                                0);
                    }
                    else{
                        answered_permissions = true;
                    }
                }
            }
        };

        //============================================================================================================
        //WebView
        //============================================================================================================

        public void setUseGeckoWebView(boolean b) {
            webViewManager.setUseGeckoWebView(b);
        }

        public void createNewWebView(int tag) {
            webViewManager.createNewWebView(tag);
        }

        public void updateWebView(int tag) {
            webViewManager.updateWebView(tag);
        }

        public void removeWebView(int tag) {
            webViewManager.removeWebView(tag);
        }

        public void moveWebView(int tag, int x, int y) {
            webViewManager.moveWebView(tag,x,y);
        }

        public void resizeWebView(int tag, int w, int h) {
            webViewManager.resizeWebView(tag,w,h);
        }

        public void attachWebViewToMainLayout(int tag) {
            webViewManager.attachWebViewToMainLayout(tag);
        }

        public void loadUrlAtWebView(int tag, String url) {
            webViewManager.loadUrlAtWebView(tag,url);
        }

        public void goBackOrForwardAtWebView(int tag, int steps) {
            webViewManager.goBackOrForwardAtWebView(tag,steps);
        }

        public void reloadWebView(int tag) {
            webViewManager.reloadWebView(tag);
        }

        public void stopWebView(int tag) {
            webViewManager.stopWebView(tag);
        }

        public void loadHtmlAtWebView(int tag, String html) {
            webViewManager.loadHtmlAtWebView(tag,html);
        }

        public void evaluateJavaScriptAtWebView(int tag, String javascript) {
            webViewManager.evaluateJavaScriptAtWebView(tag,javascript);
        }

        public void setUpdatesEnabledWebView(int tag, boolean b) {
            webViewManager.setUpdatesEnabledWebView(tag,b);
        }

        public Bitmap getBitmap(int tag) {
            return webViewManager.getBitmap(tag);
        }

        public int getTexture(int tag) {
            return webViewManager.getTexture(tag);
        }

        public String getHitTestContent(int tag) {
            return webViewManager.getHitTestContent(tag);
        }

        public int getHorizontalScrollRange(int tag) {
            return webViewManager.getHorizontalScrollRange(tag);
        }

        public int getVerticalScrollRange(int tag) {
            return webViewManager.getVerticalScrollRange(tag);
        }

        public int getWidthWebView(int tag) {
            return webViewManager.getWidthWebView(tag);
        }

        public int getHeightWebView(int tag) {
            return webViewManager.getHeightWebView(tag);
        }

        public int getScrollXWebView(int tag) {
            return webViewManager.getScrollXWebView(tag);
        }

        public int getScrollYWebView(int tag) {
            return webViewManager.getScrollYWebView(tag);
        }

        public String getURLWebView(int tag) {
            return webViewManager.getURLWebView(tag);
        }

        public void scrollByWebView(int tag, int x, int y) {
            webViewManager.scrollByWebView(tag,x,y);
        }

        public void scrollToWebView(int tag, int x, int y) {
            webViewManager.scrollToWebView(tag,x,y);
        }

        public void mousePressWebView(int tag, int x, int y) {
            webViewManager.mousePressWebView(tag,x,y);
        }

        public void mouseMoveWebView(int tag, int x, int y) {
            webViewManager.mouseMoveWebView(tag,x,y);
        }

        public void mouseReleaseWebView(int tag, int x, int y) {
            webViewManager.mouseReleaseWebView(tag,x,y);
        }

        public boolean getRepaintRequestedAtWebView(int tag) {
            return webViewManager.getRepaintRequestedAtWebView(tag);
        }

        public boolean getScrollRequestedAtWebView(int tag) {
            return webViewManager.getScrollRequestedAtWebView(tag);
        }

        public boolean getURLChangedAtWebView(int tag) {
            return webViewManager.getURLChangedAtWebView(tag);
        }

        //============================================================================================================
        //Controls
        //============================================================================================================

        public void onJoystickMoved(float x, float y, boolean b, int id){
            if (id == walk_joystick.getId()){
                walk_x = x;
                walk_y = y;
                running = b;
            }
            if (id == view_joystick.getId()){
                view_x = x;
                view_y = y;
                //running = b;
            }
        }

        public void onButtonPressed(boolean switched, int id){
            if (id == talk_button.getId()){
                talking = true;
            }
            if (id == jump_button.getId()){
                jumping = true;
            }
        }

        public void onButtonReleased(boolean switched, int id){
            if (id == talk_button.getId()){
                talking = false;
            }
            if (id == jump_button.getId()){
                jumping = false;
            }
        }

        public void onButtonDoubleTapped(boolean switched, int id){
            if (id == jump_button.getId()){
                if (!switched)
                {
                    jumping = false;
                    flying = false;
                }
                else
                {
                    flying = true;
                    jumping = false;
                }
            }
            if (id == talk_button.getId()){
                if (!switched)
                {
                    talking = false;
                    always_talking = false;
                }
                else
                {
                    always_talking = true;
                    talking = false;
                }
            }
        }

        public int getScreenOrientation(){
            int angle = 90;
            /*int rotation =  getWindowManager().getDefaultDisplay().getRotation();
            switch (rotation) {
                case Surface.ROTATION_90:
                    angle = 90;
                    break;
                case Surface.ROTATION_180:
                    angle = 180;
                    break;
                case Surface.ROTATION_270:
                    angle = 270;
                    break;
                default:
                    angle = 0;
                    break;
            }*/
            return angle;
        }

        public float getWalkJoystickX(){
            return walk_x;
        }

        public float getWalkJoystickY(){
            return walk_y;
        }

        public float getViewJoystickX(){
            return view_x;
        }

        public float getViewJoystickY(){
            return view_y;
        }

        public boolean getRunning(){
            return running;
        }

        public boolean getSpeaking(){
            return talking;
        }

        public boolean getAlwaysSpeaking(){
            return always_talking;
        }


        public boolean getJumping(){
            return jumping;
        }

        public boolean getFlying(){
            return flying;
        }

        public void setButtonMargin(int margin){
            Message msg = new Message();
            msg.what = margin;

            moveButtonMarginHandler.sendMessage(msg);
        }

        protected Handler moveButtonMarginHandler = new Handler() {
                public void handleMessage(Message msg) {
                    DisplayMetrics displayMetrics = new DisplayMetrics();
                    getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
                    int size =  Math.min(displayMetrics.heightPixels, displayMetrics.widthPixels) / 3;

                    FrameLayout mainLayout = (FrameLayout) findViewById(R.id.content);
                    FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(size, size);

                    float i = (Math.max(msg.what, 0) == 0) ? 1.0f/2.0f : 2.0f/3.0f - 80.0f/((float)displayMetrics.widthPixels);
                    int margin_value =  (int) (displayMetrics.widthPixels * i);

                    params = new FrameLayout.LayoutParams(3*size/4, 3*size/4);
                    if (show_view_joystick)
                        params.rightMargin = margin_value - size/16; // + Math.max(msg.what, 0);
                    else
                        params.rightMargin = 5*size/8 + Math.max(msg.what, 0);
                    params.bottomMargin = 0;
                    params.gravity = Gravity.BOTTOM | Gravity.RIGHT;

                    talk_button.setLayoutParams(params);

                    params = new FrameLayout.LayoutParams(3*size/4, 3*size/4);
                    if (show_view_joystick)
                        params.rightMargin = margin_value - 3*size/4 + size/16; // + Math.max(msg.what, 0);
                    else
                        params.rightMargin = Math.max(msg.what, 0);
                    params.bottomMargin = 0;
                    params.gravity = Gravity.BOTTOM | Gravity.RIGHT;

                    jump_button.setLayoutParams(params);

                    params = new FrameLayout.LayoutParams(size, size);
                    params.rightMargin = Math.max(msg.what, 0);
                    params.bottomMargin = 0;
                    params.gravity = Gravity.BOTTOM | Gravity.RIGHT;

                    view_joystick.setLayoutParams(params);
                }
        };

        public void setControlsVisible(boolean b, boolean s){
            Message msg = new Message();
            msg.what = b?1:0;

            show_view_joystick = s;

            setControlsVisibleHandler.sendMessage(msg);
        }

        protected Handler setControlsVisibleHandler = new Handler() {
                public void handleMessage(Message msg) {
                    if (msg.what == 0) {
                        walk_joystick.setVisibility(View.INVISIBLE);
                        view_joystick.setVisibility(View.INVISIBLE);
                        talk_button.setVisibility(View.INVISIBLE);
                        jump_button.setVisibility(View.INVISIBLE);
                    }
                    else {
                        walk_joystick.setVisibility(View.VISIBLE);
                        if (show_view_joystick)
                            view_joystick.setVisibility(View.VISIBLE);
                        else
                            view_joystick.setVisibility(View.INVISIBLE);
                        talk_button.setVisibility(View.VISIBLE);
                        jump_button.setVisibility(View.VISIBLE);
                    }
                }
        };

        //============================================================================================================
        //GoogleVR
        //============================================================================================================

        public void setupGvr()
        {
            gvrManager.setupGvr();
        }

        public long getGvrContext()
        {
            return gvrManager.getGvrContext();
        }

        public void showGVR(boolean b)
        {
            gvrManager.showGVR(b);
        }

        //============================================================================================================
        //GearVR
        //============================================================================================================

        public void setupGear()
        {
            gearManager.setupGear();
        }

        public boolean getGearReady()
        {
            return gearManager.getGearReady();
        }

        public void showGear(boolean b)
        {
            gearManager.showGear(b);
        }

        public Surface getWindowSurface() {
            return gearManager.getWindowSurface();
        }

        //============================================================================================================
        //Native methods
        //============================================================================================================

        public native void nativeExitVR();
        public native void nativeEnterVR();
        public native void nativePause();
        public native void nativeResume();
}
