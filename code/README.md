# Panaplex Clock PIC Firmware

This code compiles with XC8 version 1.38 in Pro mode. (A free trial is available from Microchip's website.) It's likely that the Pro optimizations are required in order for the firmware to fit within the PIC16F1508's 4K ROM. 

A precompiled hex file is included as well.

I have not made any attempts to compile the code with SDCC. SDCC's PIC backend has not been updated to use the new features of the enhanced midrange parts and will generate subpar assembly. The code uses several features exclusive to XC8. (`bit` globals, 24-bit `long short int`s... yes that is a thing)

All the code is in one big ol' C file. I haven't taken the time to split the code into multiple files. It's not exactly the pinnacle of production-quality embedded C code.
