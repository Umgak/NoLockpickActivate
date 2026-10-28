# Changelog
## Unreleased

## [v4.0.2] - 2026-10-28
- Reduced codegen buffer allocation size.
- Reduced trampoline buffer allocation size.
  - Users have finally discovered the 64kb SKSE hook trampoline limit due to the number of AI generated SKSE plugins.
    - I don't remember why it was 1kb before. It's a single jmp instruction.

## [v4.0.1] - 2026-09-12
- Properly forward stack arguments to TESObjectREFR::Activate
  - Had messed up the stack layout in my head totally. Ignore me.

## [v4.0.0] - 2026-09-11
- Full rewrite.
- No longer dependent on Address Library.
- Compatible with EVERY version of Skyrim Special Edition supported by Skyrim Script Extender.
- Added config options to allow disabling key/lockpick and CONT/DOOR handling individualy.