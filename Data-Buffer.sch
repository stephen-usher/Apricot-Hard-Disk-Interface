EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L 74xx:74LS245 U?
U 1 1 626FB5B9
P 8010 1810
F 0 "U?" H 8010 2791 50  0000 C CNN
F 1 "74LS245" H 8010 2700 50  0000 C CNN
F 2 "" H 8010 1810 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS245" H 8010 1810 50  0001 C CNN
	1    8010 1810
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS245 U?
U 1 1 626FF5F7
P 8060 4430
F 0 "U?" H 8060 5411 50  0000 C CNN
F 1 "74LS245" H 8060 5320 50  0000 C CNN
F 2 "" H 8060 4430 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS245" H 8060 4430 50  0001 C CNN
	1    8060 4430
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS112 U?
U 1 1 62764D16
P 1540 6310
F 0 "U?" H 1540 6791 50  0000 C CNN
F 1 "74LS112" H 1540 6700 50  0000 C CNN
F 2 "" H 1540 6310 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS112" H 1540 6310 50  0001 C CNN
	1    1540 6310
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS112 U?
U 3 1 6276768C
P 1550 7060
F 0 "U?" H 1780 7106 50  0000 L CNN
F 1 "74LS112" H 1780 7015 50  0000 L CNN
F 2 "" H 1550 7060 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS112" H 1550 7060 50  0001 C CNN
	3    1550 7060
	0    1    1    0   
$EndComp
$Comp
L SamacSys_Parts:CD4040BE IC?
U 1 1 6277D409
P 1060 4710
F 0 "IC?" H 1560 4975 50  0000 C CNN
F 1 "CD4040BE" H 1560 4884 50  0000 C CNN
F 2 "DIP794W53P254L1930H508Q16N" H 1910 4810 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/cd4040b.pdf" H 1910 4710 50  0001 L CNN
F 4 "CD4040BE, 12-stage Binary Counter, Up Counter, 3  18 V, 16-Pin PDIP" H 1910 4610 50  0001 L CNN "Description"
F 5 "5.08" H 1910 4510 50  0001 L CNN "Height"
F 6 "595-CD4040BE" H 1910 4410 50  0001 L CNN "Mouser Part Number"
F 7 "https://www.mouser.co.uk/ProductDetail/Texas-Instruments/CD4040BE?qs=L4Mc90zKIpgVInmXWEixLQ%3D%3D" H 1910 4310 50  0001 L CNN "Mouser Price/Stock"
F 8 "Texas Instruments" H 1910 4210 50  0001 L CNN "Manufacturer_Name"
F 9 "CD4040BE" H 1910 4110 50  0001 L CNN "Manufacturer_Part_Number"
	1    1060 4710
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 627842A9
P 1150 7060
F 0 "#PWR?" H 1150 6810 50  0001 C CNN
F 1 "GND" V 1155 6932 50  0000 R CNN
F 2 "" H 1150 7060 50  0001 C CNN
F 3 "" H 1150 7060 50  0001 C CNN
	1    1150 7060
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 62784803
P 1950 7060
F 0 "#PWR?" H 1950 6910 50  0001 C CNN
F 1 "VCC" V 1965 7188 50  0000 L CNN
F 2 "" H 1950 7060 50  0001 C CNN
F 3 "" H 1950 7060 50  0001 C CNN
	1    1950 7060
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 62784FB3
P 1240 6410
F 0 "#PWR?" H 1240 6260 50  0001 C CNN
F 1 "VCC" V 1255 6537 50  0000 L CNN
F 2 "" H 1240 6410 50  0001 C CNN
F 3 "" H 1240 6410 50  0001 C CNN
	1    1240 6410
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 627852AD
P 1240 6210
F 0 "#PWR?" H 1240 6060 50  0001 C CNN
F 1 "VCC" V 1255 6337 50  0000 L CNN
F 2 "" H 1240 6210 50  0001 C CNN
F 3 "" H 1240 6210 50  0001 C CNN
	1    1240 6210
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 62785B1E
P 1540 6010
F 0 "#PWR?" H 1540 5860 50  0001 C CNN
F 1 "VCC" H 1555 6183 50  0000 C CNN
F 2 "" H 1540 6010 50  0001 C CNN
F 3 "" H 1540 6010 50  0001 C CNN
	1    1540 6010
	1    0    0    -1  
