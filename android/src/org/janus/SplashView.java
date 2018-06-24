package org.janus;

import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

class SplashView extends SurfaceView implements SurfaceHolder.Callback
{
    private Timer timer;

    private boolean stopped = false;
    private boolean vr_mode_enabled = true;

    private float time = 0.0f;

    private Bitmap background = null;
    private Bitmap logo = null;
    private float progress = 0;
    private float temp_progress = 0;

    public void setBitmaps(Context context, String b, String l)
    {
        //Set bitmap for both states
        InputStream istr = null;
        background = null;
        try {
            istr = context.getAssets().open(b);
            background = BitmapFactory.decodeStream(istr);
            //b.recycle();
        } catch (final IOException e) {
            background = null;
        } finally {
            if (istr != null) {
                try {
                    istr.close();
                } catch (IOException ignored) {
                }
            }
        }

        istr = null;
        logo = null;
        try {
            istr = context.getAssets().open(l);
            logo = BitmapFactory.decodeStream(istr);
        } catch (final IOException e) {
            logo = null;
        } finally {
            if (istr != null) {
                try {
                    istr.close();
                } catch (IOException ignored) {
                }
            }
        }

        timer = new Timer("SplashViewWatcher", false);
        // Schedule task to start immeditaly and re-fire every 50 ms
        timer.scheduleAtFixedRate(new SplashViewTask(), 0, 50);
    }

    public SplashView(Context context)
    {
        super(context);
        setId(View.generateViewId());
        getHolder().addCallback(this);
    }

    public SplashView(Context context, AttributeSet attributes, int style)
    {
        super(context, attributes, style);
        setId(View.generateViewId());
        getHolder().addCallback(this);
    }

    public SplashView(Context context, AttributeSet attributes)
    {
        super(context, attributes);
        setId(View.generateViewId());
        getHolder().addCallback(this);
    }

    public void setProgress(int i)
    {
        progress = (float) i;
    }

    public void setVRModeEnabled(boolean b)
    {
        vr_mode_enabled = b;
    }

    public void stop()
    {
        stopped = true;

        timer.cancel();
        timer.purge();
    }

    public void start()
    {
        stopped = false;
    }

    public void drawSplash()
    {
        if (!stopped && getHolder().getSurface().isValid() && background != null && logo != null)
        {
            Canvas myCanvas = this.getHolder().lockCanvas();

            if (myCanvas == null) return;

            myCanvas.drawColor(Color.BLACK);
            Paint colors = new Paint();
            colors.setAntiAlias(true);
            colors.setFilterBitmap(true);
            colors.setDither(true);

            //Preserve aspect ratio
            //Draw background
            float h = getHeight();
            float w = getWidth();
            float screen_ratio = w/h;
            float background_ratio = background.getWidth()/background.getHeight();

            Rect src;
            Rect dest = new Rect(0,0,(int) w, (int) h);

            if (!vr_mode_enabled){
                if (screen_ratio > background_ratio){
                    float crop = (background.getHeight() - background.getWidth() * h/w) / 2.0f;
                    src = new Rect(0,(int)(crop),background.getWidth(),(int)(background.getHeight()-(crop)));
                }
                else if (screen_ratio < background_ratio){
                    float crop = (background.getWidth() - background.getHeight() * w/h) / 2.0f;
                    src = new Rect((int)(crop),0,(int)(background.getWidth()-(crop)),background.getHeight());
                }
                else{
                    src = new Rect(0,0,background.getWidth(),background.getHeight());
                }
                myCanvas.drawBitmap(background, src, dest, colors);

                //Draw pulsing logo
                float logo_length = h/5.0f;
                time += 5.0f;
                float pulse = (float) (0.025f*Math.sin(Math.toRadians(time)) * logo_length);
                src = new Rect(0,0,logo.getWidth(), logo.getHeight());
                dest = new Rect((int) (w/2.0f - logo_length/2.0f - pulse),
                                (int) (h/5.0f - pulse),
                                (int) (w/2.0f + logo_length/2.0f + pulse),
                                (int) (h/5.0f + logo_length + pulse));
                myCanvas.drawBitmap(logo, src, dest, colors);

                //Draw progress bar (62BD6C)
                colors = new Paint();
                colors.setColor(0xFF2F363B);
                colors.setStrokeWidth(h/25.0f + 10.0f);
                colors.setStrokeCap(Paint.Cap.ROUND);
                colors.setAntiAlias(true);
                myCanvas.drawLine((int) (w/3.0f), (int) (4.0f*h/5.0f), 2.0f*w/3.0f, (int) (4.0f*h/5.0f), colors);

                if (progress == 0) {
                    temp_progress += 0.125f; //HACK: SHOW THE PROGRESS BAR IS MOVING
                    temp_progress = Math.min(100.0f, temp_progress);
                }
                else {
                    temp_progress=progress;
                }

                float p = temp_progress / 100.0f;
                colors = new Paint();
                colors.setColor(0xFF62BD6C);
                colors.setStrokeWidth(h/25.0f);
                colors.setStrokeCap(Paint.Cap.ROUND);
                colors.setAntiAlias(true);
                myCanvas.drawLine((int) (w/3.0f), (int) (4.0f*h/5.0f), Math.min((int) (w/3.0f + p*w/3.0f), 2.0f*w/3.0f), (int) (4.0f*h/5.0f), colors);
            }

            getHolder().unlockCanvasAndPost(myCanvas);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
    }

    public class SplashViewTask extends TimerTask {
        @Override
        public void run() {
            drawSplash();
        }
    }
}
