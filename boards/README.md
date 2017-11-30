# Panaplex Clock Design Files

The clock is built from four PCBs:

- the display board
- two "wing" boards for the cathode drivers
- the microcontroller board

All board designs are single-sided, as they were designed to be milled onto single-sided FR1 using an [Othermill V2](https://support.bantamtools.com/hc/en-us/articles/115001668853-Othermill-V2-). I'm including the board layouts for completeness; I would **not** recommend fabricating them in their current state, even if you have a desktop CNC mill. In the future I may redo the layouts and get double-sided boards professionally fabricated.

 Some notes:

- Traces and surface-mount components are on the back copper layer. (This is the side that gets milled.)
- Through-hole component footprints (displays, headers, etc.) are on the front "copper" layer. (This is the "blank" side with no copper.)
- The front copper layer is used for wire links and is not intended to be fabricated.
- The wing boards are soldered directly to either side of the display board, at right angles.
- No attention has been paid to silkscreen/soldermask layout.
- The boards were designed to be milled with 1/32" and 1/64" flat endmills. Trace width/spacing was set with these conditions in mind.

In hindsight, milled single-sided FR1 boards were not great for this application. The rapid turnaround time was great, but:

- I tried to make the display board millable using only a 1/32" endmill, which resulted in some traces that were too thin and broke during milling.
- The right-angle joints between the wings and the display board are not structurally sound. Without additional reinforcement, the bonding between the copper and the phenolic substrate becomes the weak point.
- It's *very* easy to lift pads and butcher traces when soldering. I had to do considerable rework during the build process. I completely botched the interconnects between the wing boards and the MCU board and wound up soldering kynar wires directly to the IC leads.
- I forgot to add a proper connector on the display board for the high-voltage supply and had to solder a jumper wire directly to the trace.
- Connections from the 5 anode selects to the drive transistors were left out of the display board schematic. Looking at the board layout, you can see five vias to nowhere. These accept an 0.1" header pin.

## Bill of materials

### Display board

- D1: Beckman SP-355 Panaplex 3 1/2-digit display module. (I haven't found any reference to these online; I pulled mine from some old panel meters.)
- D2, D3: Beckman [SP-353](http://www.industrialalchemy.org/articleview.php?item=20) Panaplex 3-digit display module. From eBay.
- D1, D2, and D3 are socketed in [Mill-Max 0566-2-15-15-21-27-10-0 pin receptacles](https://www.mouser.com/Search/ProductDetail.aspx?R=0566-2-15-15-21-27-10-0virtualkey57510000virtualkey575-055621). They are open on both sides to accomodate pins of arbitrary length.
- Q1, Q3, Q5, Q7, Q9: MMBTA92 high-voltage PNP transistors
- Q2, Q4, Q6, Q8, Q10: MMBTA42 high-voltage NPN transistors
- All resistors 1206, 1/4 watt

### Wings

- U1, U2: TPIC6B595DWRG4 power logic 8-bit shift registers
- All resistors 1206, 1/4 watt

### Microcontroller board

- U1: PIC16F1508-I/P
- U2: [SparkFun DeadOn DS3234 breakout board](https://www.sparkfun.com/products/10160) (I used a breakout board because I could get one with free 2-day shipping on Amazon, which wouldn't have been possible if I had ordered the bare DS3234s.)
- J5: [Taylor Electronics 1363](http://www.tayloredge.com/storefront/SmartNixie/PSU/index.html) high voltage power supply since I have a couple on hand. I desoldered the straight dual-row header and replaced it with a right-angle header to make the design more compact.
- C1, R1, R2: resistors and capacitors 1206. All resistors 1/4 watt.
- C2: bodged-in 470uF electrolytic capacitor. Switching on the high-voltage supply can cause the 5V supply to drop for a brief period, which would cause the PIC to brown out.