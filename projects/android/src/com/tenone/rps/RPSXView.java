package com.tenone.rps;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.View;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.os.SystemClock;


///////////////////////////////////////////////////////////////////////////////////////////////////
// RPSXView - main view that extends GLSurfaceView
class RPSXView extends GLSurfaceView
{
    private static final String VIEW_TAG = "RPSXView";

    private RPSXRenderer m_pRenderer;

    public RPSXView( Context context ) 
    {
        super( context );

        // Create an OpenGL ES 2.0 context
        setEGLContextClientVersion( 2 );

        Log.i(VIEW_TAG, "Create renderer");
        // Set the renderer associated with this view
        m_pRenderer = new RPSXRenderer();
        setRenderer( m_pRenderer );
    }

    public boolean onTouchEvent( final MotionEvent event ) 
    {
        queueEvent( new Runnable(){
            public void run() {
                m_pRenderer.processTouch( event.getX(), event.getY() );
            }
        } );

        return true;
    }

    private static class RPSXRenderer implements GLSurfaceView.Renderer 
    {
        private static final String RENDERER_TAG = "RPSXRenderer";

        private long m_nLastTime;
        private float m_fPreviousX = 0.0f;
        private float m_fPreviousY = 0.0f;

        public void onDrawFrame( GL10 gl )
        {
            // calculate elapsed time
            if( m_nLastTime == 0 )
                m_nLastTime = SystemClock.elapsedRealtime();

            long nCurrentTime = SystemClock.elapsedRealtime();
            long nElapsedTime = nCurrentTime - m_nLastTime;
            float fElapsedTime = nElapsedTime / 1000.0f;
            m_nLastTime = nCurrentTime;

            RPSXLib.step( fElapsedTime );
        }

        public void processTouch(float x, float y) 
        {
            float fCurrentY = y;

            float fDeltaY = fCurrentY - m_fPreviousY;

            m_fPreviousY = fCurrentY;
        }

        public void onSurfaceChanged( GL10 gl, int width, int height )
        {
            Log.i(RENDERER_TAG, "Call init");
            RPSXLib.init( width, height );
        }

        public void onSurfaceCreated( GL10 gl, EGLConfig config )
        {
        }
    }
}

