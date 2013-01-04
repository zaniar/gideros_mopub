#ifndef GMOPUB_H
#define GMOPUB_H

#include <gglobal.h>
#include <gevent.h>

enum
{
	GMOPUB_AD_WILL_LOAD_EVENT,
	GMOPUB_AD_LOADED_EVENT,
	GMOPUB_AD_FAILED_EVENT,
	GMOPUB_AD_CLOSED_EVENT,
	GMOPUB_INTERSTITIAL_LOADED_EVENT,
	GMOPUB_INTERSTITIAL_FAILED_EVENT,
	GMOPUB_INTERSTITIAL_CLOSED_EVENT
};

#ifdef __cplusplus
extern "C" {
#endif

G_API int gmopub_isAvailable();

G_API void gmopub_init();
G_API void gmopub_cleanup();

G_API void gmopub_loadBanner(const char* adUnitId);
G_API void gmopub_removeBanner();
G_API void gmopub_showBanner();
G_API void gmopub_hideBanner();
G_API const char* gmopub_getAlignment();
G_API void gmopub_setAlignment(const char* alignment);
G_API bool gmopub_getAutoRefresh();
G_API void gmopub_setAutoRefresh(bool enabled);
G_API void gmopub_loadInterstitial(const char* adUnitId);
G_API void gmopub_showInterstitial();

G_API g_id gmopub_addCallback(gevent_Callback callback, void *udata);
G_API void gmopub_removeCallback(gevent_Callback callback, void *udata);
G_API void gmopub_removeCallbackWithGid(g_id gid);

#ifdef __cplusplus
}
#endif

#endif
