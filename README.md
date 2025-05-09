scons target=template_debug debug_symbols=yes
https://www.youtube.com/watch?v=8WSIMTJWCBk

https://stackoverflow.com/questions/13040777/lldb-error-variable-not-available



SG Physics 3D
=============

**SG Physics 3D** is a deterministic 3D physics engine for Godot,
based on fixed-point math. It's useful for online multiplayer games, especially
those using rollback netcode.


Documentation XML Generation
--------

# Replace "godot" with the full path to a Godot editor binary
# if Godot is not installed in your `PATH`.
godot --doctool ../ --gdextension-docs


Download
--------

You can download prebuilt binaries for several platforms from the "Releases" page:

[https://gitlab.com/snopek-games/sg-physics-3d/-/releases](https://gitlab.com/snopek-games/sg-physics-3d/-/releases)

What is "deterministic" physics and why would I need it?
--------------------------------------------------------

A physics engine is "deterministic" if it will play out exactly the same on
two different computers, if both have the exact same starting state.

Most physics engines aren't deterministic (including the builtin physics
engine in Godot), or are only deterministic under certain conditions (ex.
Box2D is deterministic if both computers are running the same binary, so
it's not deterministic cross-platform).

And that's usually fine, because most games don't need deterministic physics!

But for some games, especially online multiplayer games using certain network
synchronization techniques (like rollback and prediction), a deterministic
physics engine is a requirement.

Fixed-point math
----------------

Normally, decimal numbers with a fractional part are represented as hardware-based
_floating-point numbers_.

However, the result of floating-point math can be slightly different on
different CPUs, operating systems or versions - which can break determinism!

There's a couple solutions to this problem:

 - Soft floats: implementing floating-point math in software, rather than using
   the hardware floating-point features of your CPU.
 - Fixed-point math: using integers to represent decimals with a fixed number of
   bits reserved for the fractional part.

SG Physics 2D went with fixed-point math.

Specifically, we use 64-bit signed integers, with the first 16 bits reserved for
the fractional part.

This means the smallest fractional part that can be represented is one 65536th
(ie. `1/65536`) and we can theoretically represent "real world" numbers in the
range from `-140737488355327` to `140737488355328`.

To convert from a "real world" number to our fixed-point format, you'd multiply
it by 65536 and discard any remaining fractional part. So, for example, `4.25`
is `278528` in our fixed-point representation.

In GDScript, we use normal integers for fixed-point values, and provide some
utilities to work with them, for example:

 - `SGFixed.mul(a, b)`: multiplies two fixed-point numbers
 - `SGFixed.div(a, b)`: divides two fixed-point numbers
 - `SGFixed.to_float(a)`: converts a fixed-point number to a float

BTW, for addition and subtraction, you can just add or subtract the integers the
normal way! However, there is an `SGFixed.add(a, b)` and `SGFixed.sub(a, b)` for
completeness.

And, we provide some new fixed-point types that mirror the built-in Godot types,
for example:

 - `SGFixedVector3` replaces `Vector3`
 - `SGFixedTransform2D` replaces `Transform3D`
 - `SGFixedRect3` replaces `Rect2`

Complete API documentation will be coming soon.

### Avoiding overflow ###

If the result of a math operation will produce a number outside the range of
values that we can represent, that's called an "overflow".

If an overflow happens, you're not going to the result you're looking for, or
your game may even crash.

**To prevent overflow, any `SGFixedVector3` shouldn't have an X or Y component
outside the range `-1966080000` to `1966080000` (which is `-30000.0` to `30000.0` in
"real world" numbers)!**

Even though we can represent much bigger numbers, once you start performing any
math on that vector, the physics engine will need to use intermediate numbers
that are several times larger than the input you gave it, which can lead to
overflow - and things just not working.

So, this means any 2D scene you create can only be up to 60,000 pixels by 60,000
pixels. This is fine for many games, but if you need to have bigger scenes,
then, unfortunately, SG Physics 2D won't work for you.

Collision objects and shapes
----------------------------

SG Physics 2D is used in a very similar way to Godot's built-in physics, and
provides Nodes and Resources that mirror the built-in Godot ones.

![Screenshot of the "Create New Node" dialog showing the additional nodes](assets/screenshots/nodes.png)

At the moment, the follow collision objects are supported:

 - `SGArea2D`
 - `SGStaticBody2D`
 - `SGKinematicBody2D`
 - `SGRayCast2D`

So, there is no rigid body node yet! This technically means this is a "collision
engine" rather than a "physics engine", but proper rigid body physics is
something could (and probably will) be added at some point.

And, we currently support the following shapes:

 - Rectangles
 - Circles
 - Capsules
 - Convex polygons (via the `SGCollisionPolygon2D` node)

These nodes and resources can be created and edited in the Godot editor in much
the same way as their built-in counterparts.

The `SGFixedNode2D` node provides the base for all of the "fixed-point nodes",
and provides special `fixed_position`, `fixed_rotation` and `fixed_scale`
properties. Updating any of those properties will update their built-in
counterparts (ie. `position`, `rotation` and `scale`) which control the visual
representation of your node.

When in the editor, updating the built-in properties, like `position`, will also
update the fixed-point version (`fixed_position` in this case). But in game,
this is not the case, and your gameplay code should only update the fixed-point
properties.

`SGFixedNode2D`s will only inherit their "true" position from parents that are
also `SGFixedNode2D`s. So, if you have a `Node2D` that's a parent of a
`SGFixedNode2D` and you move the `Node2D`, the visual representation of the
`SGFixedNode2D` will move, but its position according the physics engine won't.

### The physics engine won't do anything until you ask! ###

Godot's builtin physics engine does a whole bunch of things automatically:

 1. After `_physics_process()` it sync's the node's position information into
    the physics engine.
 2. It does all the physics calculations, and sync's any changes back from the
    physics engine into the nodes.
 3. It emits a bunch of signals if certain collisions have occured (for example,
    a body entering an area).
 4. Then it calls `_physics_process()`, and starts over again.

_However, SG Physics 2D won't do anything until you ask it to!_

So, for example, if you change the `fixed_position` on a physics node (ie.
`SGKinematicBody2D` or `SGArea2D`), you need to manually call its
`sync_to_physics_engine()` method so the physics engine knows about its new
position. The same goes for changing its rotation, scale or any of its shapes.

> **Note:** if you are also using the [Godot Rollback Netcode addon](https://gitlab.com/snopek-games/godot-rollback-netcode), you will need to call `sync_to_physics_engine()` at the end of any `_load_state` implementation that loads physics-related state!

And no signals will be emitted in the case of a body entering an area! Instead,
you need to explicitly call `area.get_overlapping_bodies()`.

By allowing the developer to fully control when and how physics happens, you can
make your game logic fully deterministic.

(BTW, just using a deterministic physics engine won't automatically make your
whole game deterministic! You will need to make sure that all the rest of your
game logic is deterministic too.)

There is only one tiny piece of automation: `sync_to_physics_engine()` is called
automatically in the `_ready()` method of any of the physics objects, so the
physics engine will have it's starting info when it's first added to the scene.
This is most helpful for `SGStaticBody2D`s or `SGArea2D`s that don't ever move
or change shape/size.

### Don't convert floats to fixed-point in game! ###

Any floating point calculation you do in game could give a non-determistic
result.

So, any math with a fractional component that is important to your game logic
will need to be done in fixed-point. You'll need to get very used to the look of
fixed-point numbers. :-)

One tip: Godot can evaluate math expressions in the editor. So, you can type in
`16*65536`, select it, and then press Ctrl+Shift+E, and Godot will turn that
into `1048576`.

Follow up tip: Use `const` variables for magic numbers. I've gotten in the habit
of writing code like:

```
const ONE_POINT_FIVE = 98304

func _physics_process(delta: float) -> void:
	var some_var = vector.mul(ONE_POINT_FIVE)
```

However, if you have parts of the game that are purely cosmetic, you're safe to
use normal floating-point math for those.

Other nodes (Godot 3 only)
--------------------------
- `SGYSort` provides the same functionality as `YSort`, and inherits its parent's `fixed_position`.
- `SGPath2D` and `SGPathFollow2D` provide the same functionality as `Path2D` and `PathFollow2D`, replacing the use of floating point numbers by fixed point numbers.
- `SGTween` provides similar functionality to `Tween`, replacing the use of floating point numbers by fixed point numbers. The only type that can be interpolated is int, representing a fixed point value.
- `SGAStar2D` provides the same functionality as `AStar2D`, replacing the use of floating point numbers by fixed point numbers.

What about tile maps?
---------------------

SG Physics 2D doesn't include a special node for tile maps. If you want to add
collision shapes to a `TileMap` node, you'd do it in code, rather than in the
editor.

Creating a thousands of `SGStaticBody3D` and `SGCollisionShape3D` nodes can
cause performance problems, so instead of creating nodes, you can create
resources using the `SGPhysics3DServer`.

There's a demo in the code base demonstrating how to do this:

[https://gitlab.com/snopek-games/sg-physics-3d/-/blob/main/projects/godot-4/demos/tilemap/Main.gd](https://gitlab.com/snopek-games/sg-physics-3d/-/blob/main/projects/godot-4/demos/tilemap/Main.gd)

Compiling from source
---------------------

### Godot 4 ###

In Godot 4, SG Physics 2D is implemented as a GDExtension.

You can download prebuilt binaries for several platforms from the "Releases" page:

[https://gitlab.com/snopek-games/sg-physics-3d/-/releases](https://gitlab.com/snopek-games/sg-physics-3d/-/releases)

However, if you'd like to build it from source yourself, you can follow these steps:

 1. Download this repository and extract it.

 2. If you used Git, make sure that all the Git submodule are checked out too:
    ```bash
		git submodule update --init --recursive
    ```

 3. Build both the debug and release version for your platform:
   ```bash
   scons target=template_debug
   scons target=template_release
   ```

### Godot 3 ###

In Godot 3, SG Physics 2D is implemented as a Godot module, which (unlike a
GDNative plugin) must be compiled into Godot.

**But you don't have to be the one who does the compiling!**

You can download pre-compiled binaries for Windows, Linux, MacOS and HTML5
from the releases page:

[https://gitlab.com/snopek-games/sg-physics-3d/-/releases](https://gitlab.com/snopek-games/sg-physics-3d/-/releases)

However, if are targeting a platform that doesn't have pre-compiled binaries,
or need to include another module, or just want to compile it yourself, you
can follow these steps:

 1. Download this repository and extract it.

 2. Download the source code for the Godot 3.x version that you are targeting. If you cloned the source code, you must switch to the 3.x branch before continuing.

 3. Using the command-line, switch to the directory containing the Godot
    source code and run:

	```
	scons platform=PLATFORM tools=yes target=release_debug custom_modules=/PATH/sg-physics-3d/src
	```

	... replacing `PLATFORM` with your desired platform (ex. windows, osx,
	x11) and `PATH` with the full path to the directory containing the source
	code for SG Physics 2D.

The instructions above will compile the Godot editor. If you want to compile
the debug export templates instead, replace the command in the 3rd step with:

```
scons platform=PLATFORM tools=no target=debug custom_modules=/PATH/sg-physics-3d/src
```

... or for the release export templates:

```
scons platform=PLATFORM tools=no target=release production=yes custom_modules=/PATH/sg-physics-3d/src
```

For more information about compiling Godot, see [the official
documentation](https://docs.godotengine.org/en/stable/development/compiling/index.html#toc-devel-compiling).

Exporting your game (Godot 3)
-----------------------------

As mentioned above, the Godot 3 version of SG Physics 2D is implemented as
a module.

This means that when you export your game, you need to use an export template
that is built with SG Physics 2D. If you don't, your game will crash with an
error like:

```
SCRIPT ERROR: Parse Error: Unknown class: "SGKinematicBody2D"
```

**NOTE: This doesn't apply to Godot 4! You can use the standard export templates with Godot 4.**

Each [release build](https://gitlab.com/snopek-games/sg-physics-3d/-/releases)
includes not only the Godot editor, but also debug and release export templates
for that platform. Alternatively, you can compile the export templates yourself
as described in the previous section.

When setting up your export preset (under **Project** -> **Export...**) you
need put the path to each export template in the "Custom Template" section.

This screenshot shows configuring an export to run on Linux systems:

![Configuring "Custom Template" when exporting for Linux](assets/screenshots/export-templates.png)

License
-------

Copyright 2021-2023 David Snopek.

Licensed under the [MIT License](LICENSE.txt).

