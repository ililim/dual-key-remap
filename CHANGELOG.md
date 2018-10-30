# Changelog
All notable changes to this project will be documented in this file.

## 0.3
### Added
- We now send hardware scan codes by default instead of virtual key codes. These inputs aren't intercepted by DirectX,
  and as a result will work better in a lot of applications (especially games).
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