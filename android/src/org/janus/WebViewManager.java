package org.janus;

import java.lang.Thread;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Arrays;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import android.R;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.ColorMatrix;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager.LayoutParams;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebView.HitTestResult;
import android.webkit.WebViewClient;
import android.webkit.ValueCallback;
import android.widget.FrameLayout;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

//import org.mozilla.gecko.gfx.CompositorController;
//import org.mozilla.gecko.gfx.GeckoDisplay;
//import org.mozilla.geckoview.GeckoView;
//import org.mozilla.geckoview.GeckoRuntime;
//import org.mozilla.geckoview.GeckoRuntimeSettings;
//import org.mozilla.geckoview.GeckoResponse;
//import org.mozilla.geckoview.GeckoSession;
//import org.mozilla.geckoview.GeckoSessionSettings;

public class WebViewManager
{
    private boolean use_gecko = false;
    private Context context;
    private boolean paused = false;
    private final float[] rgbSwapTransform =
        {
            0,  0,  1f, 0,  0,
            0,  1f, 0,  0,  0,
            1f, 0,  0,  0,  0,
            0,  0,  0,  1f, 0
        };
    private final ColorMatrix colorMatrix = new ColorMatrix(rgbSwapTransform);
    private final ColorMatrixColorFilter colorFilter = new ColorMatrixColorFilter(colorMatrix);
    private final Paint rgbSwapPaint = new Paint();

    class AWebView extends WebView
    {
        int width = 1000;
        int height = 800;

        public AWebView(Context context){
            super(context);
            super.setWillNotDraw(false);
        }

        public int computeHorizontalScrollRange(){
            return super.computeHorizontalScrollRange();
        }

        public int computeVerticalScrollRange(){
            return super.computeVerticalScrollRange();
        }

        // draw magic
        @Override
        public void onDraw( Canvas canvas ) {
            // Requires a try/catch for .lockCanvas( null )
            try {
                Bitmap b = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                Canvas c = new Canvas(b);
                //c.scale(1, -1, w/2, h/2);
                float xScale = c.getWidth() / (float)c.getWidth();
                c.scale(xScale, xScale);
                c.translate(-getScrollX(), -getScrollY());
                super.onDraw(c); // Call the WebView onDraw targetting the canvas

                if (bitmapsList.get((Integer)AWebView.this.getTag()) != null){
                    bitmapsList.get((Integer)AWebView.this.getTag()).recycle();
                }
                bitmapsList.put((Integer)AWebView.this.getTag(), b);
            } catch ( Exception e ) {
            }
            // super.onDraw( canvas ); // <- Uncomment this if you want to show the original view
        }
        public void setTextureWidth(int w) {
            width = w;
        }