$EndComp
Text HLabel 1060 4710 0    50   Input ~ 0
BA11
Text HLabel 1240 6310 0    50   Input ~ 0
BA11
Text HLabel 1840 6210 2    50   Input ~ 0
BA12
Text HLabel 2060 4810 2    50   Input ~ 0
BA10
Text HLabel 2060 4910 2    50   Input ~ 0
BA9
Text HLabel 2060 5010 2    50   Input ~ 0
BA7
Text HLabel 2060 5110 2    50   Input ~ 0
BA8
Text HLabel 2060 5410 2    50   Input ~ 0
BA0
Text HLabel 1060 5310 0    50   Input ~ 0
BA1
Text HLabel 1060 5210 0    50   Input ~ 0
BA2
Text HLabel 1060 5110 0    50   Input ~ 0
BA3
Text HLabel 1060 5010 0    50   Input ~ 0
BA6
Text HLabel 1060 4910 0    50   Input ~ 0
BA4
Text HLabel 1060 4810 0    50   Input ~ 0
BA5
$Comp
L power:VCC #PWR?
U 1 1 62787998
P 2060 4710
F 0 "#PWR?" H 2060 4560 50  0001 C CNN
F 1 "VCC" V 2075 4838 50  0000 L CNN
F 2 "" H 2060 4710 50  0001 C CNN
F 3 "" H 2060 4710 50  0001 C CNN
	1    2060 4710
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 62788009
P 1060 5410
F 0 "#PWR?" H 1060 5160 50  0001 C CNN
F 1 "GND" V 1065 5282 50  0000 R CNN
F 2 "" H 1060 5410 50  0001 C CNN
F 3 "" H 1060 5410 50  0001 C CNN
	1    1060 5410
	0    1    1    0   
$EndComp
Text HLabel 2060 5210 2    50   Input ~ 0
_BRESET
Text HLabel 1540 6610 2    50   Input ~ 0
_BRESET
Text HLabel 1270 2350 0    50   Input ~ 0
BA0
Text HLabel 1270 2250 0    50   Input ~ 0
BA1
Text HLabel 1270 2150 0    50   Input ~ 0
BA2
Text HLabel 1270 2050 0    50   Input ~ 0
BA3
Text HLabel 1270 1950 0    50   Input ~ 0
BA4
Text HLabel 1270 1850 0    50   Input ~ 0
BA5
Text HLabel 1270 1750 0    50   Input ~ 0
BA6
Text HLabel 1270 1650 0    50   Input ~ 0
BA7
Text HLabel 2270 1750 2    50   Input ~ 0
BA8
Text HLabel 2270 1850 2    50   Input ~ 0
BA9
Text HLabel 2270 1950 2    50   Input ~ 0
BA11
Text HLabel 2270 2150 2    50   Input ~ 0
BA10
$Comp
L SamacSys_Parts:AS6C6264-55PCN IC?
U 1 1 62705B9F
P 1270 1450
F 0 "IC?" H 1770 1715 50  0000 C CNN
F 1 "AS6C6264-55PCN" H 1770 1624 50  0000 C CNN
F 2 "DIP1524W51P254L3632H394Q28N" H 2120 1550 50  0001 L CNN
F 3 "https://datasheet.datasheetarchive.com/originals/distributors/Datasheets-DGA23/1496008.pdf" H 2120 1450 50  0001 L CNN
F 4 "Alliance Memory, AS6C6264-55PCN SRAM Memory, 64kbit, 55ns, 2.7  5.5 V 28-Pin PDIP" H 2120 1350 50  0001 L CNN "Description"
F 5 "3.937" H 2120 1250 50  0001 L CNN "Height"
F 6 "913-AS6C6264-55PCN" H 2120 1150 50  0001 L CNN "Mouser Part Number"
F 7 "https://www.mouser.co.uk/ProductDetail/Alliance-Memory/AS6C6264-55PCN?qs=LD2UibpCYJoQnvGJLyETNg%3D%3D" H 2120 1050 50  0001 L CNN "Mouser Price/Stock"
F 8 "Alliance Memory" H 2120 950 50  0001 L CNN "Manufacturer_Name"
F 9 "AS6C6264-55PCN" H 2120 850 50  0001 L CNN "Manufacturer_Part_Number"
	1    1270 1450
	1    0    0    -1  
$EndComp
Text HLabel 1270 1550 0    50   Input ~ 0
BA12
Text HLabel 1270 2450 0    50   Input ~ 0
BD0
Text HLabel 1270 2550 0    50   Input ~ 0
BD1
Text HLabel 1270 2650 0    50   Input ~ 0
BD2
Text HLabel 2270 2750 2    50   Input ~ 0
BD3
Text HLabel 2270 2650 2    50   Input ~ 0
BD4
Text HLabel 2270 2550 2    50   Input ~ 0
BD5
Text HLabel 2270 2450 2    50   Input ~ 0
BD6
Text HLabel 2270 2350 2    50   Input ~ 0
BD7
$Comp
L power:VCC #PWR?
U 1 1 6278CF33
P 2270 1450
F 0 "#PWR?" H 2270 1300 50  0001 C CNN
F 1 "VCC" V 2285 1578 50  0000 L CNN
F 2 "" H 2270 1450 50  0001 C CNN
F 3 "" H 2270 1450 50  0001 C CNN
	1    2270 1450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6278D25E
P 1270 2750
F 0 "#PWR?" H 1270 2500 50  0001 C CNN
F 1 "GND" V 1275 2622 50  0000 R CNN
F 2 "" H 1270 2750 50  0001 C CNN
F 3 "" H 1270 2750 50  0001 C CNN
	1    1270 2750
	0    1    1    0   
$EndComp
$EndSCHEMATC
