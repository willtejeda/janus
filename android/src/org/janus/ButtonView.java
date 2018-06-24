package org.janus;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

class ButtonView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener, GestureDetector.OnGestureListener, GestureDetector.OnDoubleTapListener
{
    private float centerX;
    private float centerY;
    private float baseRadius;

    private boolean doubleTapEnabled = false;
    private boolean stateChanged = false;

    private Bitmap pressedState = null;
    private Bitmap unpressedState = null;
    private Bitmap switchedPressedState = null;
    private Bitmap switchedUnpressedState = null;

    private GestureDetector mDetector;

    private ButtonListener ButtonCallback;

    public void setStates(Context context, String pressed, String unpressed)
    {
        //Set bitmap for both states
        InputStream istr = null;
        pressedState = null;
        try {
            istr = context.getAssets().open(pressed);
            pressedState = BitmapFactory.decodeStream(istr);
            //pressedState = Bitmap.createScaledBitmap(b, 2*Math.min(getWidth(), getHeight())/3, 2*Math.min(getWidth(), getHeight())/3, false);
            //b.recycle();
        } catch (final IOException e) {
            pressedState = null;
        } finally {
            if (istr != null) {
                try {
                    istr.close();
                } catch (IOException ignored) {
                }
            }
        }

        istr = null;
        unpressedState = null;
        try {
            istr = context.getAssets().open(unpressed);
            unpressedState = BitmapFactory.decodeStream(istr);
        } catch (final IOException e) {
            unpressedState = null;
        } finally {
            if (istr != null) {
                try {
                    istr.close();
                } catch (IOException ignored) {
                }
            }
        }
    }

    public void setDoubleTapState(Context context, String switchedPressed, String switchedUnpressed)
    {
        doubleTapEnabled = true;

        //Set bitmap for switched stateInputStream istr;
        InputStream istr = null;
        switchedPressedState = null;
        try {
            istr = context.getAssets().open(switchedPressed);
            switchedPressedState = BitmapFactory.decodeStream(istr);
        } catch (final IOException e) {
            switchedPressedState = null;
        } finally {
            if (istr != null) {
                try {
                    istr.close();
                } catch (IOException ignored) {
                }
            }
        }

        istr = null;
        switchedUnpressedState = null;
        try {
            istr = context.getAssets().open(switchedUnpressed);
            switchedUnpressedState = BitmapFactory.decodeStream(istr);
        } catch (final IOException e) {
            switchedUnpressedState = null;
        } finally {
            if (istr != null) {
                try {
                    istr.close();
                } catch (IOException ignored) {
                }
            }
        }
    }

    private void setupDimensions()
    {
        centerX = getWidth() / 2;
        centerY = getHeight() / 2;
        baseRadius = Math.min(getWidth(), getHeight())/3;
    }

    public ButtonView(Context context)
    {
        super(context);
        setId(View.generateViewId());
        getHolder().addCallback(this);
        setOnTouchListener(this);
        mDetector = new GestureDetector(context,this);
        mDetector.setOnDoubleTapListener(this);
        if(context instanceof ButtonListener) ButtonCallback = (ButtonListener) context;
    }

    public ButtonView(Context context, AttributeSet attributes, int style)
    {
        super(context, attributes, style);
        setId(View.generateViewId());
        getHolder().addCallback(this);
        setOnTouchListener(this);
        mDetector = new GestureDetector(context,this);
        mDetector.setOnDoubleTapListener(this);
        if(context instanceof ButtonListener) ButtonCallback = (ButtonListener) context;
    }

    public ButtonView(Context context, AttributeSet attributes)
    {
        super(context, attributes);
        setId(View.generateViewId());
        getHolder().addCallback(this);
        setOnTouchListener(this);
        mDetector = new GestureDetector(context,this);
        mDetector.setOnDoubleTapListener(this);
        if(context instanceof ButtonListener) ButtonCallback = (ButtonListener) context;
    }