        public void setTextureHeight(int h) {
            height = h;
        }
    }

//    class GeckoWebView extends GeckoView implements CompositorController.GetPixelsCallback
//    {
//        private String url;
//        private boolean canGoBack;
//        private boolean canGoForward;
//        private Lock drawLock = new ReentrantLock();

//        //private int scrollx = 0;
//        //private int scrolly = 0;

//        public GeckoWebView(Context context){
//            super(context);
//            //super.setWillNotDraw(false);
//            //super.setWillNotCacheDrawing(true);
//        }

//        public int computeHorizontalScrollRange(){
//            return super.computeHorizontalScrollRange();
//        }

//        public int computeVerticalScrollRange(){
//            return super.computeVerticalScrollRange();
//        }

//        public String getUri(){
//            return url;
//        }

//        public boolean getCanGoBack(){
//            return canGoBack;
//        }

//        public SurfaceView getSurfaceView(){
//            return super.mSurfaceView;
//        }

//        public boolean getCanGoForward(){
//            return canGoForward;
//        }

//        /*public int scrollX(int x){
//            scrollx += x;
//            return scrollx;
//        }

//        public int scrollY(int y){
//            scrolly += y;
//            return scrolly;
//        }*/

//        public void onPixelsResult(int w, int h, IntBuffer pixels){
//            if (w == 0 || h == 0) return;

//            if (drawLock.tryLock()){
//                Bitmap b1 = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
//                b1.copyPixelsFromBuffer(pixels);

//                boolean r = false;
//                if (b1 != null){
//                    /*Bitmap b = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
//                    Canvas c = new Canvas(b);
//                    c.scale(1, -1, w/2, h/2);
//                    c.drawBitmap(b1, 0, 0, rgbSwapPaint);*/

//                    if (!(bitmapsList.contains((Integer)GeckoWebView.this.getTag())) ||
//                        (bitmapsList.contains((Integer)GeckoWebView.this.getTag()) && bitmapsList.get((Integer)GeckoWebView.this.getTag()) != null && !b1.sameAs(bitmapsList.get((Integer)GeckoWebView.this.getTag())))){
//                        r = true;
//                    }
//                    if (r) {
//                        if (bitmapsList.get((Integer)GeckoWebView.this.getTag()) != null){
//                            bitmapsList.get((Integer)GeckoWebView.this.getTag()).recycle();
//                        }
//                        bitmapsList.put((Integer)GeckoWebView.this.getTag(), b1);
//                    }
//                }
//                repaintRequestedList.put((Integer)GeckoWebView.this.getTag(), r );
//                drawLock.unlock();
//            }
//        }

//        /*@Override
//        public boolean onTouchEvent(final MotionEvent event) {
//            Log.i("TOUCHEVENTACTION", MotionEvent.actionToString(event.getAction()));
//            Log.i("TOUCHEVENTAXISX", Float.toString(event.getAxisValue(MotionEvent.AXIS_HSCROLL)));
//            Log.i("TOUCHEVENTAXISY", Float.toString(event.getAxisValue(MotionEvent.AXIS_VSCROLL)));
//            Log.i("TOUCHEVENTX", Float.toString(event.getX()));
//            Log.i("TOUCHEVENTY", Float.toString(event.getY()));
//            return super.onTouchEvent(event);
//        }

//        @Override
//        public boolean onHoverEvent(final MotionEvent event) {
//            Log.i("HOVEREVENTACTION", MotionEvent.actionToString(event.getAction()));
//            Log.i("HOVEREVENTAXISX", Float.toString(event.getAxisValue(MotionEvent.AXIS_HSCROLL)));
//            Log.i("HOVEREVENTAXISY", Float.toString(event.getAxisValue(MotionEvent.AXIS_VSCROLL)));
//            Log.i("HOVEREVENTX", Float.toString(event.getX()));
//            Log.i("HOVEREVENTY", Float.toString(event.getY()));
//            return super.onHoverEvent(event);
//        }

//        @Override
//        public boolean onGenericMotionEvent(final MotionEvent event) {
//            Log.i("GENERICEVENTACTION", MotionEvent.actionToString(event.getAction()));
//            Log.i("GENERICEVENTAXISX", Float.toString(event.getAxisValue(MotionEvent.AXIS_HSCROLL)));
//            Log.i("GENERICEVENTAXISY", Float.toString(event.getAxisValue(MotionEvent.AXIS_VSCROLL)));
//            Log.i("GENERICEVENTX", Float.toString(event.getX()));
//            Log.i("GENERICEVENTY", Float.toString(event.getY()));
//            return super.onGenericMotionEvent(event);
//        }*/

//        public GeckoSession.ContentDelegate createContentDelegate(){
//            return new GeckoSession.ContentDelegate(){
//                @Override
//                public void onTitleChange(GeckoSession session, String title) {
//                }

//                @Override
//                public void onFullScreen(GeckoSession session, boolean fullScreen) {
//                }

//                @Override
//                public void onContextMenu(GeckoSession session, int screenX, int screenY, String uri, @ElementType int elementType, String elementSrc) {
//                }

//                @Override
//                public void onExternalResponse(GeckoSession session, GeckoSession.WebResponseInfo response) {
//                }

//                @Override
//                public void onFocusRequest(GeckoSession geckoSession) {
//                }

//                @Override
//                public void onCloseRequest(GeckoSession geckoSession) {
//                }
//            };
//        }

//        public GeckoSession.ProgressDelegate createProgressDelegate(){
//            return new GeckoSession.ProgressDelegate(){
//                @Override
//                public void onPageStart(GeckoSession session, String url) {
//                }

//                @Override
//                public void onPageStop(GeckoSession session, boolean success) {
//                }

//                @Override
//                public void onSecurityChange(GeckoSession session,
//                                             GeckoSession.ProgressDelegate.SecurityInformation securityInfo) {
//                }
//            };
//        }

//        public GeckoSession.NavigationDelegate createNavigationDelegate(){
//            return new GeckoSession.NavigationDelegate(){
//                @Override
//                public void onLocationChange(GeckoSession session, String url) {
//                    GeckoWebView.this.url = url;

//                    int tag = ((Integer)GeckoWebView.this.getTag()).intValue();
//                    urlList.put(tag, url);
//                    urlChangedList.put(tag, true);
//                }

//                @Override
//                public void onCanGoBack(GeckoSession session, boolean canGoBack) {
//                    GeckoWebView.this.canGoBack =  canGoBack;
//                }

//                @Override
//                public void onCanGoForward(GeckoSession session, boolean canGoForward) {
//                    GeckoWebView.this.canGoForward =  canGoForward;
//                }

//                @Override
//                public void onLoadRequest(GeckoSession session, String uri, int target, GeckoResponse<Boolean> response) {
//                //public void onLoadRequest(GeckoSession session, String uri, int target, int flags, GeckoResponse<Boolean> response) {
//                    // If this is trying to load in a new tab, just load it in the current one
//                    if (target == GeckoSession.NavigationDelegate.TARGET_WINDOW_NEW) {
//                        GeckoWebView.this.getSession().loadUri(uri);
//                        response.respond(true);
//                    }

//                    // Otherwise allow the load to continue normally
//                    response.respond(false);
//                }

//                @Override
//                public void onNewSession(GeckoSession geckoSession, String s, GeckoResponse<GeckoSession> response) {
//                }
//            };
//        }

//        public GeckoSession.TrackingProtectionDelegate createTrackingProtectionDelegate(){
//            return new GeckoSession.TrackingProtectionDelegate(){
//                @Override
//                public void onTrackerBlocked(GeckoSession geckoSession, String s, int i) {
//                }
//            };
//        }

//        public GeckoSession.PromptDelegate createPromptDelegate(){
//            return new GeckoSession.PromptDelegate(){
//                @Override
//                public void onAlert(final GeckoSession session, final String title, final String msg,
//                                  final AlertCallback callback) {
//                }

//                @Override
//                public void onButtonPrompt(final GeckoSession session, final String title,
//                                            final String msg, final String[] btnMsg,
//                                            final ButtonCallback callback) {
//                }

//                @Override
//                public void onTextPrompt(final GeckoSession session, final String title,
//                                          final String msg, final String value,
//                                          final TextCallback callback) {
//                }

//                @Override
//                public void onAuthPrompt(GeckoSession geckoSession, String s, String s1, AuthOptions authOptions, final AuthCallback authCallback) {
//                }

//                @Override
//                public void onChoicePrompt(final GeckoSession session, final String title,
//                                            final String msg, final int type,
//                                            final Choice[] choices, final ChoiceCallback callback) {
//                }

//                @Override
//                public void onColorPrompt(final GeckoSession session, final String title,
//                                           final String value, final TextCallback callback) {
//                }

//                @Override
//                public void onDateTimePrompt(final GeckoSession session, final String title,
//                                              final int type, final String value, final String min,
//                                              final String max, final TextCallback callback) {
//                }

//                @Override
//                public void onFilePrompt(GeckoSession session, String title, int type, String[] mimeTypes, FileCallback callback) {
//                }
//            };
//        }
//    }

    protected ArrayList<View> webViewsList = new ArrayList<View>();
//    protected Hashtable<Integer, GeckoSession> sessionsList = new Hashtable<Integer, GeckoSession>();
    protected Hashtable<Integer, Lock> updatesEnabledList = new Hashtable<Integer, Lock>();
    protected Hashtable<Integer, Bitmap> bitmapsList = new Hashtable<Integer, Bitmap>();
    protected Hashtable<Integer, Integer> texturesList = new Hashtable<Integer, Integer>();
    protected Hashtable<Integer, Integer> widthList = new Hashtable<Integer, Integer>();
    protected Hashtable<Integer, Integer> heightList = new Hashtable<Integer, Integer>();
    protected Hashtable<Integer, Boolean> repaintRequestedList = new Hashtable<Integer, Boolean>();
    protected Hashtable<Integer, String> hitTestsList = new Hashtable<Integer, String>();
    protected Hashtable<Integer, Integer> scrollXList = new Hashtable<Integer, Integer>();
    protected Hashtable<Integer, Integer> scrollYList = new Hashtable<Integer, Integer>();
    protected Hashtable<Integer, Boolean> scrollRequestedList = new Hashtable<Integer, Boolean>();
    protected Hashtable<Integer, String> urlList = new Hashtable<Integer, String>();
    protected Hashtable<Integer, Boolean> urlChangedList = new Hashtable<Integer, Boolean>();
    protected Hashtable<Integer, Integer> horizontalRangeList = new Hashtable<Integer, Integer>();
    protected Hashtable<Integer, Integer> verticalRangeList = new Hashtable<Integer, Integer>();
    protected Hashtable<Integer, Lock> webViewLockList = new Hashtable<Integer, Lock>();
    protected Hashtable<Integer, Lock> hitTestLockList = new Hashtable<Integer, Lock>();

//    private static volatile GeckoRuntime geckoRuntime;

    protected View findWebViewByTag(int tag) {
        View webViewRes = null;
        for (int i = 0; i < webViewsList.size(); i++) {
            View wv = (View) webViewsList.get(i);
            if (((Integer)wv.getTag()).intValue() == tag) {
                webViewRes = wv;
                break;
            }
        }

        return webViewRes;
    }

