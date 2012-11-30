
package com.tenone.rps;

import android.content.res.AssetManager;

// Wrapper for native library

public class RPSXLib 
{

     static 
     {
         System.loadLibrary( "rps" );
     }

    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void init( int width, int height );
     public static native void step( float fElapsedTime );
     public static native void createAssetManager( AssetManager assetManager );

     public static native void handleTouch( float x, float y );
}
