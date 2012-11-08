
package com.tenone.rps;

import android.app.Activity;
import android.content.res.AssetManager;
import android.util.Log;
import android.os.Bundle;


public class RPSXActivity extends Activity 
{
    private static final String ACTIVITY_TAG = "RPSXActivity";

    static AssetManager sAssetManager;
    RPSXView mView;

    // On applications creation
    @Override protected void onCreate( Bundle savedInstanceState ) 
    {
        super.onCreate( savedInstanceState );
        
        Log.i(ACTIVITY_TAG, "Create asset manager");
        // Pass the asset manager to the native code
        sAssetManager = getAssets();
        RPSXLib.createAssetManager( sAssetManager );
        
        Log.i(ACTIVITY_TAG, "Create view");
        // Create our view for OpenGL rendering
        mView = new RPSXView( getApplication() );
        
        setContentView( mView );
    }

    @Override protected void onPause() 
    {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() 
    {
        super.onResume();
        mView.onResume();
    }
}
