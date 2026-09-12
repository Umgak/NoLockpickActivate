# Changelog
## Unreleased
* Grab arguments in the correct order, from the correct place.
  * Was accidentally pulling from shadow space and flipping the argument order before passing to the hook, which is undefined behavior. In practice it didn't seem to matter, but it should be corrected for... correctness.
## [v4.0.0] - 2026-09-11
* Full rewrite.
* No longer dependent on Address Library.
* Compatible with EVERY version of Skyrim Special Edition supported by Skyrim Script Extender.
* Added config options to allow disabling key/lockpick and CONT/DOOR handling individualy.