# Description
This is the main source code repository of the Emflex firmware.

Emflex was designed to provide users with unified framework for building embedded devices.
It incorporates RTOS, a set of drivers for different sensors and modules, 
BSP that abstracts application code from underlying boards.  
Application code (called "profiles") is meant to incorporate all the capabilities
provided by framework to easily build embedded devices for different areas, like
Home automation, Security, IoT, etc.
To ease debugging, Emflex provides CLI and logging components.

## Prerequisite
- Clone or fetch the code repository with all git submodules
  To clone the code repository recursively (git 1.9+)

        git clone --recursive https://github.com/emflex/emflex-firmware.git

    If it is already cloned, manually fetch all the git submodules.

        git submodule update --init --recursive

 - Install ARM cross compiler toolchain

## Building
Set building configuration parameters inside make_target file.
You can choose from the following target platforms.

Name                 | Description
-------------------- | ----------------------
ST_STM32L_DISCOVERY  | STM32L Discovery Kit
ST_STM32VL_DISCOVERY | STM32VL Discovery Kit
gtrack               | GSM security board

Also you have to choose one of available profiles.

Name     | Description
-------- | -----------------------
tracking | GSM security device
actuator | Window opening solution

To proceed please execute "make"
## Burning
* Connect your device to USB port
* Just type "make load" to upload firmware

## [LICENSE](LICENSE)

Unless stated elsewhere, file headers or otherwise, all files herein are licensed under an LGPLv3 license. For more information, please read the LICENSE file.
