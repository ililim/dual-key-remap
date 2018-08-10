# Windows Dual Key Remap: Capslock to Ctrl/Escape

Remap any key to any other two keys on Windows. Most commonly used to remap CapsLock to Escape when pressed alone and Ctrl when pressed with other keys. Written in C to run at the lowest Windows API level for speed and reliability 🔥.

## Features

- Fast with minimal cpu footprint
- Does not swallow or leak inputs
- Handles edge cases

Current implementations of this key remapping can leak inputs, get stuck, or do not handle edge cases. Dual Key Remap runs at the lowest Windows API level making it both fast and reliable with an absolutely tiny cpu footprint.

## Installation

1) Download and unzip the latest [release](https://github.com/ililim/dual-key-remap/releases).
2) Put both 'dual-key-remap.exe' and 'config.txt' in a permament directory of your choice. (e.g. `C:\Program Files\dual-key-remap`).
3) Create a shortcut to 'dual-key-remap.exe' in your startup directory (e.g. `C:\Users\[USERNAME]\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\dual-key-remap.lnk`).
4) Optionally edit config.txt (see below) and run 'dual-key-remap.exe'. Your chosen keys are now remapped!

To uninstall, terminate the script from the task manager and remove the startup shortcut.

## Configuration

With the default configuration Dual Key Remap will remap CapsLock to Escape when pressed alone and Ctrl when pressed with other keys. To change this simply edit config.txt and adjust the key values. Refer to keys by their [virtual key codes](https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes). The characters A-Z and 0-9 can be referred to literally.

An example config.txt could look like this:

```
remap_key=VK_MENU
when_alone=A
with_other=VK_CONTROL
```

This would remap the alt key (VK_MENU) to output 'A' when tapped alone and Ctrl when pressed with other keys.

## Tips and Tricks

Below are a few optional advanced tips for configuring your system and using Dual Key Remap. They assume you are using it to rebind CapsLock to Ctrl/Escape, but if you are rebinding other keys they might still be helpful to you.

### Shortcuts that require Escape

Certain applications might require shortcuts such as Shift+Escape. These can still be launched with Dual Key Remap. To launch these, simply hold the modifier keys (e.g. Shift, Ctrl) first and press the CapsLock key last. Dual Key Remap will send the Shift+Escape key combination as expected!

The reason this works is because Dual Key Remap sends Escape or Ctrl depending on whether any other keys where pressed _after_ CapsLock was held down, so tapping Escape as the last part of a sequence of keys will always send Escape.

### Rebind keys during login

Dual Key Remap won't rebind any keys until the application is running, which means that during the login screen (and until Windows has launched all startup processes) your keys aren't rebound yet. This is not a problem for most people, but if it bothers you there are additional steps you could take to fix this.

By using an utility like [Sharp Keys](https://github.com/randyrants/sharpkeys) you can rebind keys in the windows registry. While this method only supports simple key-to-key rebinding, these rebindings persist in the Windows login screen. So if you want to use CapsLock as Ctrl/Escape you could do the following: Rebind CapsLock to Escape in Sharp Keys, then rebind Escape to Ctrl/Escape in Dual Key Remap. As a result, your CapsLock key will work as Escape during login and early startup, and once Windows is fully loaded the dual key functionality will activate.
