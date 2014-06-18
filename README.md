![Kami-sama](http://i.imgur.com/t6BrZ9Y.png)

If you like my releases, feel free to [buy my meso]('https://ccplz.net/threads/s-meso-16%E2%82%AC-b-taxes-covered-paypal-btc-ltc-doge.60888/') or [donate]('https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=5E289LJ5UUG3Q')!

Note: this is still a draft and it might not be 100% stable and the code needs to be cleaned up.

Requirements:
============
- [Visual C++ 2012 Redist (x86 & x64)]('http://www.microsoft.com/en-us/download/details.aspx?id=30679')

When you change the settings, you will have to untick and re-tick kami for them to be effective.

Quick feature overview (read this before you use it!):
============
* Offset X/Y: when you stick to mobs, your position will be shifted by these amounts. Mostly useful for classes/skills that must attack from a certain range or huge mobs.
* Speed: this is how often kami will teleport. A faster kami will switch mobs faster and loot faster, but it will also use more cpu. Without No Mov., any speed above 17 teleports / s will most likely D/C for most people. But test it and let me know your results.
* Loot: enable this and the kami will automatically teleport to items and press Z to loot them (you don't need any extra autoloot, but at low teleport speeds an external autoloot might speed things up slightly).
* Spam CTRL: this will autoattack by pressing CTRL when sticking to mobs. All the automatic keypresses in this bot use the internal maplestory input handler CWndMan::OnKey, so it will never miss a keystroke and it will also work minimized just like PostMessage.
* No Mov. (**not bypassless!**): enabling this will allow you to kami as fast as you can but it has some important drawbacks: first of all, your movement will be client-sided and other people will see you standing where you were when you ticked kami. This means that the only monsters you will see and kill are the ones your game loaded from your initial position, so your range will be smaller and it will get stuck on big maps. Also, **as far as I know, using anything besides skill inject and GnD *might autoban* with No Mov. ticked and it will also cause you to randomly D/C when disabling kami because of invalid movement packets (I'll fix this in future updates by teleporting back to the initial position)**.
* GnD (**not bypassless!**): general no-delay. It will allow you to spam some skills and attacks infinitely fast (in fact, most skills & attacks).
* P.Loot (**not bypassless!**): perfect loot. Disables item drop animations and speeds up looting.
* Unlimited MP (**not bypassless!**)
* Full GM (**not bypassless!**): full godmode. Makes you invincible.
* Filter (**not bypassless!**): item filter. At the moment it will hide any item (even meso) when ticked. I will add a gui to choose to allow / reject items ASAP.
* CPU Hax (**not bypassless!**): disables clouds, backgrounds, terrain and damage to minimize cpu / memory usage
* FA (**not bypassless!**): fusion attack. Makes AoE attacks only focus on one mobs to maximise DPS on a single target.
* Mob Lag (**not bypassless!**): freezes monsters.
* Skill Injection (**not bypassless!**): no-delay for skills that don't work with GnD. At the moment the delay is set to 1, but it will be configurable in the next revisions.

Introduction:
============
So I've been messing with Kami for quite a while to try and solve D/Cs and it seems that the main causes of D/C are how often you teleport w/o char block (anything over 17 teleports / second D/Cs) and the fact that the hook that AIRRIDE was using to call kami & keypress functions now randomly D/Cs for some people (I even tried limiting it to less than 10 teleports / second with a time counter but it still D/Cd).

Running kami in my own thread would've solved D/Cs with no mov. disabled, but it creates too many other issues that break the stability of kami because it's not synced to the game's rendering loop, so this is just the old kami hook plus thread-safe item coords and block char movement.

Toubleshoot if you get random D/Cs:
============
* You might need to enable No Mov.
* Stand in the middle of the map when ticking kami
* Your map might be too big
* You might be killing mobs too slowly
* You're missing too often with skill inject / gnd
* You D/Ced when disabling it. This is normal.

How to compile the source:
============
* Follow [this guide]('http://wiki.wxwidgets.org/Compiling_WxWidgets_with_MSVC_(2)#Static_Compilation') to compile a static version of wxWidgets for your version of Visual Studio.
* Download [boost 1.55.0]('http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.zip/download') and unzip it anywhere you want
* Start a visual studio x86 command prompt and type "cd C:\path\where\you\extracted\boost"
* Type "bootstrap.bat"
* Once it's done, type bjam and let it run (this will take hours)
* Open Kami-sama.sln on visual studio and right click Kami-sama (not Solution, the one below) and click Properties
* In C/C++ -> General replace "$(BOOST_include)" in the Additional Include Directories with the path where you extracted boost earlier
* In Linker -> General replace "$(BOOST_lib)\x86" in the Additional Library Directories with the path where you extracted boost followed by "\stage\lib"
* Build -> Rebuild all and your project should build just fine!