    public WebViewManager(Context c) {
        this.context = c;
        AWebView.enableSlowWholeDocumentDraw();
        rgbSwapPaint.setColorFilter(colorFilter);

//        if (geckoRuntime == null) {
//            final GeckoRuntimeSettings.Builder runtimeSettingsBuilder =
//                    new GeckoRuntimeSettings.Builder();
//            runtimeSettingsBuilder.useContentProcessHint(true);
//            geckoRuntime = GeckoRuntime.create(context.getApplicationContext(), runtimeSettingsBuilder.build());
//        }
    }

    public void setUseGeckoWebView(boolean b) {
        use_gecko = false; //b;
    }

    public void createNewWebView(int tag) {
        Message msg = new Message();
        msg.what = tag;

        createNewWebViewHandler.sendMessage(msg);
    }

    protected Handler createNewWebViewHandler = new Handler() {
        @SuppressLint("SetJavaScriptEnabled")
        public void handleMessage(Message msg) {
            Lock l = new ReentrantLock();
            l.lock();
            try{
                if (!use_gecko){
                    webViewLockList.put(msg.what, l);

                    AWebView webView = new AWebView(context);
                    webView.setTag(msg.what);

                    webView.getSettings().setJavaScriptEnabled(true);
                    webView.getSettings().setBuiltInZoomControls( true );
                    webView.getSettings().setUseWideViewPort(true);
                    webView.getSettings().setLoadWithOverviewMode(true);
                    //webView.setScrollBarStyle(WebView.SCROLLBARS_OUTSIDE_OVERLAY);
                    //webView.setScrollbarFadingEnabled(false);
                    webView.getSettings().setDomStorageEnabled(true);
                    webView.getSettings().setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
                    webView.getSettings().setMediaPlaybackRequiresUserGesture(false);
                    //webView.getSettings().setOffscreenPreRaster(true);
                    webView.getSettings().setUserAgentString("Desktop"); //Android
                    webView.setWebViewClient(new WebViewClient() {
                        @Override
                        public boolean shouldOverrideUrlLoading(WebView view, String url) {
                            return false;
                        }

                        @Override
                        public void onPageFinished(WebView view, String url) {
                            Integer tag = (Integer) view.getTag();
                            urlList.put(tag, view.getUrl());
                            urlChangedList.put(tag, true);
                        }
                    });
                    webView.setWebChromeClient(new WebChromeClient() {
                        //@Override
                        //public void onShowCustomView(View view, WebChromeClient.CustomViewCallback callback) {
                        //    Log.i("SHOWINGCUSTOMVIEW","Here");
                        //}
                    });
                    webView.setBackgroundColor(Color.WHITE); //TRANSPARENT,BLACK
                    webView.setVisibility(View.VISIBLE);
                    //webView.setDrawingCacheQuality(View.DRAWING_CACHE_QUALITY_LOW);
                    //webView.setLayerType(View.LAYER_TYPE_HARDWARE, rgbSwapPaint);

                    webViewsList.add(webView);

                    updatesEnabledList.put(msg.what, new ReentrantLock());
                    repaintRequestedList.put(msg.what, false);
                    urlChangedList.put(msg.what, false);
                    scrollRequestedList.put(msg.what, false);
                    widthList.put(msg.what, 1000);
                    heightList.put(msg.what, 800);
                    scrollXList.put(msg.what, 0);
                    scrollYList.put(msg.what, 0);
                    urlList.put(msg.what, "");
                    hitTestsList.put(msg.what,"");
                    hitTestLockList.put(msg.what, new ReentrantLock());

                    updateWebView(((Integer)webView.getTag()).intValue());
                }
//                else{
//                    webViewLockList.put(msg.what, l);

//                    GeckoWebView webView = new GeckoWebView(context);
//                    webView.setTag(msg.what);
//                    webView.setVisibility(View.INVISIBLE);

//                    final GeckoSessionSettings settings = new GeckoSessionSettings();
//                    settings.setBoolean(GeckoSessionSettings.USE_MULTIPROCESS, true);
//                    settings.setBoolean(GeckoSessionSettings.USE_PRIVATE_MODE, true);
//                    settings.setBoolean(GeckoSessionSettings.USE_DESKTOP_MODE, true);
//                    settings.setBoolean(GeckoSessionSettings.USE_TRACKING_PROTECTION, true);

//                    GeckoSession geckoSession = new GeckoSession(settings);

//                    geckoRuntime.getSettings().setJavaScriptEnabled(true);
//                    geckoRuntime.getSettings().setWebFontsEnabled(true);

//                    int categories = GeckoSession.TrackingProtectionDelegate.CATEGORY_SOCIAL +
//                        GeckoSession.TrackingProtectionDelegate.CATEGORY_AD +
//                        GeckoSession.TrackingProtectionDelegate.CATEGORY_ANALYTIC +
//                        GeckoSession.TrackingProtectionDelegate.CATEGORY_CONTENT;

//                    if (geckoSession != null) {
//                        geckoSession.enableTrackingProtection(categories);
//                    }

//                    geckoSession.setContentDelegate(webView.createContentDelegate());
//                    geckoSession.setProgressDelegate(webView.createProgressDelegate());
//                    geckoSession.setNavigationDelegate(webView.createNavigationDelegate());
//                    geckoSession.setTrackingProtectionDelegate(webView.createTrackingProtectionDelegate());
//                    geckoSession.setPromptDelegate(webView.createPromptDelegate());

//                    geckoSession.open(geckoRuntime);
//                    geckoSession.getPanZoomController().setIsLongpressEnabled(false);
//                    webView.setSession(geckoSession);

//                    //geckoSession.loadUri("about:blank");

//                    webViewsList.add(webView);

//                    updatesEnabledList.put(msg.what, new ReentrantLock());
//                    repaintRequestedList.put(msg.what, false);
//                    urlChangedList.put(msg.what, false);
//                    scrollRequestedList.put(msg.what, false);
//                    widthList.put(msg.what, 1000);
//                    heightList.put(msg.what, 800);
//                    scrollXList.put(msg.what, 0);
//                    scrollYList.put(msg.what, 0);
//                    urlList.put(msg.what, "");
//                    hitTestsList.put(msg.what,"");
//                    hitTestLockList.put(msg.what, new ReentrantLock());

//                    //Log.i("CREATINGGECKO", "WEBVIEW");
//                }
            }
            finally{
                l.unlock();
            }
        }
    };

    public void updateWebView(int tag) {
        Message msg = new Message();
        msg.what = tag;

        if (!updateWebViewHandler.hasMessages(msg.what)){
            updateWebViewHandler.sendMessage(msg);
        }
    }

    protected Handler updateWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            while (paused){
                try {
                    Thread.sleep(100);
                } catch (Exception e) {
                }
            }

            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                updateWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View webView = findWebViewByTag(msg.what);

