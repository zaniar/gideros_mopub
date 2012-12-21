#include "gmopub.h"
#include <jni.h>
#include <stdlib.h>
#include <glog.h>

extern "C" {
JavaVM *g_getJavaVM();
JNIEnv *g_getJNIEnv();
}

class GMoPub
{
public:
	GMoPub()
	{
		gid_ = g_nextgid();

		JNIEnv *env = g_getJNIEnv();

		jclass localClass = env->FindClass("com/nightspade/gideros/android/plugins/mopub/GMoPub");
		cls_ = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "init", "(J)V"), (jlong)this);
	}

	~GMoPub()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "cleanup", "()V"));

		env->DeleteGlobalRef(cls_);

		gevent_removeEventsWithGid(gid_);
	}

	void onAdWillLoad()
	{
		gevent_enqueueEvent(gid_, callback_s, GMOPUB_AD_WILL_LOAD_EVENT, NULL, 1, this);
	}

	void onAdLoaded()
	{
		gevent_enqueueEvent(gid_, callback_s, GMOPUB_AD_LOADED_EVENT, NULL, 1, this);

	}

	void onAdFailed()
	{
		gevent_enqueueEvent(gid_, callback_s, GMOPUB_AD_FAILED_EVENT, NULL, 1, this);

	}

	void onAdClosed()
	{
		gevent_enqueueEvent(gid_, callback_s, GMOPUB_AD_CLOSED_EVENT, NULL, 1, this);

	}

	void onInterstitialLoaded()
	{
		gevent_enqueueEvent(gid_, callback_s, GMOPUB_INTERSTITIAL_LOADED_EVENT, NULL, 1, this);

	}

	void onInterstitialFailed()
	{
		gevent_enqueueEvent(gid_, callback_s, GMOPUB_INTERSTITIAL_FAILED_EVENT, NULL, 1, this);

	}

	void onInterstitialClosed()
	{
		gevent_enqueueEvent(gid_, callback_s, GMOPUB_INTERSTITIAL_CLOSED_EVENT, NULL, 1, this);
	}

	void loadBanner(const char* adUnitId)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "loadBanner", "(Ljava/lang/String;)V"), env->NewStringUTF(adUnitId));
	}

	void removeBanner()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "removeBanner", "()V"));
	}

	const char* getAlignment()
	{
		JNIEnv *env = g_getJNIEnv();

		jstring jalignment = (jstring) env->CallStaticObjectMethod(cls_, env->GetStaticMethodID(cls_, "getAlignment", "()Ljava/lang/String;"));

		const char* alignment = env->GetStringUTFChars(jalignment, NULL);

		env->ReleaseStringUTFChars(jalignment, alignment);

		return alignment;
	}

	void setAlignment(const char* alignment)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "setAlignment", "(Ljava/lang/String;)V"), env->NewStringUTF(alignment));
	}

	bool getAutoRefresh()
	{
		JNIEnv *env = g_getJNIEnv();

		jboolean jenabled = (jboolean) env->CallStaticBooleanMethod(cls_, env->GetStaticMethodID(cls_, "getAutoRefresh", "()Z"));

		return jenabled;
	}

	void setAutoRefresh(bool enabled)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "setAutoRefresh", "(Z)V"), enabled);
	}

	void loadInterstitial(const char* adUnitId)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "loadInterstitial", "(Ljava/lang/String;)V"), env->NewStringUTF(adUnitId));
	}

	void showInterstitial()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "showInterstitial", "()V"));
	}

	g_id addCallback(gevent_Callback callback, void *udata)
	{
		return callbackList_.addCallback(callback, udata);
	}

	void removeCallback(gevent_Callback callback, void *udata)
	{
		callbackList_.removeCallback(callback, udata);
	}

	void removeCallbackWithGid(g_id gid)
	{
		callbackList_.removeCallbackWithGid(gid);
	}

	private:
		static void callback_s(int type, void *event, void *udata)
		{
			((GMoPub*)udata)->callback(type, event);
		}

		void callback(int type, void *event)
		{
			callbackList_.dispatchEvent(type, event);
		}

	private:
		gevent_CallbackList callbackList_;

	private:
		jclass cls_;
		g_id gid_;
};

extern "C" {
	void Java_com_nightspade_gideros_android_plugins_mopub_GMoPub_onAdWillLoad(JNIEnv *env, jclass clz, jlong data)
	{
		((GMoPub*)data)->onAdWillLoad();
	}

	void Java_com_nightspade_gideros_android_plugins_mopub_GMoPub_onAdLoaded(JNIEnv *env, jclass clz, jlong data)
	{
		((GMoPub*)data)->onAdLoaded();
	}

	void Java_com_nightspade_gideros_android_plugins_mopub_GMoPub_onAdFailed(JNIEnv *env, jclass clz, jlong data)
	{
		((GMoPub*)data)->onAdFailed();
	}

	void Java_com_nightspade_gideros_android_plugins_mopub_GMoPub_onAdClosed(JNIEnv *env, jclass clz, jlong data)
	{
		((GMoPub*)data)->onAdClosed();
	}

	void Java_com_nightspade_gideros_android_plugins_mopub_GMoPub_onInterstitialLoaded(JNIEnv *env, jclass clz, jlong data)
	{
		((GMoPub*)data)->onInterstitialLoaded();
	}

	void Java_com_nightspade_gideros_android_plugins_mopub_GMoPub_onInterstitialFailed(JNIEnv *env, jclass clz, jlong data)
	{
		((GMoPub*)data)->onInterstitialFailed();
	}

	void Java_com_nightspade_gideros_android_plugins_mopub_GMoPub_onInterstitialClosed(JNIEnv *env, jclass clz, jlong data)
	{
		((GMoPub*)data)->onInterstitialClosed();
	}
}

static GMoPub *s_mopub = NULL;

extern "C" {

int gmopub_isAvailable()
{
	return 1;
}

void gmopub_init()
{
	s_mopub = new GMoPub();

}

void gmopub_cleanup()
{
	delete s_mopub;
	s_mopub = NULL;
}

void gmopub_loadBanner(const char* adUnitId)
{
	s_mopub->loadBanner(adUnitId);
}

void gmopub_removeBanner()
{
	s_mopub->removeBanner();
}

const char* gmopub_getAlignment()
{
	return s_mopub->getAlignment();
}

void gmopub_setAlignment(const char* alignment)
{
	s_mopub->setAlignment(alignment);
}

bool gmopub_getAutoRefresh()
{
	return s_mopub->getAutoRefresh();
}

void gmopub_setAutoRefresh(bool enabled)
{
	s_mopub->setAutoRefresh(enabled);
}

void gmopub_loadInterstitial(const char* adUnitId)
{
	s_mopub->loadInterstitial(adUnitId);
}

void gmopub_showInterstitial()
{
	s_mopub->showInterstitial();
}

g_id gmopub_addCallback(gevent_Callback callback, void *udata)
{
	return s_mopub->addCallback(callback, udata);
}

void gmopub_removeCallback(gevent_Callback callback, void *udata)
{
	s_mopub->removeCallback(callback, udata);
}

void gmopub_removeCallbackWithGid(g_id gid)
{
	s_mopub->removeCallbackWithGid(gid);
}

}
