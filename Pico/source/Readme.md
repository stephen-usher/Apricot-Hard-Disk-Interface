# Raspberry Pi Pico firmware source.
 This source includes a copy of the ["no-OS-FatFS-SD-SPI-RPi-Pico" code by "carlk"](https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico), so as to access the micoSD card via SPI. This in turn has a copy of  [ChaN's FatFs - Generic FAT Filesystem Module.](http://elm-chan.org/fsw/ff/00index_e.html).

 The architecture is quite simple. The system first sets up the pins on the Pico, initialises the SD card interface, reads the config file and sets up the configuration for the virtual hard drives. It then  runs the host interface code on core 1 whilst core 0 waits for WD1010-05 commands and actions them.

 The buffer and many variables are shareed between the two cores, the variables being defined as volatile so that the compiler knows not to optimise accesses away or cache the values. This acts as the inter  core communication.