                if (webView != null)
                {
                    if (!use_gecko) {

                        //if (updatesEnabledList.get(msg.what).tryLock()){
                            /*Bitmap b1 = null;

                            webView.buildDrawingCache();
                            b1 = webView.getDrawingCache();

                            boolean r = false;
                            if (b1 != null){
                                Bitmap b = b1.copy(b1.getConfig(), true);

                                webView.destroyDrawingCache();
                                if (b != null) {
                                    if (!(bitmapsList.contains(msg.what)) ||
                                        (bitmapsList.contains(msg.what) && bitmapsList.get(msg.what) != null && !b.sameAs(bitmapsList.get(msg.what)))){
                                        if (bitmapsList.get(msg.what) != null){
                                            bitmapsList.get(msg.what).recycle();
                                        }
                                        bitmapsList.put(msg.what, b);
                                        r = true;
                                    }
                                }
                            }
                            repaintRequestedList.put(msg.what, r );*/
                            //updatesEnabledList.get(msg.what).unlock();
                        //}

                        HitTestResult h = ((AWebView) webView).getHitTestResult();
                        if (h.getType() == WebView.HitTestResult.SRC_ANCHOR_TYPE ||
                            h.getType() == WebView.HitTestResult.SRC_IMAGE_ANCHOR_TYPE ||
                            h.getType() == WebView.HitTestResult.IMAGE_TYPE){

                                if(h.getType()==HitTestResult.SRC_IMAGE_ANCHOR_TYPE) {
                                    //Workaround; For images inside anchor tags, get link URL, NOT image
                                    hitTestsList.put((Integer)((AWebView) webView).getTag(), "");
                                    ((AWebView) webView).setWebViewClient(new WebViewClient(){
                                        @Override
                                        public boolean shouldOverrideUrlLoading(WebView view, String url) {
                                            //Log.i("OVERRIDINGURL",url);
                                            hitTestLockList.get((Integer)view.getTag()).lock();

                                            hitTestsList.put((Integer)view.getTag(), url);

                                            view.setWebViewClient(new WebViewClient() {
                                                @Override
                                                public boolean shouldOverrideUrlLoading(WebView view, String url) {
                                                    return false;
                                                }

                                                @Override
                                                public void onPageFinished(WebView view, String url) {
                                                    Integer tag = (Integer) view.getTag();
                                                    urlList.put(tag, view.getUrl());
                                                    urlChangedList.put(tag, true);
                                                }
                                            });

                                            hitTestLockList.get((Integer)view.getTag()).unlock();

                                            return true;
                                        }
                                    });

                                    KeyEvent event1 = new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_DPAD_CENTER);
                                    webView.dispatchKeyEvent(event1);
                                    KeyEvent event2 = new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_DPAD_CENTER);
                                    webView.dispatchKeyEvent(event2);

                                    hitTestLockList.get(msg.what).lock();
                                    if (!hitTestsList.containsKey((Integer)webView.getTag()) || hitTestsList.get((Integer)webView.getTag()) == ""){
                                        hitTestsList.put(msg.what, h.getExtra());
                                    }
                                    hitTestLockList.get(msg.what).unlock();
                                }
                                else{
                                    //Log.i("HITTESTRESULT", h.getExtra());
                                    hitTestsList.put(msg.what, h.getExtra());
                                }
                        }