    private void drawButton(boolean pressed)
    {
        if (getHolder().getSurface().isValid())
        {
            Canvas myCanvas = this.getHolder().lockCanvas();
            myCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            Paint colors = new Paint();
            colors.setAntiAlias(true);
            colors.setFilterBitmap(true);
            colors.setDither(true);

            if (stateChanged && switchedPressedState != null && pressed)
            {
                //Draw bitmap for switched state
                Rect src = new Rect(0,0,switchedPressedState.getWidth(), switchedPressedState.getHeight());
                Rect dest = new Rect((int)baseRadius/2,(int)baseRadius/2,5*(int)baseRadius/2, 5*(int)baseRadius/2);
                myCanvas.drawBitmap(switchedPressedState, src, dest, colors);
            }
            else if (stateChanged && switchedUnpressedState != null && !pressed)
            {
                //Draw bitmap for switched state
                Rect src = new Rect(0,0,switchedUnpressedState.getWidth(), switchedUnpressedState.getHeight());
                Rect dest = new Rect((int)baseRadius/2,(int)baseRadius/2,5*(int)baseRadius/2, 5*(int)baseRadius/2);
                myCanvas.drawBitmap(switchedUnpressedState, src, dest, colors);
            }
            else if (pressed && pressedState != null)
            {
                //Draw bitmap for pressed state
                Rect src = new Rect(0,0,pressedState.getWidth(), pressedState.getHeight());
                Rect dest = new Rect((int)baseRadius/2,(int)baseRadius/2,5*(int)baseRadius/2,5*(int)baseRadius/2);
                myCanvas.drawBitmap(pressedState, src, dest, colors);
            }
            else if (!pressed && unpressedState != null)
            {
                //Draw bitmap for unpressed state
                Rect src = new Rect(0,0,unpressedState.getWidth(), unpressedState.getHeight());
                Rect dest = new Rect((int)baseRadius/2,(int)baseRadius/2,5*(int)baseRadius/2, 5*(int)baseRadius/2);
                myCanvas.drawBitmap(unpressedState, src, dest, colors);
            }
            getHolder().unlockCanvasAndPost(myCanvas);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        setupDimensions();
        drawButton(false);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
    }

    @Override
    public boolean onTouch(View v, MotionEvent e)
    {
        if (v.equals(this))
        {
            boolean b = false;
            //System.out.printf("Button %f:", e.getX());
            if (e.getAction() != MotionEvent.ACTION_UP)
            {
                float d = (float) Math.sqrt(Math.pow(e.getX() - centerX, 2) + Math.pow(e.getY() - centerY, 2));
                if (d < baseRadius)
                {
                    b = true;
                }
            }
            drawButton(b);
            if (b) ButtonCallback.onButtonPressed(stateChanged, getId());
            else ButtonCallback.onButtonReleased(stateChanged, getId());
        }
        mDetector.onTouchEvent(e);
        return true;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event){
        mDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }

    @Override
    public boolean onDoubleTap(MotionEvent e)
    {
        //Log.i("doubletapstate","here");
        if (doubleTapEnabled)
        {
            boolean b = false;
            float d = (float) Math.sqrt(Math.pow(e.getX() - centerX, 2) + Math.pow(e.getY() - centerY, 2));
            if (d < baseRadius)
            {
                b = true;
                stateChanged = !stateChanged;
            }
            drawButton(!b);
            ButtonCallback.onButtonDoubleTapped(stateChanged, getId());
        }
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onDown(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
    {
        return true;
    }

    @Override
    public void onLongPress(MotionEvent e)
    {
        return;
    }

    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
    {
        return true;
    }

    @Override
    public void onShowPress(MotionEvent e)
    {
        return;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent e)
    {
        return true;
    }

    public interface ButtonListener
    {
        void onButtonPressed(boolean b, int id);
        void onButtonReleased(boolean b, int id);
        void onButtonDoubleTapped(boolean b, int id);
    }
}
