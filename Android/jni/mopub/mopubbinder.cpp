#include "gmopub.h"
#include "gideros.h"
#include <glog.h>

// some Lua helper functions
#ifndef abs_index
#define abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#endif

static void luaL_newweaktable(lua_State *L, const char *mode)
{
	lua_newtable(L);			// create table for instance list
	lua_pushstring(L, mode);
	lua_setfield(L, -2, "__mode");	  // set as weak-value table
	lua_pushvalue(L, -1);             // duplicate table
	lua_setmetatable(L, -2);          // set itself as metatable
}

static void luaL_rawgetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_rawget(L, idx);
}

static void luaL_rawsetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_insert(L, -2);
	lua_rawset(L, idx);
}

static const char *AD_WILL_LOAD = "adWillLoad";
static const char *AD_LOADED = "adLoaded";
static const char *AD_FAILED = "adFailed";
static const char *AD_CLOSED = "adClosed";
static const char *INTERSTITIAL_LOADED = "interstitialLoaded";
static const char *INTERSTITIAL_FAILED = "interstitialFailed";
static const char *INTERSTITIAL_CLOSED = "interstitialClosed";

static char keyWeak = ' ';

class MoPub : public GEventDispatcherProxy
{
public:
	MoPub(lua_State *L) : L(L)
	{
		gmopub_init();
		gmopub_addCallback(callback_s, this);
	}

	~MoPub()
	{
		gmopub_removeCallback(callback_s, this);
		gmopub_cleanup();
	}

	void loadBanner(const char* adUnitId)
	{
		gmopub_loadBanner(adUnitId);
	}

	void removeBanner()
	{
		gmopub_removeBanner();
	}

	const char* getAlignment()
	{
		return gmopub_getAlignment();
	}

	void setAlignment(const char* alignment)
	{
		gmopub_setAlignment(alignment);
	}

	bool getAutoRefresh()
	{
		return gmopub_getAutoRefresh();
	}

	void setAutoRefresh(bool enabled)
	{
		gmopub_setAutoRefresh(enabled);
	}

	void loadInterstitial(const char* adUnitId)
	{
		gmopub_loadInterstitial(adUnitId);
	}

	void showInterstitial()
	{
		gmopub_showInterstitial();
	}

private:
	static void callback_s(int type, void *event, void *udata)
	{
		static_cast<MoPub*>(udata)->callback(type, event);
	}

	void callback(int type, void *event)
	{
		dispatchEvent(type, event);
	}

	void dispatchEvent(int type, void *event)
	{
		luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
		luaL_rawgetptr(L, -1, this);

		if (lua_isnil(L, -1))
		{
			lua_pop(L, 2);
			return;
		}

		lua_getfield(L, -1, "dispatchEvent");

		lua_pushvalue(L, -2);

		lua_getglobal(L, "Event");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);

		switch (type)
		{
		case GMOPUB_AD_WILL_LOAD_EVENT:
			lua_pushstring(L, AD_WILL_LOAD);
			break;
		case GMOPUB_AD_LOADED_EVENT:
			lua_pushstring(L, AD_LOADED);
			break;
		case GMOPUB_AD_FAILED_EVENT:
			lua_pushstring(L, AD_FAILED);
			break;
		case GMOPUB_AD_CLOSED_EVENT:
			lua_pushstring(L, AD_CLOSED);
			break;
		case GMOPUB_INTERSTITIAL_LOADED_EVENT:
			lua_pushstring(L, INTERSTITIAL_LOADED);
			break;
		case GMOPUB_INTERSTITIAL_FAILED_EVENT:
			lua_pushstring(L, INTERSTITIAL_FAILED);
			break;
		case GMOPUB_INTERSTITIAL_CLOSED_EVENT:
			lua_pushstring(L, INTERSTITIAL_CLOSED);
			break;
		}

		lua_call(L, 1, 1);

		lua_call(L, 2, 0);

		lua_pop(L, 2);
	}

private:
	lua_State *L;
	bool initialized_;
};