                        horizontalRangeList.put(msg.what, ((AWebView) webView).computeHorizontalScrollRange());
                        verticalRangeList.put(msg.what, ((AWebView) webView).computeVerticalScrollRange());
                    }
                    /*else {
                        GeckoSession s = ((GeckoWebView) webView).getSession();
                        s.getCompositorController().getPixels((GeckoWebView) webView);
                        horizontalRangeList.put(msg.what, ((GeckoWebView) webView).computeHorizontalScrollRange());
                        verticalRangeList.put(msg.what, ((GeckoWebView) webView).computeVerticalScrollRange());
                    }*/
                }
                this.removeMessages(msg.what);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
                updateWebView(msg.what);
            }
        }
    };

    public void removeWebView(int tag) {
        Message msg = new Message();
        msg.what = tag;

        removeWebViewHandler.sendMessage(msg);
    }

    protected Handler removeWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                removeWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View webView = findWebViewByTag(msg.what);
                if (webView != null) {
                    /*if (use_gecko) {
                        GeckoSession session = ((GeckoWebView) webView).releaseSession();
                        session.close();
                    }*/

                    webViewsList.remove(context);
                    FrameLayout mainLayout = (FrameLayout) ((Activity) context).findViewById(R.id.content);
                    mainLayout.removeView(webView);
                }
                if (bitmapsList.contains(msg.what) && bitmapsList.get(msg.what) != null){
                    bitmapsList.get(msg.what).recycle();
                    bitmapsList.remove(msg.what);
                }
                if (repaintRequestedList.contains(msg.what)){
                    repaintRequestedList.remove(msg.what);
                }
                if (scrollRequestedList.contains(msg.what)){
                    scrollRequestedList.remove(msg.what);
                }
                if (urlChangedList.contains(msg.what)){
                    urlChangedList.remove(msg.what);
                }
                if (widthList.contains(msg.what)){
                    widthList.remove(msg.what);
                }
                if (heightList.contains(msg.what)){
                    heightList.remove(msg.what);
                }
                if (scrollXList.contains(msg.what)){
                    scrollXList.remove(msg.what);
                }
                if (scrollYList.contains(msg.what)){
                    scrollYList.remove(msg.what);
                }
                if (urlList.contains(msg.what)){
                    urlList.remove(msg.what);
                }
                if (horizontalRangeList.contains(msg.what)){
                    horizontalRangeList.remove(msg.what);
                }
                if (verticalRangeList.contains(msg.what)){
                    verticalRangeList.remove(msg.what);
                }
            }
            finally{
                if (webViewLockList.containsKey(msg.what) && webViewLockList.containsKey(msg.what)){
                    Lock l = webViewLockList.get(msg.what);
                    webViewLockList.remove(msg.what);

                    createNewWebViewHandler.removeMessages(msg.what);
                    updateWebViewHandler.removeMessages(msg.what);
                    moveWebViewHandler.removeMessages(msg.what);
                    resizeWebViewHandler.removeMessages(msg.what);
                    attachWebViewToMainLayoutHandler.removeMessages(msg.what);
                    loadUrlAtWebViewHandler.removeMessages(msg.what);
                    goBackOrForwardAtWebViewHandler.removeMessages(msg.what);
                    reloadWebViewHandler.removeMessages(msg.what);
                    stopWebViewHandler.removeMessages(msg.what);
                    loadHtmlAtWebViewHandler.removeMessages(msg.what);
                    evaluateJavaScriptAtWebViewHandler.removeMessages(msg.what);
                    scrollByWebViewHandler.removeMessages(msg.what);
                    scrollToWebViewHandler.removeMessages(msg.what);
                    mousePressWebViewHandler.removeMessages(msg.what);
                    mouseMoveWebViewHandler.removeMessages(msg.what);
                    mouseReleaseWebViewHandler.removeMessages(msg.what);
                    removeWebViewHandler.removeMessages(msg.what);

                    l.unlock();
                }
            }
        }
    };

    public void moveWebView(int tag, int x, int y) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = x;
        msg.arg2 = y;

        moveWebViewHandler.sendMessage(msg);
    }

    protected Handler moveWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                moveWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToMove = null;
                viewToMove = findWebViewByTag(msg.what);

                if (viewToMove != null) {
                        FrameLayout.LayoutParams params = (android.widget.FrameLayout.LayoutParams) viewToMove
                                        .getLayoutParams();
                        params.leftMargin = msg.arg1;
                        params.topMargin = msg.arg2;
                        viewToMove.setLayoutParams(params);
                }
                //repaintRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void resizeWebView(int tag, int w, int h) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = w;
        msg.arg2 = h;

        resizeWebViewHandler.sendMessage(msg);
    }

    protected Handler resizeWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                resizeWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToResize = null;
                viewToResize = findWebViewByTag(msg.what);

                if (viewToResize != null) {
                        FrameLayout.LayoutParams params = (android.widget.FrameLayout.LayoutParams) viewToResize
                                        .getLayoutParams();
                        params.width = msg.arg1;
                        params.height = msg.arg2;
                        viewToResize.setLayoutParams(params);

                        ((AWebView) viewToResize).setTextureWidth(msg.arg1);
                        ((AWebView) viewToResize).setTextureHeight(msg.arg2);

                        widthList.put(msg.what, msg.arg1);
                        heightList.put(msg.what, msg.arg2);
                }
                //repaintRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void attachWebViewToMainLayout(int tag) {
        Message msg = new Message();
        msg.what = tag;

        attachWebViewToMainLayoutHandler.sendMessage(msg);
    }

    protected Handler attachWebViewToMainLayoutHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                attachWebViewToMainLayoutHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToAttach = null;
                viewToAttach = findWebViewByTag(msg.what);

                if (viewToAttach != null) {
                        FrameLayout mainLayout = (FrameLayout) ((Activity) context).findViewById(R.id.content);
                        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(1000, 800); //Default params
                        params.leftMargin = 0;
                        params.topMargin = 0;

                        mainLayout.addView(viewToAttach, params);
                }

                //updateWebView(msg.what);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void loadUrlAtWebView(int tag, String url) {
        Message msg = new Message();
        msg.what = tag;
        msg.obj = url;

        loadUrlAtWebViewHandler.sendMessage(msg);
    }

    protected Handler loadUrlAtWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                loadUrlAtWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                if (!use_gecko) {
                    AWebView webView = (AWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                        String s = "";
                        if (!(msg.obj.toString()).contains(":")) s += "http://";
                        //webView.clearCache(true);
                        webView.loadUrl(s + msg.obj.toString());

                        //Log.i("url", msg.obj.toString());
                    }

                    urlList.put(msg.what, webView.getUrl());
                    urlChangedList.put(msg.what, true);
                }
                /*else {
                    GeckoWebView webView = (GeckoWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                        String s = "";
                        if (!(msg.obj.toString()).contains(":")) s += "http://";
                        //webView.clearCache(true);
                        webView.getSession().loadUri(s + msg.obj.toString());

                        //Log.i("url", msg.obj.toString());
                    }
                }*/
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void goBackOrForwardAtWebView(int tag, int steps) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = steps;

        goBackOrForwardAtWebViewHandler.sendMessage(msg);
    }

    protected Handler goBackOrForwardAtWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                goBackOrForwardAtWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                if (!use_gecko){
                    AWebView webView = (AWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                        if (webView.canGoBackOrForward(msg.arg1))
                        {
                            webView.goBackOrForward(msg.arg1);
                        }
                    }

                    urlList.put(msg.what, webView.getUrl());
                    urlChangedList.put(msg.what, true);
                }
                /*else{
                    GeckoWebView webView = (GeckoWebView) findWebViewByTag(msg.what);
                    int steps = msg.arg1;
                    if (webView != null) {
                        while (steps != 0) {
                            if (steps > 0) {
                                if (webView.getCanGoForward())
                                {
                                    webView.getSession().goForward();
                                }
                                steps--;
                            }
                            if (steps < 0){
                                if (webView.getCanGoBack())
                                {
                                    webView.getSession().goBack();
                                }
                                steps++;
                            }
                        }
                    }
                }*/
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void reloadWebView(int tag) {
        Message msg = new Message();
        msg.what = tag;

        reloadWebViewHandler.sendMessage(msg);
    }

    protected Handler reloadWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                reloadWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                if (!use_gecko){
                    AWebView webView = (AWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                        webView.reload();
                    }

                    urlList.put(msg.what, webView.getUrl());
                    urlChangedList.put(msg.what, true);
                }
                /*else{
                    GeckoWebView webView = (GeckoWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                            webView.getSession().reload();
                    }
                }*/
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void stopWebView(int tag) {
        Message msg = new Message();
        msg.what = tag;

        stopWebViewHandler.sendMessage(msg);
    }

    protected Handler stopWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                stopWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                if (!use_gecko){
                    AWebView webView = (AWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                        webView.stopLoading();
                    }

                    urlList.put(msg.what, webView.getUrl());
                    urlChangedList.put(msg.what, true);
                }
                /*else{
                    GeckoWebView webView = (GeckoWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                            webView.getSession().stop();
                    }
                }*/
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void loadHtmlAtWebView(int tag, String html) {
        Message msg = new Message();
        msg.what = tag;
        msg.obj = html;

        loadHtmlAtWebViewHandler.sendMessage(msg);
    }

    protected Handler loadHtmlAtWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                loadHtmlAtWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                if (!use_gecko){
                    AWebView webView = (AWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                            webView.loadData(msg.obj.toString(), "text/html; charset=UTF-8", "UTF-8");
                    }
                }
                /*else{
                    GeckoWebView webView = (GeckoWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                        webView.getSession().loadString(msg.obj.toString(), "text/html");
                    }

                }*/
                //repaintRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void evaluateJavaScriptAtWebView(int tag, String javascript) {
        Message msg = new Message();
        msg.what = tag;
        msg.obj = javascript;

        evaluateJavaScriptAtWebViewHandler.sendMessage(msg);
    }

    protected Handler evaluateJavaScriptAtWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                evaluateJavaScriptAtWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                if (!use_gecko){
                    AWebView webView = (AWebView) findWebViewByTag(msg.what);
                    if (webView != null) {
                        webView.evaluateJavascript(msg.obj.toString(), new ValueCallback<String>() {
                            @Override
                            public void onReceiveValue(String s) {
                                //Log.d("EvaluatedJS", s);
                            }
                        });
                    }
                }
                else{
                    //TODO
                }
                //repaintRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void setUpdatesEnabledWebView(int tag, boolean b) {
        if (updatesEnabledList.containsKey(tag) && updatesEnabledList.get(tag) != null) {
            if (!b)
                updatesEnabledList.get(tag).lock();
            else
                updatesEnabledList.get(tag).unlock();
        }
    }

    public Bitmap getBitmap(int tag) {
        if (bitmapsList.containsKey(tag) && bitmapsList.get(tag) != null)
        {
            return bitmapsList.get(tag);
        }
        return null;
    }

    public int getTexture(int tag) {
        if (texturesList.containsKey(tag) && texturesList.get(tag) != null)
        {
            return texturesList.get(tag);
        }
        return 0;
    }

    public String getHitTestContent(int tag) {
        if (hitTestsList.containsKey(tag))
        {
            return hitTestsList.get(tag);
        }
        return "";
    }

    public int getHorizontalScrollRange(int tag) {
        if (horizontalRangeList.containsKey(tag))
        {
            return horizontalRangeList.get(tag);
        }
        return 0;
    }

    public int getVerticalScrollRange(int tag) {
        if (verticalRangeList.containsKey(tag))
        {
            return verticalRangeList.get(tag);
        }
        return 0;
    }

    public int getWidthWebView(int tag) {
        if (widthList.containsKey(tag))
        {
            return widthList.get(tag);
        }
        return 0;
    }

    public int getHeightWebView(int tag) {
        if (heightList.containsKey(tag))
        {
            return heightList.get(tag);
        }
        return 0;
    }

    public int getScrollXWebView(int tag) {
        if (scrollXList.containsKey(tag))
        {
            return scrollXList.get(tag);
        }
        return 0;
    }

    public int getScrollYWebView(int tag) {
        if (scrollYList.containsKey(tag))
        {
            return scrollYList.get(tag);
        }
        return 0;
    }

    public String getURLWebView(int tag) {
        if (urlList.containsKey(tag))
        {
            return urlList.get(tag);
        }
        return "";
    }

    public void scrollByWebView(int tag, int x, int y) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = x;
        msg.arg2 = y;

        scrollByWebViewHandler.sendMessage(msg);
    }

    protected Handler scrollByWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                scrollByWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToScroll = null;
                viewToScroll = findWebViewByTag(msg.what);

                if (!use_gecko) {
                    viewToScroll.scrollBy(msg.arg1, msg.arg2);
                }
