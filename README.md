# NoLockpickActivate

## Dependencies
* [SKSE64](https://skse.silverlock.org/)

## Installation
Install with your favorite mod manager, or unzip the release into `Skyrim Special Edition/Data` directory.

## Configuration
By default, the mod only affects opening containers.

The default configuration file is as follows:
```ini
[NoLockpickActivate]
; Set to 4 to see the outputs from AOB scans
; Set to 5 for trace logging of every time the entry point hits
LogLevel = 3

; Activate when using a key?
[ActivateWithKey]
CONT = false
DOOR = true

; Activate when using a lockpick?
[ActivateWithLockpick]
CONT = false
DOOR = true
```

However, if you want to change the behavior so that doors don't activate, or make keys and lockpicks behave separately, you can do so in `NoLockpickActivate.ini`.

## Build Dependencies
* [Script Extender Common](https://github.com/ianpatt/common)
* [SKSE64](https://github.com/ianpatt/skse64)
* [Pattern16](https://github.com/Dasaav-dsv/Pattern16)
* [inicpp](https://github.com/dujingning/inicpp)

## Build Instructions
Requires MSBuild to be added to your PATH.
```ps
git clone git@github.com:Umgak/NoLockpickActivate.git
cd NoLockpickActivate
./scripts/build.ps1
```
The .dll will be output in `x64/Release/NoLockpickActivate.dll`


## License
[MIT (c) Sayuri ('Umgak')](https://github.com/Umgak/NoLockpickActivate/blob/main/LICENSE)

[Pattern16 Copyright (c) 2023 Dasaav-dsv ](https://github.com/Dasaav-dsv/Pattern16/blob/master/LICENSE.txt)

[inicpp Copyright (c) 2023 dujingning](https://github.com/dujingning/inicpp/blob/main/LICENSE)