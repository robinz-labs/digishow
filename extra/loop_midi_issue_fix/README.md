# The loopMIDI issue on Windows 11 and how to fix it

## The Problem

The Windows 11 update **KB5077241** introduced a new **MIDI 2.0 service** that broke compatibility with loopMIDI. The old virtual driver isn't recognized by the new system, so your DAW can't see loopMIDI ports.

## Quick Fixes

**1. Restart MIDI Service (temporary fix)**
Run as Administrator:
```
 net stop midisrv && net start midisrv
```
Works until next reboot.

**2. Use Windows Native Loopback Ports (permanent fix)**
- Install Windows MIDI Services SDK: `winget install Microsoft.WindowsMIDIServicesSDK`
- Download **Windows MIDI Port Creator** (from Steinberg forums)
- Import your loopMIDI ports
- Enable auto-recreate on startup

## Bottom Line
Native Windows MIDI 1.0 loopback ports fully replace loopMIDI. Use the Port Creator tool to migrate your existing ports in 2 minutes.
