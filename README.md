# Apricot-Hard-Disk-Interface
 This is a board for the ACT Apricot series of computers providing virtual hard disks held on a FAT format microSD card.

 The core of this is a Raspberry Pi Pico which pretends to be most of the circuity on the original "Winchester Card", i.e. the WD1010-05 MFM hard disk controller chip, the 8K transfer static RAM and the  control latch. The main address translation is done via the two GALs and data/addresses and voltage levels are managed via the 74LS and 74AHCT chips controlled by the GALs.

 The board can be built in two forms. The first is fully populated and is a stand-alone unit which just plugs into the Apricot expansion bus connector. The second form is as a daughter board to my RAM  expansion card (which needs one bodge wire added to bring the ALE signal to the expansion connector) in which case it only requires the two 74AHCT chips and one GAL as all the other functionality is  provided by the RAM card.

 The source and binaries for the GAL code can be found in the GAL sub-directory.

 The Raspberry Pi Pico's software is written in C using the Pico SDK. I used the version which runs on a Raspberry Pi 4 to develop this. One core is used for the host interface and the other for the less  time critical parts such as emulating the WD1010-05 commands and talking to the microSD card.

 The microSD card interface I used was easily found on Amazon and targetted at the Arduino, hence why it is a 5V model. It's an "AZ-Delivery SPI Reader Micro". Others will probably work but the pin-outs may  be different.

 The PCB was designed using KiCAD 6.