gideros_mopub
=============

MoPub Plugin for Gideros

Simple example:
```
require "mopub"

print("MoPub test")

mopub:loadBanner("AD_UNIT_ID")
mopub:loadInterstitial("AD_UNIT_ID")

stage:addEventListener(Event.KEY_UP, function(event)
	if event.keyCode == KeyCode.BACK then
		application:exit()
	elseif event.keyCode == KeyCode.MENU then
		mopub:showInterstitial()
	end
end)
```