static int destruct(lua_State* L)
{
	void *ptr =*(void**)lua_touserdata(L, 1);
	GReferenced* object = static_cast<GReferenced*>(ptr);
	MoPub *mopub = static_cast<MoPub*>(object->proxy());

	mopub->unref();

	return 0;
}

static MoPub *getInstance(lua_State *L, int index)
{
	GReferenced *object = static_cast<GReferenced*>(g_getInstance(L, "MoPub", index));
	MoPub *mopub = static_cast<MoPub*>(object->proxy());

	return mopub;
}

static int loadBanner(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	const char *adUnitId = lua_tostring(L, 2);

	mopub->loadBanner(adUnitId);

	return 0;
}

static int removeBanner(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	mopub->removeBanner();

	return 0;
}

static int getAlignment(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	lua_pushstring(L, mopub->getAlignment());

	return 0;
}

static int setAlignment(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	const char *alignment = lua_tostring(L, 2);

	mopub->setAlignment(alignment);

	return 0;
}

static int getAutoRefresh(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	lua_pushboolean(L, mopub->getAutoRefresh());

	return 0;
}

static int setAutoRefresh(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	bool enabled = lua_toboolean(L, 2);

	mopub->setAutoRefresh(enabled);

	return 0;
}

static int loadInterstitial(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	const char *adUnitId = lua_tostring(L, 2);

	mopub->loadInterstitial(adUnitId);

	return 0;
}

static int showInterstitial(lua_State *L)
{
	MoPub *mopub = getInstance(L, 1);

	mopub->showInterstitial();

	return 0;
}

static int loader(lua_State *L)
{
	const luaL_Reg functionList[] = {
		{"loadBanner", loadBanner},
		{"removeBanner", removeBanner},
		{"getAlignment", getAlignment},
		{"setAlignment", setAlignment},
		{"getAutoRefresh", getAutoRefresh},
		{"loadInterstitial", loadInterstitial},
		{"showInterstitial", showInterstitial},
		{NULL, NULL}
	};

    g_createClass(L, "MoPub", "EventDispatcher", NULL, destruct, functionList);

    // create a weak table in LUA_REGISTRYINDEX that can be accessed with the address of keyWeak
	luaL_newweaktable(L, "v");
	luaL_rawsetptr(L, LUA_REGISTRYINDEX, &keyWeak);

	lua_getglobal(L, "MoPub");
	lua_pushstring(L, "top");
	lua_setfield(L, -2, "ALIGN_TOP");
	lua_pushstring(L, "bottom");
	lua_setfield(L, -2, "ALIGN_BOTTOM");
	lua_pop(L, 1);

    lua_getglobal(L, "Event");
	lua_pushstring(L, AD_WILL_LOAD);
	lua_setfield(L, -2, "AD_WILL_LOAD");
	lua_pushstring(L, AD_LOADED);
	lua_setfield(L, -2, "AD_LOADED");
	lua_pushstring(L, AD_FAILED);
	lua_setfield(L, -2, "AD_FAILED");
	lua_pushstring(L, AD_CLOSED);
	lua_setfield(L, -2, "AD_CLOSED");
	lua_pushstring(L, INTERSTITIAL_LOADED);
	lua_setfield(L, -2, "INTERSTITIAL_LOADED");
	lua_pushstring(L, INTERSTITIAL_FAILED);
	lua_setfield(L, -2, "INTERSTITIAL_FAILED");
	lua_pushstring(L, INTERSTITIAL_CLOSED);
	lua_setfield(L, -2, "INTERSTITIAL_CLOSED");
	lua_pop(L, 1);

	MoPub *mopub = new MoPub(L);
	g_pushInstance(L, "MoPub", mopub->object());

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, mopub);
	lua_pop(L, 1);

	lua_pushvalue(L, -1);
	lua_setglobal(L, "mopub");

    return 1;
}

static void g_initializePlugin(lua_State *L)
{
    lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");

	lua_pushcfunction(L, loader);
	lua_setfield(L, -2, "mopub");

	lua_pop(L, 2);
}

static void g_deinitializePlugin(lua_State *L)
{

}

REGISTER_PLUGIN("MoPub", "2012.12")
