# Windows Dual Key Remap: Capslock to Ctrl/Escape

Remap any key to any other two keys on Windows. Most commonly used to remap CapsLock to Escape when pressed alone and Ctrl when pressed with other keys. Written in C to run at the lowest Windows API level for speed and reliability 🔥.

## Features

- Fast with minimal cpu footprint
- Does not swallow or leak inputs
- Handles edge cases

Current implementations of this key remapping can leak inputs or do not handle edge cases. Dual Key Remap runs at the lowest Windows API level making it both fast and reliable with an absolutely tiny cpu footprint.

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

