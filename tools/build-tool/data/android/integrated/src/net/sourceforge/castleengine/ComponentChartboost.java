/* -*- tab-width: 4 -*- */
package net.sourceforge.castleengine;

import android.util.Log;
import android.widget.Toast;

import com.chartboost.sdk.CBLocation;
import com.chartboost.sdk.Chartboost;
import com.chartboost.sdk.ChartboostDelegate;
import com.chartboost.sdk.Libraries.CBLogging.Level;
import com.chartboost.sdk.Model.CBError.CBImpressionError;

/**
 * Chartboost (https://www.chartboost.com/)
 * integration with Castle Game Engine Android application.
 */
public class ComponentChartboost extends ComponentAbstract
{
    private static final String TAG = "${NAME}.castleengine.ComponentChartboost";

    private boolean initialized, scheduledStart, scheduledResume;
    private boolean mShown;

    public ComponentChartboost(MainActivity activity)
    {
        super(activity);
    }

    ChartboostDelegate delegate = new ChartboostDelegate()
    {
        private void interstitialDone(boolean retry)
        {
            messageSend(new String[]{"ads-chartboost-interstitial-display", "shown"});
            mShown = false;
            if (retry) {
                // cache next interstitial.
                // Don't do this in case loading of previous one failed,
                // as we would spam console (and Toast notifications) with failures.
                Chartboost.cacheInterstitial(CBLocation.LOCATION_DEFAULT);
            }
        }

        // Override the Chartboost delegate callbacks you wish to track and control
        @Override
        public void didCloseInterstitial(String location)
        {
            super.didCloseInterstitial(location);
            Log.i(TAG, "Chartbooost Interstitial Close, location: "+ (location != null ? location : "null"));
            interstitialDone(true);
        }

        @Override
        public void didDismissInterstitial(String location) {
            super.didDismissInterstitial(location);
            Log.i(TAG, "Chartbooost Interstitial Dismiss, location: "+ (location != null ? location : "null"));
            // react to dismiss (this actually happens before ad is closed,
            // it's when user switches out of our app).
            // Needed, since we don't get Closed callback
            // when user presses on the app.
            interstitialDone(true);
        }

        @Override
        public void didFailToLoadInterstitial(String location, CBImpressionError error) {
            Log.i(TAG, "Chartbooost Interstitial FAIL TO LOAD, location: " +
                (location != null ? location : "null") + ", error: " + error.name());
            // do not show Toast outside of show cycle
            // (e.g. when cacheInterstitial from onStart fails)
            if (mShown) {
                Toast.makeText(getActivity().getApplicationContext(),
                    "No ads available, continuing without ad.", Toast.LENGTH_SHORT).show();
            }
            interstitialDone(false);
        }

        @Override
        public void didClickInterstitial(String location) {
            super.didClickInterstitial(location);
            Log.i(TAG, "Chartbooost Interstitial Click, location: "+ (location != null ? location : "null"));
        }

        @Override
        public void didDisplayInterstitial(String location) {
            super.didDisplayInterstitial(location);
            Log.i(TAG, "Chartbooost Interstitial Display, location: " +  (location != null ? location : "null"));
        }
    };

    private void initialize(String appId, String appSignature)
    {
        if (initialized) {
            return;
        }

        Chartboost.startWithAppId(getActivity(), appId, appSignature);
        // necessary following
        // https://github.com/freshplanet/ANE-Chartboost/issues/12
        // https://answers.chartboost.com/hc/en-us/articles/201219545-Android-Integration
        Chartboost.setImpressionsUseActivities(true);
        //Chartboost.setLoggingLevel(Level.ALL); // not on prod!
        Chartboost.setDelegate(delegate);
        Chartboost.onCreate(getActivity());
        Log.i(TAG, "Chartboost initialized with appId " + appId + " (will send delayed onStart: " + scheduledStart + ", will send delayed onResume: " + scheduledResume + ")");
        initialized = true;

        if (scheduledStart) {
            onStart();
            scheduledStart = false;
        }
        if (scheduledResume) {
            onResume();
            scheduledResume = false;
        }
    }

    @Override
    public void onDestroy()
    {
        if (!initialized) {
            return;
        }
        Chartboost.onDestroy(getActivity());
    }

    @Override
    public void onResume()
    {
        if (!initialized) {
            scheduledResume = true; // send onResume to Chartboost SDK when we will be initialized
            return;
        }
        Chartboost.onResume(getActivity());
    }

    @Override
    public void onPause()
    {
        scheduledResume = false;
        if (!initialized) {
            return;
        }
        Chartboost.onPause(getActivity());
    }

    @Override
    public void onStart()
    {
        if (!initialized) {
            scheduledStart = true; // send onStart to Chartboost SDK when we will be initialized
            return;
        }
        Chartboost.onStart(getActivity());
        // we cannot call Chartboost.cacheInterstitial in onCreate, chartboost makes exception then
        // java.lang.Exception: Session not started: Check if Chartboost.onStart() is called, if not the session won't be invoked
        Chartboost.cacheInterstitial(CBLocation.LOCATION_DEFAULT);
    }

    @Override
    public void onStop()
    {
        scheduledStart = false;
        if (!initialized) {
            return;
        }
        Chartboost.onStop(getActivity());
    }

    @Override
    public boolean onBackPressed()
    {
        if (!initialized) {
            return false; // let default activity onBackPressed to work
        }

        // If an interstitial is on screen, close it.
        return Chartboost.onBackPressed();
    }

    private void showInterstitial()
    {
        if (initialized) {
            if (!Chartboost.hasInterstitial(CBLocation.LOCATION_DEFAULT)) {
                Log.i(TAG, "Interstitial not in cache yet, will wait for it");
            }
            Log.i(TAG, "Interstitial showing");
            Chartboost.showInterstitial(CBLocation.LOCATION_DEFAULT);
            mShown = true;
        } else {
            // pretend that ad was displayed, in case native app waits for it
            messageSend(new String[]{"ads-chartboost-interstitial-display", "shown"});
        }
    }

    /* TODO: track purchases using chartboost analytics, see

    `Google`

            CBAnalytics.trackInAppGooglePlayPurchaseEvent("xxx-title",
                            "xxx-description",
                            "$0.99",
                            "USD",
                            "xxx-id",
                            "xxx-data",
                            "xxx-signature");

    `Amazon`

            CBAnalytics.trackInAppAmazonStorePurchaseEvent("xxx-title",
                            "xxx-description",
                            "$0.99",
                            "USD",
                            "xxx-id",
                            "xxx-userId",
                            "xxx-token");

    */

    @Override
    public boolean messageReceived(String[] parts)
    {
        if (parts.length == 3 && parts[0].equals("ads-chartboost-initialize")) {
            initialize(parts[1], parts[2]);
            return true;
        } else
        if (parts.length == 1 && parts[0].equals("ads-chartboost-show-interstitial")) {
            showInterstitial();
            return true;
        } else {
            return false;
        }
    }
}