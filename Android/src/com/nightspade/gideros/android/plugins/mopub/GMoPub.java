package com.nightspade.gideros.android.plugins.mopub;

import java.lang.ref.WeakReference;
import java.util.Hashtable;

import com.mopub.mobileads.MoPubInterstitial;
import com.mopub.mobileads.MoPubInterstitial.MoPubInterstitialListener;
import com.mopub.mobileads.MoPubView;
import com.mopub.mobileads.MoPubView.OnAdClosedListener;
import com.mopub.mobileads.MoPubView.OnAdFailedListener;
import com.mopub.mobileads.MoPubView.OnAdLoadedListener;
import com.mopub.mobileads.MoPubView.OnAdWillLoadListener;

import android.app.Activity;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

public class GMoPub {
	private static WeakReference<Activity> sActivity;
	private static GMoPub sInstance;
	private static long sData;
	private static boolean sEnable;
	
	private static RelativeLayout adPlaceholder;
	private static RelativeLayout.LayoutParams sParams;
	private static MoPubView sAdView;
	
	private static Hashtable<String, Integer> sAlign;
	private static String sCurAlignment = "top";
	
	public static MoPubInterstitial sInterstitial;
	
	public static void onCreate(Activity activity)
	{
		sActivity = new WeakReference<Activity>(activity);
		
		sAlign = new Hashtable<String, Integer>();
		sAlign.put("top", Gravity.TOP);
		sAlign.put("bottom", Gravity.BOTTOM);
	}
	
	static public void init(long data)
	{
		sData = data;
		sInstance = new GMoPub(sActivity.get());
	}
	
	static public void cleanup()
	{
		if (sInstance != null)
		{
			sData = 0;
			removeBanner();
			sInterstitial = null;
			sInstance = null;
		}
	}
	
	public GMoPub(Activity activity)
	{
		sEnable = false;
	}
	
	static public void onDestroy()
	{
		cleanup();
	}
	
	public static void loadBanner(final String adUnitId)
	{
		try
		{
			sActivity.get().runOnUiThread(new Runnable() {
			
				@Override
				public void run() {
					FrameLayout viewport = (FrameLayout)sActivity.get().getWindow().getDecorView();
					
					if(sAdView != null)
					{
						viewport.removeView(sAdView);
						sAdView.destroy();
					}
					
					sAdView = new MoPubView(sActivity.get());
					sAdView.setAdUnitId(adUnitId);
					sAdView.loadAd();
					
					FrameLayout.LayoutParams adParams = new FrameLayout.LayoutParams(
		                    FrameLayout.LayoutParams.MATCH_PARENT ,
		                    FrameLayout.LayoutParams.WRAP_CONTENT ,
		                    sAlign.get(sCurAlignment));
					viewport.addView(sAdView, adParams);
					
					sAdView.setOnAdWillLoadListener(new OnAdWillLoadListener() {
						
						@Override
						public void OnAdWillLoad(MoPubView arg0, String arg1) {
							onAdWillLoad(sData);
						}
					});
					
					sAdView.setOnAdLoadedListener(new OnAdLoadedListener() {
						
						@Override
						public void OnAdLoaded(MoPubView arg0) {
							onAdLoaded(sData);
						}
					});
			
					sAdView.setOnAdFailedListener(new OnAdFailedListener() {
						
						@Override
						public void OnAdFailed(MoPubView arg0) {
							onAdFailed(sData);
						}
					});
					
					sAdView.setOnAdClosedListener(new OnAdClosedListener() {
						
						@Override
						public void OnAdClosed(MoPubView arg0) {
							onAdClosed(sData);
						}
					});
				}
			});
		
		}
		catch(Exception ex)	{}
	}
	
	public static void removeBanner()
	{
		try
		{
			sActivity.get().runOnUiThread(new Runnable() {
				
				@Override
				public void run() {
					if(sAdView != null)
					{
						FrameLayout viewport = (FrameLayout)sActivity.get().getWindow().getDecorView();

						viewport.removeView(sAdView);
						sAdView.destroy();
					}
				}
			});
		}
		catch(Exception ex){}
	}
	
	public static String getAlignment()
	{
		return sCurAlignment;
	}
	
	public static void setAlignment(String alignment)
	{
		FrameLayout.LayoutParams adParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT ,
                FrameLayout.LayoutParams.WRAP_CONTENT ,
                sAlign.get(alignment));
		sAdView.setLayoutParams(adParams);
	}
	
	public static boolean getAutoRefresh()
	{
		return sAdView.getAutorefreshEnabled();
	}
	
	public static void setAutoRefresh(boolean enabled)
	{
		sAdView.setAutorefreshEnabled(enabled);
	}
	
	public static void loadInterstitial(final String interstitialAdUnitId)
	{
		try
		{
			sActivity.get().runOnUiThread(new Runnable() {
			
				@Override
				public void run() {
					sInterstitial = new MoPubInterstitial(sActivity.get(), interstitialAdUnitId);
				}
			});
		
		}
		catch(Exception ex)	{}
	}
	
	public static void showInterstitial()
	{
		sInterstitial.setListener(new MoPubInterstitialListener() {
			
			@Override
			public void OnInterstitialLoaded() {
				if (sInterstitial.isReady()) {
					sInterstitial.show();
					onInterstitialLoaded(sData);
				} else {
				}
			}
			
			@Override
			public void OnInterstitialFailed() {
				onInterstitialFailed(sData);
			}
			
			public void OnInterstitialClosed() {
				onInterstitialClosed(sData);
			}
		});
		
		sInterstitial.load();
	}
	
	public static native void onAdWillLoad(long data);
	public static native void onAdLoaded(long data);
	public static native void onAdFailed(long data);
	public static native void onAdClosed(long data);
	public static native void onInterstitialLoaded(long data);
	public static native void onInterstitialFailed(long data);
	public static native void onInterstitialClosed(long data);
}
