# Changelog
All notable changes to this project will be documented in this file.

## 0.8
### Changed
- Improved debug mode with clearer logs. In addition if the DEBUG env var is set DKR will launch in debug mode.
### Fixed
- Support sending extended key metadata for key events that support it (notably RIGHT_CTRL). This should resolve an issue where AHK was not able to distinguish between LEFT_CTRL and RIGHT_CTRL sent by dual-key-remap.

## 0.7
### Fixed
- Dual-key-remap will no longer show a blank console window when launched via the Windows Terminal app. This issue mainly affected Win11 users due to Terminal replacing Command Prompt as the default console application.
- A console window will still show up if you specify `DEBUG=1` in your config.

## 0.6
### Changed
- Improve how we send inputs, Dual-key-remap should now work in more applications and locales!
- Removed support for specifying virtual keys in the config as all key inputs will now be sent with both virtual and scancode values. If you relied on sending virtual keys previously to make dual-key-remap work with some apps you can use the regular key names again.
- Dual key remap will no longer try to automatically remap both modifiers keys if you don't specify whether it's the left or the right key. If you don't specify a side then it will rebind the LEFT only. For those that need both keys remapped, you can create two remappings in your config.
- Renamed and cleaned up the available key names. You may have to update your config.
- If a keycode that you need is missing: open a new issue to report it.
### Fixed
- Remappings bound to left or right modifiers will no longer be triggered by both left and right keys.

## 0.5
### Changed
- Mouse scrolling will now trigger the `with_other` modifier. So holding down the dual key and scrolling will send CTRL+scroll in the default config.

## 0.4
### Added
- Dual-key-remap now supports remapping multiple keys.
- Mouse presses (keydown) will now work with the dual key. They trigger the with_other modifier (CTRL by default).
### Changed
- The dual key will only become the 'with_other' key when keys (or mouse) is pressed down as opposed both up and down. This means
  that fast typists should no longer experience unexpected results due to key rollover.
- Some keynames had incorrectly capitalized names, this has now been fixed (i.e. SCROLLLOCK instead of ScrollLock). If you remapped one
  of these keys you will have to update your config to use the all capitalized names.
### Fixed
- The config will now always be loaded from the same directory as the executable instead of the directory the executable was launched from.

## 0.3
### Added
- We now send hardware scan codes by default instead of virtual key codes. These inputs will work better in most applications (especially games).
- Now both hardware inputs and virtual inputs are supported in the config, for more information see the config.txt documentation.
- Added additional capabilities to handle both left and right modifier keys.
- Added logging support by setting `debug=1` in config.txt.

## 0.2
### Added
- Support remapping keys to themselves. For example: Escape can now be rebound to Escape alone and Ctrl with others.
### Fixed
- Fix no error messages showing in certain cases when "config.txt" could not be found.

## 0.1
### Added
- First release