//                else {
//                    MotionEvent.PointerProperties mProperties[] = new MotionEvent.PointerProperties[1];
//                    mProperties[0] = new MotionEvent.PointerProperties();
//                    mProperties[0].id = 0;
//                    mProperties[0].toolType = MotionEvent.TOOL_TYPE_FINGER; // TOOL_TYPE_FINGER;
//                    MotionEvent.PointerCoords mCoords[] = new MotionEvent.PointerCoords[1];
//                    mCoords[0] = new MotionEvent.PointerCoords();
//                    mCoords[0].toolMajor = 2;
//                    mCoords[0].toolMinor = 2;
//                    mCoords[0].touchMajor = 2;
//                    mCoords[0].touchMinor = 2;
//                    mCoords[0].setAxisValue(MotionEvent.AXIS_HSCROLL, (float)(msg.arg1));
//                    mCoords[0].setAxisValue(MotionEvent.AXIS_VSCROLL, (float)(msg.arg2));

//                    MotionEvent e = MotionEvent.obtain(
//                            /*mDownTime*/ SystemClock.uptimeMillis(),
//                            /*eventTime*/ SystemClock.uptimeMillis(),
//                            /*action*/ MotionEvent.ACTION_SCROLL,
//                            /*pointerCount*/ 1,
//                            /*pointerProperties*/ mProperties,
//                            /*pointerCoords*/ mCoords,
//                            /*metaState*/ 0,
//                            /*buttonState*/ 0,
//                            /*xPrecision*/ 0,
//                            /*yPrecision*/ 0,
//                            /*deviceId*/ 0,
//                            /*edgeFlags*/ 0,
//                            /*source*/ InputDevice.SOURCE_TOUCHSCREEN, // SOURCE_TOUCHSCREEN,
//                            /*flags*/ 0);

//                    ((GeckoWebView) viewToScroll).getSession().getPanZoomController().setScrollFactor(1.0f);
//                    ((GeckoWebView) viewToScroll).getSession().getPanZoomController().onMotionEvent(e);

//                    mCoords[0].setAxisValue(MotionEvent.AXIS_VSCROLL, 0.0f);
//                    mCoords[0].setAxisValue(MotionEvent.AXIS_HSCROLL, 0.0f);

