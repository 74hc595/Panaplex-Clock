EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:panaplex
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L TPIC6B595 U1
U 1 1 59ED26D5
P 2400 1350
F 0 "U1" H 2650 1350 60  0000 C CNN
F 1 "TPIC6B595" H 2150 1350 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-20W_7.5x12.8mm_Pitch1.27mm" H 2400 1350 60  0001 C CNN
F 3 "" H 2400 1350 60  0001 C CNN
	1    2400 1350
	1    0    0    -1  
$EndComp
$Comp
L TPIC6B595 U2
U 1 1 59ED2706
P 5950 1300
F 0 "U2" H 6200 1300 60  0000 C CNN
F 1 "TPIC6B595" H 5700 1300 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-20W_7.5x12.8mm_Pitch1.27mm" H 5950 1300 60  0001 C CNN
F 3 "" H 5950 1300 60  0001 C CNN
	1    5950 1300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 59ED2777
P 1700 2150
F 0 "#PWR01" H 1700 1900 50  0001 C CNN
F 1 "GND" H 1700 2000 50  0000 C CNN
F 2 "" H 1700 2150 50  0001 C CNN
F 3 "" H 1700 2150 50  0001 C CNN
	1    1700 2150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 59ED27E5
P 5250 2100
F 0 "#PWR02" H 5250 1850 50  0001 C CNN
F 1 "GND" H 5250 1950 50  0000 C CNN
F 2 "" H 5250 2100 50  0001 C CNN
F 3 "" H 5250 2100 50  0001 C CNN
	1    5250 2100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR03
U 1 1 59ED2806
P 1700 1450
F 0 "#PWR03" H 1700 1300 50  0001 C CNN
F 1 "VCC" H 1700 1600 50  0000 C CNN
F 2 "" H 1700 1450 50  0001 C CNN
F 3 "" H 1700 1450 50  0001 C CNN
	1    1700 1450
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR04
U 1 1 59ED2826
P 5250 1400
F 0 "#PWR04" H 5250 1250 50  0001 C CNN
F 1 "VCC" H 5250 1550 50  0000 C CNN
F 2 "" H 5250 1400 50  0001 C CNN
F 3 "" H 5250 1400 50  0001 C CNN
	1    5250 1400
	1    0    0    -1  
$EndComp
NoConn ~ 6450 2350
$Comp
L CONN_01X06 J10
U 1 1 59ED28B3
P 1100 1850
F 0 "J10" H 1100 2200 50  0000 C CNN
F 1 "CONN_01X06" V 1200 1850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 1100 1850 50  0001 C CNN
F 3 "" H 1100 1850 50  0001 C CNN
	1    1100 1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5250 2050 5250 2100
Wire Wire Line
	5250 1400 5250 1450
Wire Wire Line
	1700 1450 1700 1500
$Comp
L R R10
U 1 1 59ED2C02
P 3650 1500
F 0 "R10" V 3600 1600 50  0000 L CNN
F 1 "22k" V 3650 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 1500 50  0001 C CNN
F 3 "" H 3650 1500 50  0001 C CNN
	1    3650 1500
	0    1    1    0   
$EndComp
$Comp
L R R11
U 1 1 59ED31AE
P 3650 1600
F 0 "R11" V 3600 1700 50  0000 L CNN
F 1 "22k" V 3650 1600 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 1600 50  0001 C CNN
F 3 "" H 3650 1600 50  0001 C CNN
	1    3650 1600
	0    1    1    0   
$EndComp
$Comp
L R R12
U 1 1 59ED31CD
P 3650 1700
F 0 "R12" V 3600 1800 50  0000 L CNN
F 1 "22k" V 3650 1700 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 1700 50  0001 C CNN
F 3 "" H 3650 1700 50  0001 C CNN
	1    3650 1700
	0    1    1    0   
$EndComp
$Comp
L R R13
U 1 1 59ED31ED
P 3650 1800
F 0 "R13" V 3600 1900 50  0000 L CNN
F 1 "22k" V 3650 1800 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 1800 50  0001 C CNN
F 3 "" H 3650 1800 50  0001 C CNN
	1    3650 1800
	0    1    1    0   
$EndComp
$Comp
L R R14
U 1 1 59ED3212
P 3650 1900
F 0 "R14" V 3600 2000 50  0000 L CNN
F 1 "22k" V 3650 1900 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 1900 50  0001 C CNN
F 3 "" H 3650 1900 50  0001 C CNN
	1    3650 1900
	0    1    1    0   
