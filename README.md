# Emflex Firmware
This is the main source code repository of the Emflex firmware.

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