//                    /*Rect r = new Rect();
//                    ((GeckoWebView) viewToScroll).getSession().getSurfaceBounds(r);
//                    Log.i("SCROLLFACTOR", Float.toString(((GeckoWebView) viewToScroll).getSession().getPanZoomController().getScrollFactor()));
//                    Log.i("SCROLLX", Integer.toString(msg.arg1));
//                    Log.i("SCROLLY", Integer.toString(msg.arg2));
//                    Log.i("BOUNDleft", Integer.toString(r.left));
//                    Log.i("BOUNDtop", Integer.toString(r.top));
//                    Log.i("BOUNDright", Integer.toString(r.right));
//                    Log.i("BOUNDbottom", Integer.toString(r.bottom));*/
//                }

                scrollXList.put(msg.what, viewToScroll.getScrollX());
                scrollYList.put(msg.what, viewToScroll.getScrollY());

                //repaintRequestedList.put(msg.what, true);
                scrollRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void scrollToWebView(int tag, int x, int y) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = x;
        msg.arg2 = y;

        scrollToWebViewHandler.sendMessage(msg);
    }

    protected Handler scrollToWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                scrollToWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToScroll = null;
                viewToScroll = findWebViewByTag(msg.what);

                if (viewToScroll != null) {
                    if (!use_gecko) {
                        viewToScroll.scrollTo(msg.arg1, msg.arg2);
                    }
//                    else {
//                        MotionEvent.PointerProperties mProperties[] = new MotionEvent.PointerProperties[1];
//                        mProperties[0] = new MotionEvent.PointerProperties();
//                        mProperties[0].id = 0;
//                        mProperties[0].toolType = MotionEvent.TOOL_TYPE_FINGER; // TOOL_TYPE_FINGER;
//                        MotionEvent.PointerCoords mCoords[] = new MotionEvent.PointerCoords[1];
//                        mCoords[0] = new MotionEvent.PointerCoords();
//                        mCoords[0].toolMajor = 2;
//                        mCoords[0].toolMinor = 2;
//                        mCoords[0].touchMajor = 2;
//                        mCoords[0].touchMinor = 2;
//                        mCoords[0].setAxisValue(MotionEvent.AXIS_HSCROLL, msg.arg1);
//                        mCoords[0].setAxisValue(MotionEvent.AXIS_VSCROLL, msg.arg2);

//                        MotionEvent e = MotionEvent.obtain(
//                                /*mDownTime*/ SystemClock.uptimeMillis(),
//                                /*eventTime*/ SystemClock.uptimeMillis(),
//                                /*action*/ MotionEvent.ACTION_SCROLL,
//                                /*pointerCount*/ 1,
//                                /*pointerProperties*/ mProperties,
//                                /*pointerCoords*/ mCoords,
//                                /*metaState*/ 0,
//                                /*buttonState*/ 0,
//                                /*xPrecision*/ 0,
//                                /*yPrecision*/ 0,
//                                /*deviceId*/ 0,
//                                /*edgeFlags*/ 0,
//                                /*source*/ InputDevice.SOURCE_TOUCHSCREEN, // SOURCE_TOUCHSCREEN,
//                                /*flags*/ 0);

//                        ((GeckoWebView) viewToScroll).getSession().getPanZoomController().setScrollFactor(1.0f);
//                        ((GeckoWebView) viewToScroll).getSession().getPanZoomController().onMotionEvent(e);

//                        mCoords[0].setAxisValue(MotionEvent.AXIS_VSCROLL, 0.0f);
//                        mCoords[0].setAxisValue(MotionEvent.AXIS_HSCROLL, 0.0f);
//                    }
                }

                scrollXList.put(msg.what, viewToScroll.getScrollX());
                scrollYList.put(msg.what, viewToScroll.getScrollY());

                //repaintRequestedList.put(msg.what, true);
                scrollRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void mousePressWebView(int tag, int x, int y) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = x;
        msg.arg2 = y;

        mousePressWebViewHandler.sendMessage(msg);
    }

    protected Handler mousePressWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                mousePressWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToPress = null;
                viewToPress = findWebViewByTag(msg.what);

                if (viewToPress != null) {
                    if (!use_gecko) {
                        WebView webView = (WebView) viewToPress;
                        webView.setWebViewClient(new WebViewClient(){
                            @Override
                            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                                //Log.i("OVERRIDINGURL",url);
                                hitTestLockList.get((Integer)view.getTag()).lock();

                                view.setWebViewClient(new WebViewClient() {
                                    @Override
                                    public boolean shouldOverrideUrlLoading(WebView view, String url) {
                                        return false;
                                    }

                                    @Override
                                    public void onPageFinished(WebView view, String url) {
                                        Integer tag = (Integer) view.getTag();
                                        urlList.put(tag, view.getUrl());
                                        urlChangedList.put(tag, true);
                                    }
                                });

                                hitTestLockList.get((Integer)view.getTag()).unlock();
                                return true;
                            }
                        });

                        //Simulate touch press/release to get hittest
                        MotionEvent e1 = MotionEvent.obtain(
                                SystemClock.uptimeMillis(), SystemClock.uptimeMillis(),
                                MotionEvent.ACTION_DOWN, msg.arg1, msg.arg2, 0);
                        viewToPress.dispatchTouchEvent(e1);
                        MotionEvent e2 = MotionEvent.obtain(
                                SystemClock.uptimeMillis(), SystemClock.uptimeMillis(),
                                MotionEvent.ACTION_UP, msg.arg1, msg.arg2, 0);
                        viewToPress.dispatchTouchEvent(e2);

                        //Actual mouse press
                        hitTestLockList.get(msg.what).lock();

                        webView.setWebViewClient(new WebViewClient() {
                            @Override
                            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                                return false;
                            }

                            @Override
                            public void onPageFinished(WebView view, String url) {
                                Integer tag = (Integer) view.getTag();
                                urlList.put(tag, view.getUrl());
                                urlChangedList.put(tag, true);
                            }
                        });
                    //repaintRequestedList.put(msg.what, true);

                        MotionEvent e = MotionEvent.obtain(
                                SystemClock.uptimeMillis(), SystemClock.uptimeMillis(),
                                MotionEvent.ACTION_DOWN, msg.arg1, msg.arg2, 0);

                        viewToPress.dispatchTouchEvent(e);
                        hitTestLockList.get(msg.what).unlock();
                    }
//                    else {
//                        MotionEvent.PointerProperties mProperties[] = new MotionEvent.PointerProperties[1];
//                        mProperties[0] = new MotionEvent.PointerProperties();
//                        mProperties[0].id = 0;
//                        mProperties[0].toolType = MotionEvent.TOOL_TYPE_FINGER; // TOOL_TYPE_FINGER;
//                        MotionEvent.PointerCoords mCoords[] = new MotionEvent.PointerCoords[1];
//                        mCoords[0] = new MotionEvent.PointerCoords();
//                        mCoords[0].toolMajor = 2;
//                        mCoords[0].toolMinor = 2;
//                        mCoords[0].touchMajor = 2;
//                        mCoords[0].touchMinor = 2;
//                        mCoords[0].x = msg.arg1;
//                        mCoords[0].y = msg.arg2;
//                        mCoords[0].pressure = 1.0f;

//                        MotionEvent e = MotionEvent.obtain(
//                                /*mDownTime*/ SystemClock.uptimeMillis(),
//                                /*eventTime*/ SystemClock.uptimeMillis(),
//                                /*action*/ MotionEvent.ACTION_DOWN,
//                                /*pointerCount*/ 1,
//                                /*pointerProperties*/ mProperties,
//                                /*pointerCoords*/ mCoords,
//                                /*metaState*/ 0,
//                                /*buttonState*/ 0,
//                                /*xPrecision*/ 0,
//                                /*yPrecision*/ 0,
//                                /*deviceId*/ 0,
//                                /*edgeFlags*/ 0,
//                                /*source*/ InputDevice.SOURCE_TOUCHSCREEN, // SOURCE_TOUCHSCREEN,
//                                /*flags*/ 0);

//                                Rect r = new Rect();
//                                ((GeckoWebView) viewToPress).getSession().getSurfaceBounds(r);
//                                Log.i("PRESSX", Integer.toString(msg.arg1));
//                                Log.i("PRESSY", Integer.toString(msg.arg2));
//                                Log.i("BOUNDleft", Integer.toString(r.left));
//                                Log.i("BOUNDtop", Integer.toString(r.top));
//                                Log.i("BOUNDright", Integer.toString(r.right));
//                                Log.i("BOUNDbottom", Integer.toString(r.bottom));