$EndComp
$Comp
L R R15
U 1 1 59ED3238
P 3650 2000
F 0 "R15" V 3600 2100 50  0000 L CNN
F 1 "22k" V 3650 2000 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 2000 50  0001 C CNN
F 3 "" H 3650 2000 50  0001 C CNN
	1    3650 2000
	0    1    1    0   
$EndComp
$Comp
L R R16
U 1 1 59ED3267
P 3650 2100
F 0 "R16" V 3600 2200 50  0000 L CNN
F 1 "22k" V 3650 2100 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 2100 50  0001 C CNN
F 3 "" H 3650 2100 50  0001 C CNN
	1    3650 2100
	0    1    1    0   
$EndComp
$Comp
L R R17
U 1 1 59ED329F
P 3650 2200
F 0 "R17" V 3600 2300 50  0000 L CNN
F 1 "220k" V 3650 2200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 2200 50  0001 C CNN
F 3 "" H 3650 2200 50  0001 C CNN
	1    3650 2200
	0    1    1    0   
$EndComp
$Comp
L CONN_01X10 J11
U 1 1 59ED343D
P 4100 1850
F 0 "J11" H 4100 2400 50  0000 C CNN
F 1 "CONN_01X10" V 4200 1850 50  0000 C CNN
F 2 "panaplex:Pads_1x10_Pitch2.00mm_SMD" H 4100 1850 50  0001 C CNN
F 3 "" H 4100 1850 50  0001 C CNN
	1    4100 1850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 59ED36D2
P 3650 1100
F 0 "#PWR07" H 3650 850 50  0001 C CNN
F 1 "GND" H 3650 950 50  0000 C CNN
F 2 "" H 3650 1100 50  0001 C CNN
F 3 "" H 3650 1100 50  0001 C CNN
	1    3650 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 1100 3900 1100
Wire Wire Line
	3900 1100 3900 1400
$Comp
L R R18
U 1 1 59ED37AF
P 3650 2300
F 0 "R18" V 3600 2400 50  0000 L CNN
F 1 "330k" V 3650 2300 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 2300 50  0001 C CNN
F 3 "" H 3650 2300 50  0001 C CNN
	1    3650 2300
	0    1    1    0   
$EndComp
Wire Wire Line
	3900 2300 3800 2300
Wire Wire Line
	3500 2300 3500 2350
$Comp
L GND #PWR08
U 1 1 59ED3741
P 3500 2350
F 0 "#PWR08" H 3500 2100 50  0001 C CNN
F 1 "GND" H 3500 2200 50  0000 C CNN
F 2 "" H 3500 2350 50  0001 C CNN
F 3 "" H 3500 2350 50  0001 C CNN
	1    3500 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 2200 3350 2200
Wire Wire Line
	3500 2100 3350 2100
Wire Wire Line
	3500 2000 3350 2000
Wire Wire Line
	3500 1900 3350 1900
Wire Wire Line
	3500 1800 3350 1800
Wire Wire Line
	3500 1700 3350 1700
Wire Wire Line
	3500 1600 3350 1600
Wire Wire Line
	3500 1500 3350 1500
Text Label 3350 1500 0    60   ~ 0
K0A
Text Label 3350 1600 0    60   ~ 0
K0F
Text Label 3350 1700 0    60   ~ 0
K0B
Text Label 3350 1800 0    60   ~ 0
K0G
Text Label 3350 1900 0    60   ~ 0
K0E
Text Label 3350 2000 0    60   ~ 0
K0C
Text Label 3350 2100 0    60   ~ 0
K0D
Text Label 3350 2200 0    60   ~ 0
K0P
Wire Wire Line
	3900 2200 3800 2200
Wire Wire Line
	3900 2100 3800 2100
Wire Wire Line
	3900 2000 3800 2000
Wire Wire Line
	3900 1900 3800 1900
Wire Wire Line
	3900 1800 3800 1800
Wire Wire Line
	3900 1700 3800 1700
Wire Wire Line
	3900 1600 3800 1600
Wire Wire Line
	3900 1500 3800 1500
Wire Wire Line
	2900 1500 3050 1500
Wire Wire Line
	2900 1600 3050 1600
Wire Wire Line
	2900 1700 3050 1700
Wire Wire Line
	2900 1800 3050 1800
Wire Wire Line
	2900 1900 3050 1900
Wire Wire Line
	2900 2000 3050 2000
Wire Wire Line
	2900 2100 3050 2100
Wire Wire Line
	2900 2200 3050 2200
Text Label 3050 2200 2    60   ~ 0
K0E
Text Label 3050 2100 2    60   ~ 0
K0C
Text Label 3050 2000 2    60   ~ 0
K0D
Text Label 3050 1900 2    60   ~ 0
K0P
Text Label 3050 1800 2    60   ~ 0
K0G
Text Label 3050 1700 2    60   ~ 0
K0B
Text Label 3050 1600 2    60   ~ 0
K0F
Text Label 3050 1500 2    60   ~ 0
K0A
Wire Wire Line
	1300 1600 1600 1600
