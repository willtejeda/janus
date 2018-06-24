package org.janus;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

class JoystickView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener
{
    private float centerX;
    private float centerY;
    private float baseRadius;
    private float midRadius;
    private float hatRadius;

    private boolean draw_base;

    private JoystickListener joystickCallback;

    private void setupDimensions()
    {
        centerX = getWidth() / 2;
        centerY = getHeight() / 2;
        baseRadius = Math.min(getWidth(), getHeight())/3;
        midRadius = 4*Math.min(getWidth(), getHeight())/15;
        hatRadius = Math.min(getWidth(), getHeight())/10;
    }

    public JoystickView(Context context)
    {
        super(context);
        setId(View.generateViewId());
        getHolder().addCallback(this);
        setOnTouchListener(this);
        if(context instanceof JoystickListener) joystickCallback = (JoystickListener) context;
    }

    public JoystickView(Context context, AttributeSet attributes, int style)
    {
        super(context, attributes, style);
        setId(View.generateViewId());
        getHolder().addCallback(this);
        setOnTouchListener(this);
        if(context instanceof JoystickListener) joystickCallback = (JoystickListener) context;
    }

    public JoystickView(Context context, AttributeSet attributes)
    {
        super(context, attributes);
        setId(View.generateViewId());
        getHolder().addCallback(this);
        setOnTouchListener(this);
        if(context instanceof JoystickListener) joystickCallback = (JoystickListener) context;
    }

    private void drawJoystick(float newX, float newY)
    {
        if (getHolder().getSurface().isValid())
        {
            Canvas myCanvas = this.getHolder().lockCanvas();
            Paint colors = new Paint();
            myCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            colors.setARGB(96, 150, 150, 150);
            myCanvas.drawCircle(centerX, centerY, (draw_base)?baseRadius:midRadius, colors);
            colors.setARGB(96, 175, 175, 175);
            myCanvas.drawCircle(centerX, centerY, midRadius, colors);

            Path path = new Path();
            path.moveTo(centerX - baseRadius/4, centerY - baseRadius/2);
            path.lineTo(centerX - baseRadius/2, centerY - baseRadius/2);
            path.lineTo(centerX - baseRadius/2, centerY - baseRadius/4);
            colors.setStyle(Paint.Style.STROKE);
            colors.setStrokeWidth(8);
            colors.setPathEffect(null);
            colors.setARGB(128, 230, 230, 230);

            myCanvas.save();
            myCanvas.rotate(45, centerX, centerY);
            myCanvas.drawPath(path, colors);
            for (int i = 0; i < 3; i++)
            {
                myCanvas.rotate(90, centerX, centerY);
                myCanvas.drawPath(path, colors);
            }
            myCanvas.restore();

            colors.setStyle(Paint.Style.FILL);

            colors.setARGB(128, 230, 230, 230);
            myCanvas.drawCircle(newX, newY, hatRadius, colors);
            getHolder().unlockCanvasAndPost(myCanvas);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        setupDimensions();
        drawJoystick(centerX, centerY);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
    }

    public boolean onTouch(View v, MotionEvent e)
    {
        if (v.equals(this))
        {
            float x, y;
            if (e.getAction() != MotionEvent.ACTION_UP)
            {
                float d = (float) Math.sqrt(Math.pow(e.getX() - centerX, 2) + Math.pow(e.getY() - centerY, 2));
                if (d < baseRadius)
                {
                    draw_base =  d>midRadius;
                    drawJoystick(e.getX(), e.getY());
                }
                else
                {
                    draw_base = true;
                    float ratio = baseRadius / d;
                    float constrainedX = centerX + (e.getX() - centerX) * ratio;
                    float constrainedY = centerY + (e.getY() - centerY) * ratio;
                    drawJoystick(constrainedX, constrainedY);
                }

                if (d < midRadius)
                {
                    x = (e.getX() - centerX) / midRadius;
                    y = (e.getY() - centerY) / midRadius;
                }
                else
                {
                    float ratio = midRadius / d;
                    float constrainedX = centerX + (e.getX() - centerX) * ratio;
                    float constrainedY = centerY + (e.getY() - centerY) * ratio;
                    x = (constrainedX - centerX) / midRadius;
                    y = (constrainedY - centerY) / midRadius;
                }
            }
            else
            {
                draw_base = false;
                drawJoystick(centerX, centerY);
                x = 0;
                y = 0;
            }
            joystickCallback.onJoystickMoved(x, y, draw_base, getId());
        }
        return true;
    }

    public interface JoystickListener
    {
        void onJoystickMoved(float x, float y, boolean b, int id);
    }
}
