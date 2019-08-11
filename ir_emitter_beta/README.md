# Ero IR Emitter

## Overview

This beta test is the first Ero Bluetooth Mesh Application. While it is a IR emitter, it's primary purpose is to test the entire entire Ero Stack from embedded systems to network to cloud.

### Testing Features:
* BLE Mesh
* IR Emitter
* SMP over Bluetooth Mesh
* Ability to reset, provision and 'ping' from user interactions
* Temperature Sensor Model

## Requirements

* NRF52832

## Building and Running

Required CMake to build
I recommend using [west from zephyr](https://docs.zephyrproject.org/latest/guides/west/index.html) but you can use CMake by itself too. 

For west boards, build and flash the application as follows:

```
west build 
west sign -t imgtool 
west flash --hex-file build/zephyr/zephyr.signed.hex 
```

For CMake alone, 

```
mkdir build
cd build
cmake ..
make app
```

Then sign using [mcuboot imgtool](https://github.com/JuulLabs-OSS/mcuboot/blob/master/docs/imgtool.md) and flash the signed image
