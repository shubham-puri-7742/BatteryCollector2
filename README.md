# BatteryCollector2
### Unreal (C++) variant of the Battery Collector game. Adds an AI opponent

This is extended from my GAM312 project which is a simple arcade style Battery Collector game where the player is thrown into the game world and drains power over time. Batteries spawn randomly and the player has to collect them to fully recharge his bot. The game ends on either reaching full power or losing all power.

Visually, the player's power status is visible in his bot's body colour (white being charged, black being discharged) and walking speed. Instinctively, the bot walks slower when low on power.

The game includes three types of battery pickups - yellow charged batteries that add to the player's power, red dead batteries that do nothing (except waste a player's time should he go after one), and black toxic batteries that drain the player's power.

Additionally, a major addition from a design perspective was the addition of an AI opponent. Further levels (not created in this prototype) may include more than one NPCs to ramp up the difficulty. The current AI logic races around to collect the spawned batteries, simulating competition for limited resources in the natural world.

### Reflection

This was my first Unreal project using C++ scripting. As such, I balanced the learning curve by picking relatively simple gameplay. Although I knew C++ before starting this project, adapting to Unreal's dialect of C++ presented a small additional learning curve.

The most significant part I learnt while doing this was creating the core class in C++ and subclassing it in Blueprint for the final version that is used in the game to minimise referencing assets in the source code.

I had worked with game AI before, so the AI logic or its fundamentals (like navmeshes) were not entirely new (about half of it was borrowed from an earlier project using a behaviour tree), although implementing it in C++ was.

As with all the other GAM495 projects, development was incremental. The initial iteration created the basic level layout and the player's power drain over time. A spawn volume that created batteries was added next. Actually adding power to the player on pickup was the next feature to be added. This led to the development of additional subclasses for dead and toxic batteries. The AI logic was added last.

One significant challenge for me was finding quality tutorials to work with Unreal in C++ and adapting them to Unreal 4.2x. Unreal's official documentation (though up-to-date) is often too pauciloquent for learners and tutorials may use earlier versions, which may differ (for instance) on whether a specific property can be accessed directly or not. More often than not, I had to refer to a tutorial or a book (which used Unreal 4.0x or 4.1x) to understand the concept, and the official documentation to figure out its implementation in UE4.2x.