Wire Wire Line
	1600 1600 1600 1700
Wire Wire Line
	1600 1700 1700 1700
Wire Wire Line
	1700 1500 1350 1500
Wire Wire Line
	1350 1500 1350 1700
Wire Wire Line
	1350 1700 1300 1700
Wire Wire Line
	1700 2100 1700 2150
Wire Wire Line
	1700 1900 1600 1900
Wire Wire Line
	1600 1900 1600 2100
Wire Wire Line
	1600 2100 1300 2100
Wire Wire Line
	1300 2000 1550 2000
Wire Wire Line
	1550 2000 1550 1800
Wire Wire Line
	1550 1800 1700 1800
Wire Wire Line
	1300 1900 1450 1900
Wire Wire Line
	1450 1900 1450 2600
Wire Wire Line
	1450 2600 3000 2600
Wire Wire Line
	3000 2600 3000 2400
Wire Wire Line
	3000 2400 2900 2400
Wire Wire Line
	1300 1800 1350 1800
Wire Wire Line
	1350 1800 1350 2150
Wire Wire Line
	1350 2150 1700 2150
Connection ~ 1700 2150
$Comp
L R R20
U 1 1 59ED63F4
P 7250 1500
F 0 "R20" V 7200 1600 50  0000 L CNN
F 1 "22k" V 7250 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 1500 50  0001 C CNN
F 3 "" H 7250 1500 50  0001 C CNN
	1    7250 1500
	0    1    1    0   
$EndComp
$Comp
L R R21
U 1 1 59ED63FA
P 7250 1600
F 0 "R21" V 7200 1700 50  0000 L CNN
F 1 "22k" V 7250 1600 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 1600 50  0001 C CNN
F 3 "" H 7250 1600 50  0001 C CNN
	1    7250 1600
	0    1    1    0   
$EndComp
$Comp
L R R22
U 1 1 59ED6400
P 7250 1700
F 0 "R22" V 7200 1800 50  0000 L CNN
F 1 "22k" V 7250 1700 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 1700 50  0001 C CNN
F 3 "" H 7250 1700 50  0001 C CNN
	1    7250 1700
	0    1    1    0   
$EndComp
$Comp
L R R23
U 1 1 59ED6406
P 7250 1800
F 0 "R23" V 7200 1900 50  0000 L CNN
F 1 "22k" V 7250 1800 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 1800 50  0001 C CNN
F 3 "" H 7250 1800 50  0001 C CNN
	1    7250 1800
	0    1    1    0   
$EndComp
$Comp
L R R24
U 1 1 59ED640C
P 7250 1900
F 0 "R24" V 7200 2000 50  0000 L CNN
F 1 "22k" V 7250 1900 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 1900 50  0001 C CNN
F 3 "" H 7250 1900 50  0001 C CNN
	1    7250 1900
	0    1    1    0   
$EndComp
$Comp
L R R25
U 1 1 59ED6412
P 7250 2000
F 0 "R25" V 7200 2100 50  0000 L CNN
F 1 "22k" V 7250 2000 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 2000 50  0001 C CNN
F 3 "" H 7250 2000 50  0001 C CNN
	1    7250 2000
	0    1    1    0   
$EndComp
$Comp
L R R26
U 1 1 59ED6418
P 7250 2100
F 0 "R26" V 7200 2200 50  0000 L CNN
F 1 "22k" V 7250 2100 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 2100 50  0001 C CNN
F 3 "" H 7250 2100 50  0001 C CNN
	1    7250 2100
	0    1    1    0   
$EndComp
$Comp
L R R27
U 1 1 59ED641E
P 7250 2200
F 0 "R27" V 7200 2300 50  0000 L CNN
F 1 "220k" V 7250 2200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 2200 50  0001 C CNN
F 3 "" H 7250 2200 50  0001 C CNN
	1    7250 2200
	0    1    1    0   
$EndComp
$Comp
L CONN_01X10 J2
U 1 1 59ED6424
P 7700 1850
F 0 "J2" H 7700 2400 50  0000 C CNN
F 1 "CONN_01X10" V 7800 1850 50  0000 C CNN
F 2 "panaplex:Pads_1x10_Pitch2.00mm_SMD" H 7700 1850 50  0001 C CNN
F 3 "" H 7700 1850 50  0001 C CNN
	1    7700 1850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59ED642A
