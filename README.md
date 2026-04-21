# OpenLSR

Your favorite isometric licensed building block track building game, but it works!

Welcome to the native attempt, for the previous attempts, check out the `godot` and `old` branches.

## Disclaimer

This attempt is in its very, very early stages. All of the code is a mess, and it barely does anything yet.
Use at your own risk, and what not.

## Current State (as of 2026-04-20)

**All of the development on this is on the shoulders of the LSR decomp that is being done on my Ghidra server.**
**There's still a lot to understand about the game until I can properly start ripping out parts of it to replace with this.**

Right now, there's a DLL that gets built that has a few basic hooks for altered functionality.

The injector running our code before the game runs its some early code is currently RNG, this is because I very poorly
copied the code for injection from OpenLRR without implementing the EIP stuff, which would make it work properly.

The one useful thing that it does at the moment is forcing a window decoration onto the game, which is good if you are
running it with dgVoodoo windowed mode. Eventually this will be resolved more properly, but for now this works.

***If*** it injects properly, the command line arguments are currently ignored, and use the ones hardcoded into the hooks.
The command line arguments are passed along to the game as you would expect, but they are currently not processed, however
this will change. It also disables the printer, so the game doesn't instantly crash if you don't have a printer.

The hooks also remove the d3drm dependency from the game, as it only used some matrix quat math stuff.

## Goals

TODO: write down stuff here

## License

Nothing yet, because it's a load of garbage, MIT License eventually once it's actually useable.