//                        ((GeckoWebView) viewToPress).requestFocus();
//                        ((GeckoWebView) viewToPress).getSession().getPanZoomController().onTouchEvent(e);
//                    }
                }
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void mouseMoveWebView(int tag, int x, int y) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = x;
        msg.arg2 = y;

        mouseMoveWebViewHandler.sendMessage(msg);
    }

    protected Handler mouseMoveWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                mouseMoveWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToMove = null;
                viewToMove = findWebViewByTag(msg.what);

                if (viewToMove != null) {
                    if (!use_gecko){
                        MotionEvent e = MotionEvent.obtain(
                                SystemClock.uptimeMillis(), SystemClock.uptimeMillis(),
                                MotionEvent.ACTION_MOVE, msg.arg1, msg.arg2, 0);
                        viewToMove.dispatchTouchEvent(e);
                    }
//                    else {
//                        MotionEvent.PointerProperties mProperties[] = new MotionEvent.PointerProperties[1];
//                        mProperties[0] = new MotionEvent.PointerProperties();
//                        mProperties[0].id = 0;
//                        mProperties[0].toolType = MotionEvent.TOOL_TYPE_FINGER; // TOOL_TYPE_FINGER;
//                        MotionEvent.PointerCoords mCoords[] = new MotionEvent.PointerCoords[1];
//                        mCoords[0] = new MotionEvent.PointerCoords();
//                        mCoords[0].toolMajor = 2;
//                        mCoords[0].toolMinor = 2;
//                        mCoords[0].touchMajor = 2;
//                        mCoords[0].touchMinor = 2;
//                        mCoords[0].x = msg.arg1;
//                        mCoords[0].y = msg.arg2;
//                        mCoords[0].pressure = 1.0f;

//                        MotionEvent e = MotionEvent.obtain(
//                                /*mDownTime*/ SystemClock.uptimeMillis(),
//                                /*eventTime*/ SystemClock.uptimeMillis(),
//                                /*action*/ MotionEvent.ACTION_MOVE,
//                                /*pointerCount*/ 1,
//                                /*pointerProperties*/ mProperties,
//                                /*pointerCoords*/ mCoords,
//                                /*metaState*/ 0,
//                                /*buttonState*/ 0,
//                                /*xPrecision*/ 0,
//                                /*yPrecision*/ 0,
//                                /*deviceId*/ 0,
//                                /*edgeFlags*/ 0,
//                                /*source*/ InputDevice.SOURCE_TOUCHSCREEN, // SOURCE_TOUCHSCREEN,
//                                /*flags*/ 0);

//                        ((GeckoWebView) viewToMove).getSession().getPanZoomController().onTouchEvent(e);
//                    }
                }
                //repaintRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public void mouseReleaseWebView(int tag, int x, int y) {
        Message msg = new Message();
        msg.what = tag;
        msg.arg1 = x;
        msg.arg2 = y;

        mouseReleaseWebViewHandler.sendMessage(msg);
    }

    protected Handler mouseReleaseWebViewHandler = new Handler() {
        public void handleMessage(Message msg) {
            if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).lock();
            else{
                mouseReleaseWebViewHandler.sendMessage(msg);
                return;
            }
            try{
                View viewToRelease = null;
                viewToRelease = findWebViewByTag(msg.what);

                if (viewToRelease != null) {
                    if (!use_gecko){
                        MotionEvent e = MotionEvent.obtain(
                                SystemClock.uptimeMillis(), SystemClock.uptimeMillis(),
                                MotionEvent.ACTION_UP, msg.arg1, msg.arg2, 0);
                        viewToRelease.dispatchTouchEvent(e);
                    }
//                    else {
//                        MotionEvent.PointerProperties mProperties[] = new MotionEvent.PointerProperties[1];
//                        mProperties[0] = new MotionEvent.PointerProperties();
//                        mProperties[0].id = 0;
//                        mProperties[0].toolType = MotionEvent.TOOL_TYPE_MOUSE; // TOOL_TYPE_FINGER;
//                        MotionEvent.PointerCoords mCoords[] = new MotionEvent.PointerCoords[1];
//                        mCoords[0] = new MotionEvent.PointerCoords();
//                        mCoords[0].toolMajor = 2;
//                        mCoords[0].toolMinor = 2;
//                        mCoords[0].touchMajor = 2;
//                        mCoords[0].touchMinor = 2;
//                        mCoords[0].x = msg.arg1;
//                        mCoords[0].y = msg.arg2;
//                        mCoords[0].pressure = 1.0f;

//                        MotionEvent e = MotionEvent.obtain(
//                                /*mDownTime*/ SystemClock.uptimeMillis(),
//                                /*eventTime*/ SystemClock.uptimeMillis(),
//                                /*action*/ MotionEvent.ACTION_UP,
//                                /*pointerCount*/ 1,
//                                /*pointerProperties*/ mProperties,
//                                /*pointerCoords*/ mCoords,
//                                /*metaState*/ 0,
//                                /*buttonState*/ 0,
//                                /*xPrecision*/ 0,
//                                /*yPrecision*/ 0,
//                                /*deviceId*/ 0,
//                                /*edgeFlags*/ 0,
//                                /*source*/ InputDevice.SOURCE_MOUSE, // SOURCE_TOUCHSCREEN,
//                                /*flags*/ 0);

//                        ((GeckoWebView) viewToRelease).getSession().getPanZoomController().onTouchEvent(e);
//                    }
                }
                //repaintRequestedList.put(msg.what, true);
            }
            finally{
                if (webViewLockList.containsKey(msg.what)) webViewLockList.get(msg.what).unlock();
            }
        }
    };

    public boolean getRepaintRequestedAtWebView(int tag) {
        boolean b = false;
        if (repaintRequestedList.containsKey(tag))
        {
            b = repaintRequestedList.get(tag);
            if (b){
                repaintRequestedList.put(tag, false);
            }
        }
        return b;
    }

    public boolean getScrollRequestedAtWebView(int tag) {
        boolean b = false;
        if (scrollRequestedList.containsKey(tag))
        {
            b = scrollRequestedList.get(tag);
            if (b){
                scrollRequestedList.put(tag, false);
            }
        }
        return b;
    }

    public boolean getURLChangedAtWebView(int tag) {
        boolean b = false;
        if (urlChangedList.containsKey(tag))
        {
            b = urlChangedList.get(tag);
            if (b){
                urlChangedList.put(tag, false);
            }
        }
        return b;
    }

    public void setPaused(boolean b) {
        paused = b;

        for (int i = 0; i < webViewsList.size(); i++) {
            View wv = (View) webViewsList.get(i);
            if (paused){
                stopWebView(((Integer)wv.getTag()).intValue());
                if (!use_gecko){
                    ((AWebView) wv).loadUrl("about:blank");
                    ((AWebView) wv).onPause();
                }
//                else{
//                    if (wv != null) {
//                        GeckoSession session = ((GeckoWebView) wv).releaseSession();
//                        session.close();
//                        sessionsList.put(((Integer)wv.getTag()).intValue(), session);
//                    }
//                }
            }
            else {
                if (!use_gecko){
                    int tag = ((Integer)wv.getTag()).intValue();
                    ((AWebView) wv).loadUrl(urlList.get(tag));
                    ((AWebView) wv).onResume();
                }
//                else{
//                    GeckoSession s = sessionsList.get(((Integer)wv.getTag()).intValue());
//                    if (wv != null  && s != null && !((((GeckoWebView) wv).getSession()).equals(s))) {
//                        int tag = ((Integer)wv.getTag()).intValue();

//                        ((GeckoWebView) wv).releaseSession();
//                        s.open(geckoRuntime);
//                        ((GeckoWebView) wv).setSession(s);
//                        s.loadUri(urlList.get(tag));
//                        sessionsList.put(tag, null);
//                    }
//                }
                reloadWebView(((Integer)wv.getTag()).intValue());
            }
        }
    }
}