P 7250 1100
F 0 "#PWR?" H 7250 850 50  0001 C CNN
F 1 "GND" H 7250 950 50  0000 C CNN
F 2 "" H 7250 1100 50  0001 C CNN
F 3 "" H 7250 1100 50  0001 C CNN
	1    7250 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 1100 7500 1100
Wire Wire Line
	7500 1100 7500 1400
$Comp
L R R28
U 1 1 59ED6432
P 7250 2300
F 0 "R28" V 7200 2400 50  0000 L CNN
F 1 "330k" V 7250 2300 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7180 2300 50  0001 C CNN
F 3 "" H 7250 2300 50  0001 C CNN
	1    7250 2300
	0    1    1    0   
$EndComp
Wire Wire Line
	7500 2300 7400 2300
Wire Wire Line
	7100 2300 7100 2350
$Comp
L GND #PWR?
U 1 1 59ED643A
P 7100 2350
F 0 "#PWR?" H 7100 2100 50  0001 C CNN
F 1 "GND" H 7100 2200 50  0000 C CNN
F 2 "" H 7100 2350 50  0001 C CNN
F 3 "" H 7100 2350 50  0001 C CNN
	1    7100 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 2200 6950 2200
Wire Wire Line
	7100 2100 6950 2100
Wire Wire Line
	7100 2000 6950 2000
Wire Wire Line
	7100 1900 6950 1900
Wire Wire Line
	7100 1800 6950 1800
Wire Wire Line
	7100 1700 6950 1700
Wire Wire Line
	7100 1600 6950 1600
Wire Wire Line
	7100 1500 6950 1500
Text Label 6950 1500 0    60   ~ 0
K1A
Text Label 6950 1600 0    60   ~ 0
K1F
Text Label 6950 1700 0    60   ~ 0
K1B
Text Label 6950 1800 0    60   ~ 0
K1G
Text Label 6950 1900 0    60   ~ 0
K1E
Text Label 6950 2000 0    60   ~ 0
K1C
Text Label 6950 2100 0    60   ~ 0
K1D
Text Label 6950 2200 0    60   ~ 0
K1P
Wire Wire Line
	7500 2200 7400 2200
Wire Wire Line
	7500 2100 7400 2100
Wire Wire Line
	7500 2000 7400 2000
Wire Wire Line
	7500 1900 7400 1900
Wire Wire Line
	7500 1800 7400 1800
Wire Wire Line
	7500 1700 7400 1700
Wire Wire Line
	7500 1600 7400 1600
Wire Wire Line
	7500 1500 7400 1500
Wire Wire Line
	6450 1450 6600 1450
Wire Wire Line
	6450 1550 6600 1550
Wire Wire Line
	6450 1650 6600 1650
Wire Wire Line
	6450 1750 6600 1750
Wire Wire Line
	6450 1850 6600 1850
Wire Wire Line
	6450 1950 6600 1950
Wire Wire Line
	6450 2050 6600 2050
Wire Wire Line
	6450 2150 6600 2150
Text Label 6600 2150 2    60   ~ 0
K1E
Text Label 6600 2050 2    60   ~ 0
K1C
Text Label 6600 1950 2    60   ~ 0
K1D
Text Label 6600 1850 2    60   ~ 0
K1P
Text Label 6600 1750 2    60   ~ 0
K1G
Text Label 6600 1650 2    60   ~ 0
K1B
Text Label 6600 1550 2    60   ~ 0
K1F
Text Label 6600 1450 2    60   ~ 0
K1A
$Comp
L CONN_01X05 J20
U 1 1 59ED736B
P 4650 2050
F 0 "J20" H 4650 2350 50  0000 C CNN
F 1 "CONN_01X05" V 4750 2050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 4650 2050 50  0001 C CNN
F 3 "" H 4650 2050 50  0001 C CNN
	1    4650 2050
	-1   0    0    -1  
$EndComp
Connection ~ 5250 2050
Wire Wire Line
	5250 1450 5000 1450
Connection ~ 5250 1450
Wire Wire Line
	5250 2050 4850 2050
Wire Wire Line
	5250 1750 4900 1750
Wire Wire Line
	4900 1750 4900 1850
Wire Wire Line
	4900 1850 4850 1850
Wire Wire Line
	5250 1850 4950 1850
Wire Wire Line
	4950 1850 4950 1950
Wire Wire Line
	4950 1950 4850 1950
Wire Wire Line
	4850 2150 5000 2150
Wire Wire Line
	5000 2150 5000 1450
Wire Wire Line
	4850 2250 5050 2250
Wire Wire Line
	5050 2250 5050 1650
Wire Wire Line
	5050 1650 5250 1650
$EndSCHEMATC
